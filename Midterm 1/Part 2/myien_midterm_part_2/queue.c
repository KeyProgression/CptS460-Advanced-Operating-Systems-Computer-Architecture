// queue.c file

int enqueue(PROC **queue, PROC *p)
{
  PROC *q  = *queue;
  if (q==0){
    *queue = p;
    p->next = 0;
    return;
  }
  if ((*queue)->priority < p->priority){
    p->next = *queue;
    *queue = p;
    return;
  }
  while (q->next && p->priority <= q->next->priority){
    q = q->next;
  }
  p->next = q->next;
  q->next = p;
}



PROC *dequeue(PROC **queue)
{
  PROC *p = *queue;
  if (p)
    *queue = p->next;
  return p;
}

int treeEnqueue(PROC *parent, PROC* newChild)
{
  if(!(parent->child))
  {
    parent->child = newChild;
    parent->child->parent = parent;
  }
  else
  {
    PROC* temp = parent->child;
    // get to the end leaf
    while (temp->sibling != 0)
    {
      temp = temp->sibling;
    }
    temp->sibling = newChild;
    temp->sibling->parent = parent;
  }
}

int printQ(PROC *p)
{
  kprintf("readyQueue = ");
  while(p){
    kprintf("[%d%d]->", p->pid,p->priority);
    p = p->next;
  }
  kprintf("NULL\n");
}

int printSleepList(PROC *p)
{
  printf("sleepList   = ");
   while(p){
     kprintf("[%devent=%d]->", p->pid,p->event);
     p = p->next;
  }
  kprintf("NULL\n"); 
}

int printList(char *name, PROC *p)
{
  kprintf("%s = ", name);
   while(p){
     kprintf("[%d%d]->", p->pid, p->priority);
     p = p->next;
  }
  kprintf("NULL\n"); 
}

int printChildList(PROC *p)
{
  PROC *temp =p;
  char statusS[10];
  printf("Proc[%d] childList:\n",p->pid);
  if (temp->child)
  {
    if (temp->child->status==1)
    {
      strcpy(statusS, "READY");
    }
    else if (temp->child->status == 4)
    {strcpy(statusS,"ZOMBIE");}
    
    printf("[%d, %s] -->",temp->child->pid,statusS);
    temp = temp->child->sibling;
    
    if (temp)
    {
      while (temp)
      {
        printf("%d, %s] -->",temp->pid);
        temp = temp->sibling;
      }
    }
    printf("NULL || End of List Reached\n");
  }
  
}

int inOrderTimerRequeue(PROC **queue, PROC *p)
{
    PROC *q = *queue;
    if(q == 0)
    {
        *queue = p;
        p->next=q;
        return;
    }
    while(q->next)
    {
        q = q->next;
    }
    q->next = p;
}

int timerEnqueue(PROC **queue, PROC *p)
{
    PROC *q = *queue;
    PROC *decrementFromhere;
    PROC *previous;

    if (q == 0 || p->priority < q->priority)
    {
        *queue = p;
        p->next = q;
        decrementFromhere = q;

        q->priority -= p->priority;

        return -1;
    }

    previous = q;

    while (q->next && p->priority >= q->next->priority)
    {
        p->priority -= q->next->priority;
        previous = q;
        q = q->next;        
    }
    int decrementValue = p->priority;
    PROC *temp = q->next;
    q->next = p;
    p->next = temp;
    decrementFromhere = temp;
    decrementFromhere->priority -= decrementValue;
}

int printTQE(PROC *p)
{
    printf("|| TimeQueue =");
    while (p)
    {
        printf("[Proc[%d]|RTime[%d]->", p->pid, p->priority);
        p = p->next;
    }
    printf("NULL ||\n");
}