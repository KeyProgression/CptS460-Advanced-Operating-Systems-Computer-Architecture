// pipe.c file

#define PSIZE 8

typedef struct pipe{
  char buf[PSIZE];
  int head, tail, data, room;
  int status;
  int nreader, nwriter;    // number of reader, writer on this pipe
}PIPE;


#define NPIPE  4
PIPE pipe[NPIPE];

int pipe_init()
{
  printf("pipe_init();");
  //status = 0 mean's pipe is free
  for(int i = 0; i < NPIPE; i++)
    pipe[i].status = 0;

}

// PIPE *create_pipe(): this creates a PIPE object in the (shared) address space of all the processes. It allocates a free PIPE
// object, initializes it and returns a pointer to the created PIPE object.
PIPE *create_pipe()
{
  PIPE *tPipe;
  printf("create_pipe();");
  for (int i = 0; i < NPIPE; i++)
  {
    tPipe = &pipe[i];
    if (tPipe->status == 0)
    {
      tPipe->status = 1;
      break;
    }
    
  }

  tPipe->head = tPipe->tail = 0; tPipe->room = PSIZE; tPipe->data = 0;
  
  printf("Pipe created! Ok;");
  return tPipe;
}


int read_pipe(PIPE *p, char *buf, int n)
{
  int byteswritten = 0; //
  if(n<=0)
    return 0;
  if (p->data == 0 && n)
  {
    return 0;
  }
  
  if (p->status != 0)
  {
    while(n)
    {
      while (p->data)
      {
        *buf = p->buf[p->tail++];
        p->tail %= PSIZE;
        p->data--;
        p->room++;
        buf++;
        byteswritten++;
        n--;
        if (n==0)
          break;      
      }

      kwakeup((int)&p->room);

      // return number of bytes written to buf
      if (byteswritten)
      {
        return byteswritten;
      }
      ksleep((int)&p->data);
    } 
  } 
}

/*
When a writer writes to a pipe, if the pipe has room, it writes as much as it needs to or until the pipe is full. If the pipe
has no room but still has readers, the writer waits for room. When a reader reads data from the pipe to create more rooms, it
wakes up the waiting writers, allowing them to continue. However, if a pipe has no more readers, the writer must detect this
as a broken pipe error and aborts.
*/
int write_pipe(PIPE *p, char *buf, int n)
{
  // fill in code;
  // add code to detect BROKEN pipe: print BROKEN pipe message, then exit
  if (n<=0)
  {
    return 0;
  }
  
  int written = 0;
  if (p->status != 0)
  {
    while (n)
    {
      while (p->room)
      {
        p->buf[p->head++] = *buf;
        p->head %= PSIZE;
        p->data++;
        p->room--;
        buf++;
        written++;
        n--;
        if(n == 0)
        {
          break;
        }
        if(p->nreader == 0)
        {
          printf("Error broken pipes, %d bytes need to be written", n);
        }
      }
        kwakeup((int)&p->data);
        if(n == 0)
        {
          printf("proc%d wrote %d bytes\n", running->pid, written);
          return written;
        }
        ksleep((int)&p->room); 
    }
  }
}


  
