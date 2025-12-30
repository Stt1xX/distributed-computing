#ifndef PROCESS_H
#define PROCESS_H

#include "ipc.h"

struct Process {
    local_id pid;
    int read_pipes[MAX_PROCESS_ID + 1];
    int write_pipes[MAX_PROCESS_ID + 1];
    int st_msgs_expected;
    int dn_msgs_expected;
};

extern int pc;
extern int const NO_PIPE;
extern int cpid;

void create_pipes(void);

void init_proc(struct Process* process, int st_msgs_expected, int dn_msgs_expected);
void start_proc_sync(struct Process* process);
void end_proc_sync(struct Process* process);
int receive_all(struct Process* process, MessageType msg_type);

#endif
