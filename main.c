#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include "exitcodes.h"
#include "utils.h"
#include "pc.h"

struct Process process;

int main(int args, char **argv) {
    pc = parse_process_count(args, argv) + 1; // +1 for parent process
    create_pipes();
    init_process(&process); // initialize parent process
    for (int i = 0; i < pc - 1; i++) {
      pid_t pid = fork();
      if (pid < 0) {
        fprintf(stderr, "Fork failed\n");
        exit(EXIT_FORK_ERROR);
      } else if (pid == 0) {
        init_process(&process);
        printf("Hello, i am child my pid %d\n", process.pid);
        print_int_arr(process.read_pipes, sizeof(process.read_pipes));
        print_int_arr(process.write_pipes, sizeof(process.write_pipes));
        break;
      } else if (pid > 0) {
        cpid++;
        printf("Hello, i am parent my pid %d and now i am creating child %d\n", process.pid, pid);
        continue;
      }
    }
    return 0;
}
