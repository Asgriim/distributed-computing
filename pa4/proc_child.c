//
// Created by asgrim on 10.09.24.
//

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#include "logs.h"
#include "proc_child.h"
#include "pa2345.h"
#include "utils.h"

static int32_t proc_num = 0;


void wait_responded(int32_t from, struct child_pipes *cp, Message *message, MessageType type) {
    while (receive(cp, from, message) != 0 || message->s_header.s_type != type) {

    }
}

void wait_all_responded(int32_t id, struct child_pipes *cp, Message *message, MessageType type) {
    for (int i = 1; i < proc_num; ++i) {
        if (i == id) {
            continue;
        }

        wait_responded(i, cp, message, type);

        set_lamport_time(message->s_header.s_local_time);
        inc_lamport_time();
    }
}



void handle_message(int32_t id, Message *mes, struct child_pipes *cp) {
    switch (mes->s_header.s_type) {
        case CS_REQUEST: {
            inc_lamport_time();
            set_up_message(mes, CS_REPLY, NULL, 0);
            send(cp, cp->received_from, mes);
            break;
        }

        case DONE: {
//            printf("DEBUG proc %d lower BM\n", id);
            lower_bitmask(&cp->done_bitmask, cp->received_from);
//            printf("DEBUG proc %d after lower BM %d\n", cp->owner_id, cp->done_bitmask);
            break;
        }
    }
}

void child_listen(int32_t id, int32_t child_num, Message *mes, struct child_pipes *cp) {
    while (cp->done_bitmask != 0) {

        //handling incoming messages
        if (receive_any(cp, mes) == 0) {
//            printf("DEBUG proc %d recV type %d\n", id, mes->s_header.s_type);
//            printf("DEBUG proc %d mes localTM=%d, TYPE=%d \n", cp->owner_id, mes->s_header.s_local_time, mes->s_header.s_type);
            inc_lamport_time();
            handle_message(
                    id,
                    mes,
                    cp
            );
        }
    }
}

int64_t child_loop(int32_t id, int32_t child_num, struct pipe_struct connected_pipes[], bool lock) {
    proc_num = child_num + 1;
    pid_t parent_pid = getppid();
    pid_t pid = getpid();
    close_pipes_other(proc_num, id);

    struct child_pipes cp = {
            .owner_id = id,
            .proc_num = proc_num,
            .connected_pipes = connected_pipes,
            .pid = pid,
            .done_bitmask = create_bitmask(id, child_num)
    };
    init_lock_q();

    inc_lamport_time();

    write_log_fmt(log_started_fmt,
                  get_lamport_time(),
                  id,
                  pid,
                  parent_pid,
                  0
                  );


    Message *mes = malloc(sizeof(Message));
    inc_lamport_time();

    set_up_message_fmt(mes,
                       STARTED,
                       log_started_fmt,
                       get_lamport_time(),
                       id,
                       pid,
                       parent_pid,
                       0
                       );

    if (send_multicast(&cp, mes) == -1 ) {
        exit(-1);
    }

    wait_all_responded(id, &cp, mes, STARTED);


    write_log_fmt(log_received_all_started_fmt,
                  get_lamport_time(),
                  id
    );

    int32_t it_num = id * 5;



    for (int i = 1; i <= it_num; ++i) {

        if (lock) {
            request_cs(&cp);
        }

        set_up_message_fmt(
                mes,
                0,
                log_loop_operation_fmt,
                id,
                i,
                it_num
        );
        print(mes->s_payload);

        if (lock) {
            release_cs(&cp);
        }

    }



    set_up_message(mes, DONE, NULL, 0);

    if (send_multicast(&cp, mes) == -1 ) {
        exit(-1);
    }





    child_listen(id, child_num, mes, &cp);

    write_log_fmt(log_done_fmt,
                  get_lamport_time(),
                  id,
                  0
    );


    write_log_fmt(
                  log_received_all_done_fmt,
                  get_lamport_time(),
                  id
                  );


    free(mes);
//    close_pipes_my(proc_num, id);
    exit(0);
    return 0;
}

void set_up_message(Message *mes, MessageType type, char *buf, uint16_t len) {
    mes->s_header.s_magic = MESSAGE_MAGIC;
    mes->s_header.s_type = type;
    mes->s_header.s_payload_len = len;
    mes->s_header.s_local_time = get_lamport_time();

    memset(mes->s_payload, 0, MAX_PAYLOAD_LEN);

    if (len > 0) {
        memcpy(mes->s_payload, buf, len);
    }



}

void set_up_message_fmt(Message *mes, MessageType type, const char *format, ...) {
    mes->s_header.s_magic = MESSAGE_MAGIC;
    mes->s_header.s_type = type;
    mes->s_header.s_local_time = get_lamport_time();

    memset(mes->s_payload, 0, MAX_PAYLOAD_LEN);

    va_list args;
    va_start(args, format);


    int32_t len = vsprintf(mes->s_payload, format, args);
    mes->s_header.s_payload_len = len;

    va_end(args);
}




