#pragma once

/* indexio.h --- 
 * 
 * 
 * Author: John R. Weingart
 * Created: Tue Oct 29 17:29:07 2019 (-0400)
 * Version: 
 * 
 * Description: 
 * 
 */

//void writeInfo(void* ep);


#include <stdint.h>                                                            
#include <stdio.h>                                                            
#include <string.h>                                        
#include <hash.h>
#include <stdlib.h>                                                           
#include <queue.h>                                                          
#include <webpage.h>    

void indexsave(hashtable_t* ht, char* passfile);



hashtable_t* indexload(char* file);
