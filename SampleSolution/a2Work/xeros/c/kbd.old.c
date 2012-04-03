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
int keyboard;

/*
 * keyboard_no_echo_open
 *
 * takes a device number and opens the no echo keyboard
 */
int keyboard_no_echo_open(int device_no)
{
	keyboard = 0;		// set keyboard to 0 so we can determine what keyboard in use later
	enable_irq(1,0);  // enable input from keyboard
}

/*
 * keyboard_no_echo_close
 *
 * takes a file descriptor and closes the associated device
 */
int keyboard_no_echo_close(int fd)
{
    enable_irq(1,1);  // disable input from keyboard
}

/*
 * keyboard_no_echo_write
 *
 * does nothing since you can't write to a keyboard
 */
int keyboard_no_echo_write(int fd, char* buff, int bufflen)
{
    return -1; // can't write to a keyboard
}

/*
 * keyboard_no_echo_read
 *
 * reads in data from the keyboard
 */
int keyboard_no_echo_read(int fd, unsigned int *buff, int bufflen, pcb * process)
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

/*
 * keyboard_no_echo_ioctl
 *
 * takes an allowed command and some parameters and performs the action correlated to that command
 */
int keyboard_no_echo_ioctl(int fd, unsigned long command, unsigned int * ioctl_va)
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

/*
 * keyboard_echo_open
 *
 * takes a device number and opens the echo keyboard
 */
int keyboard_echo_open(int device_no)
{
	//kprintf("KEYBOARD_ECHO_OPEN WAS CALLED\n");
	keyboard = 1;  // set keyboard to 0 so we can determine what keyboard in use later
    enable_irq(1,0);// enable input from keyboard
}

/*
 * keyboard_echo_close
 *
 * takes a file descriptor and closes the associated device
 */
int keyboard_echo_close(int fd)
{
    enable_irq(1,1);// disable input from keyboard
}

/*
 * keyboard_echo_write
 *
 * does nothing since you can't write to a keyboard
 */
int keyboard_echo_write(int fd, char* buff, int bufflen)
{
    return -1; // can't write to a keyboard
}

/*
 * keyboard_echo_read
 *
 * reads in data from the keyboard
 */
int keyboard_echo_read(int fd, unsigned int *buff, int bufflen, pcb * process)
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

/*
 * copy
 *
 * copys data to upper half's buffer 
 */
int copy(unsigned int * keybuff){
    int i = 0;
	//kprintf("buffercount %d bufferlength %d\n", buffercount, bufferlength);
    while(buffercount < bufferlength && i < 4){
		if(keybuff[i] != NULL)
		{
        	thegoodbuffer[buffercount] = keybuff[i];
	        buffercount++;
		}

        keybuff[i] = NULL;
        i++;
    }
    /*
    kprintf("(");
	int m;
    for (m = 0; m< bufferlength; m++)
    {
        if (thegoodbuffer[m] != NULL)
		{
            //kprintf("%lu", thegoodbuffer[m]);
            kputc("%lu", thegoodbuffer[m]);
		}
        else    
            kprintf("-");
    }
    
    kprintf(")\n"); */
	
    
    
    if(buffercount == bufferlength || 
    	 checkForEOF(thegoodbuffer, buffercount)== 2 ||
			 checkForNewLine(thegoodbuffer, buffercount) == 2){
		//kprintf("process pid = %d\n", myprocess->pid);
        unblock(myprocess);
		unsigned int * esp_pt;
		esp_pt = (unsigned int *) myprocess->esp;					// get pointer to esp of the process stack
		esp_pt += 7;
		*esp_pt = buffercount;
		ready(myprocess);
		return 0;
    }else{
        return -1;
    }

        
}

/*
 * getUpperBufferLength()
 *
 * returns the length of the upper halfs buffer
 */
int getUpperBufferLength()
{
	return bufferlength;
}

/*
 * getUpperBufferCount()
 *
 * returns the fullness of the upper halfs buffer
 */
int getUpperBufferCount()
{
	return buffercount;
}

/*
 * getKeyboard()
 *
 * returns which keyboard is currently in use
 */
int getKeyboard()
{
	return keyboard;
}

/*
 * keyboard_echo_ioctl
 *
 * takes an allowed command and some parameters and performs the action correlated to that command
 */
int keyboard_echo_ioctl(int fd, unsigned long command, unsigned int * ioctl_va)
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
