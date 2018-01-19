#include "shell_fct.h"

pid_t pid;

void alarmEvent(){
    kill(pid, SIGKILL);
}

int exec_command_with_fork(cmd* my_cmd){
    int tube[2],status;
    char **currentMember;

    int in,out;
    if(strcmp(my_cmd->redirection[0][0],"") != 0){ //file to stdin redirection
        //printf("redirection !!!!!!! %s",my_cmd->redirection[0][0]);
        in = open(my_cmd->redirection[0][0],O_RDONLY);
        if(in < 0){
            printf("ERROR : failed to open file \"%s\" for redirection\n",my_cmd->redirection[0][0]);
            return 0;
        }
    }
    else{
        in = 0;
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
            return 0;
        }
    }
    else if(strcmp(my_cmd->redirection[my_cmd->nbCmdMembers-1][2],"") != 0){ //stderr redirection to file
        switch(my_cmd->redirectionType[my_cmd->nbCmdMembers-1][2]){
        case APPEND:
            out = open(my_cmd->redirection[my_cmd->nbCmdMembers-1][2],O_APPEND|O_WRONLY);
            if(out < 0)
                out = open(my_cmd->redirection[my_cmd->nbCmdMembers-1][1],O_CREAT|O_WRONLY,S_IWUSR|S_IRUSR);
            break;

        case OVERRIDE:
            out = open(my_cmd->redirection[my_cmd->nbCmdMembers-1][2],O_CREAT|O_WRONLY,S_IWUSR|S_IRUSR);
            break;
        }

        if(out < 0){
            printf("ERROR : failed to open file \"%s\" for redirection\n",my_cmd->redirection[my_cmd->nbCmdMembers-1][2]);
            return 0;
        }
    }

    for(int i=0; i<my_cmd->nbCmdMembers;i++){ // Loop for command members to execute with pipes and fork (n members)
            printf("boucle\n");
        currentMember = my_cmd->cmdMembersArgs[i];
        pipe(tube);
        pid = fork();
        if(pid == 0){ //Child process
            if(in!=0){
                printf("connect\n");
                dup2(in,0); //Connect old pipe output to stdin
                close(in); //Close old pipe output (to open entry)
            }
            if(i!=my_cmd->nbCmdMembers-1){
                dup2(tube[1],1); //Connect stdout to current pipe input
                close(tube[0]); //Close current pipe output to allow writing in pipe input
            }
            else if(strcmp(my_cmd->redirection[i][1],"") != 0 || strcmp(my_cmd->redirection[i][2],"") != 0){ //stdout redirection to file
                dup2(out,1); //Connect stdout to file
                close(tube[0]); //Close current pipe output to allow writing in pipe input
            }

            return execvp(currentMember[0], currentMember);
        }
        else if(pid == -1){
            printf("FATAL ERROR : fork() failed\n");
        }

        signal(SIGALRM,alarmEvent);

        alarm(5);

        //wait(&pid);
        waitpid(pid,&status,0);
        printf("yolo\n");
        close(tube[1]); //Close current pipe entry (to open output)
        in=tube[0]; //Store the output of the current pipe to reuse it on the next pipe
    }

    if(out>0){
        close(out);
    }

    close(tube[0]);
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
