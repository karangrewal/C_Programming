#include <stdio.h>
#include <string.h>
#include <strings.h>

int main(int argc, char **argv) {

    /*
	criterion values
	1: CPU usage
	2: memory usage
    */
    int criterion = 1;
    char *username = NULL;

    if (argc > 3) {
	printf("Invalid number of arguments provided\n");
	return 1;
    } else if (argc > 1) {
	
	if (argc == 2) {
	    username = argv[1];
	} else {
	    username = argv[2];
	    if (argv[1][1] == 'm') {
		
	    	criterion = 2;
	    }
	}
    }

    char line[100];

    /* process user name */
    char p_user[10];
    
    /* process command */
    char cmd[128];
    
    /* process ID */
    int pid = 0;

    /* most recent process ID */
    int pid_last;

    /* process CPU usage */
    float cpu;

    /* process memory usage */
    float mem;


    /* storing maximum process ID, CPU / memory, command */
    int max_pid;
    float max_criteria = 0;
    char max_cmd[128];


    /* first line of process input file */
    for (int i = 0; i < 11; i++) {
	scanf("%s", line);
    }

    while (pid != pid_last || pid == 0) {
	
	pid_last = pid;
	scanf("%s %d %f %f %*s %*s %*s %*s %*s %*s %127[^\n]", p_user, &pid,
	&cpu, &mem, cmd);

	if (criterion == 1) {

	    if (cpu >= max_criteria && (username == NULL || 0 == strcmp(username,
		p_user))) {

		if (cpu > max_criteria || username == NULL || strncasecmp(username,
		p_user, 1) >= 0) {
		    max_criteria = cpu;
		    max_pid = pid;
		    strncpy(max_cmd, cmd, 127);
		}
	    }
	} else {

	    if (mem >= max_criteria && (username == NULL || 0 == strcmp(username,
		p_user))) {
		if (mem > max_criteria || strncasecmp(username, p_user, 1) >= 0) {		
		    max_criteria = mem;
		    max_pid = pid;
		    strncpy(max_cmd, cmd, 127);
	    	}
	    }
	}
    }

    /* Program output */
    printf("%d\t%.2f\t%s\n", max_pid, max_criteria, max_cmd);

    return 0;
}