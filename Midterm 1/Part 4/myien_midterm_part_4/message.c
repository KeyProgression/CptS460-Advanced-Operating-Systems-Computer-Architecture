/******** message.c file ************/

// READ Chapter 5.13.4.3; Use its code

#define NMBUF 10
struct semaphore nmbuf, mlock;

int checkPid()
{
    return running->pid;
}

int menqueue(MBUF **queue, MBUF *p)
{
    MBUF *q = *queue;
    if(q == 0)
    {
        *queue = p;
        p->next = 0;
        return;
    }
    if ((*queue)->pri < p->pri)
    {
        p->next = *queue;
        *queue = p;
        return;
    }
    while (q->next && p->pri <= q->next->pri)
    {
        q = q->next;
    }
    p->next = q->next;
    q->next = p;  
}

MBUF *mdequeue(MBUF **queue)
{
    MBUF *q = *queue;
    if(q)
        *queue = q->next;
    return q;
}
    
MBUF mbuf[NMBUF], *mbufList;
		    
int msg_init()
{
    int i; MBUF *mp;
    printf("mesg_init()\n");
    mbufList = 0;
    for (i=0; i<NMBUF; i++) // initialize mbufList
        menqueue(&mbufList, &mbuf[i]); // all priority=0, so use menqueue()
    nmbuf.value = NMBUF; nmbuf.queue = 0; // counting semaphore
    mlock.value = 1; mlock.queue = 0; // lock semaphore
}

MBUF *get_mbuf()
{
    P(&nmbuf);
    P(&mlock);
    MBUF *mp = mdequeue(&mbufList);
    V(&mlock);
    return mp;
}

int put_mbuf(MBUF *mp)
{
    P(&mlock);
    menqueue(&mbufList, mp);
    V(&mlock);
    V(&nmbuf);
}

int send(char *msg, int pid)
{
    if (checkPid()<0) // validate receiving pid
        return -1;
    PROC *p = &proc[pid];
    MBUF *mp = get_mbuf();
    mp->pid = running->pid;
    mp->pri = 1;
    strcpy(mp->contents, msg);
    P(&p->mlock);
    menqueue(&p->mqueue, mp);
    V(&p->mlock);
    V(&p->nmsg);
    return 0;
}

int recv(char *msg)
{
    P(&running->nmsg);
    P(&running->mlock);
        MBUF *mp = mdequeue(&running->mqueue);
    V(&running->mlock);
    strcpy(msg, mp->contents);
    int sender = mp->pid;
    put_mbuf(mp);
    return sender;
}


  
