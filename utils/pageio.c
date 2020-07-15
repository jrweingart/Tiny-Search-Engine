#include <stdint.h>
#include <webpage.h>
#include <stdio.h>
#include <stdlib.h>

/* pageio.c --- 
 * 
 * 
 * Author: Catherine D. Parnell
 * Created: Tue Oct 22 16:58:21 2019 (-0400)
 * Version: 
 * 
 * Description: 
 * 
 */
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
/*                                                                    * pageload -- loads the numbered filename <id> in direcory <dirnm>   * into a new webpage                                                
 * returns: non-NULL for success; NULL otherwise                      */

webpage_t *pageload(int id, char *dirnm){

	FILE *file;
	char path[100];

	sprintf(path, "%s/%d", dirnm, id);

	file = fopen(path, "r");

	if(file == NULL){
		//printf("file cannot be opened\n");
		return NULL;
	}
	

	char* URL;
	URL = malloc(100); //not sure what the size should be here
	int depth;
	int HTMLlen;
	//char* fpHTML;
	char *HTML,*p;

	fscanf(file, "%s", URL);
	fscanf(file, "%d", &depth);
	fscanf(file, "%d\n", &HTMLlen);
	//perror();

	HTML = (char *)calloc(HTMLlen, (sizeof(char) + 1));
	p = HTML;
	//pHTML = fpHTML;

	char c;
	while((c = fgetc(file)) != EOF){
		*p++= c;
	}

	//fgetc(file, 
	webpage_t* res = webpage_new(URL, depth, HTML);

	free(URL);
	fclose(file);
	//free(file);
	return res;
}
