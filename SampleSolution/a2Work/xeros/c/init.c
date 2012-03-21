/* initialize.c - initproc */

#include <i386.h>
#include <kernel.h>

extern	int	entry();        /* start of kernel image, use &start    */
extern	int	end();          /* end of kernel image, use &end        */
extern  long	freemem; 	/* start of free memory (set in i386.c) */
extern char	*maxaddr;	/* max memory address (set in i386.c)	*/

/************************************************************************/
/***				NOTE:				      ***/
/***								      ***/
/***   This is where the system begins after the C environment has    ***/
/***   been established.  Interrupts are initially DISABLED.  The     ***/
/***   interrupt table has been initialized with a default handler    ***/
/***								      ***/
/***								      ***/
/************************************************************************/


pcb proctab[MAX_PROC];          /* process table */
static void idleproc( void );   /* idle process */


/*------------------------------------------------------------------------
 *  The init process, this is where it all begins...
 *------------------------------------------------------------------------
 */
initproc()				/* The beginning */
{
	kprintf( "\n\nCPSC 415, Jan 2012\n32 Bit Xeros 1.1\nLocated at: %x to %x\n", &entry, &end );

        /* Your code goes here */

	kprintf("Max addr is %d %x\n", maxaddr, maxaddr);

	kmeminit();
	kprintf("memory inited\n");
	
	dispatchinit();
	kprintf("dispatcher inited\n");
  
	contextinit();
	kprintf("context inited inited\n");

	kprintf("Creating Idle Process\n");
	create(idleproc, PROC_STACK);

        kprintf("Creating Root Process\n");
	create( root, PROC_STACK );
	kprintf("System initialization completed\n");
  
	dispatch();
  
	kprintf("Returned to init, you should never get here!\n");
        /* This code should never be reached after you are done */
	for(;;); /* loop forever */
}
/*------------------------------------------------------------------------
 *  The idle process 
 *------------------------------------------------------------------------
 */
static void idleproc( void )	
{
    int	i;
    //    kprintf("I");
    for( i = 0; ; i++ ) {
       sysyield();
    }
}
