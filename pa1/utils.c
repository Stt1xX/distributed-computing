#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include "common.h"
#include "utils.h"
#include "exitcodes.h"
#include "ipc.h"

int events_log_fd;
int pipes_log_fd;

int parse_process_count(int args, char **argv){
  int opt;
  while((opt = getopt(args, argv, ":p:")) != -1){
    switch(opt){
      case 'p': {
        char *end;
        long p = strtol(optarg, &end, 10);
        if (errno == ERANGE || *end != '\0' || p <= 0) {
            exit_pc(EXIT_PARAM_PARSE_ERROR);
        }
        if (p > MAX_PROCESS_ID){
            exit_pc(EXIT_PARAM_PARSE_ERROR);

        }
        return (int)p;
      }
      case ':':
      case '?':
        exit_pc(EXIT_PARAM_PARSE_ERROR);
    }
  }
  exit_pc(EXIT_PARAM_PARSE_ERROR);
  return -1;
}

void print_int_arr(int* arr, int size){
    for (int i = 0; i < size / sizeof(int); i++){
        printf("%d ", arr[i]);
    }
    printf("\n");
}


void open_log_file(void){
    events_log_fd = open(events_log, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (events_log_fd == -1){
        exit_pc(EXIT_LOG_FILE_OPEN_ERROR);
    }
}

void close_log_file(void){
    if (events_log_fd != -1){
        close(events_log_fd);
    }
}

void log_event(const char* string){
    dprintf(events_log_fd, "%s", string);
    dprintf(1, "%s", string); // stdout = 1
}

void exit_pc(enum ExitCodes code){
    switch(code){
        case EXIT_OK:
            exit(0);
        case EXIT_PARAM_PARSE_ERROR:
            fprintf(stderr, "Parameter parsing error\n");
            exit(EXIT_PARAM_PARSE_ERROR);
        case EXIT_FORK_ERROR:
            fprintf(stderr, "Fork error\n");
            exit(EXIT_FORK_ERROR);
        case EXIT_MEMORY_ALLOCATION_ERROR:
            fprintf(stderr, "Memory allocation error\n");
            exit(EXIT_MEMORY_ALLOCATION_ERROR);
        case EXIT_PIPE_CREATION_ERROR:
            fprintf(stderr, "Pipe creation error\n");
            exit(EXIT_PIPE_CREATION_ERROR);
        case EXIT_IPC_ERROR:
            fprintf(stderr, "IPC error\n");
            exit(EXIT_IPC_ERROR);
        case EXIT_LOG_FILE_OPEN_ERROR:
            fprintf(stderr, "Log file open error\n");
            exit(EXIT_LOG_FILE_OPEN_ERROR);
        default:
            fprintf(stderr, "Unknown exit code\n");
            exit(0);
    }
}
