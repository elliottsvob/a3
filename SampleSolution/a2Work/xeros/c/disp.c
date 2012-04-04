/* disp.c : dispatcher
*/


#include <i386.h>
#include <stdarg.h>
#include <kernel.h>


static pcb      *head = NULL;
static pcb      *tail = NULL;

char kernel_buffer[4];
int cursor = 0;
extern int end();
//extern int entry();
//extern void 	sigtramp(void(*handler)(void*),void*cntx,void*osp);
//extern int 	signal(int pid, int sig_no);

void     dispatch( void ) {
	/********************************/

	pcb         *p;
	int         r;
	funcptr     fp;
	int         stack;
	va_list     ap;
	char       *str;
	int         len;
	int 	sig;
	void* 	handler;
	int		pid;
	int		sig_set;
	int 	rc;
	int 	fd;
	void*   buff;
	int 	bufflen;
	unsigned long command;
	va_list  	ioctl_list;
	int		device_no;

	for( p = next(); p; ) {
		//kprintf("Process %x selected stck %x, signal %x\n", p, p->esp,p->signal);
		
		//Checks whether the process has signals and sets up the context frame if its does
/*		if(p->pid ==4){*/
/*			//kprintf("PROCESSING PID 4\n");*/
/*		}*/
		if(p->signal){
			//TODO fix method call
			//signal (p);
			//kprintf("I've been signaled\n");			 
			int retu = signal(p->pid, get_priority_signal(p));
		//	kprintf("context frame set up set up\n");
		}
			enable_irq(1,0);
		
		int i,c;
		r = contextswitch( p );
		//kprintf("-------------------------------\n");
		switch( r ) {
		case( SYS_CREATE ):
			ap = (va_list)p->args;
			fp = (funcptr)(va_arg( ap, int ) );
			stack = va_arg( ap, int );
			p->ret = create( fp, stack );
			break;
		case( SYS_YIELD ):
			ready( p );
			p = next();
			break;
		case( SYS_STOP ):
			p->state = STATE_STOPPED;
			p = next();
			break;
		case( SYS_PUTS ):
			ap = (va_list)p->args;
			str = va_arg( ap, char * );
			kprintf( "%s", str );
			p->ret = 0;
			break;
		case( SYS_GETPID ):
			p->ret = p->pid;
			break;
		case( SYS_SLEEP ):
			ap = (va_list)p->args;
			len = va_arg( ap, int );
			sleep( p, len );
			p = next();
			break;
		case( SYS_TIMER ):
			tick();
			ready( p );
			p = next();
			end_of_intr();
			break;
		case (SYS_HANDLER):
			ap = (va_list)p->args;
			sig = va_arg(ap, int);
			handler = va_arg(ap, void*);
			
				
			if(!is_valid_signal(sig) /*|| !is_valid_handler(&handler)*/){
				p->ret =-1;	
			}
			else 
			{
				p->signal_handlers[sig] = handler;
				print_handlers(p);				
				p->ret =0;
			}
			//p = next();	
			break;
		case (SYS_RET):
			
			ap = (va_list)p->args;
			p->esp = va_arg(ap, long);
			p->signal -= p->current_signal;
			p->current_signal = -1;
			
			
	    //ready(p);	  
			//p = next();
			break;
		case (SYS_KILL):
			ap = (va_list)p->args;
			pid = va_arg(ap, int);
			sig = va_arg(ap, int);
			 
			//The signal method is supposed to do something different.
			//what was in the signal method is now in a new process	
			rc = signal_process(pid,sig);			
			
			if ( rc == -1){
				p->ret = PID_NOTEXIST;
				
			}
			else if ( rc == -2){
				p->ret = SYSERR;
				
			}else{
				p->ret = rc; 
				//kprintf("signal sent\n");
			}
			 
			//p=next();			
			break;

		case (SYS_WAIT):
			//default return value is SYS_ERR (-1)
			//when signal is received it is changed to 1
			p->ret =SYSERR;			
			p->state = STATE_SIGWAIT;
			p = next();
			break;
		case (SYS_OPEN):	
			
			ap = (va_list)p->args;
			device_no = va_arg(ap, int);
			p->ret = di_open(device_no,p);
			kprintf("The file descriptor returned is %d\n", p->ret);
			break;
		case (SYS_CLOSE):
			ap = (va_list)p->args;
			fd = va_arg(ap,int);
			p->ret=di_close(fd,p);
		
			break;
		case (SYS_WRITE):
			ap = (va_list)p->args;
			fd = va_arg(ap, int);
			buff = va_arg(ap, void*);
			bufflen = va_arg(ap, int);	
			p->ret=di_write(fd,buff,bufflen,p);		
			
			break;
		case (SYS_READ):
			ap = (va_list)p->args;
			fd = va_arg(ap, int);
			buff = va_arg(ap, void*);
			bufflen = va_arg(ap, int);
			p->ret=di_read(fd,buff,bufflen,p);
			
			break;
		case (SYS_IOCTL):	
			ap = (va_list)p->args;
			fd = va_arg(ap, int);
			command = va_arg(ap, unsigned long );
			ioctl_list = va_arg(ap, va_list);
			p->ret=di_ioctl(fd,command,p,ioctl_list);
			
			break;
		
		case (SYS_KEY):
			
			if(p->pid !=1){ready(p);};
			
				while(inb(0x64)%2 == 1){
					unsigned char a = inb(0x60);
					a = kbtoa(a);
		
					if(a == 4)
					{
						kernel_buffer_add(4);
					}else if( a>=0 && a <=127){
						//echo keyboard device
						if (get_kb()){
							kputc("%c", a);
							}
						kernel_buffer_add(a);	
					 }
					}				
				end_of_intr();
			
			break;
			
		default:
			kprintf( "Bad Sys request %d, pid = %d\n", r, p->pid );
		}
		
		
	}
	
	kprintf( "Out of processes: dying\n" );
	
	for( ;; );
}

extern void dispatchinit( void ) {
	/********************************/

	//bzero( proctab, sizeof( pcb ) * MAX_PROC );
	memset(proctab, 0, sizeof( pcb ) * MAX_PROC);
}

extern void     ready( pcb *p ) {
	/*******************************/

	p->next = NULL;
	p->state = STATE_READY;

	if( tail ) {
		tail->next = p;
	} else {
		head = p;
	}

	tail = p;
}

extern pcb      *next( void ) {
	/*****************************/

	pcb *p;
 
	p = head;

	if( p ) {
		head = p->next;
		if( !head ) {
			tail = NULL;
		}
	} else {
		kprintf( "NO next process\n" );
		for(;;);
	}

	p->next = NULL;
	p->prev = NULL;
	return( p );
}


extern pcb *findPCB( int pid ) {
	/******************************/

	int	i;

	for( i = 0; i < MAX_PROC; i++ ) {
		if( proctab[i].pid == pid ) {
			return( &proctab[i] );
		}
	}

	return( NULL );
}

extern int is_valid_handler(unsigned int address)
{
	if (address > (int)&end){
		kprintf( "Disp.c: invalid handler\n" );
		return -1;
	}
	return 1;
}

extern int is_valid_signal(int sig)
{
	if(sig < SIG_MIN || sig > SIG_MAX){
		return -1;
	}
	return 1;
}

extern int get_priority_signal(pcb* p)
{
	//Helper method.  gets the highest priority signal of the process
	
	int sig = p->signal;
	int i;
	for (i = SIG_MAX; i >=SIG_MIN; i--){		
		if(sig>>i){
			return i;
		}		
	}
	return -1;
}

extern int signal_process(int pid, int sig_no)
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
	kprintf("State is %d\n", sig_p->state);
	 if(sig_p->state == STATE_SIGWAIT){		
	 			sig_p->ret=1;
	    	ready(sig_p);
	    }

	//Checks for a defined signal handler, ignored otherwise
	int i = sig_p->signal_handlers[sig_no];
	//kprintf("Signal handler address is %x\n", i);
	if (sig_p->signal_handlers[sig_no]){
		//kprintf("PID TO signal is: %d, Signal number is: %d\n", pid, sig_no);
		//int sig_set = (1<<sig);		
		int sig_set = (1<<sig_no);
		if(!(sig_p->signal & sig_set)){
			//kprintf("p->pid is: %d\n",sig_p->pid );
			sig_p->signal+= sig_set;
			//kprintf("p=>signal is: %d\n",sig_p->signal );
		}
	}			
	return 0;
}

 void print_handlers(pcb * p){
	int i;
	int address;
	for(i = 0; i < 10; i ++){
		address = p->signal_handlers[i];
//TODO Commneted		kprintf("Signal handler: %d, is address:%x\n", i, address);
	}
}


void kernel_buffer_add(char c)
{
	if(cursor<3){
		kernel_buffer[cursor] = c;
		cursor++;
		}
	
}
void reset_cursor()
{
	cursor =0;
}
void kernel_buffer_clear()
{
	int i = 0;
	for(;i<4;i++)
	{
		kernel_buffer[i]=NULL;
	}
}




