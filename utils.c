#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include "utils.h"
#include "exitcodes.h"
#include "ipc.h"

int parse_process_count(int args, char **argv){
  int opt;
  while((opt = getopt(args, argv, ":p:")) != -1){
    switch(opt){
      case 'p': {
        char *end;
        long p = strtol(optarg, &end, 10);
        if (errno == ERANGE || *end != '\0' || p <= 0) {
            fprintf(stderr, "Invalid process count: %s\n", optarg);
            exit(EXIT_PARAM_PARSE_ERROR);
        }
        if (p > MAX_PROCESS_ID){
            fprintf(stderr, "Process count exceeds maximum allowed: %d\n", MAX_PROCESS_ID);
            exit(EXIT_PARAM_PARSE_ERROR);

        }
        return (int)p;
      }
      case ':':
        fprintf(stderr, "Option -%c requires an argument.\n", optopt);
        break;
      case '?':
        fprintf(stderr, "Unknown option: -%c\n", optopt);
        break;
      default:
        fprintf(stderr, "Unexpected error in option parsing.\n");
        break;
    }
  }
  fprintf(stderr, "Process count not specified.\n");
  exit(EXIT_PARAM_PARSE_ERROR);
}

void print_int_arr(int* arr, int size){
    for (int i = 0; i < size / sizeof(int); i++){
        printf("%d ", arr[i]);
    }
    printf("\n");
}
