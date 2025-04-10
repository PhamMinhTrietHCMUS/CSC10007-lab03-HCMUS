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
    if(argc != 3)
    {
        printf("Usage: pgatest <pages_to_check> <number_of_integers_to_spawn>\n" );
        exit(0);
    }
    
    int spawn_int_count = atoi(argv[2]);
    int pages_num = atoi(argv[1]);
    if(spawn_int_count <= 0 || pages_num <= 0){
        printf("invalid argument (cannot be negative)\n");
    }
    
    uint64 DUMMY = 123456; // this is the dummy thing to start from

    int* i = malloc(spawn_int_count * sizeof(int));
    for(int it = 0; it < spawn_int_count; it += 1)
        i[it] = 0;
    uint64 res;
    if(pgaccess(&DUMMY, pages_num, &res) == 0) {
        //printf("%p\n", (uint64*)res);
        printf("%ld\n", res);
        print_as_bin(res);
    }
    else
        printf("somethings wrong\n");
    free(i);
    return 0;
}