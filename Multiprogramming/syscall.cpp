/* SPIM S20 MIPS simulator.
   Execute SPIM syscalls, both in simulator and bare mode.
   Execute MIPS syscalls in bare mode, when running on MIPS systems.
   Copyright (c) 1990-2010, James R. Larus.
   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

   Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

   Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation and/or
   other materials provided with the distribution.

   Neither the name of the James R. Larus nor the names of its contributors may be
   used to endorse or promote products derived from this software without specific
   prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
   OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _WIN32
#include <unistd.h>
#endif
#include <sys/time.h>
#ifdef NEED_TERMIOS
#include <sys/ioctl.h>
#include <sgtty.h>
#else
#include <termios.h>

#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

#ifdef _WIN32
#include <io.h>
#endif

#include "spim.h"
#include "string-stream.h"
#include "inst.h"
#include "reg.h"
#include "mem.h"
#include "sym-tbl.h"
#include "syscall.h"
#include "data.h"

#include <iostream>



/*------------- YENİ -----------------*/
#include "spim-utils.h"
/*------------- YENİ -----------------*/
#define DEFAULT_TEXT_LOAD 0x00400000
#define MAX_PROCESS_SIZE 20
#define MAX_PROCESS_NAME_LENGTH 30
#define ROUND_ROBIN_QUANTUM 2
#define BLOCKED -2
#define CREATED -1
#define RUNNING  0
#define WAITING   1
#define TERMINATED  2

static char states[5][12] = {"BLOCKED","CREATED","RUNNING","READY","TERMINATED"};

int timerCount = 0;
int quantum = 0;


typedef struct {
	char *name;
	int id;
	int pid = 0;
	mem_addr currPC;
	int currentSpentTime;
	int state;
	mem_addr startAddress;
	mem_addr endAddress;
	mem_addr stackPointer;
	reg_word R2[R_LENGTH];
	reg_word HI2, LO2;
	mem_addr data_segment;
	mem_addr dataPoint;

}Spim_Process;

typedef Spim_Process* P_Spim_Process;
typedef struct {
	int processCount =0;
	int currentProcessIndex = -1;
	P_Spim_Process processList[20];
	mem_addr last_memAddr = DEFAULT_TEXT_LOAD;
	mem_addr last_data = DATA_BOT;

}Sch_Table;

Sch_Table processTable;

mem_addr get_next_text_pointer(){   
	uint32 q = 0;
	for(q = processTable.last_memAddr;q<text_top;q=q+4 ){
	  if(read_mem_inst(q) == 0){
		break;
	}
}
return q;
}


using namespace std;

#ifdef _WIN32
/* Windows has an handler that is invoked when an invalid argument is passed to a system
   call. https://msdn.microsoft.com/en-us/library/a9yf33zb(v=vs.110).aspx

   All good, except that the handler tries to invoke Watson and then kill spim with an exception.

   Override the handler to just report an error.
*/


#include <stdlib.h>
#include <crtdbg.h>



void myInvalidParameterHandler(const wchar_t* expression,
   const wchar_t* function, 
   const wchar_t* file, 
   unsigned int line, 
   uintptr_t pReserved)
{
  if (function != NULL)
    {
      run_error ("Bad parameter to system call: %s\n", function);
    }
  else
    {
      run_error ("Bad parameter to system call\n");
    }
}

static _invalid_parameter_handler oldHandler;

void windowsParameterHandlingControl(int flag )
{
  static _invalid_parameter_handler oldHandler;
  static _invalid_parameter_handler newHandler = myInvalidParameterHandler;

  if (flag == 0)
    {
      oldHandler = _set_invalid_parameter_handler(newHandler);
      _CrtSetReportMode(_CRT_ASSERT, 0); // Disable the message box for assertions.
    }
  else
    {
      newHandler = _set_invalid_parameter_handler(oldHandler);
      _CrtSetReportMode(_CRT_ASSERT, 1);  // Enable the message box for assertions.
    }
}
#endif

//////////// Copy  Elements for reg.h
reg_word R2[R_LENGTH];
reg_word HI2, LO2;
mem_addr PC2, nPC2;



/* -------------------------CREATE PROCES --------------------------------------------------*/

/* -------------------------CREATE PROCES --------------------------------------------------*/

void free_memory(mem_addr start){
	int q =0;	
	for(q =start;q<text_top;q=q+4 )
	  
		
		set_mem_inst(q,NULL);
	

}

//***** Backing up for CPU registers. **//

void backup_cpu_state(){

	int i =0;

	for(i = 0; i < R_LENGTH; i++) {
	
	      R2[i] = R[i];
  	 
	}
	HI2 =HI;
	LO2 = LO;
	PC2 = PC;        
	nPC2 = nPC;


}

void restore_cpu_state(){

	int i =0;
	for(i = 0; i < R_LENGTH; i++) {
		R[i] = R2[i];
	}
	HI =HI2;
	LO = LO2;
	PC = PC2;        
	nPC = nPC2;
	

}

//***** These functions back up and restore process cpu states
void backup_Process_CPU_state(P_Spim_Process process){

	int i =0;

	for(i = 0; i < R_LENGTH; i++) {
	      process->R2[i] = R[i];
  	     
	}
	process->HI2 =HI;
	process->LO2 = LO;
}




void restore_Process_CPU_state(P_Spim_Process process){

	int i =0;
	for(i = 0; i < R_LENGTH; i++) {
		R[i] = process->R2[i];
	}
	HI =process->HI2;
	LO = process->LO2;


}
/** Sometime C I/O makes problems and append additional  
   new line constant so i remove it in here**///
char *trimString(char *str){

	int i =0 ;
	char *fileName = (char *)malloc(sizeof(char)*MAX_PROCESS_NAME_LENGTH); // Create a new string 
	for(i=0;str[i]!='\0';i++) 
	{
	  if(str[i]=='\n')
	   {
		if(i ==0 || i == 1){
			
			return NULL; 
		}
	      str[i]='\0';
	   }
	   fileName[i] =str[i];
        }
	return fileName;
}


mem_addr loadProgram(char *program){


	mem_addr text_address = get_next_text_pointer();

	/***** LOAD THE CHILD PROCESS***//////	
	bool assembly_file_loaded = read_assembly_file (program);

	if(!assembly_file_loaded){
		write_output(console_out," %s\n","File is not Loaded");
		return 0;
	}
	return text_address;


}

void runFrom(mem_addr addr){
	bool continuable;
	initialize_registers ()	;
       // run_program (addr, DEFAULT_RUN_STEPS, false, false, &continuable);
}
/*** Loads the child process in to memory and runs it, returns the result */
int loadAndRun(mem_addr addr, char *program){


	bool continuable;
	bool assembly_file_loaded;
	//initialize_world (NULL, true);
	mem_addr text_address = get_next_text_pointer();
	/***** LOAD THE CHILD PROCESS***//////	
	assembly_file_loaded = read_assembly_file (program);

	if(!assembly_file_loaded){
		write_output(console_out," %s\n","File is not Loaded");
		return 0;
	}
	initialize_registers ()	;
       // run_program (text_address, DEFAULT_RUN_STEPS, false, false, &continuable);

}

/***  just styling processName ***/
void printProcessName(char *s){
	
	int i = 0;
	for(i = 0; i< MAX_PROCESS_NAME_LENGTH;i++){
		if(*(s+i) != 0)
		    printf("%c",*(s+i));
		else
		 break;
	}
	for(i = i; i< MAX_PROCESS_NAME_LENGTH;i++){

    		printf(" ");					
	}
}



int CreateProcess()
{
	
	static mem_addr addr;
        bool continuable;
	bool assembly_file_loaded;
		

	
	char *fileName =  (char *)mem_reference (R[REG_A0]);
	char *fileName2 = trimString(fileName);
	
	if(fileName2 != NULL){ //IO CHECK
		addr = starting_address ();
		backup_cpu_state();	
	   	loadAndRun(addr,fileName2);
	}	


	
	restore_cpu_state(); // RESTORING REGISTER STATES and reloading the shell
	run_program (PC+4, DEFAULT_RUN_STEPS, false, false, &continuable);
	return 1;
}

/** 
 * Spawns a new process like NtCreateProcess. Child has a seperate memory space
*/ 
P_Spim_Process newProcess(char * name){

 	

        int i = 0;
	data_begins_at_point(processTable.last_data + 2*K);
	processTable.last_data = processTable.last_data + 2*K;
	mem_addr addr = loadProgram(name);

	
	P_Spim_Process process = (P_Spim_Process)malloc(sizeof(Spim_Process));

	process->name = (char *) malloc(sizeof(char)*MAX_PROCESS_NAME_LENGTH);
	for(i = 0;name[i] != '\0';i++){

		process->name[i] = name[i];
	}
	process->name[i+1] = '\0';

	process->id = processTable.processCount;

	process->pid = processTable.processList[processTable.currentProcessIndex]->id;
	process->currPC = 0;
	process->state = CREATED;
	process->startAddress =addr;
	processTable.last_memAddr = get_next_text_pointer();	
	process->endAddress = processTable.last_memAddr-1;
	process->stackPointer = 0;
	process->R2[REG_GP] = processTable.last_data + 64*K;

	return process;
}


/**
*
*  replaces Child processes text data sgement with a new program, initializes new stack, registers and data pointers 
***/
P_Spim_Process execProcess(char * name){

        int i = 0;
	data_begins_at_point(processTable.last_data + 2*K);
	processTable.last_data = processTable.last_data + 2*K;
	mem_addr addr = loadProgram(name);

	
	P_Spim_Process process = processTable.processList[processTable.currentProcessIndex];

	process->name = (char *) malloc(sizeof(char)*MAX_PROCESS_NAME_LENGTH);
	for(i = 0;name[i] != '\0';i++){
		
		process->name[i] = name[i];
		
	}
	process->name[i] = '\0';
	
	process->currPC = addr;
	process->state = WAITING;
	process->startAddress =addr;
	processTable.last_memAddr = get_next_text_pointer();	
	process->endAddress = processTable.last_memAddr-1;
	process->stackPointer = STACK_TOP -(BYTES_PER_WORD + 4*K + process->id*(2*K));
        process->R2[REG_SP] =STACK_TOP -(BYTES_PER_WORD + 4*K + process->id*(2*K));

	process->R2[REG_GP] = processTable.last_data + 64*K;
	PC = addr;
	return process;

}

/* Execv sytem call replaces Child processes text data sgement with a new program, initializes new stack, registers and data pointers 
***/
void execv(){
	
	
	char *fileName =  (char *)mem_reference (R[REG_A0]);
	P_Spim_Process process = execProcess(fileName);
	PC = process->currPC ;
        R[REG_SP] = process->R2[REG_SP]; 
	R[REG_GP] = process->R2[REG_GP];
	onScheduleEvent();  // Process Creation is a new scheduler event: I call it but it does not guarantee that this process will be executed
	
	
}
/* Wait System call  Takes process address */

int waitpid(int processId,void* resultAddress){
	
	
	printf("\nWaiting for %d",processId);
	
	processTable.processList[processTable.currentProcessIndex]->state = BLOCKED;
	P_Spim_Process currentProcess = processTable.processList[processTable.currentProcessIndex];
	for(int i = 0; i< processTable.processCount;i++){
		
		P_Spim_Process process  =  processTable.processList[i];
		if( process->state ==TERMINATED && process->id == processId && process->pid == currentProcess->id){
			R[REG_RES] =1;
			return 1;					
		}
	
	}
	return 0;
}
/*
 Fork current process;

 Forked child works on the same address space of the parent except different registers.
*/
	
int fork(){
   
	int i =0;
	
	P_Spim_Process forked = (P_Spim_Process)malloc(sizeof(Spim_Process));
	P_Spim_Process currentProcess = processTable.processList[processTable.currentProcessIndex];
	
	int len =strlen(currentProcess->name);
	forked->name = (char *)malloc(sizeof(char)*MAX_PROCESS_NAME_LENGTH);
	for(i = 0;i < len;i++){
		forked->name[i] =currentProcess->name[i];	
	}
	forked->name[len] ='-';


	for(i = 0 ;i< R_LENGTH;i++){ /// Copy Register values
	   	forked->R2[i] =	R[i];
	}
	forked->currPC = PC+4;
	forked->state = WAITING;	
	
	forked->HI2 = currentProcess->HI2;
	forked->LO2 = currentProcess->LO2;
	forked->id = processTable.processCount;
	processTable.processList[processTable.processCount] = forked;	
	processTable.processCount +=1;

	forked->pid = currentProcess->id;
	R[REG_A0+1] =2;	// Inform Parent Process that it is a parent process;
	forked->R2[REG_A0+1] =0;  // Inform Chlid Process that it is a child process;

	return forked->id;
}

/*** Print process info, state text_start _text_end and stack values***/
void printProcessInfo(){
	
	printf("\nPROCESS_NAME            \tPID   Par_ID      PC\t         STATE\t         START\t END\t STACK  \n");
	printf("__________________________________________________________________________________________________________ \n");
	for(int i =0; i< processTable.processCount; i++){
		P_Spim_Process process =processTable.processList[i];
		printProcessName(process->name);
		printf("\t%d \t%d \t%x \t\t%s    \t%x \t%x \t%x \n",process->id,process->pid,process->currPC,states[process->state+2],process->startAddress,process->endAddress,process->R2[REG_SP]);	
		
	}
	
}
bool initialized = false;

/***
   Initialize system call. 
   Assigns currentprocess as init process and it can be called only once
*/	
void initProcess(){

	if(initialized){
		printf("Already Initialized \n");
		return;	
	}
	P_Spim_Process process = (P_Spim_Process)malloc(sizeof(Spim_Process));
	
	process->name = "init";
	process->id = 0;
	process->pid = -1;
	process->currPC = PC+4;
	process->state = RUNNING;
	process->startAddress =DEFAULT_TEXT_LOAD;
	processTable.last_memAddr = get_next_text_pointer();	
	process->endAddress = processTable.last_memAddr-1;
	process->stackPointer =R[REG_SP];
	process->R2[REG_SP]=R[REG_SP];
	processTable.processList[processTable.processCount] = process;
        processTable.currentProcessIndex =0;
        processTable.processCount =1;
	initialized = true;


}

void onScheduleEvent(){
	bool continuable;
	int finished = 0;
	bool debug = false;

	
	int nextProcess = (processTable.currentProcessIndex+1)%processTable.processCount;

	/// Check is there any running processes
	while(processTable.processList[nextProcess]->state ==TERMINATED){
		finished++;
		nextProcess++;
		nextProcess = (nextProcess)%processTable.processCount;
		if(finished ==processTable.processCount){
			printf("\n All Processes are finished.\n");
			exit(0);		
		}
	}

	if(processTable.processList[processTable.currentProcessIndex]->state !=TERMINATED){
	    
	    processTable.processList[processTable.currentProcessIndex]->currPC =PC;	 
	    processTable.processList[processTable.currentProcessIndex]->state = WAITING;
		
	    backup_Process_CPU_state( processTable.processList[processTable.currentProcessIndex]);	
	}	

	printf("\n\Context Switcing: %d -> %d\n",processTable.currentProcessIndex,nextProcess);
	P_Spim_Process cProcess = processTable.processList[processTable.currentProcessIndex];
	
	processTable.currentProcessIndex =nextProcess;
       	P_Spim_Process process = processTable.processList[processTable.currentProcessIndex];
	
	
	if(process->state == CREATED ){
 		process->state = RUNNING;
		printProcessInfo();
		initialize_registers ()	;	
		restore_Process_CPU_state(process);		
		run_program (process->currPC, DEFAULT_RUN_STEPS, debug, debug, &continuable); 
	}else{
		process->state = RUNNING;
		printProcessInfo();
		restore_Process_CPU_state(process);
		
		run_program (process->currPC, DEFAULT_RUN_STEPS, debug, debug, &continuable); 

	}
}
/***
* Exit syscall calls this function and immediately, a scheduling event occurs;
*/
void onProcessTerminated(){

     	 processTable.processList[processTable.currentProcessIndex]->state = TERMINATED;
	onScheduleEvent();
}
/***
* For Testing stuff
*/
void TestScheduler(){
	initProcess();
	printProcessInfo();
	printf("TEsting stufff__________________________________________________\n");
        int i = 0;
	
	for(i = 0;i<5;i++){

	   P_Spim_Process process = newProcess("SelectionSort.asm");

           processTable.processList[processTable.processCount] = process;
	   process->R2[REG_SP] =STACK_TOP -(BYTES_PER_WORD + 4*K + processTable.processCount*(2*K));

	   processTable.processCount +=1;

	}

	onScheduleEvent();

} 

 
/*You implement your handler here*/

void SPIM_timerHandler()
{
	
	timerCount++;
	quantum++;
	
	if(quantum == ROUND_ROBIN_QUANTUM){ // We switch processs on every other timer event.
	

	    quantum =0;
	    onScheduleEvent();
	}
	
   
}
static int q = 0;

/* Decides which syscall to execute or simulate.  Returns zero upon
   exit syscall and non-zero to continue execution. */
int
do_syscall ()
{
#ifdef _WIN32
    windowsParameterHandlingControl(0);
#endif

  /* Syscalls for the source-language version of SPIM.  These are easier to
     use than the real syscall and are portable to non-MIPS operating
     systems. */

  switch (R[REG_V0])
    {
    case PRINT_INT_SYSCALL:
      write_output (console_out, "%d", R[REG_A0]);
      break;

    case PRINT_FLOAT_SYSCALL:
      {
	float val = FPR_S (REG_FA0);

	write_output (console_out, "%.8f", val);
	break;
      }

    case PRINT_DOUBLE_SYSCALL:
      write_output (console_out, "%.18g", FPR[REG_FA0 / 2]);
      break;

    case PRINT_STRING_SYSCALL:
      write_output (console_out, "%s", mem_reference (R[REG_A0]));
      break;

    case READ_INT_SYSCALL:
      {
	static char str [256];

	read_input (str, 256);
	R[REG_RES] = atol (str);
	break;
      }

    case READ_FLOAT_SYSCALL:
      {
	static char str [256];

	read_input (str, 256);
	FPR_S (REG_FRES) = (float) atof (str);
	break;
      }

    case READ_DOUBLE_SYSCALL:
      {
	static char str [256];

	read_input (str, 256);
	FPR [REG_FRES] = atof (str);
	break;
      }

    case READ_STRING_SYSCALL:
      {
	read_input ( (char *) mem_reference (R[REG_A0]), R[REG_A1]);
	data_modified = true;
	break;
      }

    case SBRK_SYSCALL:
      {
	mem_addr x = data_top;
	expand_data (R[REG_A0]);
	R[REG_RES] = x;
	data_modified = true;
	break;
      }

    case PRINT_CHARACTER_SYSCALL:
      write_output (console_out, "%c", R[REG_A0]);
      break;

    case READ_CHARACTER_SYSCALL:
      {
	static char str [2];

	read_input (str, 2);
	if (*str == '\0') *str = '\n';      /* makes xspim = spim */
	R[REG_RES] = (long) str[0];
	break;
      }

    case EXIT_SYSCALL:{
      spim_return_value = 0;
      onProcessTerminated();
      
	break;      
	}
    case EXIT2_SYSCALL:
      spim_return_value = R[REG_A0];	/* value passed to spim's exit() call */
      return (0);

    case OPEN_SYSCALL:
      {
#ifdef _WIN32
        R[REG_RES] = _open((char*)mem_reference (R[REG_A0]), R[REG_A1], R[REG_A2]);
#else
	R[REG_RES] = open((char*)mem_reference (R[REG_A0]), R[REG_A1], R[REG_A2]);
#endif
	break;
      }

    case READ_SYSCALL:
      {
	/* Test if address is valid */
	(void)mem_reference (R[REG_A1] + R[REG_A2] - 1);
#ifdef _WIN32
	R[REG_RES] = _read(R[REG_A0], mem_reference (R[REG_A1]), R[REG_A2]);
#else
	R[REG_RES] = read(R[REG_A0], mem_reference (R[REG_A1]), R[REG_A2]);
#endif
	data_modified = true;
	break;
      }

    case WRITE_SYSCALL:
      {
	/* Test if address is valid */
	(void)mem_reference (R[REG_A1] + R[REG_A2] - 1);
#ifdef _WIN32
	R[REG_RES] = _write(R[REG_A0], mem_reference (R[REG_A1]), R[REG_A2]);
#else
	R[REG_RES] = write(R[REG_A0], mem_reference (R[REG_A1]), R[REG_A2]);
#endif
	break;
      }

    case CLOSE_SYSCALL:
      {
#ifdef _WIN32
	R[REG_RES] = _close(R[REG_A0]);
#else
	R[REG_RES] = close(R[REG_A0]);
#endif
	break;
      }
	 /* ----------------CREATE PROCESS SYSCALL---------------------------------------------- */
	 case CREATE_PROCESS:
	{
		
		CreateProcess();
		     
		break;
	}
  	case EXECV:
	{
		
		execv();

		break;
	}
 	 case FORK:
	{	

		R[REG_A0] = fork();
		break;
	}
 	case WAIT:
	{	
		
		R[REG_A0+1] = waitpid(R[REG_A0],NULL);
		break;

	}
  	case INIT:
	{

		initProcess();

		break;
	}
    	case WHOAMI:
	{
		R[REG_A1] = processTable.processList[processTable.currentProcessIndex]->id;
		break;
	}
	case RANDOM:
	{
		q++;  // Most of the time random seed generator generates same number since call times are same so i 
		time_t t;
		srand((unsigned)time(&t)+q);
		if(!(R[REG_A0] > 0))
		   R[REG_A1] =1;
	        else
		R[REG_A1] = rand() %(int)(R[REG_A0]);
		printf("\n\n%u\n\n",q);
		break;
	}

    default:
      run_error ("Unknown system call: %d\n", R[REG_V0]);
      break;
    }

#ifdef _WIN32
    windowsParameterHandlingControl(1);
#endif
  return (1);
}


void
handle_exception ()
{
  if (!quiet && CP0_ExCode != ExcCode_Int)
    error ("Exception occurred at PC=0x%08x\n", CP0_EPC);

  exception_occurred = 0;
  PC = EXCEPTION_ADDR;

  switch (CP0_ExCode)
    {
    case ExcCode_Int:
      break;

    case ExcCode_AdEL:
      if (!quiet)
	error ("  Unaligned address in inst/data fetch: 0x%08x\n", CP0_BadVAddr);
      break;

    case ExcCode_AdES:
      if (!quiet)
	error ("  Unaligned address in store: 0x%08x\n", CP0_BadVAddr);
      break;

    case ExcCode_IBE:
      if (!quiet)
	error ("  Bad address in text read: 0x%08x\n", CP0_BadVAddr);
      break;

    case ExcCode_DBE:
      if (!quiet)
	error ("  Bad address in data/stack read: 0x%08x\n", CP0_BadVAddr);
      break;

    case ExcCode_Sys:
      if (!quiet)
	error ("  Error in syscall\n");
      break;

    case ExcCode_Bp:
      exception_occurred = 0;
      return;

    case ExcCode_RI:
      if (!quiet)
	error ("  Reserved instruction execution\n");
      break;

    case ExcCode_CpU:
      if (!quiet)
	error ("  Coprocessor unuable\n");
      break;

    case ExcCode_Ov:
      if (!quiet)
	error ("  Arithmetic overflow\n");
      break;

    case ExcCode_Tr:
      if (!quiet)
	error ("  Trap\n");
      break;

    case ExcCode_FPE:
      if (!quiet)
	error ("  Floating point\n");
      break;

    default:
      if (!quiet)
	error ("Unknown exception: %d\n", CP0_ExCode);
      break;
    }
}
