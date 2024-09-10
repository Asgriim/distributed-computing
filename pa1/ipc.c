//
// Created by asgrim on 11.09.24.
//
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

#include "ipc.h"
#include "proc_child.h"


int send_multicast(void *self, const Message *msg) {
    struct child_pipes *cp = self;
    for (int i = 0; i < cp->proc_num; ++i) {
        if (i == cp->owner_id) {
            continue;
        }
        //todo full msg
        if(write(cp->connected_pipes[i].write_fd, msg, sizeof(MessageHeader)) == -1) {
            printf("pizda\n");
            return -1;
        };

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
//            printf("msg head %d\n", msg->s_header.s_payload_len);
            return 0;
        }
    }
    return -1;
}
