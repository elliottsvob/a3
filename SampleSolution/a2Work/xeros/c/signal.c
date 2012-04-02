/*signal.c
*/

#include <kernel.h>

int signal( int pid, int sig_no) {
/***********************************************/

	//TODO make this method how it should be...
	
	int num = get_priority_signal(p);
	if(is_valid_signal(num) && num>(p->current_signal) ){
	    p->current_signal = num;
	    //sigtramp(p->signal_handlers[num],(void *)((p->esp)-sizeof(context_frame)), (void *)(p->esp));
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
	    return 1;
	}
	return 0;
}

extern void sigtramp(void(*handler)(void*),void*cntx,void*osp)
{
	handler(cntx);
	sigreturn(osp);	
}
