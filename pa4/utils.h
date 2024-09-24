//
// Created by asgrim on 19.09.24.
//

#ifndef DISTT_SYS_UTILS_H
#define DISTT_SYS_UTILS_H


#include <stdbool.h>
#include "banking.h"

void set_lamport_time(timestamp_t timestamp);

void inc_lamport_time();

uint16_t create_bitmask(int32_t id, int32_t child_num);

void lower_bitmask(uint16_t *bitmask, int32_t received_from);

void init_lock_q();

void lock_q_add_req(int32_t id, timestamp_t timestamp);

void lock_q_release(int32_t id);

bool lock_q_is_ready(int32_t id, timestamp_t timestamp);
#endif //DISTT_SYS_UTILS_H
