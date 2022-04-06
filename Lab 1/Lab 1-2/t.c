/*******************************************************
*                      t.c file                        *
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

char buf1[BLK], buf2[BLK];
int color = 0x0A;
u8 ino;

main()
{ 
  u16    i, iblk;
  char   c, temp[64];

  //prints("read block# 2 (GD)\n\r");
  getblk(2, buf1);


// 1. WRITE YOUR CODE to get iblk = bg_inode_table block number
  gp = (GD *)buf1; // group descriptor name
  iblk = (u16)gp->bg_inode_table; // point to start of inode table
  prints("inode_block="); putc(iblk+'0'); prints("\n\r"); 

// 2. WRITE YOUR CODE to get root inode
  //prints("read inodes begin block to get root inode\n\r");
  getblk(iblk,buf1);
  ip = (INODE *)buf1 +1;
  ip++; // next inode is == root



// 3. WRITE YOUR CODE to step through the data block of root inode
   for( i = 0; i < 12; i++)
   {
	   if(i < 12)
	   {
		   prints("Block,i = ");
		   putc(i+'0');
		   prints("\n\r");
	   }
	   
	   if (ip->i_block[i] == 0)
	   {
		   prints("No data in this block\n\r");
		   break;
	   }
	   else
	   {
		   getblk((u16)ip->i_block[i],buf2);
		   
		   dp = (DIR *) buf2;

		   while ((char *) dp < buf2 + BLK)
		   {
			   /*
			   strncpy(temp,dp->name,dp->name_len);
			   if(temp || strlen(temp + '\0') == 0) {whileflag = 1;}
			   else
			   {
				   prints(temp);
				   putc(' ');

				   dp = (DIR *)((char *)dp + dep->rec_len);
			   }
			   */
			  dp->name[dp->name_len] = 0;
			  prints(dp->name);prints("\n\r");
		   }
		   prints("\n\r");
		   
	   }
   }
   prints("read data block of root DIR\n\r");
   
// 4. print file names in the root directory /

}  

int prints(char *s)
{
	while(*s)
	{
		putc(*s++);
	}
}

int gets(char *s) 
{
	while((*s = getc() != 0x0D))
	{
		putc(*s++);
	}
	s++;
	*s=0;
}

int getblk(u16 blk, char *buf)
{
   readfd( (2*blk)/CYL, ( (2*blk)%CYL)/TRK, ((2*blk)%CYL)%TRK, buf);
  //readfd( blk/18, ((blk)%18)/9, ( ((blk)%18)%9)<<1, buf);
}
