#include "keymap2"

#define KCNTL 0x00 // 
#define KSTAT 0x04
#define KDATA 0x08 // data register
#define KCLK  0x0C // clock divisor register
#define KISTA 0x10 // interrupt status register

typedef volatile struct kbd{
  char *base; // base address of KBD
  char buf[128]; // input buff
  int head, tail, data, room; // control variables
}KBD;

volatile KBD kbd;
int keyset = 0;
int release = 0;
int shiftKey = 0;
int ctrlKey = 0;
int ctrl_c = 0;

int release;       // key release flag


int kbd_init()
{
  KBD *kp = &kbd;
  kp->base = (char *)0x10006000;
  *(kp->base + KCNTL) = 0x10;    // bit4=Enable bit0=INT on // try 0x14
  *(kp->base + KCLK)  = 8;       // ARM manual says clock=8 
  kp->data = 0;
  kp->room = 128;                // counters
  kp->head = kp->tail = 0;       // circular buffer char buf[128]

  release = 0;
}

/*
void kbd_handler()
{
  u8 scode, c;
  KBD *kp = &kbd;

  color = YELLOW;

  scode = *(kp->base + KDATA);   // get scan code of this interrpt

  kputs("kbd interrupt scancode = "); kprintx(scode);

  if (scode == 0xF0){  // it's key release 
     release = 1;      // set release flag
     return;
  }

  if (release == 1){   // scan code after 0xF0
     release = 0;      // reset release flag
     return;
  }

  // map scode to ASCII in lowercase 
  c = ltab[scode];

  kputs("kbd interrupt : "); kputc(c); kputs("\n");

  kp->buf[kp->head++] = c;
  kp->head %= 128;
  kp->data++; kp->room--;
}
*/

oid kbd_handler() // KBD interrupt handler in C
{
    kprintf("TEST KBD\n");

    char kbd_temp[8];
    u8 scode, c;
    int i;
    KBD *kp = &kbd;
    color = RED;                 // int color in vid.c file
    scode = *(kp->base + KDATA); // read scan code in data register

    //  if (scode & 0x80)            // ignore key releases
    //    return;
    //c = unsh[scode]; // map scan code to ASCII
    //c = unshift[scode]; // map scan code to ASCII

    kprintf("TEST KBD\n");

    // Key Release
    if (scode == 0xF0) 
    {
        release = 1;

        return;
    }

    //* Check scan scodes:
    //Left Control Key check
    if (scode == 0x14)
    {
        ctrlKey = 1;
    }

    // Left Shift Key Check
    if (scode == 0x12)
    {
        shiftKey = 1;
    }

    // if realease left control shift key
    if (scode == 0x14 && release)
    {
        //ctrl_d = 0;
        ctrl_c = 0; //Flag for ctrl+c to signal close program
        ctrlKey = 0;
        release = 0;

        return;
    }
    else if (scode == 0x12 && release) //if Left Shift Key release
    {
        shiftKey = 0;
        release = 0;

        return;
    }
    else if (release)
    {
        release = 0;

        return;
    }

    
    if (scode == 0x23 && ctrlKey)
    {
        c = 0x04; //Set to EOF

        kprintf("CTRL + D: %x \n", c);

        kgets(kbd_temp);
    }

    if (scode == 0x21 && ctrlKey)
    {
        kprintf("CTRL + C \n"); //Trying to kill program
        kgets(kbd_temp);

        return;
    }



     
    if (shiftKey)
    {
        c = utab[scode];
    }
    else
    {
        c = ltab[scode];
    }        


    kprintf("kbd interrupt: c=%x %c\n", c, c);
    kgets(kbd_temp);

    int k = 0;

    while(k < 1000000){
        k++;
    }

void kbd_handler_1()
{
    char kbd_temp[8];
    u8 scode, c;
    int i;
    KBD *kp = &kbd;
    color = RED;                 // int color in vid.c file
    scode = *(kp->base + KDATA); // read scan code in data register
    if (scode & 0x80)            // ignore key releases
        return;
    c = utab[scode];
    if (c != '\r')
    {
        kprintf("kbd interrupt: c=%x %c\n", c, c);
        kgets(kbd_temp);
    }
    kp->buf[kp->head++] = c; // enter key into CIRCULAR buf[ ]
    kp->head %= 128;
    kp->data++;
    kp->room--; // update counters
}

int kgetc()
{
  char c;
  KBD *kp = &kbd;

  unlock();                          // unmask IRQ in case it was masked out
  while(kp->data == 0);              // BUSY wait while kp->data is 0 

  lock();                            // mask out IRQ
    c = kp->buf[kp->tail++]; 
    kp->tail %= 128;                 /*** Critical Region ***/
    kp->data--; kp->room++;
    unlock();                        // unmask IRQ
  return c;
}

int kgets(char s[ ])
{
  char c;
  while( (c = kgetc()) != '\r'){
    *s = c;
    s++;
  }
  *s = 0;
}
