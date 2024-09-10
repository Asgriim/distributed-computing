//
// Created by asgrim on 10.09.24.
//

#ifndef DISTT_SYS_PROC_CHILD_H
#define DISTT_SYS_PROC_CHILD_H


#include <stdint.h>
#include <stdlib.h>

#include "pipes.h"
#include "ipc.h"


struct child_pipes {
    local_id owner_id;
    local_id received_from;
    int16_t proc_num;
    pid_t pid;
    struct pipe_struct *connected_pipes;
};

int64_t child_loop(int64_t id, uint64_t child_num, struct pipe_struct connected_pipes[]);
#endif //DISTT_SYS_PROC_CHILD_H
