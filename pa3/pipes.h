//
// Created by asgrim on 10.09.24.
//

#ifndef DISTT_SYS_PIPES_H
#define DISTT_SYS_PIPES_H
#include <stdint.h>


struct pipe_struct {
    int32_t read_fd;
    int32_t write_fd;
};

extern struct pipe_struct **pipes_matrix;

void alloc_pipes_mtx(uint64_t proc_num);

void free_pipes_mtx(uint64_t proc_num);

int64_t open_pipes(uint64_t proc_num);

int64_t close_pipes_my(uint64_t proc_num, int32_t id);

int64_t close_pipes_other(uint64_t proc_num, int32_t id);
#endif //DISTT_SYS_PIPES_H
