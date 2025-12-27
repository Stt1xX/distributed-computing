#ifndef PROCESS_H
#define PROCESS_H

#include "ipc.h"

struct Process {
    local_id pid;
    int read_pipes[MAX_PROCESS_ID + 1];
    int write_pipes[MAX_PROCESS_ID + 1];
};

extern int pc;
extern int const NO_PIPE;
extern int cpid;

void create_pipes(void);

void init_process(struct Process* process);

#endif
