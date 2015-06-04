#ifndef __LRU_h__
#define __LRU_h__

static int g_size;

void LRU_Init(int size);
int LRU_Insert(int element);
int LRU_Access(int element);
int LRU_Remove(int element);
int LRU_Size();
void LRU_Dump(); // just for your own sake

#endif // __LRU_h__
