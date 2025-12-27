#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include "exitcodes.h"
#include "utils.h"
#include "pc.h"
#include "ipc.h"

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
        // create test message

        Message msg;
        msg = (Message) {
            .s_header = {
                .s_magic = MESSAGE_MAGIC,
                .s_payload_len = 5,
                .s_type = 1,
                .s_local_time = 0
            },
            .s_payload = "Hello"
        };
        send(&process, PARENT_ID, &msg);
        break;
      } else if (pid > 0) {
        cpid++;
        // parent proc stuff
        continue;
      }
    }
    Message received_msg;
    if (process.pid == PARENT_ID) {
        receive(&process, 1, &received_msg);
        printf("Parent received message: %s\n", received_msg.s_payload);
    }

    return 0;
}
