/* kernel.h - disable, enable, halt, restore, isodd, min, max */


#ifndef KERNEL_H_INCLUDED
#define KERNEL_H_INCLUDED

int sprintf(char *, char *, ...);
void * memset(void *pch, int c, int len);


/* Symbolic constants used throughout Xinu */

typedef	char		Bool;		/* Boolean type			*/
#define	FALSE		0		/* Boolean constants		*/
#define	TRUE		1
#define	EMPTY		(-1)		/* an illegal gpq		*/
#define	NULL		0		/* Null pointer for linked lists*/
#define	NULLCH		'\0'		/* The null character		*/


/* Universal return constants */

#define	OK		 1		/* system call ok		*/
#define	SYSERR		-1		/* system call failed		*/
#define	EOF		-2		/* End-of-file (usu. from read)	*/
#define	TIMEOUT		-3		/* time out  (usu. recvtim)	*/
#define	INTRMSG		-4		/* keyboard "intr" key pressed	*/
					/*  (usu. defined as ^B)	*/
#define	BLOCKERR	-5		/* non-blocking op would block	*/

extern void kmeminit(void);
extern void *kmalloc(int size);
extern void kfree(void *ptr);

#define MAX_PROC        64
#define KERNEL_INT      80
#define TIMER_INT       (TIMER_IRQ + 32)
#define PROC_STACK      (4096 * 4)

#define STATE_STOPPED   0
#define STATE_READY     1
#define STATE_SLEEP     2
#define STATE_SIGWAIT	3

#define SYS_STOP        0
#define SYS_YIELD       1
#define SYS_CREATE      2
#define SYS_TIMER       3
#define SYS_GETPID      8
#define SYS_PUTS        9
#define SYS_SLEEP       10
#define SYS_HANDLER     11
#define SYS_RET	        12
#define SYS_KILL	13
#define SYS_WAIT	14

#define SIG_MIN		0
#define SIG_MAX		31
#define PID_NOTEXIST 	-15;


typedef void    (*funcptr)(void);

typedef struct struct_pcb pcb;
struct struct_pcb {
  long        esp;
  pcb         *next;
  pcb         *prev;
  int         state;
  int         pid;
  int         otherpid;
  void *      buffer;
  int         bufferlen;
  int         ret;
  int         sleepdiff;
  long        args;
  unsigned int signal_handlers[32];
  int 		signal;
  int		current_signal;
};

extern pcb   *findPCB( int pid);

#pragma pack(1)

typedef struct context_frame {
  unsigned int        edi;
  unsigned int        esi;
  unsigned int        ebp;
  unsigned int        esp;
  unsigned int        ebx;
  unsigned int        edx;
  unsigned int        ecx;
  unsigned int        eax;
  unsigned int        iret_eip;
  unsigned int        iret_cs;
  unsigned int        eflags;
  unsigned int        stackSlots[0];
} context_frame;

extern pcb      proctab[MAX_PROC];

extern void     kmeminit( void );
extern void     *kmalloc( int size );
extern void     dispatch( void );
extern void     dispatchinit( void );
extern void     ready( pcb *p );
extern pcb      *next( void );
extern void     contextinit( void );
extern int      contextswitch( pcb *p );
extern int      create( funcptr fp, int stack );
//signal 
extern int 	signal(int pid, int sig_no);

extern void     root( void );

void printCF (void * stack);

int syscall(int call, ...);
int syscreate(void (*func)(), int stack);
int sysyield(void);
int sysstop(void);



#endif
