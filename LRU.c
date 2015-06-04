#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include "lru.h"

typedef struct lruL{
	struct lruL *next;
	struct lruL *prev;
	struct lruL *left;
	struct lruL *right;
	struct lruL *bprev;
	int val;
}lruL;

typedef struct hashT {
	int size;
	lruL **heads;
}hashT;

static lruL *headL = NULL, *tailL = NULL;
static hashT *Hnode;
static int currS = 0, numofBucks = 0;

pthread_mutex_t lock;
pthread_mutexattr_t attr;

void Binsert (lruL *node);
void Bdele (lruL *node);
bool searchBST (int ele);
lruL *searchBSTE (int ele);
void treeDump(lruL *node);
int hashCal(int ele);


void LRU_Init(int size) {
	int i =0;
	Hnode = malloc(sizeof(hashT));
	numofBucks = 2;
	Hnode->heads = malloc(sizeof(lruL*)*numofBucks);
	for (i=0; i<numofBucks;i++){
		Hnode->heads[i] = NULL;
	}
	pthread_mutexattr_init(&attr);
	pthread_mutex_init(&lock, &attr);
	g_size = size;
}

int LRU_Insert(int element) {

	pthread_mutex_lock(&lock);
	if (element < 1){
		pthread_mutex_unlock(&lock);
		return -1;
	}

	int temp = hashCal(element);
	if (searchBST(element)){
	
		pthread_mutex_unlock(&lock);
		return -1;
	}
	if (headL == NULL){

		Hnode->heads[temp] = malloc(sizeof(lruL));
		Hnode->heads[temp]->next = NULL;
		Hnode->heads[temp]->prev = NULL;
		Hnode->heads[temp]->left = NULL;
		Hnode->heads[temp]->right = NULL;
		Hnode->heads[temp]->val = element;
		Hnode->heads[temp]->bprev = NULL;
		headL = Hnode->heads[temp];
		tailL = Hnode->heads[temp];
		currS++;
		pthread_mutex_unlock(&lock);
		return 0;
	}
	else {
		lruL *newN = malloc(sizeof(lruL));
		newN->next = headL;
		newN->prev = NULL;
		newN->val = element;
		newN->left = NULL;
		newN->right = NULL;
		newN->bprev = NULL;
		headL->prev = newN;
		headL = newN;
		if (currS ==g_size){
		Bdele(tailL);
		tailL = tailL->prev;
		tailL->next = NULL;
		}
		Binsert(newN);

		currS++;
	}
	if (currS == g_size+1){
		int temp = tailL->val;
		currS--;
		pthread_mutex_unlock(&lock);
		return temp;
	}
	else {
		pthread_mutex_unlock(&lock);
		return 0;
	}
}

void Binsert (lruL *node) {
	int temp = hashCal(node->val);
	lruL *curr = Hnode->heads[temp];
	lruL *prev = Hnode->heads[temp];
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

void Bdele (lruL *node) {
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
			lruL *pred = node;
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

			lruL *pred = node;
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

bool searchBST(int ele) {
	int temp = hashCal(ele);
	if (Hnode->heads[temp] == NULL)
		return false;
	lruL *node = Hnode->heads[temp];
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

lruL *searchBSTE(int ele) {
	int temp = hashCal(ele);

	if (Hnode->heads[temp]==NULL){
		return NULL;
	}
	lruL *node = Hnode->heads[temp];
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

void treeDump(lruL *node) {
	if (node!=NULL){
	if (node->left != NULL && node->right != NULL){
			printf("%d\t", node->val);
		treeDump(node->left);
		treeDump(node->right);
	}
	if (node->left == NULL && node->right != NULL){

			printf("%d\t", node->val);
		treeDump(node->right);
	}
	if (node->left != NULL && node->right == NULL){

			printf("%d\t", node->val);
		treeDump(node->left);
	}
	else if (node->left == NULL && node->right == NULL){

		printf("%d\t", node->val);
	}
}
	return;
}

int LRU_Access(int element) {
	pthread_mutex_lock(&lock);
	if (element==headL->val){
		pthread_mutex_unlock(&lock);
		return 0;
	}
	lruL *node;
	if (!(node = searchBSTE(element))){
		pthread_mutex_unlock(&lock);
		return -1;
	}
	if (node==tailL){
		tailL = tailL->prev;
		tailL->next = NULL;
	}
	else {
		node->prev->next = node->next;
		node->next->prev = node->prev;
	}
	headL->prev = node;
	node->next = headL;
	headL = node;
	headL->prev = NULL;
	pthread_mutex_unlock(&lock);
	return 0;
}

void LRU_Dump () {
	lruL *node = headL;
	while (node!=NULL){
		printf("%d\t", node->val);
		node=node->next;
	}
	printf("\n");
}

int LRU_Size() {
	return currS;
}

int LRU_Remove(int element) {
	pthread_mutex_lock(&lock);
	lruL *node;
	if (!(node = searchBSTE(element))){
		pthread_mutex_unlock(&lock);
		return -1;
	}
	Bdele(node);
	if (node==headL){
		headL = headL->next;
		if (headL!=NULL)
			headL->prev = NULL;
	}
	else if (node == tailL){
		tailL = tailL->prev;
		tailL->next = NULL;
	}
	else{
		node->prev->next = node->next;
		node->next->prev = node->prev;
	}

	pthread_mutex_unlock(&lock);
	currS--;
	return 0;
}

int hashCal(int ele){
	return ele%numofBucks;
}
