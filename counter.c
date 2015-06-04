#include<stdio.h>
#include<stdlib.h>
#include"counter.h"
#include<string.h>
#include <time.h>

pthread_mutex_t lock;
pthread_mutexattr_t attr;
volatile int balance;

void Counter_Init() {
	balance = 0;
	pthread_mutexattr_init(&attr);
	pthread_mutex_init(&lock, &attr);
}
int  Counter_GetValue() {
	return balance;
}
void Counter_Increment() {
	pthread_mutex_lock(&lock);
	balance++;
	pthread_mutex_unlock(&lock);
}
void Counter_Decrement() {
	pthread_mutex_lock(&lock);
	balance--;
	pthread_mutex_unlock(&lock);
}
