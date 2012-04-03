/* kbd.c 
 */

#include <kbd.h>
#include <icu.h>
#include <i386.h>
#include <stdarg.h>

unsigned int * thegoodbuffer;
int bufferlength;
int buffercount;
pcb * myprocess;
int current_kboard;

int kb_open_noecho(int device_no)
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

int kb_write_noecho(int fd, char* buff, int bufflen)
{
	//Can't write to the Keyboard
	return -1;
}

int kb_read_noecho(int fd, unsigned int *buff, int bufflen, pcb * process)
{       
	//kprintf("buffer length set to %d\n", bufflen);
  bufferlength = bufflen;
	thegoodbuffer = buff;
	myprocess = process;

	int i;
	for(i = 0; i < bufferlength; i++){
		thegoodbuffer[i] = NULL;
		//kprintf("current buffer = ");
		//kputc("%lu", thegoodbuffer[i]);
		//kprintf("\n");
	}
    buffercount = 0;
}

int kb_ioctl_noecho(int fd, unsigned long command, unsigned int * ioctl_va)
{
    va_list ap = (va_list) *(ioctl_va);                     // get the va_list                                
    
    if(command == 49){
        int eoftochange = va_arg(ap, int);
        seteof(eoftochange);
    }
    else
    {
        return -1;
    }
    va_end(ap);
}






