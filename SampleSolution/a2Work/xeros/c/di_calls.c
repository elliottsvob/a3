/* di_calls.c
*/

#include <stdarg.h>
#include <kernel.h>
#include <kbd.h>


int di_open(int device_no, pcb*p)
{
	//Verify dv number is in range
	if(device_no < 0 || device_no > 1){
		kprintf("Invalid device number: %d\n", device_no);
		return -1;
	}

	devsw * new_dv = &device_table[device_no];
	
	//case specific device open function pointed to by the device block
	
	new_dv->dvopen(device_no);
	
	
	int i;
	int j = -1;
	for(i=0; i<FDT_SIZE; i++)
	{
		devsw * tmp_dv = p->fdt[i];
		if(!tmp_dv){
			//stores the last empty position in the fdt
			j = i;
		}
		else if(tmp_dv == &device_table[device_no]){
			//if the device is already on the fdt table returns its position
			return i;
		}
	}
	if (j!=-1){
		//add entry to the fd table in the pcb
		p->fdt[j] = &device_table[device_no];
	}
	return j;	
}
/**
*
**/
int di_close(int fd,pcb *p)
{
	if(!valid_arguments(fd,p)){
	kprintf("di_close was aborted.  invalid arguments\n");
		return -1;
	}

	//determine the appropriate devices index in the dev table
	devsw * tmp_dv = p->fdt[fd];
	//determine and calls the appropriate lower half
	//TODO verify call is done properly
	int ret =	tmp_dv->dvclose();
	//handles return value. 
	//TODO is there any handling for the return value?
	return ret;
}
int di_write(int fd, void*buff, int bufflen,pcb *p)
{
	if(!valid_arguments(fd,p)){
	kprintf("di_write was aborted.  invalid arguments\n");
		return -2;
	}
	//is the fd valid?
	
	if(p->fdt[fd]){
		devsw * dev = p->fdt[fd];
		return dev->dvwrite(buff,bufflen);
		}
	
	return -1;
	

}
int di_read(int fd, void*buff, int bufflen,pcb *p)
{//verify the fd is valid and identify open device
	if(!valid_arguments(fd,p)){
	kprintf("di_close was aborted.  invalid arguments\n");
		return -2;
	}
	if(p->fdt[fd]){
		devsw * dev = p->fdt[fd];
		return dev->dvread(buff, bufflen,p);		
	}
	return -1;

	//determin the appropriate devices index in the dev table
	//determine and calls the appropriate lower half
	//handles return value. 

}

extern int is_valid_command (unsigned long command) {
	if(command!=49){
		return 0;
	}
	return 1;
}

int di_ioctl(int fd, unsigned long command,pcb*p, va_list ioctl_list)
{//verify the fd is valid and identify open device
	//determin the appropriate devices index in the dev table
	//determine and calls the appropriate lower half
	//handles return value. 
	if(!is_valid_command(command))
	{
		kprintf("di_ioctl was aborted.  invalid Commands\n");
		return -3;
	}
		kprintf("di_ioctl!!!\n");
	
	if(!valid_arguments(fd,p)){
	kprintf("di_ioctl was aborted.  invalid arguments\n");
		return -2;
	}


}


extern valid_arguments (int fd,pcb *p){
	//Verify dv number is in range
	if(fd< 0 || fd >= FDT_SIZE){
		kprintf("Invalid file descriptor #: %d\n", fd);
		return 0;
	}
	if(!p->fdt[fd]){
	//verify the fd is valid and identify open device
		kprintf("File descriptor %d is not bound to any device\n", fd);
		return 0;
	}
	return 1;
}

void device_init(){

	device_table[0].dvnum   = 0; //no echo keyboard
	device_table[0].dvopen  = &kb_open_noecho;
	device_table[0].dvclose = &kb_close_noecho;
	device_table[0].dvread	= &kb_read_noecho;
	device_table[0].dvwrite = &kb_write;
	device_table[0].dvcntl  = &kb_ioctl_noecho; 
	device_table[0].dvminor = 1;
	
	device_table[1].dvnum = 1; //   echo keyboard
	device_table[1].dvopen  = &kb_open_echo;
	device_table[1].dvclose = &kb_close_echo;
	device_table[1].dvread	= &kb_read_echo;
	device_table[1].dvwrite = &kb_write;
	device_table[1].dvcntl  = &kb_ioctl_echo; 
	device_table[1].dvminor = 1;
	
}

