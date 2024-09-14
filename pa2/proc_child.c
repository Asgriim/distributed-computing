//
// Created by asgrim on 10.09.24.
//

#include "proc_child.h"
#include <stdio.h>
#include <unistd.h>
#include "logs.h"
#include <string.h>

static int32_t proc_num = 0;


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

    struct child_pipes cp = {
            .owner_id = id,
            .proc_num = proc_num,
            .connected_pipes = connected_pipes,
            .pid = pid
    };


    write_log_started(id, pid, parent_pid);

    Message *mes = malloc(sizeof(Message));


    set_up_message_fmt(mes,
                       STARTED,
                       log_started_fmt,
                       id,
                       pid,
                       parent_pid);

    if (send_multicast(&cp, mes) == -1 ) {
        exit(-1);
    }

    wait_all_responded(id, &cp, mes, STARTED);

    write_log_received_all_started(id);

    write_log_done(id);

    set_up_message_fmt(mes,
                       DONE,
                       log_done_fmt,
                       id);

    if (send_multicast(&cp, mes) == -1 ) {
        exit(-1);
    }

    wait_all_responded(id, &cp, mes, DONE);

    write_log_received_all_done(id);

    free(mes);
    exit(0);
    return 0;
}

void set_up_message(Message *mes, MessageType type, char *buf, uint16_t len) {
    mes->s_header.s_magic = MESSAGE_MAGIC;
    mes->s_header.s_type = type;
    memset(mes->s_payload, 0, MAX_PAYLOAD_LEN);
    memcpy(mes->s_payload, buf, len);
    mes->s_header.s_payload_len = len;
}

void set_up_message_fmt(Message *mes, MessageType type, const char *format, ...) {
    mes->s_header.s_magic = MESSAGE_MAGIC;
    mes->s_header.s_type = type;
    memset(mes->s_payload, 0, MAX_PAYLOAD_LEN);

    va_list args;
    va_start(args, format);

    int32_t len = sprintf(mes->s_payload, format, args);
    mes->s_header.s_payload_len = len;
}


