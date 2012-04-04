/* kbd.c 
 */

#include <kbd.h>
#include <icu.h>
#include <i386.h>
#include <stdarg.h>


int current_kboard;

extern char kernel_buffer[4];

int get_kb(){
	return current_kboard;
}

 void kb_open_noecho()
{
	//Keyboard device indicator
	current_kboard = 0;		
	//Enable Keyboard input	
	enable_irq(1,0); 
}

int kb_close_noecho(int fd)
{
	//Disable Keyboard input	
	enable_irq(1,1);
}

//Write is the same for both types of keyboard
int kb_write(int fd, char* buff, int bufflen)
{
	//Can't write to the Keyboard
	return -1;
}

 int kb_read_noecho(int fd, char *buff, int bufflen, pcb * process)
{       
	//copy the internal buffer into the buff provided by the process 
	
	return copy(buff, bufflen);
	
}

int copy(char *buff, int bufflen)
{
	int i = 0;
	for(;i<bufflen;i++){
		*buff = kernel_buffer[i];
		}
	reset_cursor();
	kernel_buffer_clear();
	return i;
}



int kb_ioctl_noecho(int fd, unsigned long command, unsigned int * ioctl_va)
{
   return -1;
}

 void kb_open_echo()
{
	//Keyboard device indicator
	current_kboard = 1;		
	//Enable Keyboard input	
	enable_irq(1,0); 
}

int kb_close_echo(int fd)
{
	//Disable Keyboard input	
	enable_irq(1,1);
}


 int kb_read_echo(int fd, char *buff, int bufflen, pcb * process)
{       
	//copy the internal buffer into the buff provided by the process 
	
	return copy(buff, bufflen);
	
}


int kb_ioctl_echo(int fd, unsigned long command, unsigned int * ioctl_va)
{
   return -1;
}



