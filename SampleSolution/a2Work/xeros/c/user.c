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

void consumer( void ) {
/****************************/

    int         i;
    char        buff[100];

    for( i = 0; i < 10; i++ ) {
      sprintf(buff, "Consume %d\n", i);
      sysputs( buff );
      sysyield();
    }


    for (i = 0; i < 200; i++) {
      sysputs("C");
      syssleep(10000);
    }

    sysstop();
}

void     root( void ) {
/****************************/

    int i, j;
    sysputs("Root has been called\n");

    sysyield();
    sysyield();
   
    syscreate( &producer, 4096 );
    syscreate( &consumer, 4096 );
    sysstop();
}
