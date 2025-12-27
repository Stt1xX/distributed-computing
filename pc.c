#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "pc.h"
#include "exitcodes.h"

int cpid = 0;
static const int MAX_PROCESS_COUNT = MAX_PROCESS_ID + 1;
const int NO_PIPE = -1;
int pc = 0;
static int *pipe_matrix = NULL;

void create_pipes (void) {
    pipe_matrix = (int*) malloc(sizeof(int) * pc * pc * 2);
    if (pipe_matrix == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_MEMORY_ALLOCATION_ERROR);
    }
    for (int i = 0; i < pc; i++) {
        for (int j = 0; j < pc; j++) {
            if (i != j) {
                if (pipe(pipe_matrix + (i * pc + j) * 2) == -1) {
                    fprintf(stderr, "Pipe creation failed\n");
                    exit(EXIT_PIPE_CREATION_ERROR);
                }
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

void init_process(struct Process* process){
    for (int i = 0; i < MAX_PROCESS_COUNT; i++){
        process->read_pipes[i] = NO_PIPE;
        process->write_pipes[i] = NO_PIPE;
    }
    process->pid = cpid++;
    set_read_pipes(process->pid, process->read_pipes);
    set_write_pipes(process->pid, process->write_pipes);
}
