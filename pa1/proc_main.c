//
// Created by asgrim on 10.09.24.
//

#include <stdlib.h>
//#include <signal.h>
//#include <stdio.h>
#include <sys/wait.h>

#include "proc_main.h"
//#include "pa1.h"
#include "pipes.h"
#include "proc_child.h"
#include "logs.h"

pid_t parent_pid;


static pid_t *child_id_arr;


void alloc_proc_ids(const uint64_t child_num) {
    child_id_arr = malloc(sizeof(pid_t) * child_num);
}

void free_proc_ids(void) {
    free(child_id_arr);
}

//return number of successfully created child procs
int64_t create_children(const uint64_t child_num) {
    pid_t pid;
    for (int i = 0; i < child_num; ++i) {
        pid = fork();
        switch (pid) {
            case -1: {
                return i;
            }

            case 0: {
                child_loop(i + 1, child_num, pipes_matrix[i + 1]);
                return -1;
            }

            default: {
                child_id_arr[i] = pid;
            }
        }

    }

    return (int64_t) child_num;
}

//void kill_children(const uint64_t child_num) {
//    for (int i = 0; i < child_num; ++i) {
//        kill(child_id_arr[i], SIGKILL);
//    }
//}

int32_t wait_child(pid_t pid) {
    int32_t status = 0;
    waitpid(pid, &status, 0);
    return status;
}

void wait_all(uint64_t child_num) {
    for (int i = 0; i < child_num; ++i) {
        wait_child(child_id_arr[0]);
    }
}

int64_t proc_main_init(uint64_t child_num) {
    open_logfile();
    parent_pid = getpid();
    alloc_proc_ids(child_num);
    open_pipes(child_num + 1);
    int64_t fork_stat = create_children(child_num);


    //exit if child process
    if (fork_stat == -1) {
        return -1;
    }

    //kill if failed to create children
//    if (fork_stat != child_num) {
//        kill_children(fork_stat);
//    }

    return 0;
}

int64_t proc_main_exit(uint64_t child_num) {


    if (getpid() == parent_pid) {
        close_pipes_other(child_num + 1, PARENT_ID);
        wait_all(child_num);
        free_proc_ids();
        close_pipes_my(child_num + 1, PARENT_ID);
        close_logfile();
        free_pipes_mtx(child_num + 1);
    }


    return 0;
}
