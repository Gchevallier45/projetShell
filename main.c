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
	{
		//Get your session info
        infos=getpwuid(getuid());
		gethostname(hostname, 256);
		getcwd(workingdirectory, 256);
        //Print it to the console
		sprintf(str, "\n{myshell}%s@%s:%s$ ", infos->pw_name, hostname, workingdirectory);
		readlineptr = readline(str);
        //printf("testy");

        if(strlen(readlineptr) != 0){
            cmd *command = malloc(sizeof(cmd));
            parseMembers(readlineptr,command);

            ret = exec_command(command);
            if(ret == -1){
                printf("ERROR : \"%s\" is not a valid command or a valid executable",command->initCmd);
            }

            freeCmd(command);
            free(command);
        }
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
