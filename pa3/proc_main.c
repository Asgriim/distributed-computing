//
// Created by asgrim on 10.09.24.
//

#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>

#include "proc_main.h"
#include "pipes.h"
#include "logs.h"
#include "utils.h"

static pid_t parent_pid;

static int32_t proc_num;

static pid_t *child_id_arr;

void alloc_proc_ids(const uint64_t child_num) {
    child_id_arr = malloc(sizeof(pid_t) * child_num);
}

void free_proc_ids(void) {
    free(child_id_arr);
}

//return number of successfully created child procs
int64_t create_children(const uint64_t child_num, balance_t *balances) {

    pid_t pid;
    for (int i = 0; i < child_num; ++i) {
        pid = fork();
        switch (pid) {
            case -1: {
                return i;
            }

            case 0: {
                child_loop(i + 1, child_num, pipes_matrix[i + 1], balances[i]);
                return -1;
            }

            default: {
                child_id_arr[i] = pid;
            }
        }

    }

    return (int64_t) child_num;
}


int32_t wait_child(pid_t pid) {
    int32_t status = 0;
    waitpid(pid, &status, 0);
    return status;
}

int32_t wait_all(uint64_t child_num) {
    for (int i = 0; i < child_num; ++i) {
        int32_t stat = wait_child(child_id_arr[0]);
        if (stat != 0) {
            return stat;
        }
    }
    return 0;
}

int64_t proc_main_init(uint64_t child_num, balance_t *balances) {
    open_logfile();
    proc_num = child_num + 1;
    parent_pid = getpid();
    alloc_proc_ids(child_num);
    open_pipes(child_num + 1);
    int64_t fork_stat = create_children(child_num, balances);


    //exit if child process
    if (fork_stat == -1) {
        return -1;
    }


    return 0;
}

int64_t proc_main_exit(uint64_t child_num) {


    if (getpid() == parent_pid) {
        close_pipes_other(proc_num, PARENT_ID);
        int32_t status = wait_all(child_num);
        if (status != 0) {
            fprintf(stderr,
                    "child proc %d (pid %d) returned %d status\n",
                    (int32_t)child_num,
                    child_id_arr[proc_num],
                    status
                    );
            return -1;
        }
        free_proc_ids();
        close_pipes_my(proc_num, PARENT_ID);
        close_logfile();
        free_pipes_mtx(proc_num);
    }


    return 0;
}

void wait_all_history_balance(struct child_pipes *cp, Message *message, AllHistory *allHistory) {
    for (int i = 1; i < proc_num; ++i) {
        while (receive(cp, i, message) != 0 || message->s_header.s_type != BALANCE_HISTORY) {

        }
        memcpy(&allHistory->s_history[cp->received_from -1], message->s_payload, message->s_header.s_payload_len);
    }
}


int64_t proc_main_loop(uint64_t child_num) {
    close_pipes_main(proc_num);

    struct child_pipes cp = {
            .owner_id = PARENT_ID,
            .proc_num = proc_num,
            .connected_pipes = pipes_matrix[PARENT_ID],
            .pid = parent_pid
    };

    AllHistory allHistory = {.s_history_len = child_num};

    Message *mes = malloc(sizeof(Message));

    wait_all_responded(PARENT_ID, &cp, mes, STARTED);

    inc_lamport_time();

    bank_robbery(&cp, child_num);

    inc_lamport_time();
    set_up_message(mes, STOP, NULL, 0);

    send_multicast(&cp, mes);

    wait_all_history_balance(&cp, mes, &allHistory);

    wait_all(child_num);
    print_history(&allHistory);

    return 0;
}

int64_t close_pipes_main(uint64_t proc_num) {

    for (int i = 1; i < proc_num; ++i) {
        for (int j = 0; j < proc_num; ++j) {
            if (i == j) {
                continue;
            }
            close(pipes_matrix[i][j].read_fd);
            close(pipes_matrix[i][j].write_fd);
        }
    }
    return 0;
}
