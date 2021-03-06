/* 
 * queue.h -- public interface to the queue module
 */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <queue.h>
#include <pthread.h>

/* the queue representation is hidden from users of the module */
typedef void lqueue_t;		

typedef struct lqueueObj{
	pthread_mutex_t lock;
	queue_t* q;
} lqueueObj;


/* create an empty queue */
lqueue_t* lqopen(void){
	struct lqueueObj *lqObj = (struct lqueueObj*) malloc(sizeof(struct lqueueObj));
	lqObj->q = qopen();
	pthread_mutex_init(&(lqObj->lock), NULL);
	lqueue_t* qp = (void*)lqObj;
	return qp;
}

/* deallocate a queue, frees everything in it */
void lqclose(lqueue_t *qp){
	struct lqueueObj *lqObj = (struct lqueueObj*)qp;
	pthread_mutex_lock(&(lqObj->lock));
	qclose(lqObj->q);
	pthread_mutex_destroy(&(lqObj->lock));
	free(lqObj);
}
	
/* put element at the end of the queue
 * returns 0 is successful; nonzero otherwise 
 */
int32_t lqput(lqueue_t *qp, void *elementp){
	struct lqueueObj *lqObj = (struct lqueueObj*)qp;
	pthread_mutex_lock(&(lqObj->lock));
	qput(lqObj->q, elementp);
	pthread_mutex_unlock(&(lqObj->lock));
	
	return 0;
}


/* get the first first element from queue, removing it from the queue */
void* lqget(lqueue_t *qp){
	struct lqueueObj *lqObj = (struct lqueueObj*)qp;                                                                            
  pthread_mutex_lock(&(lqObj->lock));
	void* res = (void*)qget(lqObj->q);
	pthread_mutex_unlock(&(lqObj->lock));
	return res;
}


/* apply a function to every element of the queue */
void lqapply(lqueue_t *qp, void (*fn)(void* elementp)){
	struct lqueueObj *lqObj = (struct lqueueObj*)qp;                                                                            
  pthread_mutex_lock(&(lqObj->lock));
	qapply(lqObj->q, fn);
	pthread_mutex_unlock(&(lqObj->lock));
}

/* search a queue using a supplied boolean function
 * skeyp -- a key to search for
 * searchfn -- a function applied to every element of the queue
 *          -- elementp - a pointer to an element
 *          -- keyp - the key being searched for (i.e. will be 
 *             set to skey at each step of the search
 *          -- returns TRUE or FALSE as defined in bool.h
 * returns a pointer to an element, or NULL if not found
 */
void* lqsearch(lqueue_t *qp, 
							bool (*searchfn)(void* elementp,const void* keyp),
							 const void* skeyp){
	struct lqueueObj *lqObj = (struct lqueueObj*)qp;                                                                            
  pthread_mutex_lock(&(lqObj->lock));
	void* res = qsearch(lqObj->q, searchfn, skeyp);
	pthread_mutex_unlock(&(lqObj->lock));
	return res;
}

	
/* search a queue using a supplied boolean function (as in qsearch),
 * removes the element from the queue and returns a pointer to it or
 * NULL if not found
 */
void* lqremove(lqueue_t *qp,
							bool (*searchfn)(void* elementp,const void* keyp),
							 const void* skeyp){
	struct lqueueObj *lqObj = (struct lqueueObj*)qp;                                                                           
  pthread_mutex_lock(&(lqObj->lock));
	void* res = qremove(lqObj->q, searchfn, skeyp);
	pthread_mutex_unlock(&(lqObj->lock));
	return res;
}


/* concatenatenates elements of q2 into q1
 * q2 is dealocated, closed, and unusable upon completion 
 */
void lqconcat(lqueue_t *q1p, lqueue_t *q2p){
	struct lqueueObj *lqObj1 = (struct lqueueObj*)q1p;                                                                          
	struct lqueueObj *lqObj2 = (struct lqueueObj*)q2p;                                                                          
	lqconcat(lqObj1->q, lqObj2->q);
	pthread_mutex_unlock(&(lqObj1->lock));
}
	
