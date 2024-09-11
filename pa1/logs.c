//
// Created by asgrim on 10.09.24.
//

#include <stdio.h>
#include "logs.h"

FILE *logfile;

int32_t open_logfile() {

    logfile = fopen(events_log, "a");
    return 0;
}

int32_t close_logfile() {
    fclose(logfile);
    return 0;
}

void write_log_started(int32_t process, int32_t pid, int32_t parent_pid) {
    printf(log_started_fmt, process, pid, parent_pid);
    fprintf(logfile, log_started_fmt, process, pid, parent_pid);
}

void write_log_received_all_started(int32_t process) {
    printf(log_received_all_started_fmt, process);
    fprintf(logfile, log_received_all_started_fmt, process);
}

void write_log_done(int32_t process) {
    printf(log_done_fmt, process);
    fprintf(logfile, log_done_fmt, process);
}

void write_log_received_all_done(int32_t process) {
    printf(log_received_all_done_fmt, process);
    fprintf(logfile, log_received_all_done_fmt, process);
}
