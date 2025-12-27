#include <unistd.h>
#include "ipc.h"
#include "pc.h"

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
    if (write(process->write_pipes[dst], msg, sizeof(Message)) != sizeof(Message)){
        return -1;
    }
    close(process->write_pipes[dst]);
    return 0;
}

int receive(void * self, local_id from, Message * msg){
    struct Process * process = (struct Process*) self;
    if (process->read_pipes[from] == NO_PIPE){
        return -1;
    }
    if (read(process->read_pipes[from], msg, sizeof(MessageHeader)) != sizeof(MessageHeader)){
        return -1;
    }
    if (read(process->read_pipes[from], msg->s_payload, msg->s_header.s_payload_len) != msg->s_header.s_payload_len){
        return -1;
    }
    return msg->s_header.s_payload_len;
}
