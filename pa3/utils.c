//
// Created by asgrim on 19.09.24.
//

#include "utils.h"
timestamp_t lamport_time = 0;

timestamp_t get_lamport_time() {
    return lamport_time;
}

void set_lamport_time(timestamp_t timestamp) {
    if (lamport_time < timestamp) {
        lamport_time = timestamp;
    }
}

void inc_lamport_time() {
    ++lamport_time;
}
