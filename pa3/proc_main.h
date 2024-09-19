//
// Created by asgrim on 10.09.24.
//

#ifndef DISTT_SYS_PROC_MAIN_H
#define DISTT_SYS_PROC_MAIN_H

#include <stdint.h>
#include <unistd.h>

#include "proc_child.h"

int64_t proc_main_init(uint64_t child_num, balance_t *balances);

int64_t proc_main_loop(uint64_t child_num);

int64_t proc_main_exit(uint64_t child_num);

int64_t close_pipes_main(uint64_t proc_num);
#endif //DISTT_SYS_PROC_MAIN_H
