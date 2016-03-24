#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <getopt.h>
#include "mapreduce.h"


/*
 * Parses command line arguments.
 */
void parse_arguements(char* dir, int *num_m, int * num_r, int argc, char 
        **argv){
    
    char c;
        
    //Parses arguements 
    while ((c = getopt(argc, argv, "r:m:d:")) != -1){
        
        switch (c){

            case 'r':
                *num_r = atoi(optarg);
                break;
            case 'm':
                *num_m = atoi(optarg);
                break;
            case 'd':
                strncpy(dir, optarg, MAX_FILENAME);
                break;
            case '?':
                exit(-1);

        }
    }
        
    // check to ensure the value of dir was changed or else exit the 
    // program
    if (strcmp(dir, "") == 0){
        
        printf("No Directory parameter was entered. \n");
        exit(-1);
    
    }
}


/*
 * Close a given pipe. In the given direction. 
 */
void close_pipe(int *pipe, int direction){

    if ((close(pipe[direction])) == -1) {
        perror("close");
        exit(-1);
    }
}


/* Main Method */
int main(int argc, char **argv){
    
    int ls_pipe[2], n;  
    int num_reduce = 2, num_map = 2;
    char directory[MAX_FILENAME] = "";

    parse_arguements(directory, &num_map, &num_reduce, argc, argv);
        
    // error checking for pipe
    if (pipe(ls_pipe) == -1){
        perror("pipe");
        exit(-1);
    }
        
    if ((n = fork()) == 0){

        /* ls process: The child process will write to the pipe, so closes 
        reading end */
        close_pipe(ls_pipe, 0); 
                
        // Reset stdout
        if ((dup2(ls_pipe[1], STDOUT_FILENO)) == -1) {

            perror("dup2");
            exit(-1);

        }
                
        // none should write to the pipe, Writing should be done by stdout
        close_pipe(ls_pipe, 1);
                
        execl("/bin/ls", "/bin/ls", directory, (char *)0);
                
        // error with execl
        exit(-1);
                
    } else if (n > 0){

        /* parent process */
        
        char file_name[MAX_FILENAME]; // The file name from the ls process 
                
        int map_pipe[num_map][2]; // pipe for map workers 
        int map_pipe_pairs[num_map][2]; // pipe for map workers' output 
                
        int reduce_pipe[num_reduce][2]; // pipe for reduce workers 
        int files_index = 0;  // keeps track of the number of files read
        int x; // indices for creating reduce and map worker processes   
                

        /* Close the writing of ls pipe. None of the following process will be 
        writing to this pipe */ 
        close_pipe(ls_pipe, 1);

        //creates the required pipes for the map workers 
        for (x = 0; x < num_map; x++ ){
            
            if (pipe(map_pipe[x]) ==-1 ){
                perror("pipe");
                exit(-1);
            }

            if (pipe(map_pipe_pairs[x]) == -1){
                perror("pipe");
                exit(-1);
            }
        }
                
        // create the required pipes for reduce workers 
        for (x = 0; x < num_reduce; x++ ){

            if (pipe(reduce_pipe[x]) == -1 ){

                perror("pipe");
                exit(-1);

            }
        }
                
        /* CREATION OF THE MAPWORKER PROCESSES */
        for (x = 0; x < num_map; x++){
            
            int map_pid = fork();
            if (map_pid ==0){
                int y;
 
                // Close the reading end  of ls pipe
                close_pipe(ls_pipe, 0);
                                
                // Close all pipes except for the correct ones which the 
                // proccess will be handling 
                for (y = 0; y < num_map; y++ ){
                
                    close_pipe(map_pipe[y], 1);
                    close_pipe(map_pipe_pairs[y], 0); 

                    if (y != x){
                        close_pipe(map_pipe[y], 0);
                        close_pipe(map_pipe_pairs[y], 1); 
                    }

                }
                
                // The map worker will not be writing to the reduce worker 
                // directly 
                for (y = 0; y < num_reduce; y++ ){

                    close_pipe(reduce_pipe[y], 0); 
                    close_pipe(reduce_pipe[y], 1); 

                }
                                
                //execute the mapworker process 
                map_worker(map_pipe_pairs[x][1], map_pipe[x][0]);

                // After excuting the map worker process, close the remaining 
                // pipes
                close_pipe(map_pipe[x], 0);
                close_pipe(map_pipe_pairs[x], 1);
                                
                exit(0); 
                
            } else if (map_pid < 0){

                perror("fork");
                exit(-1); 

            }
        }
        
        /* CREATION OF THE REDUCEWORKER PROCESSES */    
        for (x = 0; x < num_reduce; x++){
            
            int reduce_pid = fork();
            if ((reduce_pid) ==0){
                
                int z;
                
                // Close the reading end  of ls pipe
                close_pipe(ls_pipe, 0);
                                
                // The reduce worker will not be using any of the map worker 
                // pipes 
                for (z = 0; z < num_map; z++ ){

                    close_pipe(map_pipe[z], 0);
                    close_pipe(map_pipe[z], 1);
                    close_pipe(map_pipe_pairs[z], 0);
                    close_pipe(map_pipe_pairs[z], 1);

                }
                                
                for (z = 0; z< num_reduce; z++){
                
                    close_pipe(reduce_pipe[z], 1);
                    if (z != x ){
                        close_pipe(reduce_pipe[z], 0);
                    }

                }
                                
                //execute the reduce worker process
                reduce_worker(reduce_pipe[x][1], reduce_pipe[x][0]);            
                close_pipe(reduce_pipe[x], 0);

                exit(0);
        
            } else if (reduce_pid < 0){

                perror("fork");
                exit(-1); 

            }
        }
        
        /* THIS IS THE MASTER PROCESS */

        //redirect reading to stdin
        if ((dup2(ls_pipe[0], STDIN_FILENO)) == -1) {

            perror("dup2");
            exit(-1);

        }
                
        //close the ls pipe now that input is redirected 
        close_pipe(ls_pipe, 0);
                
                
        // Parent process will not be reading from reduce_pipe
        int s; 
        for (s = 0; s < num_reduce; s++ ){

            close_pipe(reduce_pipe[s], 0);

        }

        for(s = 0; s < num_map; s++){

            close_pipe(map_pipe[s], 0);
            close_pipe(map_pipe_pairs[s], 1);

        }
        
                
        // Distribute files to the map workers 
        while(scanf("%s", file_name) > 0){

            char path[MAX_FILENAME] = ""; 
            strncpy(path, directory, MAX_FILENAME);
            strncat(path, "/", MAX_FILENAME);
            strncat(path, file_name, MAX_FILENAME);
            write(map_pipe[files_index % num_map][1], path, MAX_FILENAME * 
                sizeof(char)); 
            files_index++; 

        }

                
        for(s = 0; s < num_map; s++){
            close_pipe(map_pipe[s], 1);
        }
                
        //Read values from map workers, distribute Pairs to reduce workers 
        for (s = 0; s < num_map; s++){

            Pair tmp = {"", "1"}; 
            while (read(map_pipe_pairs[s][0], &tmp, sizeof(Pair)) > 0){
                
                // Evenly distribute keys
                char first_letter = tmp.key[0];
                write(reduce_pipe[first_letter % num_reduce][1], &tmp, sizeof(Pair));
        
            }
        
        }
        
        // Close writing end of map pipes 
        for (s = 0; s < num_map; s++ ){

            close_pipe(map_pipe_pairs[s], 0);

        }
                
        // Completed writing to reduce workers, close pipes
        for (s = 0; s < num_reduce; s++ ){

            close_pipe(reduce_pipe[s], 1);

        }
                
    } else {
        
        // error while checking for fork
        perror("fork");
        exit(-1);
    }
        
    return 0; 
}
