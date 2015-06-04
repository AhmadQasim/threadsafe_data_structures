#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include "hash.h"

typedef struct hashT{
	struct hashT *next;
	struct hashT *prev;
	struct hashT *bprev;
	struct hashT *right;
	struct hashT *left;
	int val;
}hashT;

int *size;

typedef struct buckets {
	hashT **headsL;
	hashT **tailsL;
    hashT **heads;
}buckets;

int hashCal(int ele);
void Binsert (hashT *node);
void Bdele (hashT *node);
bool searchBST (int ele);
hashT *searchBSTE (int ele);

static buckets *Hnode;
static int numofBucks = 0;

pthread_mutex_t **lock;
pthread_mutexattr_t **attr;


void Hash_Init(int numOfBuckets) {
	numofBucks = numOfBuckets-1;
	int i = 0;
	Hnode = malloc(sizeof(buckets));
	Hnode->headsL = malloc(sizeof(hashT*)*numOfBuckets);
	Hnode->tailsL = malloc(sizeof(hashT*)*numOfBuckets);
	Hnode->heads = malloc(sizeof(hashT*)*numOfBuckets);
	size = malloc(sizeof(int)*numofBucks);
	lock = malloc(sizeof(pthread_mutex_t*)*numOfBuckets);
	attr = malloc(sizeof(pthread_mutexattr_t*)*numOfBuckets);
	for (i=0; i<=numofBucks;i++){
		attr[i] = malloc(sizeof(pthread_mutexattr_t));
		lock[i] = malloc(sizeof(pthread_mutex_t));
		Hnode->headsL[i] = NULL;
		Hnode->tailsL[i] = NULL;
		Hnode->heads[i] = NULL;
		pthread_mutexattr_init(attr[i]);
		pthread_mutex_init(lock[i], attr[i]);
	}
}

int  Hash_Insert(int aNumber) {
	int temp = hashCal(aNumber);
	pthread_mutex_lock(lock[temp]);
	
	
	if (searchBST(aNumber)){
		pthread_mutex_unlock(lock[temp]);
		return -1;
	}
	if (Hnode->headsL[temp] == NULL){
		hashT *node = malloc(sizeof(hashT));
		node->next = NULL;
		node->prev = NULL;
		node->bprev = NULL;
		node->right = NULL;
		node->left = NULL;
		node->val = aNumber;
		Hnode->headsL[temp] = node;
		Hnode->tailsL[temp] = node;
		Hnode->heads[temp] = node;
		size[temp]++;
		pthread_mutex_unlock(lock[temp]);
		return 0;
	}
	else {
		hashT *newN = malloc(sizeof(hashT));
		newN->next = Hnode->headsL[temp];
		newN->prev = NULL;
		newN->bprev = NULL;
		newN->right = NULL;
		newN->left = NULL;
		newN->val = aNumber;
		Hnode->headsL[temp]->prev = newN;
		Hnode->headsL[temp] = newN;
		size[temp]++;
		Binsert(newN);
		}
		pthread_mutex_unlock(lock[temp]);
	return 0;
}

int hashCal(int ele) {
	return ele%(numofBucks+1);
}

int  Hash_CountElements() {
	int sum = 0, i =0;
	for (i =0;i<=numofBucks; i++){
		sum += size[i];
	}
	return sum;
}

int  Hash_CountBucketElements(int bucketNumber) {
	return size[bucketNumber];
}

int  Hash_Remove(int aNumber) {
	int temp = hashCal(aNumber);
	pthread_mutex_lock(lock[temp]);
	
	
	hashT *node;
	if (!(node = searchBSTE(aNumber))){
		pthread_mutex_unlock(lock[temp]);
		return -1;
	}
	Bdele(node);
	if (node==Hnode->headsL[temp]){
		Hnode->headsL[temp] = Hnode->headsL[temp]->next;
		Hnode->headsL[temp]->prev = NULL;
	}
	else if (node == Hnode->tailsL[temp]){
		Hnode->tailsL[temp] = Hnode->tailsL[temp]->prev;
		Hnode->tailsL[temp]->next = NULL;
	}
	else{
		node->prev->next = node->next;
		node->next->prev = node->prev;
	}
	size[temp]--;
	pthread_mutex_unlock(lock[temp]);
	return 0;
}

void Hash_Dump() {
	int i = 0;
	for (i=0;i<=numofBucks;i++){
		hashT *node = Hnode->headsL[i];
		printf("%d : ", i);
		while (node!=NULL){
			printf("%d->", node->val);
			node=node->next;
		}
		printf("\n");
	}
}

void Binsert (hashT *node) {
	int temp = hashCal(node->val);
	hashT *curr = Hnode->heads[temp];
	hashT *prev = Hnode->heads[temp];
	if (curr==NULL){
		Hnode->heads[temp]=node;
		return;
	}
	while (curr!=NULL){
		prev = curr;
		if (curr->val>node->val)
			curr = curr->left;
		else
			curr = curr->right;
	}
	if (prev->val>node->val)
		prev->left = node;
	else
		prev->right = node;

	node->bprev = prev;
}

bool searchBST(int ele) {
	int temp = hashCal(ele);
	if (Hnode->heads[temp] == NULL)
		return false;
	hashT *node = Hnode->heads[temp];
	while (node!=NULL){
		
		if (node->val<ele){
			
			node = node->right;
		}
		else if (node->val>ele){
			
			node = node->left;
		}
		else if (node->val == ele){
			return true;
		}
	}
	return false;
}

hashT *searchBSTE(int ele) {
	int temp = hashCal(ele);

	if (Hnode->heads[temp]==NULL){
		return NULL;
	}
	hashT *node = Hnode->heads[temp];
	while (node!=NULL){
		
		if (node->val<ele)
			node = node->right;
		else if (node->val>ele)
			node = node->left;
		else if(node->val == ele){
			
			return node;
		}
	}
	return NULL;
}

void Bdele (hashT *node) {
	int temp = hashCal(node->val);
	if (node!=Hnode->heads[temp]){
		if (node->left == NULL && node->right == NULL){
			if (node->bprev->val > node->val)
				node->bprev->left = NULL;
			else
				node->bprev->right = NULL;
		}
		else if (node->left == NULL && node->right != NULL){

			node->right->bprev = node->bprev;

			if (node->bprev->val > node->val){
				node->bprev->left = node->right;
			}
			else{
				node->bprev->right = node->right;
			}
		}
		else if (node->left != NULL && node->right == NULL){
			
			node->left->bprev = node->bprev;
			if (node->bprev->val > node->val){
				node->bprev->left = node->left;
			}
			else{
				node->bprev->right = node->left;
			}
		}
		else if (node->left != NULL && node->right != NULL){
			hashT *pred = node;
			pred = pred->left;
			while (pred->right!=NULL)
				pred=pred->right;
			
			Bdele(pred);

			if (node->left == NULL)
				node->right->bprev = pred;
			else if (node->right == NULL)
				node->left->bprev = pred;
			else {
				node->left->bprev = pred;
				node->right->bprev = pred;
			}
			if (node->bprev->val > node->val){
				node->bprev->left = pred;
			}
			else{
				node->bprev->right = pred;
			}

			pred->bprev = node->bprev;
			pred->left = node->left;
			pred->right = node->right;
		}
	}
	else if (node==Hnode->heads[temp]){
		
		if (node->left == NULL && node->right == NULL){
			Hnode->heads[temp] = NULL;
			if (Hnode->heads[temp] != NULL)
			
			return;
		}
		else if (node->left == NULL && node->right != NULL){
		
			Hnode->heads[temp] = node->right;
		}
		else if (node->left != NULL && node->right == NULL){
	
			Hnode->heads[temp] = node->left;
		}
		else if (node->left != NULL && node->right != NULL){

			hashT *pred = node;
			pred = pred->left;
			while (pred->right!=NULL)
				pred=pred->right;
			
			
		
			Bdele(pred);
			if (node->left == NULL)
				node->right->bprev = pred;
			else if (node->right == NULL)
				node->left->bprev = pred;
			else {
				node->left->bprev = pred;
				node->right->bprev = pred;
			}
			pred->bprev = node->bprev;
			pred->left = node->left;
			pred->right = node->right;

			Hnode->heads[temp] = pred;
			
		}
	}
}
