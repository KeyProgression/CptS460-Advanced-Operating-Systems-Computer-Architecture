/****************** vid.c file *******************/
int volatile *fb;  // display buffer

int WIDTH = 640;

int fbuf_init()
{
  int i;

  /********* for VGA 640x480 ************************/
  *(volatile unsigned int *)(0x1000001c) = 0x2C77;        // LCDCLK SYS_OSCCLK
  *(volatile unsigned int *)(0x10120000) = 0x3F1F3F9C;    // time0
  *(volatile unsigned int *)(0x10120004) = 0x090B61DF;    // time1
  *(volatile unsigned int *)(0x10120008) = 0x067F1800;    // time2
  *(volatile unsigned int *)(0x10120010) = (0x400000);    // panelBaseAddress
  *(volatile unsigned int *)(0x10120018) = 0x82B;         // control register

  // |code|data|bss|wsu.o|stack| may > 3MB; fb must be higher
  fb = (int *)(0x400000);  // fb[ ] at 4MB; for VGA 640x480
  // clear fb[]
  for (i=0; i<640*480; i++)
    fb[i] = 0x00000000;    // clean screen; all pixels are BLACK
}

int show_bmp(char *p, int startRow, int startCol)
{ 
   int h, w, pixel, r2, i, j; 
   unsigned char r, g, b;
   char *pp;
 
   int *q = (int *)(p+14); // skip over 14 bytes file header 
   q++;                    // skip 4 bytes in image header
   w = *q;                 // width in pixels 
   h = *(q + 1);           // height in pixels

   p += 54;                // p point at pixels now 

   // AS SHOWN, the picture is up-side DOWN fb[ (i/2) * 640 + (j/2)] = pixel;
   r2 = 4*((3*w+3)/4);     // row size is a multiple of 4 bytes  

   int myht = h/2, myw = w/2;

   for (i=startRow; i<h+startRow; i++){
     pp = p;
     
     for (j=startCol; j<startCol+w; j++){
         b = *pp;  // Points to Blue Pixel
         g = *(pp+1); // Points to Green Pixel
         r = *(pp+2); // Points to Red pixel
         pixel = (b<<16) + (g<<8) + r;
         fb[(i/2)*WIDTH + (j/2)] = pixel;
         pp += 3;    // back pp by 3 bytes
     }
     p -= r2;
   }

   // REQUIRED: use YOUR uprintf() of Part 1 to print h,w to UART0
   ufprintf(&uart[0],"\nBMP image height=%d width=%d\n", myht, myw);

}