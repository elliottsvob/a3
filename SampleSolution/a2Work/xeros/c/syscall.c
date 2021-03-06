/* syscall.c : syscalls
 */

#include <stdarg.h>
#include <kernel.h>

 int syscall( int req, ... ) {
/**********************************/
    va_list	ap;
    int		rc;

    va_start( ap, req );

    __asm __volatile( " \
        movl %1, %%eax \n\
        movl %2, %%edx \n\
        int  %3 \n\
        movl %%eax, %0 \n\
        "
        : "=g" (rc)
        : "g" (req), "g" (ap), "i" (KERNEL_INT)
        : "%eax" 
    );
 	
    va_end( ap );
		//kprintf("Va_end in syscall.c\n");
    return( rc );
}


 int syscreate( funcptr fp, int stack ) {
/*********************************************/

    return( syscall( SYS_CREATE, fp, stack ) );
}

 int sysyield( void ) {
/***************************/

    return( syscall( SYS_YIELD ) );
}

 int sysstop( void ) {
/**************************/

    return( syscall( SYS_STOP ) );
}

int sysgetpid( void ) {
/****************************/

    return( syscall( SYS_GETPID ) );
}

void sysputs( char *str ) {
/********************************/

    syscall( SYS_PUTS, str );
}

unsigned int syssleep( unsigned int t ) {
/*****************************/

    syscall( SYS_SLEEP, t );
}


int syssighandler( int signal, void (*handler)(void *))
/*****************************/
{
	
	return syscall(SYS_HANDLER, signal, handler);
}


void sigreturn(void *old_sp)
/*****************************/
{
	syscall(SYS_RET,old_sp);
}
int syskill(int PID, int signal_number)
/*****************************/
{
	
	return syscall(SYS_KILL,PID,signal_number);
}

int syssigwait()
/*****************************/
{
	return syscall(SYS_WAIT);
}

extern int sysopen(int device_no)
/*****************************/
{
		
	return syscall(SYS_OPEN, device_no);
}

extern int sysclose(int fd)
/*****************************/
{
	return syscall(SYS_CLOSE, fd);
}

extern int syswrite(int fd, void*buff, int bufflen)
/*****************************/
{
	return syscall(SYS_WRITE, fd, buff, bufflen);	
}

extern int sysread(int fd, void*buff, int bufflen)
/*****************************/
{
	return syscall(SYS_READ, fd, buff, bufflen);
}

extern int sysioctl(int fd, unsigned long command, ...)
{
	va_list ap;
	va_start(ap, command);

	int rc = syscall(SYS_IOCTL, fd, command, ap);
	
	va_end(ap);
	
	return rc;
}

