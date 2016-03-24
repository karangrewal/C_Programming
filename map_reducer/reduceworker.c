#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include "mapreduce.h"
#include "linkedlist.h"


/*
 * Write to file if write is successful, exits with  1 otherwise.
 */
void write_out(Pair pair, FILE *file) {

    if (fwrite(&pair, sizeof(Pair), 1, file) != 1) {
        perror("write");        
        exit(-1);
    }
    
}

/*
 * The main function the preforms the reduce worker process.
 * Will read for infd and create a file [pid].out and then write all 
 * unique pairs  
 */
void reduce_worker(int outfd, int infd){
        
    //The recieved pair and the reduced pair
    Pair rec_pair;
    Pair reduced; 

    // LinkedList Structures
    LLKeyValues *keys = NULL;

  
    // read a Pair from the pipe
    while (read(infd, &rec_pair, sizeof(Pair)) > 0) {
    
        //inserts the pair into LLKeyValues 
        insert_into_keys(&keys, rec_pair);
    
    }
        
    /* PID of reduce worker proces */
    int process_id = getpid();
    char filename[MAX_FILENAME];
    
    // get filename
    sprintf(filename, "%d", process_id);
    strncat(filename, ".out", 4);
    FILE *fpointer = fopen(filename, "wb");
    
    // Error checking for open
    if (fpointer == NULL) {
        exit(-1);
    }
        
        
    LLKeyValues *current = keys; 
    while(((*current).head_value != NULL) && (*current).next != NULL){
                
        // Get the reduced key values from reduce 
        reduced = reduce((const char *) (*current).key, 
            (*current).head_value);
                
        // write pair to file
        write_out(reduced, fpointer);
                
        // next key 
        current = (*current).next;

    }
  
        
    // close file since all writing has been completed 
    fclose(fpointer);
    
    // Free all memory on the heap invloving the linked list
    free_key_values_list(keys);
}
