//
// Created by asgrim on 10.09.24.
//

#include "proc_child.h"
#include <stdio.h>
#include <unistd.h>
#include "logs.h"
#include <string.h>

int32_t proc_num = 0;


void wait_all_responded(int32_t id, struct child_pipes *cp, Message *message, MessageType type) {
    for (int i = 1; i < proc_num; ++i) {
        if (i == id) {
            continue;
        }

        //todo damn
        while (receive(cp, i, message) != 0 && message->s_header.s_type != type) {

        }
    }
}

int64_t child_loop(int32_t id, int32_t child_num, struct pipe_struct connected_pipes[]) {
    proc_num = child_num + 1;
    pid_t parent_pid = getppid();
    pid_t pid = getpid();
    close_pipes_other(proc_num, id);

//    open_pipes_my(proc_num, id);
//    int16_t bitmask = create_bitmask(id, child_num);
    struct child_pipes cp = {
            .owner_id = id,
            .proc_num = proc_num,
            .connected_pipes = connected_pipes,
            .pid = pid
    };


    write_log_started(id, pid, parent_pid);

    Message *message = malloc(sizeof(Message));
    message->s_header.s_magic = MESSAGE_MAGIC;
    message->s_header.s_type = STARTED;

    memset(message->s_payload, 0, MAX_PAYLOAD_LEN);

    int32_t len = sprintf(message->s_payload, log_started_fmt ,id, pid, parent_pid);
    message->s_header.s_payload_len = len;

    if (send_multicast(&cp, message) == -1 ) {
        exit(-1);
    }

    wait_all_responded(id, &cp, message, STARTED);

    write_log_received_all_started(id);
    write_log_done(id);

    message->s_header.s_magic = MESSAGE_MAGIC;
    message->s_header.s_type = DONE;
    memset(message->s_payload, 0, MAX_PAYLOAD_LEN);

    len = sprintf(message->s_payload, log_done_fmt ,id);
    message->s_header.s_payload_len = len;


    if (send_multicast(&cp, message) == -1 ) {
        exit(-1);
    };

    wait_all_responded(id,  &cp, message, DONE);

    write_log_received_all_done(id);

    free(message);
    exit(0);
    return 0;
}


