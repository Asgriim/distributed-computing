//
// Created by asgrim on 24.09.24.
//
#include <stdio.h>

#include "utils.h"
#include "pa2345.h"
#include "proc_child.h"


int request_cs(const void * self) {
    struct child_pipes *cp = (struct child_pipes *)self;
    Message *mes = malloc(sizeof(Message));

    inc_lamport_time();
    set_up_message(mes, CS_REQUEST, NULL, 0);
    send_multicast(cp, mes);

    timestamp_t req_time = get_lamport_time();
    bool lock = true;
    uint16_t resp_mask = create_bitmask(cp->owner_id, cp->proc_num - 1);
    lock_q_add_req(cp->owner_id, req_time);

    while (lock) {
        if (receive_any(cp, mes) == 0) {
            inc_lamport_time();
            set_lamport_time(mes->s_header.s_local_time);
//            printf("DEBUG proc %d received mes_t %d time %d from %d\n", cp->owner_id, mes->s_header.s_type, mes->s_header.s_local_time, cp->received_from);
            switch (mes->s_header.s_type) {
                case CS_REQUEST: {
                    lock_q_add_req(cp->received_from, mes->s_header.s_local_time);
                    inc_lamport_time();
                    set_up_message(mes, CS_REPLY, NULL, 0);
                    send(cp, cp->received_from, mes);
                    break;
                }

                case CS_REPLY: {
                    lower_bitmask(&resp_mask, cp->received_from);
                    break;
                }

                case CS_RELEASE: {
                    lock_q_release(cp->received_from);
                    break;
                }
            }
        }

        if (resp_mask == 0 && lock_q_is_ready(cp->owner_id, req_time)) {
//            printf("DEBUG proc %d is exiting lock time %d\n", cp->owner_id, get_lamport_time());
            lock = false;
        }
    }

    return 0;
}

int release_cs(const void * self) {
    struct child_pipes *cp = (struct child_pipes *)self;

    Message *mes = malloc(sizeof(Message));

    inc_lamport_time();
    set_up_message(mes, CS_RELEASE, NULL, 0);
    send_multicast(cp, mes);
    lock_q_release(cp->owner_id);
    return 0;
}

