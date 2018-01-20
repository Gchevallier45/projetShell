#include "shell_fct.h"

pid_t pid;

void alarmEvent(){
    kill(pid, SIGKILL);
}

int exec_command_with_fork(cmd* my_cmd){
    int **tube,status;
    char **currentMember;
    int in = 0, out = 0;

    tube = malloc(my_cmd->nbCmdMembers*sizeof(int*));

    if(strcmp(my_cmd->redirection[0][0],"") != 0){ //file to stdin redirection
        //printf("redirection !!!!!!! %s",my_cmd->redirection[0][0]);
        in = open(my_cmd->redirection[0][0],O_RDONLY);
        if(in < 0){
            printf("ERROR : failed to open file \"%s\" for redirection\n",my_cmd->redirection[0][0]);
            return -2;
        }
    }

    if(strcmp(my_cmd->redirection[my_cmd->nbCmdMembers-1][1],"") != 0){ //stdout redirection to file
        switch(my_cmd->redirectionType[my_cmd->nbCmdMembers-1][1]){
        case APPEND:
            out = open(my_cmd->redirection[my_cmd->nbCmdMembers-1][1],O_APPEND|O_WRONLY);
            if(out < 0)
                out = open(my_cmd->redirection[my_cmd->nbCmdMembers-1][1],O_CREAT|O_WRONLY,S_IWUSR|S_IRUSR);
            break;

        case OVERRIDE:
            out = open(my_cmd->redirection[my_cmd->nbCmdMembers-1][1],O_CREAT|O_WRONLY,S_IWUSR|S_IRUSR);
            break;
        }

        if(out < 0){
            printf("ERROR : failed to open file \"%s\" for redirection\n",my_cmd->redirection[my_cmd->nbCmdMembers-1][1]);
            return -2;
        }
    }
    else if(strcmp(my_cmd->redirection[my_cmd->nbCmdMembers-1][2],"") != 0){ //stderr redirection to file
        switch(my_cmd->redirectionType[my_cmd->nbCmdMembers-1][2]){
        case APPEND:
            out = open(my_cmd->redirection[my_cmd->nbCmdMembers-1][2],O_APPEND|O_WRONLY);
            if(out < 0)
                out = open(my_cmd->redirection[my_cmd->nbCmdMembers-1][2],O_CREAT|O_WRONLY,S_IWUSR|S_IRUSR);
            break;

        case OVERRIDE:
            out = open(my_cmd->redirection[my_cmd->nbCmdMembers-1][2],O_CREAT|O_WRONLY,S_IWUSR|S_IRUSR);
            break;
        }

        if(out < 0){
            printf("ERROR : failed to open file \"%s\" for redirection\n",my_cmd->redirection[my_cmd->nbCmdMembers-1][2]);
            return -2;
        }
    }

    for(int i=0; i<my_cmd->nbCmdMembers;i++){ // Loop for command members to execute with pipes and fork (n members)
        tube[i] = malloc(2*sizeof(int));
        pipe(tube[i]);
        currentMember = my_cmd->cmdMembersArgs[i];
        pid = fork();

        if(pid == 0){ //Child process
            if(i>0){
                close(tube[i-1][1]);
                dup2(tube[i-1][0],0);
                close(tube[i-1][0]);
            }
            else if(in != 0){ //file to stdin redirection
                dup2(in,0);
            }

            if(i<my_cmd->nbCmdMembers-1){
                close(tube[i][0]);
                dup2(tube[i][1],1);
                close(tube[i][1]);
            }
            else if(out != 0){ //stdout redirection to file
                dup2(out,1); //Connect stdout to file
            }

            return execvp(currentMember[0], currentMember);

        }
        else if(pid == -1){
            printf("FATAL ERROR : fork() failed\n");
            return -2;
        }

        if(i > 0)
		{
			close(tube[i-1][0]);
			close(tube[i-1][1]);
		}
    }

    signal(SIGALRM,alarmEvent);
    alarm(5);

    waitpid(pid,&status,0);

    if(in!=0)
        close(in);

    if(out!=0)
        close(out);

    for(int i=0;i<my_cmd->nbCmdMembers;i++)
    {
        free(tube[i]);
    }
    free(tube);
}

int exec_command(cmd* my_cmd){
    printf("COMMAND OUTPUT : (on pid %d)\n-------------------------------\n",getpid());
    if(strcmp(my_cmd->initCmd,"exit") == 0){ //If command is exit, close the shell
        return MYSHELL_FCT_EXIT;
    }
    else if(strcmp(my_cmd->cmdMembersArgs[0][0],"cd") == 0){ //If command id cd, change directory
        if(chdir(my_cmd->cmdMembersArgs[0][1]) != 0){
            printf("ERROR : invalid directory\n");
        }
        return 0;
    }
    else{ //Otherwise, execute command with pipes and fork
        return exec_command_with_fork(my_cmd);
    }
}
