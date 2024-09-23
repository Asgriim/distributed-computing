#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>

#include "proc_main.h"
#include "utils.h"

void transfer(void * parent_data, local_id src, local_id dst,
              balance_t amount)
{
    struct child_pipes *cp = parent_data;

    inc_lamport_time();

    TransferOrder transferOrder = {.s_src = src, .s_dst = dst, .s_amount = amount};
    Message *mes = malloc(sizeof(Message));

    set_up_message(mes, TRANSFER, (char *) &transferOrder, sizeof(TransferOrder));

    send(cp, src, mes);

    wait_responded(dst, cp, mes, ACK);


    set_lamport_time(mes->s_header.s_local_time);

    inc_lamport_time();
}


//
//int main(int argc, char * argv[])
//{

//}
