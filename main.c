#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include "exitcodes.h"
#include "utils.h"
#include "pc.h"
#include "ipc.h"
#include "common.h"

struct Process process;

int main(int args, char **argv) {
    pc = parse_process_count(args, argv) + 1; // +1 for parent process
    open_log_file();
    create_pipes();
    init_proc(&process); // initialize parent process
    for (int i = 0; i < pc - 1; i++) {
      pid_t pid = fork();
      if (pid < 0) {
          exit_pc(EXIT_FORK_ERROR);
      } else if (pid == 0) {
          init_proc(&process);
          start_proc_sync(&process);
          // skip working for child processes for now
        //   end_proc_sync(&process);
          break;
      } else if (pid > 0) {
          cpid++;
          // parent proc stuff
          continue;
      }
    }
    if (process.pid == PARENT_ID) {
        while (wait(NULL) > 0);
    }
    close_log_file();
    return 0;
}
