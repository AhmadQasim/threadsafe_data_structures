#ifndef __hash_h__
#define __hash_h__

#include <pthread.h>  

void Hash_Init(int numOfBuckets);
int  Hash_CountElements();
int  Hash_CountBucketElements(int bucketNumber);
int  Hash_Insert(int aNumber);
int  Hash_Remove(int aNumber);
void Hash_Dump(); 

#endif
