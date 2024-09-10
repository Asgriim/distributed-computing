//
// Created by asgrim on 10.09.24.
//

#ifndef DISTT_SYS_LOGS_H
#define DISTT_SYS_LOGS_H

#include <stdint.h>

#include "common.h"
#include "pa1.h"

static const char * const pipe_log_tmp = "open pipe between %d pid and %d pid";

int32_t open_logfile();
int32_t close_logfile();

void write_log_started(int64_t process,int64_t pid, int64_t parent_pid);

void write_log_received_all_started (
        int64_t process
        );

void write_log_done(int64_t process);

void write_log_received_all_done(int64_t process);

#endif //DISTT_SYS_LOGS_H
