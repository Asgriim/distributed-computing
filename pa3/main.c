#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>

#include "proc_main.h"

int main(int argc, char *argv[]) {
    uint8_t par_name = 'p';
    uint64_t child_num = 0;

    int64_t rez = 0;
    balance_t balances[10];
    //get number of procs
    while (rez != -1) {
        rez = getopt(argc, argv, "p:");
        if (rez == par_name) {
            child_num = strtol(optarg, NULL, 10);

            for (int i = optind; i < argc; ++i) {
                balances[i - 3] = (balance_t)strtol(argv[i], NULL, 10);
            }
        }

    }

    int64_t init_status = proc_main_init(child_num, balances);
    proc_main_loop(child_num);

    if (init_status == 0) {
        return (int)proc_main_exit(child_num);
    }

    return 0;
}
