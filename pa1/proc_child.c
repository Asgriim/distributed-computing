//
// Created by asgrim on 10.09.24.
//

#include "proc_child.h"
#include <stdio.h>
#include <unistd.h>
#include "logs.h"

int64_t child_loop(int64_t id, uint64_t child_num, struct pipe_struct connected_pipes[]) {
    pid_t parent_pid = getppid();
    pid_t pid = getpid();

    int16_t bitmask = (~(1 << id - 1)) & (0xFFFF >> (16 - child_num));
    struct child_pipes cp = {
            .owner_id = id,
            .proc_num = child_num + 1,
            .connected_pipes = connected_pipes,
            .pid = pid
    };


    write_log_started(id, pid, parent_pid);

    Message *message = malloc(sizeof(Message));
    message->s_header.s_magic = MESSAGE_MAGIC;
    message->s_header.s_type = STARTED;
    int32_t len = sprintf(message->s_payload, log_started_fmt ,id, pid, parent_pid);
    message->s_header.s_payload_len = len;

    send_multicast(&cp, message);

    while (bitmask != 0) {
        if(receive_any(&cp, message) == 0) {

            bitmask &= ~(1 << (cp.received_from - 1)) ;
//            printf("bitmask %d ,proc %d\n", bitmask, id);
        };
    }
    write_log_received_all_started(id);

    free(message);
    exit(0);
    return 0;
}


