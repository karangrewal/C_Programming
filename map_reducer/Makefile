CFLAGS = -Wall -std=c99 -Werror

mapreduce: mapworker.o linkedlist.o reduceworker.o word_freq.o 
        gcc $(CFLAGS) -o mapreduce master.c mapworker.o reduceworker.o word_freq.o linkedlist.o

mapworker.o: mapworker.c  
        gcc $(CFLAGS) -c mapworker.c 

reduceworker.o: reduceworker.c  
        gcc $(CFLAGS) -c reduceworker.c  

word_freq.o: word_freq.c mapreduce.h 
        gcc $(CFLAGS) -c word_freq.c mapreduce.h  

linkedlist.o: linkedlist.c linkedlist.h
        gcc $(CFLAGS) -c linkedlist.c linkedlist.h

clean: 
        rm mapreduce *.o
