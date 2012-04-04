/* user.c : User processes
 */

#include <kernel.h>

/* Your code goes here */
static int root_pid;
static int flag=1;
void producer( void ) {
/****************************/
	
    int         i;
    char        buff[100];

    for( i = 0; i < 10; i++ ) {
      sprintf(buff, "Produce %d\n", i);
      //sysputs(buff);
      sysyield();
    }
    
    for (i = 0; i < 200; i++) {
      //sysputs("P");
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


	//syssighandler(3,print_foo);
	//syssigwait();
	
	//int i = 3;
	//for(;i>0;i--){
	//	sysputs("babab\n");
	//	sysyield();
		//}


}
void print_sig18(){
	sysputs("handler for signal 18 called\n");
	flag = 0;
}
void root_signaler()
{
	syssleep(100);
	//sends sleeping signal to child
	sysputs(" root_signaler: sending signal 20 to root\n");
	syskill (root_pid, 20);
	sysputs(" root_signaler: sending signal 18 to root\n");
	syskill (root_pid, 18);
}


void     root( void ) {
/****************************/

		sysputs("Welcome to test_program\n");
		int fd = sysopen(1);
		sysputs("Echo keyboard opened\n");
    int i, j;
    //syssleep(10000);
    
  	sysputs("Skipping steps 3 to 5\n"); 
		  
		sysputs("Closing echo keyboard\n");
		
		sysclose(fd);
		
		sysputs("Non-echo keyboard opened\n");
		fd = sysopen(0);
		sysclose(fd);
		
		
		
		syssighandler(18,&print_sig18);
		
		root_pid = sysgetpid();
		
		syscreate(&root_signaler, 4096);	   
    
    //will not exit untill the signaling process does so
    while(flag){
   		sysyield(); 
   	
   		}
   		sysputs("Root test program completed\n");
   		sysstop();
}




//----------------------------------
//							TESTS
//----------------------------------

#define LOW_PRIORITY_SIGNAL		1
#define MED_PRIORITY_SIGNAL		5
#define HIGH_PRIORITY_SIGNAL	10



 void sleep_handler( void ) {
/****************************/
	sysputs("  Sleeping for 15 seconds...\n");
	syssleep(15000);
	sysputs("  Woke up from sleep!\n");
 }

  void high_interrupt_handler( void ) {
/****************************/
	sysputs("  ++ High Priority Signal!\n");
 }
 
   void low_interrupt_handler( void ) {
/****************************/
	sysputs("  -- Low priority signal!\n");
 }
/**
  *  Priority test case.  The high priority signal should be printed first before the low priority one
  */
 void test1_child( void ) {
	syssighandler(LOW_PRIORITY_SIGNAL,&low_interrupt_handler);
	syssighandler(MED_PRIORITY_SIGNAL,&sleep_handler);
	syssighandler(HIGH_PRIORITY_SIGNAL,&high_interrupt_handler);
	sysputs("  Child: handlers set up, waiting for signals\n");
	syssigwait();
 }

 void	test1( void ) {
/****************************/
	sysputs("//-------------------\nWelcome to Test 1:  Priority test\n");
    int child_id = syscreate(&test1_child, 4096);
	sysputs("  Root: child created\n");
	syssleep(100);
	//sends sleeping signal to child
	sysputs("  Root: sending the child to bed (medium priority)\n");
	syskill (child_id, MED_PRIORITY_SIGNAL);
	syssleep(100);
	sysputs("  Root: sending low priority signal\n");
	syskill (child_id, LOW_PRIORITY_SIGNAL);
//	syssleep(100);
	sysputs("  Root: sending high priority signal\n");
	syskill (child_id, HIGH_PRIORITY_SIGNAL);
     for( ;; ) 
    {
        sysyield();
    };
}
/**
  *  Syshandler test case.  Child should sleep after root's signal
  */
  void test2_child( void ) {
	syssighandler(MED_PRIORITY_SIGNAL,&sleep_handler);
	sysputs("  Child: handler set up, waiting for signal to sleep\n");
	syssigwait();
	sysputs("  Child: Succesfully handled, bye\n//-------------------\n");
 }

  void	test2( void ) {
/****************************/
	sysputs("//-------------------\nWelcome to Test 2:  sighandler test\n");
    int child_id = syscreate(&test2_child, 4096);
	sysputs("  Root: child created\n");
	syssleep(100);
	//sends sleeping signal to child
	sysputs("  Root: sending the child to bed - sighandler will make him sleep\n");
	syskill (child_id, MED_PRIORITY_SIGNAL);
	syssleep(100);
     for( ;; ) 
    {
        sysyield();
    };
}

/**
  *  Syskill test case.  Child should receive root's signal
  */
 void test3_child( void ) {
	syssighandler(LOW_PRIORITY_SIGNAL,&low_interrupt_handler);
	sysputs("  Child: handler set up, waiting for low priority signal\n");
	syssigwait();
	sysputs("  Child: signal received from syskill\n//-------------------\n");
 }
  void	test3( void ) {
/****************************/
	sysputs("//-------------------\nWelcome to Test 3:  sigkill test\n");
    int child_id = syscreate(&test3_child, 4096);
	sysputs("  Root: child created\n");
	syssleep(100);
	sysputs("  Root: using syskill to send Low Priority Signal\n");
	syskill (child_id, LOW_PRIORITY_SIGNAL);
	syssleep(100);
     for( ;; ) 
    {
        sysyield();
    };
}
/**
  *  Syssigwait test case.  Root should wait for child's signal
  */
   void test4_child( void ) {
	sysputs("  Child: handler set up, sleeping for 5 seconds\n");
	syssleep(15000);
	sysputs("  Child: sending low priority signal to root\n");
	syskill (root_pid, LOW_PRIORITY_SIGNAL);
 }
  void	test4( void ) {
/****************************/
	sysputs("//-------------------\nWelcome to Test 4:  syssigwait test\n");
	root_pid = sysgetpid();
	syssighandler(LOW_PRIORITY_SIGNAL,&low_interrupt_handler);
    int child_id = syscreate(&test4_child, 4096);
	sysputs("  Root: child created; waiting for signals\n");
	syssigwait();
	sysputs("  Waited for signal successfuly\n//-------------------\n");
     for( ;; ) 
    {
        sysyield();
    };
}

  void	test5( void ) {
/****************************/
	sysputs("//-------------------\nWelcome to Test 5:  sysopen with invalid arguments\n");
	//Tries openning a device that using a device number that does not exist
	sysopen(7);
     for( ;; ) 
    {
        sysyield();
    };
}

  void	test6( void ) {
/****************************/
	sysputs("//-------------------\nWelcome to Test 6:  syswrite with invalid file descriptor\n");
	char buf[7];
	syswrite(13, buf, 7);
     for( ;; ) 
    {
        sysyield();
    };
}

  void	test7( void ) {
/****************************/
	sysputs("//-------------------\nWelcome to Test 7:  sysioctl with invalid commands\n");
	int fd =	sysopen(0);
	sysioctl(fd, 29,0);
     for( ;; ) 
    {
        sysyield();
    };
}

  void	test8( void ) {
/****************************/
	sysputs("//-------------------\nWelcome to Test 8:  sysread buffer is too large\n");
	
	char buff[10];
	int fd = sysopen(1);
	sysread(fd, buff, 10);
	sysputs("There are unread characters in the buffer\n");
     for( ;; ) 
    {
        sysyield();
    };
}



  void	test_Program( void ) {
/****************************/
	sysputs("//-------------------\nWelcome to the Test Program:\n");

	//TODOs: 
	//2. Open echo keyboard
	//3. Reads characters, one at a time until 10 characters are read. 
	//4. Attempts to open the \no echo” keyboard 
	//5. Attempts to open the \echo” keyboard 
	//6. Closes keyboard 
	//7. Opens the \no echo” 
	//8. Does three reads of 10 characters and has the application print them (Each typed line is to be at least 15 characters long.) 
	//9. Continues reading characters until an EOF indication is received. 
	//10. Closes keyboard and opens the \echo” keyboard 
	//11. Installs a signal handler for signal 18 that simply prints that it was called. 
	//12. Creates a new process that sleeps for 100 milliseconds and then sends a signal 20 to the root process followed by signal 18. 
	//13. The root process does another read (don't type anything) 
	//14. The read should be interrupted by the signal and get back an error, print out a message to indicate this. 
	//15. Read input until an EOF indication is returned. 
	//16. Attempt to read again and print the result that read returns 
	//17. Print termination message and exit. In the above be sure to include appropriate print statements to clearly illustrate what is happening. 
     for( ;; ) 
    {
        sysyield();
    };
}
 //----------------------------------
//							END OF TESTS
//----------------------------------

