#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "pc.h"
#include "pa1.h"
#include "exitcodes.h"
#include "msg_builder.h"
#include "utils.h"

int cpid = 0;
static const int MAX_PROCESS_COUNT = MAX_PROCESS_ID + 1;
const int NO_PIPE = -1;
int pc = 0;
static int *pipe_matrix = NULL;

void create_pipes (void) {
    pipe_matrix = (int*) malloc(sizeof(int) * pc * pc * 2);
    if (pipe_matrix == NULL) {
        exit_pc(EXIT_MEMORY_ALLOCATION_ERROR);
    }
    for (int i = 0; i < pc; i++) {
        for (int j = 0; j < pc; j++) {
            int *fd = pipe_matrix + (i * pc + j) * 2;
            if (i != j) {
                if (pipe(fd) == -1) {
                    exit_pc(EXIT_PIPE_CREATION_ERROR);
                }
                int flags = fcntl(*fd, F_GETFL); // non-blocking reading
                fcntl(*fd, F_SETFL, flags | O_NONBLOCK);
            } else {
                fd[0] = NO_PIPE;
                fd[1] = NO_PIPE;
            }
        }
    }
}

static void set_write_pipes(int pid, int* out_arr){
    for (int i = 0; i < pc; i++) {
        out_arr[i] = (pipe_matrix + ((pid * pc + i) * 2))[1];
    }
}

static void set_read_pipes(int pid, int* out_arr){
    for (int i = 0; i < pc; i++) {
        out_arr[i] = (pipe_matrix + ((i * pc + pid) * 2))[0];
    }
}

void init_proc(struct Process* process){
    for (int i = 0; i < MAX_PROCESS_COUNT; i++){
        process->read_pipes[i] = NO_PIPE;
        process->write_pipes[i] = NO_PIPE;
    }
    process->pid = cpid++;
    set_read_pipes(process->pid, process->read_pipes);
    set_write_pipes(process->pid, process->write_pipes);
}

void start_proc_sync(struct Process* process){
    Message msg = build_started_msg(process);
    log_event(msg.s_payload);
    if (send_multicast(process, &msg) != 0) {
        exit_pc(EXIT_IPC_ERROR);
    }

    for( int i = 0; i < pc - 2;) {
        Message rcv_msg;
        if (receive_any(process, &rcv_msg) == 0) {
            if (rcv_msg.s_header.s_type == STARTED) {
                i++;
            }
        }
    }
    msg = build_received_all_started_msg(process);
    log_event(msg.s_payload);
}

void end_proc_sync(struct Process* process){
    char payload[MAX_PAYLOAD_LEN];
    snprintf(payload, sizeof(payload), log_done_fmt, process->pid);
    Message msg = {
        .s_header = {
            .s_magic = MESSAGE_MAGIC,
            .s_payload_len = strlen(payload),
            .s_type = DONE,
            .s_local_time = 0 // ignored for now
        }
    };
    memcpy(msg.s_payload, payload, strlen(payload));
    send_multicast(process, &msg);
}
