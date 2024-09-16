//
// Created by asgrim on 10.09.24.
//

#ifndef DISTT_SYS_PROC_CHILD_H
#define DISTT_SYS_PROC_CHILD_H


#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/types.h>

#include "pipes.h"
#include "ipc.h"
#include "banking.h"


struct child_pipes {
    local_id owner_id;
    local_id received_from;
    int16_t proc_num;
    pid_t pid;
    struct pipe_struct *connected_pipes;
};

int64_t child_loop(int32_t id, int32_t child_num, struct pipe_struct connected_pipes[], balance_t start_balance);


void set_up_message(Message *mes, MessageType type, char *buf, uint16_t len);

void set_up_message_fmt(Message *mes, MessageType type, const char *format, ...);

void wait_all_responded(int32_t id, struct child_pipes *cp, Message *message, MessageType type);
#endif //DISTT_SYS_PROC_CHILD_H
