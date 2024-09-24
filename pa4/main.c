#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdbool.h>

#include "proc_main.h"

int main(int argc, char *argv[]) {
    uint8_t par_name = 'p';
    uint64_t child_num = 0;

    int64_t rez = 0;
    bool mutexl = false;

    struct option opt = {.name = "mutexl",
            .flag = 0,
            .has_arg = 0,
            .val = 0
    };
    //get number of procs
    while (rez != -1) {
        rez = getopt_long(argc, argv, "p:", &opt, NULL);
        if (rez == par_name) {
            child_num = strtol(optarg, NULL, 10);
        }
        if (rez == 0) {
            mutexl = true;
        }
    }

    int64_t init_status = proc_main_init(child_num, mutexl);

    if (init_status == 0) {
        proc_main_exit(child_num);

    }



    return 0;
}
