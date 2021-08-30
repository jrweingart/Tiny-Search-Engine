/* crawler.c --- 
 * 
 * 
 * Author: John Weingart, Grant Dumanian, Catherine D. Parnell
 * Created: Wed Oct 16 15:38:55 2019 (-0400)
 * Version: 
 * 
 * Description: 
 * 
 */
#include <stdio.h>                                                                            
#include <stdlib.h>                                                                           
#include <stdbool.h>   
#include <webpage.h>
#include <lqueue.h>
#include <lhash.h>
#include <string.h>
#include <unistd.h>


//int level = 0;
static lqueue_t *lq;
static lhashtable_t *lh;



pthread_mutex_t id_lock;
pthread_mutex_t numWaiting_lock;
pthread_mutex_t lh_lock;

char* seedurl;
int id = 1;
int numWaiting = 0;

typedef struct crawlObj{
	//	lqueue_t* lq;
	//	lhashtable_t* lh;
	int maxdepth;
	char* pagedir;
	int totalThreads;
	int crawlNum;
} crawlObj;

int32_t pagesave(webpage_t *pagep, int id, char *dirname){
	char path[100];
	char *URL = webpage_getURL(pagep);
	int depth = webpage_getDepth(pagep);
	int HTMLlen = webpage_getHTMLlen(pagep);
	char *HTML = webpage_getHTML(pagep);
	FILE *file;

	sprintf(path, "../%s/%d", dirname, id);
	file = fopen(path, "w");

	fprintf(file, "%s\n", URL);
	fprintf(file, "%d\n", depth);
	fprintf(file, "%d\n", HTMLlen);
	fprintf(file, "%s", HTML);
	
	return fclose(file);
}



bool search(void* elementp, const void* searchkeyp){
	char *keyADDY = (char*)searchkeyp;
	char *elemURL = (char*)elementp;
	if(strcmp(keyADDY, elemURL) == 0){
		free(keyADDY); //why do we need to free keyADDY but not free elemURL?	
		return true;
	}
	return false;
}


void* crawl(void* ep){
	//coerce crawl object
	struct crawlObj* crawlO = (struct crawlObj*)ep;

	//handle elements in crawl object
 	int maxdepth = crawlO->maxdepth;
	int totalThreads = crawlO->totalThreads;
	char* pagedir = malloc(100);
	strcpy(pagedir, crawlO->pagedir);
	//int crawlID = crawlO->crawlNum;

	//instantiate variables
	bool didWait = false;
	char* result;
	webpage_t* curr;
	webpage_t* newPage;
	//int level;

	//keep running while there are still threads waiting for the queue
	pthread_mutex_lock(&numWaiting_lock);
	while(numWaiting != totalThreads){

		//get current from the queue
		curr = lqget(lq);
		pthread_mutex_unlock(&numWaiting_lock);
		//level = 0;
		
		//if thread is unable to get a page
		while(curr == NULL){
			//if thread did not previously wait
			if(didWait == false){
				pthread_mutex_lock(&numWaiting_lock);
				numWaiting++;
				//printf("numWaiting %d\n", numWaiting);
				pthread_mutex_unlock(&numWaiting_lock);
				didWait = true;
			}
			//if all threads are now waiting, exit
			if(numWaiting == totalThreads){
				free(pagedir);
				free(crawlO);
				//printf("all threads waiting\n");
				//webpage_delete(curr);
				return NULL;
			}
			sleep(1);
			//keep trying to get page from queue
			curr = lqget(lq);
			//if (curr != NULL){
			//level = webpage_getDepth(curr);
			//}
		}

		//printf("%d got %s\n", crawlID, webpage_getURL(curr));

		//if thread previously waited, decrement numWaiting
		if(didWait){
			pthread_mutex_lock(&numWaiting_lock);
			numWaiting--;
			//printf("numwaiting %d\n", numWaiting);
			pthread_mutex_unlock(&numWaiting_lock);
		}

		//run loop for next urls connected to current
		pthread_mutex_lock(&lh_lock);
		int pos = 0;
		while ((pos = webpage_getNextURL(curr, pos, &result)) > 0){
			bool internal = IsInternalURL(result);
			if(internal){

				//if url not in hashtable, put it in the queue
				
				if(searchandput(lh, search, (const char*)result, strlen(result))){
					//printf("%s\n", result);

					//create new page
					newPage = webpage_new(result, webpage_getDepth(curr)+1, NULL);
					//printf("made %s at level %d\n", webpage_getURL(newPage), webpage_getDepth(newPage));
					//pthread_mutex_unlock(&lh_lock);
					if(!webpage_fetch(newPage)){
						webpage_delete(newPage);
					}
					else{
						//if webpage is fetched, if newpage is not the seedurl, pagesave newpage
						if(strcmp(webpage_getURL(newPage), seedurl) != 0){
							pthread_mutex_lock(&id_lock);
							pagesave(newPage, id, pagedir);
							//printf("%d saved: %d url: %s\n",  crawlID, id, webpage_getURL(newPage));					
							id++;
							pthread_mutex_unlock(&id_lock);
							
							//if depth is less than or equal to maxdepth put newpage in queue
							//printf("DEPTH: %d, %s\n",webpage_getDepth(newPage), webpage_getURL(newPage));
							if(webpage_getDepth(newPage) < maxdepth){
								lqput(lq, newPage);
							}
							//if depth is greater than maxdepth, delete newPage
							else{
								webpage_delete(newPage);
							}
						}
						//if webpage is not fetched, delete newpage
						else{
							webpage_delete(newPage);
						}
					}
				}
				
			}
			//if result is external
			else{
				free(result);
			}
		}
		pthread_mutex_unlock(&lh_lock);
		//thread didn't wait
		didWait = false;
		pthread_mutex_lock(&numWaiting_lock);
		webpage_delete(curr);
	}
	//free page directory
	free(pagedir);
	pthread_mutex_unlock(&numWaiting_lock);
	printf("got thru\n");
	return NULL;
}

//if 0
int main(int argc, char* argv[]){
	
	//handle arguments
	if(argc == 100000){
		printf("Error: incorrect number of arguments\n");
		exit(EXIT_FAILURE);
	}
	else if((int)*argv[3] < 0){
		printf("Error: max depth must be greater than zero\n");
		exit(EXIT_FAILURE);
	}

	//initialize locks
	pthread_mutex_init(&id_lock, NULL);
	pthread_mutex_init(&numWaiting_lock, NULL);
	pthread_mutex_init(&lh_lock, NULL);


	//open global queue and hash
	lq = lqopen();                                                                                            
	lh = lhopen(1000);

	//malloc and handle seedurl
	if((seedurl=(char*)malloc(300))==NULL) {
		printf("failed to malloc seedurl\n");
		exit(EXIT_FAILURE);
	}
	strcpy(seedurl, argv[1]);

	//handle arguments
	char pagedir[100];
	strcpy(pagedir, argv[2]);
	int maxdepth = atoi(argv[3]);
	int threadNum = atoi(argv[4]);

	//make seedurl webpage
	webpage_t* seed = webpage_new(seedurl, 0, NULL);
	
	if(!webpage_fetch(seed)){                                                                            
		webpage_delete(seed);
	}

	//put seed page in queue. and seed url in hash
	lqput(lq, seed);
	lhput(lh, seedurl, (const char*)seedurl, strlen(seedurl));

	//pagesave and increment id
	pagesave(seed, id, pagedir);
	id++;

	//create array of threads
	pthread_t threads[threadNum];

	//create threads and call crawl
	for(int i = 0; i < threadNum; i++){
		struct crawlObj* crawlO = malloc(sizeof(crawlObj));

		crawlO->maxdepth = maxdepth;
		crawlO->totalThreads = threadNum;
		crawlO->pagedir = pagedir;
		crawlO->crawlNum = i;
		
		pthread_create(&threads[i], NULL, crawl, (void*)crawlO);
		//sleep(3);
	}
	//printf("pthreads created\n");

	//join all threads
	for(int i = 0; i < threadNum; i++){
		pthread_join(threads[i], NULL);
	}

	//destroy locks
	pthread_mutex_destroy(&id_lock);
	pthread_mutex_destroy(&numWaiting_lock);
  pthread_mutex_destroy(&lh_lock);   
	
 	

	//close queues
	lqclose(lq);
	lhclose(lh);
	exit(EXIT_SUCCESS);
}


