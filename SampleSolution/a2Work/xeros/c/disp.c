/* disp.c : dispatcher
 */


#include <i386.h>
#include <stdarg.h>
#include <kernel.h>


static pcb      *head = NULL;
static pcb      *tail = NULL;

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

    for( p = next(); p; ) {
    //kprintf("Process %x selected stck %x\n", p, p->esp);
      
    //Checks whether the process has signals and sets up the context frame if its does
    	if(p->signal){
		//TODO fix method call
		//signal (p);
		int retu = signal(p->pid, get_priority_signal(p));
    	}

	int i;
	r = contextswitch( p );
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
	if(!is_valid_signal(sig) || !is_valid_handler(&handler)){
		p->ret =-1;	
	}
	else 
	{
		p->signal_handlers[sig] = (unsigned int) &handler;
		p->ret =0;
	}
	p = next();	
	break;
      case (SYS_RET):
	ap = (va_list)p->args;
	p->esp = va_arg(ap, long);
	p = next();
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
	}			
	break;

      case (SYS_WAIT):
	//default return value is SYS_ERR (-1)
	// when signal is received it is changed to 1
	p->ret =SYSERR;	
	//TODO once the signal is delivered the return value shoul be changed to 1
	p->state = STATE_SIGWAIT;
	p = next();
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
        kprintf( "BAD\n" );
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
	int a;
	int sig = p->signal;
	int i;
	for (i = SIG_MAX; i >=SIG_MIN; i--){
		a>>i;
		if(a){
			return i;
		}
		a = sig;
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
	//Checks for a defined signal handler, ignored otherwise
	if (sig_p->signal_handlers[sig_no]){
		//int sig_set = (1<<sig);		
		int sig_set = (1<<sig_no);
		if(!(sig_p->signal & sig_set)){
	 		sig_p->signal+= sig_set;
		}
	}			
	return 0;
}
