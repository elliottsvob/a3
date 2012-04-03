/* user.c : User processes
 */

#include <kernel.h>

/* Your code goes here */

void producer( void ) {
/****************************/

/*    int         i;*/
/*    char        buff[100];*/

/*    for( i = 0; i < 10; i++ ) {*/

/*      sprintf(buff, "Produce %d\n", i);*/
/*      sysputs(buff);*/
/*      sysyield();*/
/*    }*/
/*    for (i = 0; i < 200; i++) {*/
/*      sysputs("P");*/
/*      syssleep(1500);*/
/*    }*/

/*    sysstop();*/
}
void print_foo()
{
	char buff[100];
	int i = 69;
	sprintf(buff, "print_foo %d\n",i);
	sysputs(buff);
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
	
	syssighandler(3,&print_foo);
	for(;;){
		//kprintf("babab\n");
		sysyield();
		}

	





}

void     root( void ) {
/****************************/

    int i, j;
    sysputs("Root has been called\n");

    sysyield();
    sysyield();
   
    syscreate( &producer, 4096 );
    int pid = syscreate( &consumer, 4096 );
    
    syskill(pid,3);
   
    sysstop();
}
