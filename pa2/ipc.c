//
// Created by asgrim on 11.09.24.
//
#include <unistd.h>
#include <stdio.h>

#include "ipc.h"
#include "proc_child.h"


int send_multicast(void *self, const Message *msg) {
    struct child_pipes *cp = self;
    for (int i = 0; i < cp->proc_num; ++i) {
        if (i == cp->owner_id) {
            continue;
        }
        if(write(cp->connected_pipes[i].write_fd, msg, (sizeof(MessageHeader))+ msg->s_header.s_payload_len) == -1) {
            return -1;
        }

    }
    return 0;
}

int receive_any(void *self, Message *msg) {
    struct child_pipes *cp = self;
    int32_t stat = -1;
    for (int i = 0; i < cp->proc_num; ++i) {
        if (i == cp->owner_id) {
            continue;
        }

        stat = read(cp->connected_pipes[i].read_fd, msg, sizeof(MessageHeader));
        if (stat != -1) {
            cp->received_from = i;
            stat = read(cp->connected_pipes[i].read_fd, msg->s_payload, msg->s_header.s_payload_len);
            return 0;
        }
    }
    return -1;
}

int send(void *self, local_id dst, const Message *msg) {
    struct child_pipes *cp = self;

    if(write(cp->connected_pipes[dst].write_fd, msg, (sizeof(MessageHeader))+ msg->s_header.s_payload_len) == -1) {
        return -1;
    }
    return 0;
}

int receive(void *self, local_id from, Message *msg) {
    struct child_pipes *cp = self;
    int32_t stat = -1;
    stat = read(cp->connected_pipes[from].read_fd, msg, sizeof(MessageHeader));
    cp->received_from = from;
    if (stat == -1) {
        return -1;
    }

    read(cp->connected_pipes[from].read_fd, msg->s_payload, msg->s_header.s_payload_len);
    return 0;
}
