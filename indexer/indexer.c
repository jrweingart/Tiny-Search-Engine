#include <stdint.h>                                                            
#include <webpage.h>                                                          
#include <stdio.h>                                                            
#include <stdlib.h>
#include <string.h>
#include <pageio.h>
#include <hash.h>
#include <queue.h>
#include <indexio.h>
#include <dirent.h>
#include <ctype.h>

int threeCount = 0;
int totCount = 0;

/* indexer.c --- 
 * 
 * 
 * Author: John R. Weingart
 * Created: Thu Oct 24 17:06:41 2019 (-0400)
 * Version: 
 * 
 * Description: 
 * 
 */

typedef struct indexObj {
	char* word;
	int freq;
} indexObj;

typedef struct wordObj {
	char *word;
	queue_t *q;
} wordObj;

typedef struct docObj {
	int id;
	int freq;
} docObj;

bool NormalizeWord(char* word) {
	char* t;
	if(strlen(word) < 3) {
		//free(word);
		//word = NULL;
		return false;
	}
	for(t = word; *t != '\0'; t++) {
		//uppercase to lowercase: add 32 to decimal or hx form
		int charDec = *t;
		if(charDec >= 65 && charDec <= 90) {
			charDec += 32;
		}
		else if(charDec < 97 || charDec > 122) {
			//not alphabetic
			//discard word?
			//free(word);
			//word = NULL;
			return false;
		}
	  *t = (char)charDec;
	}
	
	return true;
}

bool stringSearch(void* indexp, const void* key){
	//char *strWord = (char*)word;
	struct indexObj* val = (struct indexObj*)indexp;
	char *res = val->word;
	char *strKey = (char*)key;
	//printf("strWord %s\n", strWord);
	//printf("strKey %s\n", strKey);
	if(strcmp(res, strKey) == 0){
		return true;
	}
	return false;
}

bool wordSearch(void* ip, const void* key) {
	struct wordObj* wObj = (struct wordObj*) ip;
	char* word = wObj->word;
	char* keyWord = (char*) key;
	
	if(strcmp(word, keyWord) == 0) {
		return true;
	}
	return false;
}

bool docSearch(void* docp, const void* keyp) {
	struct docObj* doc = (struct docObj*)docp;
	int* idp = (int*)keyp;

	if(doc->id == *idp) {
		return true;
	}
	return false;
}


//takes a word and doc id, and adds one to the count for that doc
void documentsToHash(hashtable_t* ht, char* word, int id){
	struct docObj* doc;
	struct queue_t* qTemp;
	struct wordObj* wordQ;
	//if the word is not in the hash, add word, and this doc freq 1
	if(hsearch(ht, wordSearch, word, strlen(word)) == NULL) {

		qTemp = qopen();

		wordQ = (struct wordObj*)malloc(sizeof(struct wordObj));
		doc = (struct docObj*)malloc(sizeof(struct docObj));
		
		doc->id = id;
		doc->freq = 1;

		qput(qTemp, doc);
		wordQ->word = word;
		wordQ->q = qTemp;

		hput(ht, wordQ, word, strlen(word));
	}

	//word is already in hash
	else {
		wordQ = hsearch(ht, wordSearch, word, strlen(word));
		qTemp = wordQ->q;
				//if this document is not found under that word, add it, with freq = 1
		if(qsearch(qTemp, docSearch, &id) == NULL) {
			
			doc = (struct docObj*)malloc(sizeof(struct docObj));
			doc->id = id;
			doc->freq = 1;
			qput(qTemp, doc);
		}

		//if this doc is already found under the word, add one to freq
		else {
			doc = (struct docObj*)qsearch(qTemp, docSearch, &id);
			doc->freq = doc->freq + 1;
		}
		free(word);
		
	}
	//free(word);
}

void countsToHash(hashtable_t* ht, char* word) {
	struct indexObj *temp;
	
	if((temp = hsearch(ht, stringSearch, word, strlen(word))) != NULL){
	
		temp->freq = temp->freq + 1;
		hput(ht, temp, word, strlen(word));
		threeCount++;
	}
	else{		
		temp = malloc(sizeof(struct indexObj));
		temp->freq = 1;
		temp->word = word;
		hput(ht, temp, word, strlen(word));
		threeCount++;
	}
	//free(word);
}

void hashCount(void* val) {
	struct indexObj *res = (struct indexObj*)val;
	//printf("freq: %d\n", res->freq);
	totCount += res->freq;
}

void counting(void* val) {
	struct docObj *doc = (struct docObj*)val;
	totCount += doc->freq;
}

void hashCountDoc(void* val) {
	struct wordObj *w = (struct wordObj*)val;
	queue_t *q = w->q;
	qapply(q, counting);
}

void lotsoPrint(void* ep){
	struct indexObj *res = (struct indexObj*)ep;
	printf("str: %s\n", res->word);
	printf("freq: %d\n", res->freq);
}
void printHT(void* ep) {
	struct wordObj *wq = (struct wordObj*)ep;
	printf("loaded word: %s\n", wq->word);
}

void closeHT(void* ep){
	struct wordObj* wq = (struct wordObj*)ep;
	free(wq->word);
	queue_t* qp = wq->q;
	qclose(qp);
}

#if 0
//Part 3 Main
int three_main(void) {
	printf("here\n"); //DO NOT REMOVE UNDER ANY CIRCUMSTANCES THIS PRINTLINE IS ESSENTIAL TO THE RUNNING OF OUR CODE
	
	webpage_t *page1 = pageload(1, "pages");
	int count = 0;
	char *word;
	int pos = 0;
	
	hashtable_t* ht = hopen(1000);
	
	while((pos = webpage_getNextWord(page1, pos, &word)) > 0) {
		if(NormalizeWord(word)) {
			countsToHash(ht, word);
			count++;
		}
	}
	
	happly(ht, hashCount); 
	hclose(ht);
	webpage_delete(page1);
	//free(word); //didnt free up memory but fixed one error?????
	
	printf("count: %d\n", count); 
	//printf("hash count: %d\n", totCount);

	return 0;
}

//part four main
int four_main(void) {
  webpage_t *page1 = pageload(1,"pages");
	int count = 0;
  char *word;
  int pos = 0;
	
  hashtable_t* ht = hopen(1000);
 
  while((pos = webpage_getNextWord(page1, pos, &word)) > 0) {
    if(NormalizeWord(word)) {
      documentsToHash(ht, word, 1);
      count++;
    }
  }
	
  happly(ht, hashCountDoc);
	
	happly(ht, closeHT);
	hclose(ht);
	webpage_delete(page1);
	free(word);
	
  printf("count: %d\n", count);
  printf("hash count: %d\n", totCount);
  return 0;
}



//part five main                                                              
int main(int argc, char *argv[]) {
	//FILE* passfile = fopen("~engs50/tse/indexpages/indexnm", "w");
	printf("here\n");
	if(argc > 2){
		printf("too many arguments\n");
		exit(EXIT_FAILURE);
	}

	int id = atoi(argv[1]);
	
	int count = 0;
  char *word;                                                                  
  int pos = 0;                                                                 
                                                                               
  hashtable_t* ht = hopen(1000);                                               
	for(int i = 1; i <= id; i++){
		webpage_t* page = pageload(i, "indexpages");
		while((pos = webpage_getNextWord(page, pos, &word)) > 0) {                  
			if(NormalizeWord(word)) {                                                
				documentsToHash(ht, word, i);                                         
				count++;
			}
			else{
				free(word);	
			}
		}
		//free(word);
		webpage_delete(page);
		pos = 0;
	}
	
  happly(ht, hashCountDoc);                                                    

	
	indexsave(ht, "../indexpages/indexnm");
	
	hashtable_t* res = indexload("../indexpages/indexnm");
	//happly(res, printHT);
	//printf("actual hash:\n");
	//happly(ht, printHT);
	
	indexsave(res, "../indexpages/result");

	happly(ht, closeHT);
	happly(res, closeHT);
	hclose(ht);
	hclose(res);
  printf("count: %d\n", count);                                                
  printf("hash count: %d\n", totCount);                                        

	return 0;                                      
}         

#endif

int main(int argc, char *argv[]) {                                                                                    
	if(argc != 3){
		printf("wrong # of arguments\n");
		exit(EXIT_FAILURE);
	}
  char* pagedir=malloc(100);
	char* indexnm=malloc(100);

	sprintf(pagedir, "%s", argv[1]);
	sprintf(indexnm, "%s", argv[2]);
	printf("pagedir:%s\n", pagedir);
	struct dirent* dp;
	DIR* dfd;

	if((dfd = opendir(pagedir)) == NULL){
		printf("could not open directory\n");
		exit(EXIT_FAILURE);
	}
	int count = 0;
	char *word;
	int pos = 0;
	hashtable_t* ht = hopen(1000);
	int pagecount = 0;

 	while((dp = readdir(dfd)) != NULL){
		int id;
	
		id = atoi(dp->d_name);
	
		webpage_t* page = pageload(id, pagedir);
		pagecount ++;
		while((pos = webpage_getNextWord(page, pos, &word)) > 0) {
			if(NormalizeWord(word)) {
				if(strcmp(word, "abrams") == 0){
					printf("yes\n");
				}
				documentsToHash(ht, word, id);
				count++;
			}
			else{
				free(word);
			}
		}
		webpage_delete(page);
		pos = 0;
	}

	printf("pagecount: %d\n", pagecount);
	indexsave(ht, indexnm);

	happly(ht, closeHT);
	hclose(ht);
	free(pagedir);
	free(indexnm);
	closedir(dfd);
	
	return 0;
}
	
