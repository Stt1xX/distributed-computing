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

void init_proc(struct Process* process, int st_msgs_expected, int dn_msgs_expected){
    for (int i = 0; i < MAX_PROCESS_COUNT; i++){
        process->read_pipes[i] = NO_PIPE;
        process->write_pipes[i] = NO_PIPE;
    }   
    process->st_msgs_expected = st_msgs_expected;
    process->dn_msgs_expected = dn_msgs_expected;
    process->pid = cpid++;
    set_read_pipes(process->pid, process->read_pipes);
    set_write_pipes(process->pid, process->write_pipes);
}

int receive_all(struct Process* process, MessageType msg_type_wait_for){ // blocked receive
    Message rcv_msg; // ignore payload for now
    int* counter_lnk = NULL;
    if (msg_type_wait_for == STARTED) counter_lnk = &process->st_msgs_expected;
    else counter_lnk = &process->dn_msgs_expected;

    while (*counter_lnk > 0){
        if (receive_any(process, &rcv_msg) != -1){
            switch (rcv_msg.s_header.s_type){
                case STARTED:
                    process->st_msgs_expected--;
                    break;
                case DONE:
                    process->dn_msgs_expected--;
                    break;
            }
        }
    }
    return 0;
}

void start_proc_sync(struct Process* process){
    Message msg = build_started_msg(process);
    if (send_multicast(process, &msg) != 0) {
        exit_pc(EXIT_IPC_ERROR);
    }
    log_event(msg.s_payload);

    msg = build_received_all_started_msg(process);
    if (receive_all(process, STARTED) != 0) {
        exit_pc(EXIT_IPC_ERROR);
    }
    log_event(msg.s_payload);
}

void end_proc_sync(struct Process* process){
    Message msg = build_done_msg(process);
    if (send_multicast(process, &msg) != 0) {
        exit_pc(EXIT_IPC_ERROR);
    }
    log_event(msg.s_payload);

    msg = build_received_all_done_msg(process);
    if (receive_all(process, DONE) != 0) {
        exit_pc(EXIT_IPC_ERROR);
    }
    log_event(msg.s_payload);
}
