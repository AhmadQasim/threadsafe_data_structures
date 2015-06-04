#include "testlru.h"
#include "lru.h"

// play around with these values
#define MIN_NUM_OF_THREADS  5
#define MIN_OPS_PER_THREAD 200000
#define MIN_NUM_OF_BUCKETS 500

#define ADDITIONAL_NUM_OF_THREADS 1
#define ADDITIONAL_OPS_PER_THREAD 10000
#define ADDITIONAL_NUM_OF_BUCKETS 10

int num_threads = 0;

// ******************************** let's begin

int main() 
{
    inits();
    start_threads();
    return 0;
}

void inits()
{
    int sizeOfList;

    // init srand
    struct timeval tv;
    gettimeofday(& tv, NULL);
    srand(tv.tv_usec);
    
    sizeOfList = rand() % ADDITIONAL_NUM_OF_BUCKETS + MIN_NUM_OF_BUCKETS;
    printf("\n  Initializing list with %d size ... \n\n", sizeOfList);
    LRU_Init(sizeOfList);    
}

void start_threads()
{
    int i, rv;
    pthread_t *workers;
    int *mytid;
    struct lru_job_desc* jds;

    int final_expected_count = 0;
 
    num_threads = rand() % ADDITIONAL_NUM_OF_THREADS + MIN_NUM_OF_THREADS;
    workers = malloc(sizeof(pthread_t) * num_threads);
    jds = malloc(num_threads* sizeof(struct lru_job_desc));

    printf("\n  Creating %d num_threads ... \n\n", num_threads);
    
    // run threads
    for (i = 0; i < num_threads; i++) {
	
	jds[i].tid = i;
	jds[i].count = 0; // will be updated by each thread
	
	if ((rv = pthread_create(& workers[i], NULL, thread_code, & jds[i])))
	    myerror("cannot create worker thread");
    }
    
    for (i = 0; i < num_threads; i++)
        pthread_join(workers[i], NULL);
    
    for (i = 0; i < num_threads; i++) {
	final_expected_count += jds[i].count;
    }
    
    printf("\n  REPORT: \n");
    printf("    Count (expected / l) : %8d / %8d \n", 
	   final_expected_count, LRU_Size());
    
    if (final_expected_count == LRU_Size()) {
	printf("    Congrats! you are an expert! ... \n");
    }
    else {
	printf("    Ooopss! cannot sleep tonight! ... \n");
    }    
}

void* thread_code(void *arg) 
{
    struct lru_job_desc *jd = (struct lru_job_desc *) arg;
    int numOfOps = rand() % ADDITIONAL_OPS_PER_THREAD + MIN_OPS_PER_THREAD;
    int i = 0;
    int *addlist = malloc(sizeof(int) * numOfOps);
    if (addlist == NULL) {
	printf(" cannot allocate more ..., reduce space \n");
	exit(1);
    }
    
    printf("  START Thread-%04d will execute %6d ops \n", jd->tid,
	   numOfOps);
    sleep(1);
    
    while (i < numOfOps) {
	
	addlist[i] = rand() % (numOfOps * num_threads);
	if (LRU_Insert(addlist[i])) {
	    // addlist[i] is already in the hash table
	    // so don't add anything
	}
	else {
	    // it is successful, so update my expected values
	    (jd->count)++;
	}
	i++;
    }
    
    i = 0;
    while (i < numOfOps) {

	if (i % 3 == 0) {
	    if (LRU_Remove(addlist[i])) {
	    // addlist[i] does not exist in the hash table
	    // so don't decrement anything
	    }
	    else {
		// it is successful, so update my expected values
		(jd->count)--;
	    }
	}
	i++;
    }


    printf("  END   Thread-%04d finishes %6d elements\n", 
	   jd->tid, jd->count);
    
    return NULL;
}


void myerror(char *arg)
{
    printf("  ERROR : %s \n", arg);
    exit(1);
}

