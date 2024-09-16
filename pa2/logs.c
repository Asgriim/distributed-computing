//
// Created by asgrim on 10.09.24.
//

#include <stdio.h>
#include "logs.h"
#include <stdarg.h>

FILE *logfile;

int32_t open_logfile() {

    logfile = fopen(events_log, "w");
    return 0;
}

int32_t close_logfile() {
    fclose(logfile);
    return 0;
}




void write_log_fmt(const char *format, ...) {
    va_list args;
    va_list copy;

    va_start(args, format);
    va_copy(copy,args);

    vprintf(format, args);
    vfprintf(logfile, format, copy);

    va_end(args);
}
