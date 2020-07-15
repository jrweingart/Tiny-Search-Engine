/* indexio.c --- 
 * 
 * 
 * Author: John R. Weingart
 * Created: Tue Oct 29 16:27:23 2019 (-0400)
 * Version: 
 * 
 * Description: 
 * 
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <hash.h>
#include <stdlib.h>
#include <queue.h>
#include <webpage.h>


FILE* file;// = fopen("../indexpages/indexnm", "w");


typedef struct wordObj {                                                        
  char *word;                                                                   
  queue_t *q;                                                                   
} wordObj;                                                                      
                                                                                
typedef struct docObj {                                                         
  int id;                                                                       
  int freq;                                                                     
} docObj; 


static void writeInfo(void* ep){

	struct wordObj *val = (wordObj*)ep;

 	char *word = (char*)val->word;
	queue_t *q = val->q;
	
	char id;
	char freq;
	
	struct docObj* doc;

	fprintf(file, "%s ", word);

	while((doc = qget(q)) != NULL){
		id = doc->id;
		freq = doc->freq;
		fprintf(file, "%d %d ",id, freq);
		free(doc);
	}
	fprintf(file, "\n");
	//free(val);
	//free(word);
	//free(doc);
	//qclose(q);
}


void indexsave(hashtable_t* ht, char* passfile){
	file = fopen(passfile, "w");
	if(file == NULL){
		//printf("file could not be opened\n");
		//fclose(file);
		return;
	}
	happly(ht, writeInfo);
	fclose(file);
}

void printHash(void* ep) {                                                                           
  struct wordObj *wq = (struct wordObj*)ep;                                                        
  printf("loaded word: %s\n", wq->word);                                                           
}

hashtable_t* indexload(char* passfile){
	
	if((file = fopen(passfile, "r")) == NULL){
		//printf("file could not be opened\n");
		return NULL;
	}
	
	hashtable_t* ht = hopen(1000);

	char line[1000];
	char charNum[1000];
	char word[1000];
	
	int intNum;
	int numArr[1000];
	int numArrCount = 0;
	int numCount = 0;
	
	while(fgets(line, 1000, file) != NULL){
		struct wordObj* wordQ = (struct wordObj*)malloc(sizeof(struct wordObj));
		queue_t* q = qopen();
		wordQ->q = q;

		int i;
		for(i = 0; line[i] != ' '; i++){
			word[i] = line[i];
		}
		word[i] = '\0';

		char* thisWord = (char*)calloc(i,(sizeof(char)+1));

		int k;
		for(k=0; k<i+1; k++) {
			thisWord[k] = word[k];
		}
		
		wordQ->word = thisWord;

		numCount = 0;
		for(int j = i + 1; line[j] != '\n'; j++){
			while(line[j] != ' '){
				charNum[numCount] = line[j];
				numCount++;
				j++;
			}
			
			char* charNumParsed = (char*)calloc(numCount,(sizeof(char)+1));
			int p;
			for(p = 0; p < numCount; p++) {
				charNumParsed[p] = charNum[p];
			}
			
			intNum = atoi(charNumParsed);
			numArr[numArrCount] = intNum;
			numArrCount++;
			numCount = 0;
			memset(&charNum[0], 0, 1000);
			free(charNumParsed);
		}

		struct docObj* docQ;	
		for(int x = 0; x <= numArrCount - 1; x+=2){
			docQ = (struct docObj*)malloc(sizeof(struct docObj));     
			docQ->id = numArr[x];
			docQ->freq = numArr[x+1];
			qput(wordQ->q, docQ);
			//printf("id: %d\n", docQ->id);
			//printf("freq: %d\n", docQ->freq);
		}
	
		numArrCount = 0;
		hput(ht, wordQ, thisWord, strlen(thisWord));
		//printf("line: %s\n", line);

		memset(&word[0], 0, 1000);
		memset(&line[0], 0, 1000);
		memset(&numArr[0], 0, 1000);
		//free(wordQ);
	}

	fclose(file);
	return ht;
}
