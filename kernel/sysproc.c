#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "syscall.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


#ifdef LAB_PGTBL
int
sys_pgpte(void)
{
  uint64 va;
  struct proc *p;  

  p = myproc();
  argaddr(0, &va);
  pte_t *pte = pgpte(p->pagetable, va);
  if(pte != 0) {
      return (uint64) *pte;
  }
  return 0;
}
#endif

#ifdef LAB_PGTBL
int
sys_kpgtbl(void)
{
  struct proc *p;  

  p = myproc();
  vmprint(p->pagetable);
  return 0;
}
#endif


uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}


// pgaccess is here

uint64 sys_pgaccess(void) // 1arg: starting VA, 2arg: number of pages, 3arg: userAddr
{
  uint64 starting_address;  
  int num_page;
  uint64 userAddress;

  argaddr(0, &starting_address);
  argint(1, &num_page);
  argaddr(2, &userAddress);

  // printf("arg0: %p\n", (void*)arg0);
  // printf("arg1: %p\n", (void*)arg1);
  // printf("arg2: %p\n", (void*)arg2);

  // printf("number of page: %d\n", num_page);
  // printf("VA: %ld\n", starting_address);  
  // printf("UA: %ld\n", userAddress);

  uint64 result = 0x0;
  struct proc *p = myproc();
  for(int i = 0; i < num_page && i < 64; i++) // capped at 64 because uint64 is only 64 bit, as such a bitmask of uint64 can only keep track of 64 pages
  {
      // printf("%d\n", i);
      if(starting_address >= p->sz) // if the pointer to walk from is outside the range of the program's highest address space then get out
        break;
      
      pte_t* tmp = walk(p->pagetable, starting_address, 0); // walk

      if(!tmp)
        break; // get out if theres no more PTE to see
      else if(*tmp & PTE_A)
      {
        // printf("on page %d\n", i);
        result |= (1L << i); // bitwise OR to add the page to the bitmask
        *tmp &= ~PTE_A; // reset the access bit
      }
      
      starting_address += 0x1000; // advance this one page ahead
  }

  if(copyout(p->pagetable, userAddress, (char*)&result, sizeof(result)) < 0)
    return -1;
  // use copyout
  return 0;
}

