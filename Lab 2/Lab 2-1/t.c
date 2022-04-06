/**************** t.c file **************/
#include "uart.c"

int v[10] = {1,2,3,4,5,6,7,8,9,10};
int sum;

int main()
{
  int i;
  char line[128];
  UART *up;
  
  uart_init();
  up = &uart[0];
  while(1){
    ufprintf(up, "enter a line : ");
    ugets(up, line);
    if (line[0]==0)
      break;
    ufprintf(up, "  line = "); uprints(up, line); ufprintf(up, "\n\r");
  }
  
  ufprintf(up, "Compute sum of array\n\r");
  sum = 0;
  for (i=0; i<10; i++)
    sum += v[i];
  ufprintf(up, "sum = ");
  uputc(up, (sum/10)+'0'); uputc(up, (sum%10)+'0');
  ufprintf(up, "\n\r");
  
  ufprintf(up, "END OF UART DEMO\n\r");
}