/*signal.c
*/

#include < kernel.h>

void sigtramp(void(*handler)(void*),void*cntx,void*osp)
{
	handler(cntx);
	sigreturn(osp);	
}

int signal(int pid, int sig_no))
{
	pcb * sig_p;
	if(!is_valid_signal(sig_no)){	
		return -2;	
	}

	//find target process and check if it exists
	sig_p = findPCB(pid);
	if(!sig_p){
		return -1;
		}
	//Checks for a defined signal handler, ignored otherwise
	if (sig_p->signal_handlers[sig_no]){
		int sig_set = (1<<sig);
		if(!(sig_p->signal & sig_set)){
	 		sig_p->signal+= sig_set;
		}
	}			
	return 0;
}
