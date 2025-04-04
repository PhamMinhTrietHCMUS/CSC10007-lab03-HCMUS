#include "kernel/types.h"
#include "kernel/syscall.h"
#include "user/user.h"

int pgaccess(uint64 *starting_ptr, int page_count, uint64 *result) // starting virtual address, number of page to check for, user address to pass the bitmnask back
{
    return syscall(SYS_pgaccess, starting_ptr, page_count, result); // , &start, &number, result
}