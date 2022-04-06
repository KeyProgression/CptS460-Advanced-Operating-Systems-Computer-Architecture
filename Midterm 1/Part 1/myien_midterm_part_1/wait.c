int tswitch();

int ksleep(int event)
{
  int SR = int_off();           // turn off IRQ interrupts then return CPSR
  running->event = event;     // Make record of the event value in the Proc
  running->status = SLEEP;    // Change currently running proc to SLEEP
  
  kprintf("\nPROC[%d] goes to sleep | EVENT: %d\n ", running->pid, running->event);
  
  enqueue(&sleepList, running);
  
  printList("Sleep List = ", sleepList);
  
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
    
  //printf("kwakeup: under construction\n");
}

int kexit(int exitValue)
{
  if (running->pid == 1)
  {
    printf("\n\nProc[1] can't be exited\n\n");
    return -1;
  }
  running->status = ZOMBIE;
  running->exitCode = exitValue;

  if (running->child)
  {
    treeEnqueue(&proc[1],running->child);
    printList("Child List = ",&proc[1]);
    running->child = 0;
    kwakeup(&proc[1]);
  }

  kprintf("proc %d in kexit(), value=%d\n", running->pid, exitValue);
  kprintf("Proc %d is Zombie Child of Proc %d\n", running->pid, running->parent->pid);
  //running->exitCode = exitValue;
  //running->status = ZOMBIE;
  kwakeup(&proc[1]);
  tswitch();
}

int kwait(int *status)
{
  int zombiePID = -1;
  int zomebieThere = 0;

  if(!(running->child))
  {
    kprintf("\nkwait: This proc has no child");
    return -1;
  }

  while (1)
  {
    PROC *maybeZombie = running->child;
    PROC *prevChild = 0;
    if(maybeZombie->status == ZOMBIE)
    {
      zomebieThere = 1;
      }
      else
      {
        while(maybeZombie->sibling)
        {
          prevChild = maybeZombie;
          maybeZombie = maybeZombie->sibling;
          if (maybeZombie->status == ZOMBIE)
          {
            zomebieThere = 1;
            break;
          }
        }
      }
      if (prevChild && zomebieThere)
      {
        prevChild->sibling = maybeZombie->sibling;
        //maybeZombie->sibling = 0; //Clear out this proc for next round
      }
      else if (zomebieThere)
      {
        zombiePID = maybeZombie->pid;
        *status = maybeZombie->exitCode;
        maybeZombie->status = FREE;
        running->child = maybeZombie->sibling;
        enqueue(&freeList, maybeZombie);
        
        return(zombiePID);
      }
      ksleep(running);
    }
}