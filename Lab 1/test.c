/*******************************************************
*                  @t.c file                          *
*******************************************************/

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

#define TRK 18
#define CYL 36
#define BLK 1024

#include "ext2.h"
typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;
GD    *gp;
INODE *ip;
DIR   *dp;

u16 NSEC = 2;
char buf1[BLK], buf2[BLK];
int color = 0x0A;
u8 ino;

int prints(char *s)
{
	while(*s)
	{
		putc(*s++);
	}
}

int gets(char *s)
{
	while((*s = getc() != 13))
	{
		putc(*s++);
	}
}

u16 getblk(u16 blk, char *buf)
{
    readfd( (2*blk)/CYL, ( (2*blk)%CYL)/TRK, ((2*blk)%CYL)%TRK, buf);

    // readfd( blk/18, ((blk)%18)/9, ( ((blk)%18)%9)<<1, buf);
}

u16 search(INODE *ip, char *search_term)
{
for(int i=0; i < 12; i++)
    {
        if ((u16)ip->i_block[i] == 0)
        {
            break;
        }

        getblk((u16)ip->i_block[i], buf2);
        dp = (DIR *)buf2;
        while((char *)dp < buf2 + BLK)
        {
            dp->name[dp->name_len] = 0;
            prints(dp->name);
            putc(' ');

            if (strcmp(search_term, dp->name) == 0)
            {
                prints("\n\r");
                return (u16)dp->inode;
            }
            dp = (char *)dp + dp->rec_len;
        }
   }
}

main()
{ 
  u32 *up;
  u16 i, iblk;
  char   c, temp[64];
  
  //1. Write YOUR C code to get the INODE of /boot/mtx
  getblk(2, buf1);


      // 1.1 WRITE YOUR CODE to get iblk = bg_inode_table block number
  gp = (GD *)buf1; // group descriptor name
  iblk = (u16)gp->bg_inode_table; // point to start of inode table
  //prints("inode_block="); putc(iblk+'0'); prints("\n\r");


      // 1.2 WRITE YOUR CODE to get root inode
  //prints("read inodes begin block to get root inode\n\r");
  getblk(iblk,buf1);
  ip = (INODE *)buf1 +1;
  ip++; // next inode is == root

   //1.3 find /boot
  ino = search(ip, "boot") - 1; // get boot inode number
  getblk(iblk + (ino/8),buf2); // mailman algorithm
  ip = (INODE *)buf2 + (ino %8); // ip now points to boot inode number

   // 1.4 find /mtx
  ino  = search(ip, "mtx") - 1;
  getblk(iblk+(ino/8),buf2);
  ip = (INODE *) buf2 + (ino % 8); //ip now points to mtx inode #

  if((u16)ip->i_block[12])
  {
     getblk((u16)ip->i_block[12],buf1);
  }



//2. setes(0x1000);  // MTX loading segment = 0x1000
   setes(0x1000);

//3. load 12 DIRECT blocks of INODE into memory
   for (i=0; i<12; i++){
      getblk((u16)ip->i_block[i], 0);
      putc('*');
      inces();
   }



//4. load INDIRECT blocks, if any, into memory
   if (ip->i_block[12]){
     up = (u32 *)buf2;      
     while(*up){
        getblk((u16)*up, 0); putc('.');
        inces();
        up++;
     }
  }
  prints("go?\n\r"); getc();
}  