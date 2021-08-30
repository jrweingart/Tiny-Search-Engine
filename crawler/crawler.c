/* crawler.c --- 
 * 
 * 
 * Author: John Weingart, Catherine Parnell, Grant Dumanian
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
#include <queue.h>
#include <hash.h>
#include <string.h>


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


void crawl(char* seedurl, char* pagedir, int maxdepth){
	int qsize = 0;
	queue_t* qp = qopen();                                                                               
  hashtable_t* hp = hopen(100); 
	webpage_t* curr = webpage_new(seedurl, 0, NULL);
	hput(hp, seedurl, (const char*)seedurl, sizeof(seedurl));
	//qsize += 1;
	int level = 0;
	int totcount = 1;
	
	bool firstTime = false;
	if(maxdepth > 0){
		firstTime = true;
	}
	
	//curr = qget(qp);
	level = webpage_getDepth(curr);
	if(!webpage_fetch(curr)){
		webpage_delete(curr);
	}
	
	char* result;
	webpage_t* newPage;
	pagesave(curr, totcount, pagedir);
	totcount++;
	while(qsize > 0 || firstTime){ //&& (webpage_getDepth(curr = qget(qp)) <= maxdepth)){
		//char *result;
		//webpage_t* newPage;
		if(firstTime){
			firstTime = false;
		}
		
		int pos = 0;
		//pos = webpage_getNextURL(curr, pos, &result);
		while ((pos = webpage_getNextURL(curr, pos, &result)) > 0){
			bool internal = IsInternalURL(result);
			if(internal){
				if(hsearch(hp, search, (const char*)result, sizeof(result)) == NULL){
					newPage = webpage_new(result, level + 1, NULL);
					if(!webpage_fetch(newPage)){
						webpage_delete(newPage);
					}
					else {
						//pagesave(newPage, pagetotal, pagedir); 
						//}
						hput(hp, result, (const char*)result, sizeof(result));                              
						pagesave(newPage, totcount, pagedir);
						printf("url: %s\n", webpage_getURL(newPage));
						//webpage_delete(newPage);
						totcount++;
						if(webpage_getDepth(newPage) < maxdepth){
							qput(qp, newPage);
						//webpage_delete(newPage);
							qsize += 1;
						}
						else{
							webpage_delete(newPage);
						}
					}
				}
			}
			else{
				free(result);
			}
		}
		
		webpage_delete(curr);
		curr = (webpage_t*) qget(qp);
		level = webpage_getDepth(curr);
		qsize -= 1;
	}
	# if 0
 	webpage_t *toDelete = (webpage_t*) qget(qp);
	while(toDelete != NULL){
		webpage_delete(toDelete);
		toDelete = (webpage_t*) qget(qp);
	}
	#endif
	webpage_delete(curr);
	totcount--;
	printf("total: %d \n", totcount);
	qclose(qp);
	hclose(hp);

}

//if 0
int main(int argc, char* argv[]){
	char *sp;
	if(argc == 100000){
		printf("Error: incorrect number of arguments\n");
		exit(EXIT_FAILURE);
	}
	else if((int)*argv[3] < 0){
		printf("Error: max depth must be greater than zero\n");
		exit(EXIT_FAILURE);
	}
	else{
		//printf("first");
		
		if((sp=(char*)malloc(300))==NULL) {
			printf("failed to malloc seedurl\n");
			exit(EXIT_FAILURE);
		}
		strcpy(sp, argv[1]);

		char pagedir[100];
		strcpy(pagedir, argv[2]);
		int maxdepth = atoi(argv[3]);
		//printf("max is: %d \n", 
		

		
		crawl(sp, pagedir, maxdepth);		 

		exit(EXIT_SUCCESS);
	}
}
//#endif

#if 0
int main(void){
	
	webpage_t* page = webpage_new("https://thayer.github.io/engs50/", 0, NULL);
	
	if(!webpage_fetch(page)) {
		webpage_delete(page);
		exit(EXIT_FAILURE);
	}

	pagesave(page, 1, "pages");
	
	
	queue_t* qp = qopen();
	hashtable_t* hp = hopen(100);
	//printf("here");
	int pos = 0;
	char *result;
	webpage_t* newPage;
	while ((pos = webpage_getNextURL(page, pos, &result)) > 0){
		printf("pos %d\n", pos);
		bool internal = IsInternalURL(result);
		if(internal){
			if(hsearch(hp, search, (const char*)result, sizeof(result)) == NULL){				
				newPage = webpage_new(result, 0, NULL);
				hput(hp, result, (const char*)result, sizeof(result));
				qput(qp, newPage);
			}
		}
	}
	
	free(result);
	//free(newPage);
	webpage_t* res = (webpage_t*) qget(qp);
	while(res != NULL){
		printf("Internal url: %s \n", webpage_getURL(res));
		webpage_delete(res);
		res = (webpage_t*)qget(qp);
	}

	qclose(qp);
	hclose(hp);

	//webpage_delete(res);
  webpage_delete(page);
	exit(EXIT_SUCCESS);
}
	
#endif
