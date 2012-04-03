/*signal.c
*/

#include <kernel.h>

#define STARTING_EFLAGS         0x00003000
#define ARM_INTERRUPTS          0x00000200

typedef struct sigtramp_frame {
  context_frame	cf;
  unsigned int	ret_addrs;
  unsigned int	handler;
  unsigned int 	cntx_frame;
  unsigned int	old_esp;
} sigtramp_frame;

extern void sigtramp(void(*handler)(void*),void*cntx,void*osp)
{
	handler(osp);	
	sigreturn(osp);	
}

int signal( int pid, int sig_no) {
/***********************************************/

	pcb *p = findPCB(pid);
	int num = get_priority_signal(p);
	
	
	
	if(is_valid_signal(num) && num>(p->current_signal) ){
	    //kprintf("Signal is %d\n", num);
	    
	    p->current_signal = 1<<num;	   
	    //sigtramp(p->signal_handlers[num],(void *)((p->esp)-sizeof(context_frame)), (void *)(p->esp));
	    sigtramp_frame *fr = (sigtramp_frame  *) (p->esp - sizeof(sigtramp_frame));
	    //sets everything to 0
	    memset(fr, 0x0, sizeof( sigtramp_frame ));
	    //instruction pointer in the blue frame
	    fr->cf.iret_eip = (int) &sigtramp;
	    fr->cf.iret_cs = getCS();
	    fr->cf.eflags = STARTING_EFLAGS | ARM_INTERRUPTS;
	    //yellow frame slide 286	    
	    fr->ret_addrs = (unsigned int) &sigtramp;
	   
	    fr->handler = p->signal_handlers[num];
	    print_handlers(p);
	    fr->cntx_frame = p->esp;
	    fr->old_esp = p->esp;
	    //change stack pointer to point at blue frame	
	    p->esp = (unsigned int) fr;  
	    //kprintf("sigtramp context set up\n");
	  
	    
	    return 1;
	}
	return 0;
}

