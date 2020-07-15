/* queue.c --- 
 * 
 * 
 * Author: Catherine D. Parnell
 * Created: Mon Oct  7 14:23:59 2019 (-0400)
 * Version: 
 * 
 * Description: implementation of reuseable queue module 
 * 
 */
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

typedef void queue_t;        

typedef void elem_t;

typedef struct queueObj{                                         
  void *front, *back;
	int size;
} queueObj;


typedef struct elemObj{
	void *value, *next;
} elemObj;

int qsize(queue_t *q){
	queueObj *qObj = (queueObj*)q;
	return qObj->size;

	//bool qIsEmpty(queue_t *q){
	//queueObj *qObj = (queueObj*)q;
	//if(qObj->front == NULL){
	//	return true;
	//}
	//else{
	//	return false;
	//}
}

// create empty queue
queue_t* qopen(void){
	struct queueObj *qObj = (struct queueObj*) malloc(sizeof(struct queueObj));
	qObj->front = NULL;
	qObj->back = NULL;
	qObj->size = 0;
	queue_t *qp = (void*)qObj;
	return qp; //not sure we want a * here ... are we returning the pointer or the queue?
}

//put element at end of queue
int32_t qput(queue_t *qp, void *elementp){
	struct elemObj *element = (struct elemObj*) malloc(sizeof(struct elemObj));
	queueObj *qObj = (queueObj*)qp; 

	element->value = elementp;
	element->next = NULL; //bc when you add it will always b tail
	
	if (qObj->front == NULL) {
		qObj->front = element;
	}
	else {
		elemObj *i = qObj->back;
	  i->next = element;
	}
	qObj->back = element;
	qObj->size = qObj->size + 1;
	return 0;
}

void* qget(queue_t *qp){
	queueObj *qObj = (queueObj*)qp;
	if(qObj->front == NULL){
		return NULL;
	}
	else{
		elemObj *temp = qObj->front;
		elem_t *temp2 = temp->value;
		//elemObj *currHead = qObj->front;
		qObj->front = temp->next;
		qObj->size = qObj->size - 1;
		free(temp);
		return (void*)temp2;                                                       
	}
}

//deallocate queue                                                                    
void qclose(queue_t *qp) {                                                           
  struct elemObj *temp = NULL;
	queueObj *qObj = (queueObj*)qp;
  struct elemObj *curr = qObj->front;
	
	while(curr!=NULL){
		temp=curr->next;
		free(curr->value);
		
		free(curr);
		
		curr = temp;
	}
 
	free(qObj);
	
}

//tried to add thisbut my core is dumping
void qapply(queue_t *qp, void (*fn)(void* elementp)){

	struct queueObj *qObj = (queueObj*)qp;
	struct elemObj *temp; 
	for(temp = qObj->front; temp != NULL; temp = temp->next){
		fn(temp->value);
	}
}

void qconcat(queue_t *q1p, queue_t *q2p){
	struct queueObj *qObj1 = (queueObj*)q1p;
	struct queueObj *qObj2 = (queueObj*)q2p;

	elemObj *front1 = qObj1->front;
	elemObj *front2 = qObj2->front;
	//if(front1 == NULL && front2 == NULL){}
	if(front1 == NULL){
		qObj1->front = qObj2->front;
		qObj1->back = qObj2->back;
	}
	else if(front2 != NULL){
		elemObj *temp1 = qObj1->back;
		temp1->next = qObj2->front; 
		qObj1->back = qObj2->back;       
	}
	q1p = qObj1;
	free(qObj2);
}


void* qsearch(queue_t *qp, bool (*searchfn)(void* elementp, const void* keyp), const void* skeyp){
	struct queueObj *qObj = (queueObj*)qp;                                                            
  struct elemObj *temp;                                                                                             
	for(temp = qObj->front; temp != NULL; temp = temp->next){                                                         
		if(searchfn(temp->value, skeyp)){
 			void* foundp = temp->value;
			return foundp;
		}
  }
	//printf("null from qsearch\n");
	return NULL;
}


void* qremove(queue_t *qp, bool(*searchfn)(void* elementp, const void* keyp), const void* skeyp){
 struct queueObj *qObj = (queueObj*)qp;                                                                                 
 struct elemObj *temp;
 struct elemObj *tempNext;
 temp = qObj->front;
 if(temp == NULL){
	 return NULL;
 }
 
 if(searchfn(temp->value, skeyp)){ //first one is to be removed
	 void* foundp = temp->value;
	 qObj->front = temp->next;
	 free(temp);
	 qObj->size = qObj->size - 1;
	 
	 return foundp;
 }
 
 for(temp = qObj->front; temp->next != NULL; temp = temp->next){                                                             
	 tempNext = temp->next;
	 if(searchfn(tempNext->value, skeyp)){                                                                              
     void* foundp = tempNext->value;
		 temp->next = tempNext->next;
		 qObj->size = qObj->size - 1;
		 free(tempNext);
		 return foundp;                                                         
   }                                                                                                                   
 }
  return NULL;  
}



bool intSearchfn(void* elementp, const void* keyp){
	double *val1 = (double *)elementp;
	double *val2 = (double *)keyp;
	
	if(*val1 == *val2){
		return true;
	}
	return false;
}


//int main(void){
//queue_t *qp = qopen();
//queue_t *qp2 = qopen();
//double x = 32.54;

//double y = 32.1233;

//double a = 54.2;
//double b = 982.1;
	
//qput(qp, &x);
//qput(qp, &y);

//qput(qp2, &a);
//qput(qp2, &b);

//qconcat(qp, qp2);

//double x = 32.54;

//double y = 32.1233;
//qput(qp, &x);
//qput(qp, &y);
	
//double *elem = qget(qp);
//printf("value: %lf \n", *elem);
	
	//	return 0;
	//}
	//double *elem1 = qget(qp);
	//double *elem2 = qget(qp);
	//double *elem3 = qget(qp);                                                         
	//double *elem4 = qget(qp);
	
	//printf("%lf \n", *elem1);
	//printf("%lf \n", *elem2);                                                   
  //printf("%lf \n", *elem3);                                                           
  //printf("%lf \n", *elem4);                                                  

	//qclose(qp);
	//qclose(qp2);
	//return 0;
//}


//int main(){
//double x = 2.1;
//double y = 7.4;
//double z = 9.3;
//
//queue_t *qp = qopen();                                                                       
//qput(qp, &x);
//qput(qp, &y);
//qput(qp, &z);

	
//double dub = 7.4;
//void* keyp = (void*)(&dub);
	
//double *res = qremove(qp, intSearchfn, keyp);
//printf("result: %lf \n", *res);
	
//double *elem1 = qget(qp);                                                      /                                       
//double *elem2 = qget(qp);                                                                                             
//double *elem3 = qget(qp);                                                                                             
                                                                                                                        
//printf("%lf \n", *elem1);                                                                                             
//printf("%lf \n", *elem2);                                                                                             
  //printf("%lf \n", *elem3); 


	//return 0;
//}
