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
    }
}

void update_history(BalanceHistory  *balanceHistory, BalanceState *balance_state) {
    if (balanceHistory->s_history_len > 0) {

        for (int i = balanceHistory->s_history_len; i <= get_physical_time(); ++i) {
            balanceHistory->s_history[i] = *balance_state;
        }


    } else {
        balanceHistory->s_history[0] = *balance_state;
    }

    balanceHistory->s_history_len = balance_state->s_time + 1;

}

void handle_transfer(int32_t id, struct child_pipes *cp, Message *mes, BalanceState *balance_state) {
    TransferOrder order;
    memcpy(&order, mes->s_payload, mes->s_header.s_payload_len);
    timestamp_t  timestamp = get_physical_time();
    balance_state->s_time = timestamp;
    if (order.s_src == id) {

        write_log_fmt(
                log_transfer_out_fmt,
                get_physical_time(),
                id,
                order.s_amount,
                order.s_dst
                );

        balance_state->s_balance -= order.s_amount;
//        printf("proc %d, time = %d, bal = %d\n", id, timestamp, balance_state->s_balance);
        send(cp, order.s_dst, mes);
        return;
    }

    if (order.s_dst == id) {

        write_log_fmt(
                log_transfer_in_fmt,
                get_physical_time(),
                id,
                order.s_amount,
                order.s_dst
        );

        balance_state->s_balance += order.s_amount;
        set_up_message(mes, ACK, NULL, 0);
        send(cp, PARENT_ID, mes);
        return;
    }

    printf("ERROR IN TRANSFER HANDLE\n");
}

uint16_t create_bitmask(int32_t id, int32_t child_num) {
    return (~(1 << (id - 1))) & (0xFFFF >> (16 - child_num));
}

void lower_bitmask(uint16_t *bitmask, int32_t received_from) {
    *bitmask &= ~(1 << (received_from - 1));
}


void handle_message(int32_t id, Message *mes, struct child_pipes *cp, BalanceState *balance_state, BalanceHistory *balance_history, bool *stop, uint16_t *done_bitmask) {
    switch (mes->s_header.s_type) {
        case TRANSFER: {
            handle_transfer(id, cp, mes, balance_state);
            update_history(balance_history, balance_state);
            break;
        }

        case STOP: {
            //todo implement 3rd fase when not all transfers readed
            write_log_fmt(log_done_fmt,
                          get_physical_time(),
                          id,
                          balance_state->s_balance
            );
            set_up_message(mes, DONE, NULL, 0);

            if (send_multicast(cp, mes) == -1 ) {
                exit(-1);
            }
            *stop = true;
            break;
        }

        case DONE: {
            lower_bitmask(done_bitmask, cp->received_from);
            break;
        }
    }
}

void child_listen(int32_t id, int32_t child_num, Message *mes, struct child_pipes *cp, BalanceState *balance_state, BalanceHistory *balance_history) {
    uint16_t done_bitmask = create_bitmask(id, child_num);
    bool received_stop = false;
    while (!received_stop || done_bitmask != 0) {
        //handling incoming messages
        if (receive_any(cp, mes) == 0) {
            handle_message(
                    id,
                    mes,
                    cp,
                    balance_state,
                    balance_history,
                    &received_stop,
                    &done_bitmask
            );
        }
    }
}

int64_t child_loop(int32_t id, int32_t child_num, struct pipe_struct connected_pipes[], balance_t start_balance) {
    proc_num = child_num + 1;
    pid_t parent_pid = getppid();
    pid_t pid = getpid();
    close_pipes_other(proc_num, id);

    BalanceHistory balance_history = {.s_id = id, .s_history_len = 0};
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

    update_history(&balance_history, &balance_state);

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

    child_listen(id, child_num, mes, &cp, &balance_state, &balance_history);

    write_log_fmt(
                  log_received_all_done_fmt,
                  get_physical_time(),
                  id
                  );

    update_history(&balance_history, &balance_state);

    set_up_message(mes, BALANCE_HISTORY, (char *) &balance_history,
                   sizeof(BalanceHistory)
    );

    send(&cp, PARENT_ID, mes);
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




