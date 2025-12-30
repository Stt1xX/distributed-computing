#ifndef MSG_BUILDER_H
#define MSG_BUILDER_H

#include "ipc.h"

Message build_started_msg(struct Process *process);
Message build_done_msg(struct Process *process);
Message build_received_all_started_msg(struct Process *process);
Message build_received_all_done_msg(struct Process *process);

#endif
