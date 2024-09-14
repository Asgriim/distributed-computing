//
// Created by asgrim on 10.09.24.
//

#ifndef DISTT_SYS_PROC_CHILD_H
#define DISTT_SYS_PROC_CHILD_H


#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>

#include "pipes.h"
#include "ipc.h"
#include <unistd.h>
#include <sys/types.h>

struct child_pipes {
    local_id owner_id;
    local_id received_from;
    int16_t proc_num;
    pid_t pid;
    struct pipe_struct *connected_pipes;
};

int64_t child_loop(int32_t id, int32_t child_num, struct pipe_struct connected_pipes[]);


void set_up_message(Message *mes, MessageType type, char *buf, uint16_t len);

void set_up_message_fmt(Message *mes, MessageType type, const char *format, ...);
#endif //DISTT_SYS_PROC_CHILD_H
