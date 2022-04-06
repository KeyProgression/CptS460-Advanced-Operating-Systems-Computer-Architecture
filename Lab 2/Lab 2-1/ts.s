/******* ts.s file ****************/

//	    .global start
//start:
//         LDR sp, =stack_top    
//         BL main               
//         B .                   

.text
.code 32
   .global start
	
start:
  LDR sp, =stack_top   // set SVC mode stack pointer to stack_top
  BL main              // branch-link to main() in C
  B .                  // looping