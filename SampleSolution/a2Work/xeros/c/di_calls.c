/* di_calls.c
*/

#include <stdarg.h>
#include <kernel.h>


int di_open(int device_no, pcb*p)
{
	//Verify major number is in range
	if(device_no < 0 || device_no > 1){
		return -1;
	}
	
	//case specific device open function pointed to by the device block
	//add entry to the fd table in the pcb
}
/**
*
**/
int di_close(int fd,pcb *p)
{
	if(!p->fdt[fd]){
		return -1;
	}
	//verify the fd is valid and identify open device
	//determine the appropriate devices index in the dev table
	//determine and calls the appropriate lower half
	//handles return value. 
}
int di_write(int fd, void*buff, int bufflen,pcb *p)
{
//verify the fd is valid and identify open device
	//determin the appropriate devices index in the dev table
	//determine and calls the appropriate lower half
	//handles return value. 

}
int di_read(int fd, void*buff, int bufflen,pcb *p)
{//verify the fd is valid and identify open device
	//determin the appropriate devices index in the dev table
	//determine and calls the appropriate lower half
	//handles return value. 

}
int di_ioctl(int fd, unsigned long command,pcb*p, va_list ioctl_list)
{//verify the fd is valid and identify open device
	//determin the appropriate devices index in the dev table
	//determine and calls the appropriate lower half
	//handles return value. 

}


void device_init(){

/*	device_table[0].dvnum   = 0; //no echo keyboard*/
/*	device_table[0].dvopen  = &kb_open_noecho;*/
/*	device_table[0].dvclose = &kb_close_noecho;*/
/*	device_table[0].dvread	= &kb_read_noecho;*/
/*	device_table[0].dvwrite = &kb_write_noecho;*/
/*	device_table[0].dvcntl  = &kb_ioctl_noecho; */
/*	device_table[0].dvminor = 1;*/
/*	*/
/*	device_table[1].dvnum = 1; //   echo keyboard*/
/*	device_table[1].dvopen  = &kb_open_echo;*/
/*	device_table[1].dvclose = &kb_close_echo;*/
/*	device_table[1].dvread	= &kb_read_echo;*/
/*	device_table[1].dvwrite = &kb_write_echo;*/
/*	device_table[1].dvcntl  = &kb_ioctl_echo; */
/*	device_table[1].dvminor = 1;*/
	
}
