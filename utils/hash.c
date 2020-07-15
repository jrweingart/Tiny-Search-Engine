/* 
 * hash.c -- implements a generic hash table as an indexed set of queues.
 *
 */
#include <stdint.h>
#include "queue.h"
#include <stdlib.h>                                         
#include <stdint.h>               
#include <stdio.h>                            
#include <stdbool.h>
#include <string.h>   

/* 
 * SuperFastHash() -- produces a number between 0 and the tablesize-1.
 * 
 * The following (rather complicated) code, has been taken from Paul
 * Hsieh's website under the terms of the BSD license. It's a hash
 * function used all over the place nowadays, including Google Sparse
 * Hash.
 */
#define get16bits(d) (*((const uint16_t *) (d)))

static uint32_t SuperFastHash (const char *data,int len,uint32_t tablesize) {
  uint32_t hash = len, tmp;
  int rem;
  
  if (len <= 0 || data == NULL)
		return 0;
  rem = len & 3;
  len >>= 2;
  /* Main loop */
  for (;len > 0; len--) {
    hash  += get16bits (data);
    tmp    = (get16bits (data+2) << 11) ^ hash;
    hash   = (hash << 16) ^ tmp;
    data  += 2*sizeof (uint16_t);
    hash  += hash >> 11;
  }
  /* Handle end cases */
  switch (rem) {
  case 3: hash += get16bits (data);
    hash ^= hash << 16;
    hash ^= data[sizeof (uint16_t)] << 18;
    hash += hash >> 11;
    break;
  case 2: hash += get16bits (data);
    hash ^= hash << 11;
    hash += hash >> 17;
    break;
  case 1: hash += *data;
    hash ^= hash << 10;
    hash += hash >> 1;
  }
  /* Force "avalanching" of final 127 bits */
  hash ^= hash << 3;
  hash += hash >> 5;
  hash ^= hash << 4;
  hash += hash >> 17;
  hash ^= hash << 25;
  hash += hash >> 6;
  return hash % tablesize;
}

typedef void hashtable_t;

typedef void helem_t;                                                                                        

typedef struct hashObj{                            
  int size;
  int count;
	queue_t** queues;        
} hashObj;     


static void openAll(hashObj *hObj) {
	int i;
	for(i=0;i<(hObj->size);i++){
		hObj->queues[i] = qopen();
	}
}

/* hopen -- opens a hash table with initial size hsize */ 
hashtable_t *hopen(uint32_t hsize) {
	struct hashObj *hObj = (struct hashObj*) malloc(sizeof(struct hashObj));
	hObj->size = hsize;
	hObj->count = 0;
	hObj->queues = calloc((size_t)hsize, sizeof(queue_t*));
	openAll(hObj);
	hashtable_t *hp = (void*)hObj;
	//printf("opened\n");
	return hp;
}

void hclose(hashtable_t *htp){

	hashObj *hObj = (hashObj*)htp;
	queue_t **queueArray = hObj->queues;
	//printf("..\n");
	int h;
	//printf(".\n");
	for(h=0; h<(hObj->size); h++){
		queue_t *q = queueArray[h];
		qclose(q);

	}
	//printf("..\n");
	//printf("closed\n");
	//printf("yooooo\n");
	free(hObj->queues);
	//printf("freed the queues\n");
	free(hObj);
	//printf("freed the obj\n");
}

/* hput -- puts an entry into a hash table under designated key                                          
 * returns 0 for success; non-zero otherwise                                                             
 */                                                                                                      
int32_t hput(hashtable_t *htp, void *ep, const char *key, int keylen){
  struct hashObj *hObj = (struct hashObj*)htp;
	//struct elemObj *element = (struct elemObj*) malloc(sizeof(struct elemObj));
	queue_t **queueArray = hObj->queues;
	
	uint32_t index = SuperFastHash(key,keylen,hObj->size);
	//printf("indexed\n");
	
	//queueArray[index] = qopen();
	//printf("opened queue\n");
	
	qput(queueArray[index], ep);
  //printf("put in queue\n");
	
	hObj->count = hObj->count + 1;
	
	return 0;
}


/* happly -- applies a function to every entry in hash table */
		
void happly(hashtable_t *htp, void (*fn)(void* ep)){
		struct hashObj *hObj = (struct hashObj*)htp;
		queue_t **queueArray = hObj->queues;

		int h;
		for(h=0; h<(hObj->size); h++){
			if(queueArray[h] != NULL){
				qapply(queueArray[h], fn);
			}
		}
}


/* hsearch -- searchs for an entry under a designated key using a                                        
 * designated search fn -- returns a pointer to the entry or NULL if                                     
 * not found                                                                                              
 */                                                                                                      
void *hsearch(hashtable_t *htp, bool (*searchfn)(void* elementp, const void* searchkeyp), const char *key, int32_t keylen){
	struct hashObj *hObj = (struct hashObj*)htp;
	queue_t **queueArray = hObj->queues;
	
	int index = SuperFastHash(key, keylen, hObj->size);
	
	void *found = qsearch(queueArray[index], searchfn, (const void*) key);
	if(found != NULL){
		return found;
	}
	//printf("null in hsearch\n");
	return NULL;
}
			
/* hremove -- removes and returns an entry under a designated key                                        
 * using a designated search fn -- returns a pointer to the entry or                                     
 * NULL if not found                                                                                     
 */                                                                                                      
void *hremove(hashtable_t *htp, bool (*searchfn)(void* elementp, const void* searchkeyp), const char *key, int32_t keylen){
	struct hashObj *hObj = (struct hashObj*)htp;
	queue_t **queueArray = hObj->queues;

	int index = SuperFastHash(key, keylen, hObj->size);
	void *found = qremove(queueArray[index], searchfn, key);          
	if(found != NULL){
		return found;
	}           
  return NULL;
}  

