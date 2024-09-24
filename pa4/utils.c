//
// Created by asgrim on 19.09.24.
//

#include "utils.h"

int32_t lock_q[MAX_PROCESS_ID];

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

uint16_t create_bitmask(int32_t id, int32_t child_num) {
    return (~(1 << (id - 1))) & (0xFFFF >> (16 - child_num));
}

void lower_bitmask(uint16_t *bitmask, int32_t received_from) {
    *bitmask &= ~(1 << (received_from - 1));
}

void init_lock_q() {
    for (int i = 0; i < MAX_PROCESS_ID; ++i) {
        lock_q[i] = 255;
    }
}

void lock_q_add_req(int32_t id, timestamp_t timestamp) {
    lock_q[id] = timestamp;
}

void lock_q_release(int32_t id) {
    lock_q[id] = 255;
}

bool lock_q_is_ready(int32_t id, timestamp_t timestamp) {

    for (int i = 0; i < MAX_PROCESS_ID; ++i) {
        if (id == i) {
            continue;
        }

        if (lock_q[i] < timestamp) {
            return false;
        }

        if (lock_q[i] == timestamp && i < id) {
            return false;
        }

    }
    return true;

}
