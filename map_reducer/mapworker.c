#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mapreduce.h"


/* Map Worker Process */
void map_worker(int outfd, int infd){

    char filename[MAX_FILENAME];
    FILE *file;
    char buffer[READSIZE + 1];

    // read input filenames
    while (read(infd, filename, MAX_FILENAME) > 0) {
                        
        // open file
        file = fopen(filename, "r");

        if (file == NULL) {
            perror("file");
            exit(-1);
        }
        
        while (fgets(buffer, READSIZE, file)) {
            
            // add null terminator
            buffer[READSIZE] = '\0'; 
                        
            // call map function
            map((const char *) buffer, outfd);
        
        }
        
        // close file
        fclose(file);

    }

}
