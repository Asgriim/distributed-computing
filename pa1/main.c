#include <stdio.h>
#include <stdint-gcc.h>
#include <getopt.h>
#include <stdlib.h>

#include "proc_main.h"
#include "pipes.h"
int main(int argc, char *argv[]) {
    uint8_t par_name = 'p';
    uint64_t child_num = 0;
    if (argc != 3) {
        return -1;
    }

    int64_t rez = 0;


    //get number of procs
    while (rez != -1) {
        rez = getopt(argc, argv, "p:");
        if (rez == par_name) {
            child_num = strtol(optarg, NULL, 10);
            break;
        }
    }

    int64_t init_status = proc_main_init(child_num);

    proc_main_exit(child_num);

    return 0;
}
