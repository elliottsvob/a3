


#ifndef KBD_H_INCLUDED
#define KBD_H_INCLUDED

#include <kernel.h>

extern void kb_open_noecho();
extern int kb_close_noecho(int fd);
extern int kb_write(int fd, char* buff, int bufflen);
extern int kb_read_noecho(int fd, unsigned int *buff, int bufflen, pcb*process);
extern int kb_ioctl_noecho(int fd, unsigned long command, unsigned int * ioctl_va);

#endif
