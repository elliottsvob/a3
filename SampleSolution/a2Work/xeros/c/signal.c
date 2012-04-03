/*signal.c
*/

#include <kernel.h>

typedef struct sigtramp_frame {
  context_frame	cf;
  unsigned int	ret_addrs;
  unsigned int	handler;
  unsigned int 	cntx_frame;
  unsigned int	old_esp;
} sigtramp_frame;

extern void sigtramp(void(*handler)(void*),void*cntx,void*osp)
{
	handler(cntx);
	sigreturn(osp);	
}

int signal( int pid, int sig_no) {
/***********************************************/

	pcb *p = findPCB(pid);
	int num = get_priority_signal(p);
	if(is_valid_signal(num) && num>(p->current_signal) ){
	    p->current_signal = num;
	    //sigtramp(p->signal_handlers[num],(void *)((p->esp)-sizeof(context_frame)), (void *)(p->esp));
	    sigtramp_frame *fr = (sigtramp_frame  *) (p->esp - sizeof(sigtramp_frame));
	    //sets everything to 0
	    memset(fr, 0, sizeof( sigtramp_frame ));
	    //instruction pointer in the blue frame
	    fr->cf.iret_eip = (int) &sigtramp;
	    //yellow frame slide 286	    
	    fr->ret_addrs = (unsigned int) &sigtramp;
	    fr->handler = p->signal_handlers[num];
	    fr->cntx_frame = p->esp;
	    fr->old_esp = p->esp;
	    //change stack pointer to point at blue frame	
	    p->esp = (unsigned int) fr;  
	    return 1;
	}
	return 0;
}

		/*
	    int *ptr = (int *)p->esp;
	    //Old_sp (a3)
	    ptr--;
	    *ptr=p->esp;
	    //Context Frame (a2)
	    ptr--;
	    //TODO create context frame
	    //Handler(a1)
	    ptr--;
	    *ptr= p->signal_handlers[num];
	    //Return address (never used)
	    ptr--;

	    *ptr = (int) &sigtramp;
	    //Blue context frame from slide 286
	    ptr -= sizeof(context_frame);
	    //TODO check if this context frame is right	

	    context_frame * cf = (context_frame *) ptr;
	    cf->iret_eip = (int) &sigtramp;
	    //change the stack pointer to point at the top of blue frame
	    p->esp = (int)ptr;  
		*/
