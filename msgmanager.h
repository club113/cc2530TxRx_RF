/*******************************************************************************

File_name:       msgmanager.h
Description:    the header file of  msgmanager.c .

*******************************************************************************/
#ifndef     __MSGMANAGER_H__
#define     __MSGMANAGER_H__

/*==============================================================================
@ Include files
*/
// #include ".h"
// #include ".c"
#include <ioCC2530.h>
/*==============================================================================
@ Typedefs
*/
#define   __disable_interrupt();  EA =0;  
#define   __enable_interrupt();    EA = 1;  
#define NULL 0
typedef enum 
{
  FALSE,
  TRUE
}BOOL;
typedef unsigned char  u8;
typedef unsigned int   u16;

/*==============================================================================
@ Constants and defines
*/
/* MemoryBuf[MEMORY_SIZE] is where the fifo message chain list was save in */
#define MEMORY_SIZE         2048 //bytes

typedef struct MEMORY_INFO_
{
	u8 MemoryBuf[MEMORY_SIZE];
	u16 MemoryLeaveSize;
}MEMORY_INFO,*PMEMORY_INFO;

typedef struct _LIST
{
   struct _LIST *Flink;
   struct _LIST *Blink;
   u8* Offset;
} LIST,*PLIST;




#define LISTSIZE   sizeof(MEMORY_INFO)

extern LIST ListHead;

void InitListHead(PLIST ListHead);
BOOL IsListEmpty(PLIST ListHead);
PLIST RemoveTailList(PLIST ListHead);
u8* GetEmptyMemory(PLIST ListHead,u16 size);
u8* NewMem(u16 size);
BOOL FreeMem(u8 size);
void InitMsgQue(void);


void InsertHeadList(PLIST ListHead,PLIST Entry);
void memcopy(u8* Source,u8* Target,u8 Numb);
u8* GetMsgMemSize( PLIST ListHead,u8 * size);
#endif
/*@*****************************end of file**********************************@*/


