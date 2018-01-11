#include <pwd.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "shell_fct.h"

//sudo apt-get install libreadline-dev

//To complete
int main(int argc, char** argv)
{
	//..........
	int ret = MYSHELL_CMD_OK;
	char* readlineptr;
	struct passwd* infos;
	char str[1024];
	char hostname[256];
	char workingdirectory[256];

	//..........
	while(ret != MYSHELL_FCT_EXIT)
	//int i=0;
	//while(i==0)
	{
		//Get your session info
        infos=getpwuid(getuid());
		gethostname(hostname, 256);
		getcwd(workingdirectory, 256);
        //Print it to the console
		sprintf(str, "\n{myshell}%s@%s:%s$ ", infos->pw_name, hostname, workingdirectory);
		readlineptr = readline(str);
        //printf("testy");

        cmd *command = malloc(sizeof(cmd));
        parseMembers(readlineptr,command);
        exec_command(command);
        freeCmd(command);
        free(command);
        free(readlineptr);
        //Your code goes here.......
        //Parse the comand
        //Execute the comand
        //Clean the house
        //..........
	}
	//..........
	return 0;
}
