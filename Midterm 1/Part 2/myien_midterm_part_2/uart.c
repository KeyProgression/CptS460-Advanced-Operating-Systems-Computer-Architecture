/**************** uart.c file ****************/
#define UDR   0x00
#define UFR   0x18

#define RXFE 0x10
#define TXFF 0x20

extern char *tab;

typedef struct uart{
  char *base;
  int n;
}UART;

UART uart[4];

int uart_init()
{
  int i; UART *up;

  for (i=0; i<4; i++){
    up = &uart[i];
    up->base = (char *)(0x101F1000 + i*0x1000);
    up->n = i;
  }
  uart[3].base = (char *)(0x10009000); // uart3 at 0x10009000
}

int ugetc(UART *up)
{
  while (*(up->base + UFR) & RXFE);
  return *(up->base + UDR);
}

int uputc(UART *up, char c)
{
  while(*(up->base + UFR) & TXFF);
  *(up->base + UDR) = c;
}

int ugets(UART *up, char *s)
{
  while ((*s = (char)ugetc(up)) != '\r'){
    uputc(up, *s);
    s++;
  }
 *s = 0;
}

int uputs(UART *up, char *s)
{
  while(*s){
    uputc(up, *s);
    if (*s == '\n')
      uputc(up, '\r');
    s++;
  }
}

/** WRITE YOUR uprintf(UART *up, char *fmt, . . .) for formatted print **/

int uprints(UART *up, char *s)
{
  while(*s)
    uputc(up, *s++);
}

int urpu(UART *up, unsigned int x)
{
  char c;
  if (x)
  {
    c = tab[x%10];
    urpu(up, x/10);
    uputc(up, c);
  }
}

int uoct(UART *up, unsigned int x)
{
  char c;
  if (x)
  {
    c = tab[x % 8];
    uoct(up, x / 8);
    uputc(up, c);
  }
}

int uhex(UART *up, unsigned int x)
{
  char c;
  if (x)
  {
    c = tab[x % 16];
    uhex(up, x / 16);
    uputc(up, c);
  }
}

/*1. Create possible formats to print*/
int uprintu(UART *up, unsigned int x)
{
  if (x==0) uputc(up, '0');
  else urpu(up, x);
  uputc(up, ' ');
}

int printx(UART *up, unsigned int x) // print in hex
{
  if (x == 0) uputc(up, '0');
  else uhex(up, x);
  uputc(up, ' ');
}

int printd(UART *up, int x) // print decimal base 10
{
  if (x == 0) uputc(up, '0');
  else urpu(up, x);
  uputc(up, ' ');
}

int printo(UART *up, unsigned int x) // print in oct
{
  if(x == 0) uputc(up, '0'); 
  else uoct(up, x);
  uputc(up, ' ');
}


int ufprintf(UART *up, char *fmt, ...)
{
  int i = 0;
  int negativeFlag = 0;
  
  char *cp;
  int *ip;
  cp = fmt;  
  ip = (int *)&fmt; 
  char *temp = cp;
  
  while (*cp)
  {
    if (*cp == '\n')
    {
      uputc(up, '\n');
      uputc(up,'\r');
      cp++;
    }
    else if (*cp != '%')
    {
      uputc(up, *cp);
      cp++;
    }
    else
    {
        i++;
        cp++; // progress character pointer to the right
        switch ((int)*cp)
        {
          // char
          case 'c':
            uputc(up,*(++ip));
            uputc(up, ' ');
            cp++;
              break;
          // string
          case 's':
            uprints(up, (char *)(*(++ip)));
            cp++;
              break;
            // signed int
            case 'd':
              negativeFlag = (*(++ip));
              if (negativeFlag < 0)
              {
                uputc(up, '-');
                negativeFlag = negativeFlag * -1;
              }
              printd(up, negativeFlag);
              cp++;
                break;
            // unsigned int in oct
            case 'o':
              printo(up, *(++ip));
              cp++;
                break;
            // unsigned int
            case 'u':
              uprintu(up, *(++ip));
              cp++;
                break;
            // unsgined int in hex
            case 'x':
              printx(up, *(++ip));
              cp++;
                break;
            default:
                break;
        }
    }
  }
}