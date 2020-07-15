/* query.c --- 
 * 
 * 
 * Author: John R. Weingart
 * Created: Thu Oct 31 17:56:05 2019 (-0400)
 * Version: 
 * 
 * Description: 
 * 
 */

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
#include <stdbool.h>
#include <hash.h>
#include <queue.h>

int linecount = 0;
int count = 0;

typedef struct wordObj{
	char *word;
	queue_t* q;
} wordObj;

typedef struct docObj{
	int id;
	int freq;
} docObj;

typedef struct docRankObj{
	int id;
	int rank;
	char* url;
} docRankObj;

void closeHT(void* ep){                                                                      
  struct wordObj* wq = (struct wordObj*)ep;                                                  
  free(wq->word);                                                                            
  queue_t* qp = wq->q;                                                                       
  qclose(qp);                                                                                
}

void closeQ(void* ep){
	queue_t* q = (queue_t*)ep;
	qclose(q);
}


void getInput(char buffer[1000]){
	char c = fgetc(stdin);
	int i = 0;
	while(c != '\n' && c != EOF){
		//c = fgetc(stdin);
		buffer[i] = tolower((char)c);
		i++;
		c = fgetc(stdin);
	}
	buffer[i] = '\0';
}

//returns queue of queues of lines from file
queue_t* getFileInput(FILE* file, queue_t* res){
	char line[1000];// = malloc(1000);
	//queue_t* res = qopen();
	while(fgets(line, sizeof(line), file)){

		//while(!(feof(file))){
		//fscanf(file, "%s\n", line);
		char* word = strtok(line, " ");
		queue_t* tempq = qopen();
		while(word != NULL){
			char* cpyWord = malloc(100);
			strcpy(cpyWord, word);
			//free(word);
			cpyWord[strcspn(cpyWord, "\n")] = 0;
			qput(tempq, cpyWord);
			word = strtok(NULL, " ");
		}
		
		qput(res, tempq);
		//char line[1000];
		//char* tempWord = qget(tempq);
		//printf("tempWord ------------------------ %s\n", tempWord);
		//qput(tempq, tempWord);
	}
	
	return res;
}

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


bool validateWord(char* input){
	int i;
	if(strcmp(input, "\n") == 0){
		printf("failed bc n\n");
		return false;
	}
  for(i = 0; input[i] != '\0'; i++){                       
		if(!(isalpha(input[i]))){
			if(input[i] != ' '){
				return false;
			}
		}
	}
	return true;
}

bool validateAll(char* wordArr[1000]){
	//memset(&wordArr[0]
	int count = 0;
	char* curr = wordArr[count];
	count++;
	while(curr != NULL){
		if(!(validateWord(curr))){
			return false;
		}
		curr = wordArr[count];
		count++;
	                     
	}
	return true;
}


bool validateDir(char* dirname){
	DIR* dir;
	if(!(dir = opendir(dirname))){                                                                                                                           
		printf("directory not found\n");
		return false;                                                                                                                                          
  }
	int n = 0;
	struct dirent *d;
	while((d = readdir(dir)) != NULL){
		n++;
		if(n > 2){
			break;
		}
	}
	if(n <= 2){
		printf("directory not crawled\n");
		closedir(dir);
		return false;
	}
	closedir(dir);
	return true;
}
	
bool validateInd(char* indexname){
	FILE* file;
	if(!(file = fopen(indexname, "r"))){
		return false;
	}			
	fclose(file);
	return true;
}


bool andOr(char* word){
	if((strcmp("and", word) == 0) || (strcmp("or", word) == 0)){
		return true;
	}
	return false;
}

bool validateQuery(char* wordArr[1000]){
	int arrCount = 0;
	char* curr = wordArr[arrCount];
	char* lagCurr;
	arrCount++;

	bool canBe = false;
	while(curr != NULL){
		if(andOr(curr)){
			if(!(canBe)){
				return false;
			}
			canBe = false;
		}
		else{
			canBe = true;
		}
		lagCurr = curr;
		curr = wordArr[arrCount];
		arrCount++;
	}
	if(andOr(lagCurr)){
		return false;
	}
	return true;
}

void queueToDumb(queue_t* qt, char* buffer[1000]){
	memset(&buffer[0], 0, 1000);
	char* word;
	int count = 0;
	//queue_t* tempQ = qopen();
	while((word = qget(qt)) != NULL){
		//qput(tempQ, word);
		buffer[count] = word;
		count++;
	}
	//char* tempW;
	//while(((tempW = qget(tempQ)) != NULL)){                                                                                        
	//qput(qt, tempW);                                                                                                               
	// }
	//qclose(tempQ);
}

int getWordArrSize(char* input[1000]){
	int count = 0;
	char* word;
	while((word = input[count]) != NULL){
		count++;
	}
	return count;
}

int makeArr(char input[1000], char* wordArr[1000]){
	memset(&wordArr[0], 0, 1000);
	int count = 0;
	char* word = strtok(input, " ");
	
	while(word != NULL){
		wordArr[count] = word;
		count++;
		word = strtok(NULL, " ");
	}
	return count;
	//wordArr[count] = NULL;
}

#if 0
queue_t* makeQueue(char* input[1000]){
	queue_t* resQ = qopen();
	char* word = strtok(input, " ");
	while(word != NULL){
		qput(resQ, word);
		word = strtok(NULL, " ");
	}
	return resQ;
}
#endif

bool wordSearch(void* elementp, const void* searchkeyp){                                                            
  struct wordObj* obj = (struct wordObj*) elementp;                                                               
  char* word = (char*)searchkeyp;
	//printf("wordObj->word: %s\n", obj->word);
	//printf("word: %s\n", word);
  if(strcmp(obj->word, word) == 0){
		//printf("word is found!!\n");                                 
    return true;                                                                                                    
  }
	//printf("word not found\n");                                   
  return false;                                                                                                     
}  

int occurrencesInID(int id, char* word, char* passfile){                                                            
  hashtable_t* index = indexload(passfile);
	//i used this to check that the hashtable is making correctly and it is??????
	//indexsave(index, "../indexpages/indexcheck");
 
  struct wordObj* wordQ = (wordObj*) hsearch(index, wordSearch, word, strlen(word));

	
  if(wordQ == NULL){
		happly(index, closeHT);
		hclose(index);
		return 0;                                                                                                       
  }                                                                                                                 
  int res = 0;                                                                                                      
  struct docObj* obj;
	//so for the words that it does find it in the hash the queue is null?!?!
  while((obj = (docObj*)qget(wordQ->q)) != NULL){
		//printf("checking for id frequency!!\n");                                 
    if(obj->id == id){                                                                                              
      res = obj->freq;
		  //printf("resres: %d\n", res);
    }
		free(obj);
    //qclose(wordQ->q);                                                                                             
  }                                                                                                                 
  //qclose(wordQ->q);
	happly(index, closeHT);
  hclose(index);                                                                                                    
  return res;                                                                                                       
}  


bool searchfn(void* elementp, const void* keyp){
	char* word = (char*) elementp;
	char* key = (char*) keyp;

	if(strcmp(word, key) == 0){
		return true;
	}
	return false;
}




int rankOfDoc(int id, queue_t* q, char* passfile, char* wordArr[1000], int wordArrSize){

	count = linecount;
  int min = 1000000;                                                                                                
  int temp;                                                                                                         
  //char* curr = wordArr[count];                                                                                   
  //count++;
	char* curr;
	//char* curr = qget(q);
	//printf("curr: %s\n")
  while((curr = qsearch(q, searchfn, wordArr[count])) != NULL){
		//printf("curr: %s\n", curr);
		//printf("wordArr: %s\n", wordArr[count]);
		if(((strlen(curr) > 2) || strcmp(curr, "or") == 0) && (strcmp(curr, "and") != 0)) {                             
      temp = occurrencesInID(id, curr, passfile);                                                                   
			
      if(temp == 0){                                                                                                
				//printf("if temp 0\n");
				min = 0;                                                                                                    
      }                                                                                                             
      if(temp < min && temp != 0){                                                                                  
        min = temp;                                                                                                 
      }                                                                                                             
    }
		//qput(q, curr);
    //curr = wordArr[count];                                                                                          
    //linecount++;
		count++;
		//printf("linecount: %d\n", linecount);
		//printf("arrSize: %d\n", wordArrSize);
		if(count >= wordArrSize){
			 break;
		}
		//printf("end of while\n");
  }                                                                                                                 
  if(min == 1000000){                                                                                               
    min = 0;                                                                                                        
  }
	//printf("almost out of ROD\n");
	//linecount = count + 1;
	return min;                                                                                                       
}



int rankOfDocARRAY(int id, char* wordArr[1000], char* passfile){                                                                                                            
  int count = 0;
	
	int min = 1000000;
	
  int temp;
	
  char* curr = wordArr[count];
	
  count++;
	
  //printf("curr: %s\n")
	
  while(curr != NULL){
		
    if((strlen(curr) > 2) && strcmp(curr, "or") != 0 && strcmp(curr, "and") != 0) {
			
      temp = occurrencesInID(id, curr, passfile);
			
     
      if(temp == 0){
			
        min = 0;			
      }
	
      if(temp < min && temp != 0){
			 
        min = temp;	
      }			
   }
 	
   curr = wordArr[count];
	 
    count++;		
 }
 
  if(min == 1000000){		
   min = 0;
 	
  }	
 return min;
 
}   


queue_t* splitForQuery(char* wordArr[1000], queue_t* resQ){
	//queue_t* resQ = qopen();
	
	char* curr;
	int count = 0;
	
	while((curr = wordArr[count]) != NULL){
		count++;
		//char* buffer[1000];
		queue_t* buffer = qopen();
		
		while(strcmp(curr, "or") != 0){
			//buffer[buffCount] = curr;
			//buffCount++;
			qput(buffer, curr);
			curr = wordArr[count];
			count++;
			if(curr == NULL){
				break;
			}
		}
		qput(resQ, buffer);
		//memset(&buffer[0], 0, 1000);
	}

	return resQ;
}

void queueToArr(int id, queue_t* resQ, int intArr[1000], char* passfile, char* wordArr[1000], int wordArrSize){
	memset(&intArr[0], 0, 1000);
	queue_t* q;
	queue_t* tempQ = qopen();
	int freq;
	int arrCount = 0;


	while(((q = qget(resQ)) != NULL)){
		qput(tempQ, q);
		//printf("ROD while loop\n");
		//for(int i = 1; i <= maxID; i++){
		//printf("through loop\n");
		freq = rankOfDoc(id, q, passfile, wordArr, wordArrSize);
		//printf("allfreq: %d\n", freq);
		if(freq > 0){
			intArr[arrCount] = freq;
			arrCount++;
		}
		//}
		linecount = count + 1;
		
		//memset(&wordArr[0], 0, 1000);
	}
	while(((q = qget(tempQ)) != NULL)){
		qput(resQ, q);
	}
	qclose(tempQ);
}

int sumArray(int intArr[1000]){
	
	int sum = 0;
	int arrCount = 0;
	//printf("here\n");
	while(intArr[arrCount] != '\0'){
		//printf("arrc: %d", intArr[arrCount]);
		sum += intArr[arrCount];
		arrCount++;
	}
	return sum;
}




int occurrences(char* word, char* passfile){
	hashtable_t* index = indexload(passfile);

	struct wordObj* wordQ = (wordObj*) hsearch(index, wordSearch, word, strlen(word));
	if(wordQ == NULL){
		return 0;
	}
	int res = 0;
	struct docObj* f;
	while((f = (docObj*)qget(wordQ->q)) != NULL){
		res += f->freq;
	}
	happly(index, closeHT);
	hclose(index);
	if(res == 0){
		return 0;
	}
	else{
		return res;
	}
}

bool docSearch(void* docp, const void* keyp) {                                               
  struct docObj* doc = (struct docObj*)docp;                                                 
  int* idp = (int*)keyp;                                                                     
                                                                                             
  if(doc->id == *idp) {                                                                      
    return true;                                                                             
  }                                                                                          
  return false;                                                                              
}   

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


hashtable_t* indexer(char* pagedir){
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
				documentsToHash(ht, word, id);                                                     
				count++;                                                                           
			}                                                                                    
        else{                                                                                
          free(word);                                                                        
        }                                                                                    
		}                                                                                      
		webpage_delete(page);                                                                  
		pos = 0;                                                                               
		//}                                                                                    
    //printf("\n");                                                                          
  }                                                                                          
	
 	// indexsave(ht, indexnm);                                                                    
  
  //happly(ht, closeHT);                                                                       
  
  //free(pagedir);                                                                             
  
  closedir(dfd);                                                                                                                                                                      
  return ht;       
}


//#if 0
//step 5
int main(int argc, char*argv[]){
  //char* passfile = "../indexpages/indexnm";
	int quietFlag = 0;

	//parameter checking, setting quiet flag
	if(argc != 3 && argc != 6) {
		printf("usage: query <pageDirectory> <indexFile> [-q]\n");
		exit(EXIT_FAILURE);
	}
	if(argc == 6) {
		if(strcmp(argv[3], "-q") == 0) {
			quietFlag = 1;
		}
		else {
			printf("usage: query <pageDirectory> <indexFile> [-q input.txt outputFile]\n");
			exit(EXIT_FAILURE);
		}
	}
	
	char* pagedir = malloc(100);
	char* indexfile = malloc(100);
	
	
	sprintf(pagedir, "%s", argv[1]);
	sprintf(indexfile, "%s", argv[2]);


	if(!(validateDir(pagedir))){
		free(pagedir);
		free(indexfile);
    exit(EXIT_FAILURE);
  }   

	if(!(validateInd(indexfile))){
		hashtable_t* ht = indexer(pagedir);
		indexsave(ht, indexfile);
		happly(ht, closeHT);
		hclose(ht);
	}
	
	struct dirent* dp;

	DIR* dir;
	dir = opendir(pagedir);
	
  FILE *file;
  char path[1000];
  
  char input[1000] = "input";
  char* wordArr[1000] = {"wordArr"};
  int intArr[1000] = {1};
  int wordArrSize;
	queue_t* resQ = qopen();
	
	if(quietFlag == 0){
	//start query
		while(strcmp(input, "eof") != 0) {                                                                                      
			//queue_t* resQ;                                                                                                       
      
			//int freq = 0;                                                                                                       
			int rank = 0;                                                                                                         
			printf("> ");                                                                                                         
			memset(&input[0], 0, 1000);
			getInput(input);                                                                                                      
			
			wordArrSize = makeArr(input, wordArr);                                                                                

			if(!(validateAll(wordArr))) {                                                                                         
				printf("invalid input\n");                                                                                               
			}                                                                                                                     
			else if(!(validateQuery(wordArr))){                                                                                   
				printf("invalid query\n");                                                                                          
			}                                                                                                                     
			else if(strcmp(input, "eof") != 0) {                                                                                  
				resQ = splitForQuery(wordArr, resQ);                                                                                      
				
				int wordCount = 0;                                                                                                  
				char* currWord = wordArr[wordCount];                                                                                
				wordCount++;                                                                                                        
				
				while(currWord != NULL){                                                                                            
					printf("%s ", currWord);                                                                                          
					currWord = wordArr[wordCount];                                                                                    
					wordCount++;                                                                                                     
				}                                                                                                                   
				
				printf("\n");                                                                                                       
				while((dp = readdir(dir)) != NULL){                                                                                 
					queueToArr(atoi(dp->d_name), resQ, intArr, indexfile, wordArr, wordArrSize); 
					
					rank = sumArray(intArr);                                                                                          
					//printf("rank: %d\n", rank);                                                                                     
					//count = 0;                                                                                                      
					//linecount = 0;                                                                                                  
					if(rank > 0){                                                                                                     
						char* url;                                                                                                      
						url = malloc(100);                                                                                              
						printf("rank: %d:", rank);                                                                                      
						printf(" doc: %s: ", dp->d_name);                                                                                        
						sprintf(path, "%s/%s", pagedir, dp->d_name);                                                                               
						file = fopen(path, "r");                                                                                        
						fscanf(file, "%s", url);                                                                                        
						printf("%s\n", url);                                                                                            
						fclose(file);                                                                                                   
						free(url);                                                                                                      
					}
					//dir = opendir(pagedir);
					count = 0;                                                                                                        
					linecount = 0;                                                                                                    
					memset(&intArr[0], 0, 1000);                                                                                      
				}
				
 			}
			closedir(dir);
			dir = opendir(pagedir);
			printf("\n");
			
			memset(&wordArr[0], 0, 1000);
			queue_t* closeq;
			char* word;
			while((closeq = qget(resQ)) != NULL){
				while((word = qget(closeq)) != NULL){
					//get word out
				}
				qclose(closeq);
			}                  
			//qclose(resQ);
		}
		//queue_t* closeq;
		//char* word;
		//while((closeq = qget(resQ)) != NULL){
		//while((word = qget(closeq)) != NULL){
		//	printf("%s\n", word);
		//}
		//qclose(closeq);
		//}
		//close queues in resq??
		//qapply(resQ, closeQ);
		qclose(resQ);
		closedir(dir);
		
 	}
	else{
		int docCount = 0;                                                                                                                     
		while((dp = readdir(dir)) != NULL){
			docCount++;
		}
		closedir(dir);
	
		char* inputfilename = malloc(100);
		sprintf(inputfilename, "%s", argv[4]);                                                                                              
		char* outputfilename = malloc(100);
		sprintf(outputfilename, "%s", argv[5]);

		//queue_t* resQ;
		//int freq = 0;
		
		int rank = 0;

		FILE* inputfile = fopen(inputfilename, "r");
		FILE* outputfile = fopen(outputfilename, "w");

		free(inputfilename);
		free(outputfilename);

		queue_t* qt = qopen();
		qt = getFileInput(inputfile, qt);

		queue_t* temp;
		char* wordArr[1000] = {"w"};
		//char* tempW;

		while((temp = qget(qt)) != NULL){
			//while((tempW = qget(temp)) != NULL){
			//printf("tempW: %s\n", tempW);
			//}
			queueToDumb(temp, wordArr);
			//temp = makeQueue(wordArr);
			//char* w;
			//while((w = qget(temp)) != NULL){
			//printf("%s\n", w);
			//free(w);
			//}
			//qclose(temp);
			
			wordArrSize = getWordArrSize(wordArr);
			int wordCount = 0;                                                                                                           
      char* currWord = wordArr[wordCount];                                                                                         
      wordCount++;
		
      //fprintf(outputfile, "%s", "\n");
			if(!(validateAll(wordArr))) {
        fprintf(outputfile, "invalid input\n");                                                                                    
				int wordArrC = 0;                                                                                                          
                                                                                                                                   
        //printf("word arr size %d\n", wordArrSize);                                                                               
                                                                                                                                   
        while(wordArrC<wordArrSize){                                                                                               
          //printf(".\n");                                                                                                         
          free(wordArr[wordArrC]);                                                                                                 
          wordArrC++;                                                                                                              
        }
        qclose(temp);
        continue;
			}
	 
      else if(!(validateQuery(wordArr))){                                                         
        fprintf(outputfile, "invalid query\n");
				int wordArrC = 0;                                                                                                             
				//printf("word arr size %d\n", wordArrSize);                                                                                  
				while(wordArrC<wordArrSize){
					//printf(".\n");
					free(wordArr[wordArrC]);
					wordArrC++;
				}
				qclose(temp);
				continue;
 			}
			
			while(currWord != NULL){                                                                                                     
        fprintf(outputfile, "%s ", currWord);                                                                   
        currWord = wordArr[wordCount];                                                                                             
        wordCount++;                                                                                                               
      }  

			fprintf(outputfile, "\n");
			
			resQ = splitForQuery(wordArr, resQ);	
				
			for(int i = 0; i < docCount; i++){
				//printf("dir while\n");
				queueToArr(i, resQ, intArr, indexfile, wordArr, wordArrSize);
				rank = sumArray(intArr);
			
				//count = 0;                                                                                                             
				//linecount = 0;                                                                                                         
				if(rank > 0){                                                                                                            
					char* url;                                                                                                             
					url = malloc(100);                                                                                                     
					fprintf(outputfile, "rank: %d: ", rank);			
					fprintf(outputfile, "doc: %d: ", i);                                                                            
					sprintf(path, "%s/%d", pagedir, i);                                                                           	
					file = fopen(path, "r");                                                                                               
					fscanf(file, "%s", url);                                                                                               
					fprintf(outputfile, "%s\n", url);                                                                                        
					fclose(file);                                                                                                          
					free(url);                                                                                                             
				}                                                                                                                        
				//dir = opendir(pagedir);
				//fprintf(outputfile, "\n");
				count = 0;
        linecount = 0;                                                                                                           
				memset(&intArr[0], 0, 1000);
			}
			//char* w;
			//while((w = qget(temp)) != NULL){
			//printf("%s\n", w);
			//free(w);
			//}
			int wordArrC = 0;
			//printf("word arr size %d\n", wordArrSize);
			while(wordArrC<wordArrSize){
				//printf(".\n");
				free(wordArr[wordArrC]);
				wordArrC++;
			}
			
			//memset(&wordArr[0], 0, 1000);
			queue_t* closeq;                                                                                                                   char* word;
			
			while((closeq = qget(resQ)) != NULL){
				while((word = qget(closeq)) != NULL){
					//printf("through\n");
					//free(word);
				}
				qclose(closeq);
			}
			
			//char* w;                                                                                                                   
			// while((w = qget(temp)) != NULL){                                                                                         
			//printf("%s\n", w);                                                                                                        
			//free(w);                                                                                                                  
      //}

			
			qclose(temp);  
		}
			
			//dir = opendir(pagedir);
			fprintf(outputfile,"\n");
			//memset(&wordArr[0], 0, 1000);		
			qclose(qt);
			qclose(resQ);
			fclose(inputfile);
			fclose(outputfile);
			
		}
	
			free(pagedir);
			free(indexfile);
			//closedir(dir);
			//free(dp);
		}

#if 0
			int main(void){                                                                                               
			char* passfile = "../indexpages/indexnm";                                                                         
	//char* indexdir = "../indexpages/";                                                                              
	char* pages = "../pages/";
	
	FILE *file;                                                                                                        
	char path[100];
	int docCount = 82;                                                                                                                                                                                                                     
  //struct queue_t* docRank = (struct queue_t*)qopen();                                                             
	char input[1000];                                                                                                 
	char* wordArr[1000];                                                                                              	
	
	int intArr[1000];		
	int wordArrSize;
	
	while(strcmp(input, "eof") != 0) {                                                                                
		queue_t* resQ;
		
		//int freq = 0;                                                                                                 
		int rank = 0;                                                                                                   
		printf("> ");                                                                                                   
		
		getInput(input);                                                           
		
		wordArrSize = makeArr(input, wordArr);                
		
		if(!(validateAll(wordArr))) {
			printf("invalid input\n");
		}
		else if(!(validateQuery(wordArr))){
			printf("invalid query\n");
		}
		else if(strcmp(input, "eof") != 0) {
			resQ = splitForQuery(wordArr);
									
			int wordCount = 0;                                                               
			char* currWord = wordArr[wordCount];                                               
			wordCount++;                                                                                
			
			while(currWord != NULL){                                                                                      
				printf("%s ", currWord);                                                                                    
				currWord = wordArr[wordCount];                                                                              
				wordCount++;                                                                                                	
		  }                                                                                                             
			
			printf("\n");                                                                                                 
			for(int i = 1; i <= docCount; i++){
				queueToArr(i, resQ, intArr, docCount, passfile, wordArr, wordArrSize);                                             
				rank = sumArray(intArr);                        
				//printf("rank: %d\n", rank);
				//count = 0;                                                                                             
				//linecount = 0;  
				if(rank > 0){                                                                                               
					char* url;                                                                                                
					url = malloc(100);                                                                                        
					printf("rank: %d:", rank);                                                                                
					printf(" doc: %d: ", i);                                                                                  
					sprintf(path, "%s/%d", pages, i);                                                                         
					file = fopen(path, "r");
					fscanf(file, "%s", url);                                                                                  
					printf("%s\n", url);                                                                                      
					fclose(file);                                                                                             
					free(url);                                                                                                
				}
				count = 0;
				linecount = 0;
				memset(&intArr[0], 0, 1000);
			}                                                                                                             
			
		}                                                                                                               
		printf("\n");                                                                                                   
		memset(&wordArr[0], 0, 1000);                                                                                   
		
	}
	free(path);
	free(wordArr);
	free(intArr);
	free(input);
}



#endif

#if 0
//three main
int three_main(void){                                                                 
	char* passfile = "../indexpages/indexnm";
	//char* indexdir = "../indexpages/";
	char* pages = "../pages/";

 FILE *file;                                                                     
 char path[100];                                                                 
                                                                                  


	
	int docCount = 7;
	
	//struct queue_t* docRank = (struct queue_t*)qopen();
  char input[1000];                                                                           
  char* wordArr[1000];                                                                        
	//char* url;
	
  while(strcmp(input, "eof") != 0) {                                                          
 
		//int freq = 0;
		int rank = 0;
		printf("> ");                                                                             
    
    getInput(input);                                                                          
    makeArr(input, wordArr);                                                                  
		
    if(!(validateAll(wordArr))) {                                                     
			printf("invalid input\n");                                                             
    }
		else if(strcmp(input, "eof") != 0) {
			int wordCount = 0;
			char* currWord = wordArr[wordCount];
			wordCount++;                                                                                                       
			while(currWord != NULL){                                                                                            
				printf("%s ", currWord);
				currWord = wordArr[wordCount];
				wordCount++;                                                                                                      
			}
			
			printf("\n");   
			for(int i = 1; i <= docCount; i++){
				rank = rankOfDocARRAY(i, wordArr, passfile);
				
				if(rank > 0){
					char* url;
					url = malloc(100);
					printf("rank: %d:", rank);
					printf(" doc: %d: ", i);
					sprintf(path, "%s/%d", pages, i);      
					file = fopen(path, "r");
					fscanf(file, "%s", url);
					printf("%s\n", url);
					fclose(file);
					free(url);
				}
			}
			
		}	
		printf("\n");                                                                           
		memset(&wordArr[0], 0, 1000);                                                          
  }                                                                               
}    
#endif

#if 0
int two_main(void){                                                            
	char* passfile = "../indexpages/1";

	char input[1000];
  char* wordArr[1000];                                                         
                                                                               
  while(strcmp(input, "eof") != 0){                                            
    int count = 0;                                                             
    int min = 10000000;                                                                           int freq = 0;
    printf("> ");                                                              
                                                                              
    getInput(input);                                                           
    makeArr(input, wordArr);                                                   
                                                                               
    if(!(validateAll(wordArr))){                                               
        printf("invalid input\n");                                             
    }                                                                          
    else if(strcmp(input, "eof") != 0){                                        
      char* curr = wordArr[count];                                             
      while(curr != NULL){
				//freq = 0;
				//printf("%s\n", curr);
				if((strlen(curr) > 2) && (strcmp(curr, "and") != 0) && (strcmp(curr, "or") != 0)){
					printf("%s:", curr);
					freq = occurrences(curr, passfile);
					if(freq < min){
						min = freq;
					}
					if(freq >  0){
						printf("%d ", freq);
					}
				}
				count++;
				curr = wordArr[count];
			}
			if(freq > 0){
			printf("-- %d", min);
			//printf("\n");
			}
			printf("\n");
		}
		//printf("\n");                                                            
		memset(&wordArr[0], 0, 1000);                                            
	}
}                                                                            
#endif 



#if 0
int one_main(void){
	char input[1000];
	char* wordArr[1000];
	
	while(strcmp(input, "eof") != 0){
		int count = 0;
		
		printf("> ");
		
		getInput(input);
		makeArr(input, wordArr);

		if(!(validateAll(wordArr))){
				printf("invalid input\n");
		}
		else if(strcmp(input, "eof") != 0){
			char* curr = wordArr[count];
			while(curr != NULL){
				printf("%s ", curr);
				count++;
				curr = wordArr[count];
			}
			printf("\n");
			memset(&wordArr[0], 0, 1000);
		}
	}
}

#endif
