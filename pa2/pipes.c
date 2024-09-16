//
// Created by asgrim on 10.09.24.
//

#include "pipes.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

struct pipe_struct **pipes_matrix;

void alloc_pipes_mtx(uint64_t proc_num){
    pipes_matrix = malloc(proc_num * sizeof(struct pipe_struct*));
    for (int i = 0; i < proc_num; ++i) {
        pipes_matrix[i] = malloc(proc_num * sizeof(struct pipe_struct));
    }

}

void free_pipes_mtx(uint64_t proc_num) {
    for (int i = 0; i < proc_num; ++i) {
        free(pipes_matrix[i]);
    }
    free(pipes_matrix);
}

void non_blocking_fd(int32_t fd){
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int64_t open_pipes(uint64_t proc_num) {
    alloc_pipes_mtx(proc_num);

    for (int i = 0; i < proc_num; ++i) {
        for (int j = i + 1; j < proc_num; ++j) {

            if (i == j) {
                continue;
            }
            int32_t pipe1[2]; // Parent -> Child
            int32_t pipe2[2]; // Child -> Parent

            if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
                perror("pipe");
                return -1;
            }
            pipes_matrix[i][j].read_fd = pipe1[0];
            pipes_matrix[i][j].write_fd = pipe2[1];

            pipes_matrix[j][i].read_fd = pipe2[0];
            pipes_matrix[j][i].write_fd = pipe1[1];


            non_blocking_fd(pipe1[0]);
            non_blocking_fd(pipe2[0]);
            non_blocking_fd(pipe1[1]);
            non_blocking_fd(pipe2[1]);


        }

    }
    return 0;
}


int64_t close_pipes_my(uint64_t proc_num, int32_t id) {

    for (int i = 0; i < proc_num; ++i) {
        if (i == id) {
            close(pipes_matrix[id][i].read_fd);
            close(pipes_matrix[id][i].write_fd);
        }
    }
    return 0;
}

int64_t close_pipes_other(uint64_t proc_num, int32_t id) {

    for (int i = 0; i < proc_num; ++i) {
        if (id == i) {
            continue;
        }
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

