#include "banking.h"
#include "pipes.h"
#include "proc_child.h"

void transfer(void * parent_data, local_id src, local_id dst,
              balance_t amount)
{
    struct child_pipes *cp = parent_data;

    TransferOrder transferOrder = {.s_src = src, .s_dst = dst, .s_amount = amount};
    Message *mes = malloc(sizeof(Message));

    set_up_message(mes, TRANSFER, (char *) &transferOrder, sizeof(TransferOrder));

    send(cp, src, mes);

    wait_responded(dst, cp, mes, ACK);

}

//int main(int argc, char * argv[])
//{
//    //bank_robbery(parent_data);
//    //print_history(all);
//
//    return 0;
//}
