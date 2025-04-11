#include "kernel/types.h"
#include "user/user.h"

void print_as_bin(uint64 btms)
{
    uint64 firstmost = 0x8000000000000000;
    for(int i = 0; i < 64; i++)
    {
        if(btms & firstmost)
            printf("1");
        else
            printf("0");
        btms <<= 1;
    }
    printf("\n");
}

int main(int argc, char** argv)
{
    uint64 DUMMY = 123456; // this is the dummy thing to start from

    
    if(argc != 4)
    {
        printf("Usage: pgatest <pages_to_check> <number_of_integers_to_spawn> <\"heap\"/\"stack\">\n");
        exit(-1);
    }
    
    int spawn_int_count = atoi(argv[2]);
    int pages_num = atoi(argv[1]);
    if(spawn_int_count <= 0 || pages_num <= 0 || (strcmp(argv[3], "heap") && strcmp(argv[3], "stack"))){
        printf("invalid argument\n");
        exit(-1);
    }
    
    
    int* i = malloc(spawn_int_count * sizeof(int));
    for(int it = 0; it < spawn_int_count; it += 1)
        i[it] = 0;
    uint64 res;
    if(strcmp(argv[3], "stack") == 0)
    {    
        if(pgaccess(&DUMMY, pages_num, &res) == 0)// DUMMY should be in the stack
            print_as_bin(res);
        else
            printf("somethings wrong\n");
    }
    else if(strcmp(argv[3], "heap") == 0)
    {
        if(pgaccess((uint64 *)i, pages_num, &res) == 0)// since i is a malloc'd array, it should be in heap
            print_as_bin(res);
        else
            printf("somethings wrong\n");
    }

    free(i);
    return 0;
}