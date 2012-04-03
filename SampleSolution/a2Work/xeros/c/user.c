/* user.c : User processes
 */

#include <kernel.h>

/* Your code goes here */

void producer( void ) {
/****************************/
	
    int         i;
    char        buff[100];

    for( i = 0; i < 10; i++ ) {
      sprintf(buff, "Produce %d\n", i);
      sysputs(buff);
      sysyield();
    }
    
    for (i = 0; i < 200; i++) {
      sysputs("P");
      syssleep(1500);
    }

    sysstop();
}
void print_foo()
{	
	
	sysputs("print_foo\n");
	
	return;	
}

void consumer( void ) {
/****************************/

/*    int         i;*/
/*    char        buff[100];*/

/*    for( i = 0; i < 10; i++ ) {*/
/*      sprintf(buff, "Consume %d\n", i);*/
/*      sysputs( buff );*/
/*      sysyield();*/
/*    }*/


/*    for (i = 0; i < 200; i++) {*/
/*      sysputs("C");*/
/*      syssleep(10000);*/
/*    }*/

/*    sysstop();*/


	syssighandler(3,print_foo);
	syssigwait();
	
	int i = 3;
	for(;i>0;i--){
		sysputs("babab\n");
		sysyield();
		}

	





}

void     root( void ) {
/****************************/
		enable_irq(1,0);
		
    int i, j;
    sysputs("Root has been called\n");

    sysyield();
    
   
    syscreate( &producer, 4096 );
   
    int pid = syscreate( &consumer, 4096 );
    kprintf("consumer pid is %d\n", pid);
    sysyield();
   
    syssleep(10000);
    syskill(pid,3);
    syssleep(1000);
    sysyield();
    
   
    sysstop();
}
