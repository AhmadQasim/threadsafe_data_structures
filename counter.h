#ifndef __counter_h__
#define __counter_h__

#include <pthread.h>  

void Counter_Init();
int  Counter_GetValue();
void Counter_Increment();
void Counter_Decrement();




#endif
