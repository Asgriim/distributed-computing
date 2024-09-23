//
// Created by asgrim on 10.09.24.
//

#ifndef DISTT_SYS_LOGS_H
#define DISTT_SYS_LOGS_H

#include <stdint.h>

#include "common.h"


int32_t open_logfile();
int32_t close_logfile();

void write_log_fmt(const char *format, ...);

#endif //DISTT_SYS_LOGS_H
