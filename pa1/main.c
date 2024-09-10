#include <stdio.h>
#include <stdint-gcc.h>
#include <getopt.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    uint8_t par_name = 'p';
    uint32_t process_num = 0;
    if (argc != 3) {
        return -1;
    }

    int32_t rez = 0;


    //get number of procs
    while (rez != -1) {
        rez = getopt(argc, argv, "p:");
        if (rez == par_name) {
            process_num = strtol(optarg, NULL, 10);
            break;
        }
    }

    printf("pr num %d", process_num);

    return 0;
}
