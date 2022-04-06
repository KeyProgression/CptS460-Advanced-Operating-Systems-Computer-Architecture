// wait.c file: ksleep(), kwakeup(), kexit()

extern PROC *running;
extern PROC *readyQueue;
extern PROC *sleepList;

int ksleep(int event)
{
  int SR = int_off();           // turn off IRQ interrupts then return CPSR
  running->event = event;     // Make record of the event value in the Proc
  running->status = SLEEP;    // Change currently running proc to SLEEP
  
  kprintf("\nPROC[%d] goes to sleep | EVENT: %d\n ", running->pid, running->event);
  
  enqueue(&sleepList, running);
  
  // printList("Sleep List = ", sleepList);
  
  tswitch();                  // Switch Process
  
  int_on(SR);                 // Restore original CPSR 

  // printf("ksleep: under construction\n");
}

int kwakeup(int event)
{
  PROC *eachProc;
  PROC *tempSleepList = 0;
  int SR = int_off();         // Disable IRQ Interrupts and return CPSR
                                // Current Program Status Register (CPSR)
  while (eachProc = dequeue(&sleepList))
  {
    if (eachProc->event == event)
    {
      kprintf("\nPROC[%d] found, matching EVENT: %d\n\t |-> WOKE UP PROC[%d]\n", eachProc->pid, event, eachProc->pid);
      eachProc->status = READY;       // READY == 1
      enqueue(&readyQueue, eachProc);
      }
      else
      {
        enqueue(&tempSleepList, eachProc);
      }
    }
    sleepList = tempSleepList;
    
    int_on(SR);

    return 0;
}

int kexit(int exitCode)
{
  running->exitCode = exitCode;
  running->status = ZOMBIE;
  running->priority = 0;
  kwakeup(running->parent);
  tswitch();
}
