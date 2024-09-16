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


static int32_t proc_num = 0;


void wait_all_responded(int32_t id, struct child_pipes *cp, Message *message, MessageType type) {
    for (int i = 1; i < proc_num; ++i) {
        if (i == id) {
            continue;
        }

        while (receive(cp, i, message) != 0 || message->s_header.s_type != type) {

        }
    }
}

void update_history(BalanceHistory  *balanceHistory, BalanceState *balance_state) {
    if (balanceHistory->s_history_len > 0) {

        for (int i = balanceHistory->s_history_len; i <= balance_state->s_time; ++i) {
            balanceHistory->s_history[i] = *balance_state;
        }


    } else {
        balanceHistory->s_history[0] = *balance_state;
    }

    balanceHistory->s_history_len = balance_state->s_time + 1;

}


int64_t child_loop(int32_t id, int32_t child_num, struct pipe_struct connected_pipes[], balance_t start_balance) {
    proc_num = child_num + 1;
    pid_t parent_pid = getppid();
    pid_t pid = getpid();
    close_pipes_other(proc_num, id);

    BalanceHistory balanceHistory = {.s_id = id, .s_history_len = 0};
    BalanceState balance_state;

    struct child_pipes cp = {
            .owner_id = id,
            .proc_num = proc_num,
            .connected_pipes = connected_pipes,
            .pid = pid
    };

    timestamp_t timestamp = get_physical_time();

    balance_state.s_time = timestamp;
    balance_state.s_balance = start_balance;

    update_history(&balanceHistory, &balance_state);

    write_log_fmt(log_started_fmt,
                  timestamp,
                  id,
                  pid,
                  parent_pid,
                  start_balance
                  );


    Message *mes = malloc(sizeof(Message));


    set_up_message_fmt(mes,
                       STARTED,
                       log_started_fmt,
                       timestamp,
                       id,
                       pid,
                       parent_pid,
                       start_balance
                       );

    if (send_multicast(&cp, mes) == -1 ) {
        exit(-1);
    }

    wait_all_responded(id, &cp, mes, STARTED);

    write_log_fmt(log_received_all_started_fmt,
                  timestamp,
                  id
    );

    bool received_stop = false;

    while (!received_stop) {
        //handling incoming messages
       if (receive_any(&cp, mes) == 0) {

           switch (mes->s_header.s_type) {
               case TRANSFER: {
                   write_log_fmt("%d proc received TRANS mes \n", id);
                   break;
               }
               case STOP: {
                   write_log_fmt("%d proc received STOP mes \n", id);
                   //todo implement calculation of history byte size
                   printf("sizeof(Balance hist ) = %lu\n", sizeof(BalanceHistory));
                   set_up_message(mes, BALANCE_HISTORY, (char *) &balanceHistory,
                                  sizeof(BalanceHistory)
                                  );

                   send(&cp, PARENT_ID, mes);
                   received_stop = true;
                   break;
               }

           }
        }
    }

    free(mes);
    exit(0);
    return 0;
}

void set_up_message(Message *mes, MessageType type, char *buf, uint16_t len) {
    mes->s_header.s_magic = MESSAGE_MAGIC;
    mes->s_header.s_type = type;
    mes->s_header.s_payload_len = len;
    mes->s_header.s_local_time = get_physical_time();

    memset(mes->s_payload, 0, MAX_PAYLOAD_LEN);

    if (len > 0) {
        memcpy(mes->s_payload, buf, len);
    }



}

void set_up_message_fmt(Message *mes, MessageType type, const char *format, ...) {
    mes->s_header.s_magic = MESSAGE_MAGIC;
    mes->s_header.s_type = type;
    mes->s_header.s_local_time = get_physical_time();

    memset(mes->s_payload, 0, MAX_PAYLOAD_LEN);

    va_list args;
    va_start(args, format);


    int32_t len = vsprintf(mes->s_payload, format, args);
    mes->s_header.s_payload_len = len;

    va_end(args);
}


