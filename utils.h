#ifndef UTILS_H
#define UTILS_H

#include "exitcodes.h"

extern int events_log_fd;
extern int pipes_log_fd;

int parse_process_count(int args, char **argv);
void print_int_arr(int* arr, int size);
void exit_pc(enum ExitCodes code);

void open_log_file(void);
void close_log_file(void);
void log_event(const char* string);

#endif
