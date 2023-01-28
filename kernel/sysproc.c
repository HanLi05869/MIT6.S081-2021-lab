#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
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
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  
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


  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
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
sys_pgaccess(void)
{
  // lab pgtbl: your code here.
  uint64 address;
  if(argaddr(0,&address)<0)//get the base
	  return -1;
  
  int len;
  if(argint(1, &len) < 0 || len > 32)//get the length
	  return -1;

  uint64 mask_address;
  if(argaddr(2, &mask_address) < 0)//get the mask
	  return -1;

  struct proc* proc = myproc();
  uint32 mask = 0;//a buffer
  for(int i=0;i<len;++i)
  {
  	pte_t* pte = walk(proc->pagetable, address + i * PGSIZE, 0);// walk is defined in kernel/vm.c
								    // it is to find the end of the vitrual address
	if(*pte & PTE_A)
	{
		mask |= 1 << i;// to sign 1 in the location of i
		*pte &= ~PTE_A;// flash PTE_A, which means let it to be zero
	}
  }
  if(copyout(proc->pagetable, mask_address, (char*)&mask, 4) < 0) // copy out, because the maxinum of variable len is 32
	  							  // thus it is enough to restore the mask result through 32-bit variable
								  // which is a int size, 4 bytes	
	  return -1;
  return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
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
