#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "pa1.h"
#include "ipc.h"
#include "pc.h"

Message build_started_msg(struct Process *process) {
    Message msg = {
        .s_header = {
            .s_magic = MESSAGE_MAGIC,
            .s_payload_len = 0, // to be set later
            .s_type = STARTED,
            .s_local_time = 0 // ignored for now
        }
    };
    snprintf(msg.s_payload, sizeof(msg.s_payload), log_started_fmt, process->pid, getpid(), getppid());
    msg.s_header.s_payload_len = strlen(msg.s_payload);
    return msg;
}

Message build_done_msg(struct Process *process){
    Message msg = {
        .s_header = {
            .s_magic = MESSAGE_MAGIC,
            .s_payload_len = 0, // to be set later
            .s_type = DONE,
            .s_local_time = 0 // ignored for now
        }
    };
    snprintf(msg.s_payload, sizeof(msg.s_payload), log_done_fmt, process->pid);
    msg.s_header.s_payload_len = strlen(msg.s_payload);
    return msg;
}

Message build_received_all_started_msg(struct Process *process){
    Message msg = {
        .s_header = {
            .s_magic = MESSAGE_MAGIC,
            .s_payload_len = 0, // to be set later
            .s_type = STARTED,
            .s_local_time = 0 // ignored for now
        }
    };
    snprintf(msg.s_payload, sizeof(msg.s_payload), log_received_all_started_fmt, process->pid);
    msg.s_header.s_payload_len = strlen(msg.s_payload);
    return msg;
}
