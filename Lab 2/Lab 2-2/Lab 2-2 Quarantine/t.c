#include "defines.h"
#include "vid.c"
#include "uart.c"


int color;
char *tab = "0123456789ABCDEF";

/* typedef struct uart{
  char *base;
  int  n;
}UART; */

extern UART uart[4];
extern UART *up;

extern char _binary_wsu_bmp_start;
extern int ufprintf(char *fmt, ...);

int color;


int main()
{
   char c, *p;
   int mode;
   uart_init();
   up = &uart[0];

   mode = 0;
   fbuf_init(mode);

   p = &_binary_wsu_bmp_start;
   show_bmp(p, 0, 0); 

   while(1){
     ufprintf("enter a key from this UART : ");
     ugetc(up);
     p = &_binary_wsu_bmp_start;
     show_bmp(p, 0, 0);
   }
   while(1);   // loop here  
}