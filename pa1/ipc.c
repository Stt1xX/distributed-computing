#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include "ipc.h"
#include "exitcodes.h"
#include "utils.h"
#include "pc.h"

static ssize_t read_all(int fd, void * buf, size_t count){
    size_t total_read = 0;
    while(total_read < count){
        ssize_t bytes_passed = read(fd, (char*)(buf) + total_read, count - total_read);
        if (bytes_passed <= 0){
            if (errno == EINTR) continue;
            return -1;
        }
        total_read += bytes_passed;
    }
    return total_read;
}

static ssize_t write_all(int fd, const void * buf, size_t count){
    size_t total_written = 0;
    while(total_written < count){
        ssize_t bytes_passed = write(fd, (char*)(buf) + total_written, count - total_written);
        if (bytes_passed <= 0){
            if (errno == EINTR) continue;
            return -1;
        }
        total_written += bytes_passed;
    }
    return total_written;
}

int send_multicast(void * self, const Message * msg){
    for (int i = 0; i < pc; i++){
        if (i != ((struct Process*)self)->pid){
            if (send(self, i, msg) != 0){
                return -1;
            }
        }
    }
    return 0;
}

int send(void * self, local_id dst, const Message * msg){
    struct Process * process = (struct Process*) self;
    if (process->write_pipes[dst] == NO_PIPE){
        return -1;
    }
    if (write_all(process->write_pipes[dst], msg, sizeof(MessageHeader)) != sizeof(MessageHeader)){
        return -1;
    }
    if (write_all(process->write_pipes[dst], msg->s_payload, msg->s_header.s_payload_len) != msg->s_header.s_payload_len){
        return -1;
    }
    return 0;
}

int receive(void * self, local_id from, Message * msg){
    struct Process * process = (struct Process*) self;
    if (process->read_pipes[from] == NO_PIPE){
        return -1;
    }
    if (read_all(process->read_pipes[from], msg, sizeof(MessageHeader)) != sizeof(MessageHeader)){
        return -1;
    }
    if (read_all(process->read_pipes[from], msg->s_payload, msg->s_header.s_payload_len) != msg->s_header.s_payload_len){
        return -1;
    }
    return 0;
}

int receive_any(void * self, Message * msg){
    struct Process * process = (struct Process*) self;
    int* fd = process->read_pipes;
    for(int i = 0; i < pc; i++){
        if (fd[i] == NO_PIPE) continue;
        int flags = fcntl(fd[i], F_GETFL) ; // non-blocking reading
        fcntl(fd[i], F_SETFL, flags | O_NONBLOCK);
        
        ssize_t header_read = 0;
        while (header_read < sizeof(MessageHeader)) {
            ssize_t n = read(fd[i], (char*)(&msg->s_header) + header_read, sizeof(MessageHeader) - header_read);
            if (n > 0){
                header_read += n;
            } else if (n == -1 && errno == EAGAIN){
                break;
            } else {
                fcntl(fd[i], F_SETFL, flags);
                return -1;  
            }
        }
        if (header_read < sizeof(MessageHeader)) {
            fcntl(fd[i], F_SETFL, flags);
            continue; // try next pipe
        }
        ssize_t payload_read = 0;
        while (payload_read < msg->s_header.s_payload_len) {
            ssize_t n = read(fd[i], msg->s_payload + payload_read, msg->s_header.s_payload_len - payload_read);
            if (n > 0){
                payload_read += n;
            } else if (n == -1 && errno == EAGAIN){
                break;
            } else {
                fcntl(fd[i], F_SETFL, flags);
                return -1;  
            }
        }
        if (payload_read < msg->s_header.s_payload_len) {
            fcntl(fd[i], F_SETFL, flags);
            continue; // try next pipe
        } else {
            fcntl(fd[i], F_SETFL, flags);
            return i; // success
        }
        fcntl(fd[i], F_SETFL, flags);
    }
    return -1;
}
