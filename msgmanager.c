/*******************************************************************************
Copyright:
File_name:       msgmanager.c
Version:	     0.0
Revised:        $Date:2017-1-8  ; $
Description:    massage fifo and manage memory .
Notes:          universal kit
Editor:		    Mr.kon

*******************************************************************************/


/*==============================================================================
@ Include files
*/
#include "msgmanager.h"


/*==============================================================================
@ Global variable
*/
/** the target of managing **/
static MEMORY_INFO MemoryInfo = {{0},MEMORY_SIZE};



/*==============================================================================
@ All functions  as follow
*/

LIST ListHead;

/*******************************************************************************
Function: InitListHead
Description:
massage chain list, when the chain list is null, the chain list point to
hisselef

Input:        the chain list head
Output:       None
Return:       None
Editor:	      Mr.kon
Others:	      2017-1-8
*******************************************************************************/
void InitListHead(PLIST ListHead)
{
  ListHead->Blink = ListHead->Flink = ListHead;
  ListHead->Offset = MemoryInfo.MemoryBuf;
}


/*******************************************************************************
Function: InitListHead
Description:
massage chain list, when the chain list is empty, the chain list point to
hisselef

Input:        the chain list head
Output:       None
Return:       TRUE  the list is empty ,FALSE  the  chain list has massages.
Editor:	      Mr.kon
Others:	      2017-1-8
*******************************************************************************/
BOOL IsListEmpty(PLIST ListHead)
{
  if(ListHead->Flink == ListHead )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

PLIST RemoveTailList(PLIST ListHead)
{
  PLIST Blink;
  PLIST Entry;
  
  Entry = ListHead->Blink;
  Blink = Entry->Blink;
  ListHead->Blink = Blink;
  Blink->Flink = ListHead;
  return Entry;
}
void InsertHeadList(PLIST ListHead,PLIST Entry)
{
  PLIST Flink;
  
  Flink = ListHead->Flink;
  Entry->Flink = Flink;
  Entry->Blink = ListHead;
  Flink->Blink = Entry;
  ListHead->Flink = Entry;
  
}
/******************************************************************************
******************************************************************************/
u8* GetMsgMemSize( PLIST ListHead,u8 * size)
{
  PLIST Blink;
  
  Blink = ListHead->Blink;
  *size =  (u8*)Blink - Blink->Offset;
  return Blink->Offset;
  
}

/***should enty CRITICAL STATUS*/
/*******************************************************************************
Function: GetEmptyMemory
Description:
massage chain list, when the chain list is empty, the chain list point to
hisselef

Input:        PLIST ListHead :the chain list head ;u16 size: How manny bytes will be get.
Output:       None
Return:       TRUE  the list is empty ,FALSE  the  chain list has massages.
Editor:	      Mr.kon
Others:	      2017-1-8
*******************************************************************************/
u8* GetEmptyMemory(PLIST ListHead,u16 size)
{
  u16 LeaveSize = 0;
  /*** Bottom ***/
  if(ListHead->Flink >= ListHead->Blink )
  {
    LeaveSize = MemoryInfo.MemoryBuf + MEMORY_SIZE - (u8*)ListHead->Flink;//need ListHead->Flink + 1?
    if( LeaveSize >=  size)
    {
      return (u8*)(ListHead->Flink + sizeof(PLIST)/2);
    }
  }
  else/* top */
  {
    LeaveSize = ListHead->Blink - ListHead->Flink;
    
    if( LeaveSize >=  size)
    {
      return (u8*)(ListHead->Blink + sizeof(PLIST)/2);
    }
  }
  return 0;
}

/*******************************************************************************
Function:     NewMem
Description:  allocate size of bytes memery and return the point 
Input:      size of bytes you wanne 
Output:       None
Return:       point of block
Others:	      Mr.kon @2017/1/18 revise
*******************************************************************************/
u8* NewMem(u16 size)
{
  u8 *pReturn = NULL;
  if(size > MemoryInfo.MemoryLeaveSize )
  {
    return NULL;
  }
  __disable_interrupt();
  if(IsListEmpty(&ListHead) == TRUE)
  {
    pReturn = MemoryInfo.MemoryBuf;
    
  }
  else
  {
    pReturn = GetEmptyMemory(&ListHead, size+sizeof(LIST)/2);
  }
  
  if(pReturn)
  {
    MemoryInfo.MemoryLeaveSize -= size;
    
    /* insert chain list */
    PLIST plist = (PLIST)(pReturn + size);
    plist ->Offset = pReturn;
    InsertHeadList(&ListHead, plist);
  }
  __enable_interrupt();  
  return pReturn;
  
}
//BOOL FreeMem(u8 *pbuffer)
//{
//  
//  /*do over target buffer ? */
//  if(pbuffer < MemoryInfo.MemoryBuf || pbuffer > (MemoryInfo.MemoryBuf+MEMORY_SIZE))
//  {
//    return FALSE;
//  }
//  __disable_interrupt();
//  
//  PLIST Entry = (PLIST)pbuffer;
//  u16 size = Entry->Flink - Entry->Blink;
//  if( size )
//  {
//    MemoryInfo.MemoryLeaveSize += size;
//    
//    __enable_interrupt();
//    return TRUE;
//  }
//  else
//  {
//    __enable_interrupt();
//    return FALSE;
//  }
//}

/*******************************************************************************
Function:     FreeMem
Description:  remove the list from chainlist tail, and recover size of memery
Input:        
u8* Source : the data copied address
u8* Targe  :  copy data to
Output:       None
Return:       None
Others:	      Mr.kon @2017/1/18 revise
*******************************************************************************/
BOOL FreeMem(u8 size)
{
  __disable_interrupt();
  
  RemoveTailList(&ListHead);

  if( size )
  {
    MemoryInfo.MemoryLeaveSize += size;
    
    __enable_interrupt();
    return TRUE;
  }
  else
  {
    __enable_interrupt();
    return FALSE;
  }
  
}

void InitMsgQue(void)
{
  InitListHead(&ListHead);
}
/*******************************************************************************
Function:     memcopy
Description:  memory copy
Input:        
u8* Source : the data copied address
u8* Targe  :  copy data to
Output:       None
Return:       None
Others:	      koing2010@2016/2/20
*******************************************************************************/
void memcopy(u8* Source,u8* Target,u8 Numb)
{
  while(Numb --)
  {
    *Target++ = *Source++;
  }
}


/*@*****************************end of file**********************************@*/
