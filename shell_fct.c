#include "shell_fct.h"

int exec_command(cmd* my_cmd){
    printf("COMMAND OUTPUT\n--------------\n");
    pid_t pid;
    int tube[2];
    char **currentMember;
    int in = 0;
    //printf("befor %d\n",my_cmd->nbCmdMembers);

    for(int i=0; i<my_cmd->nbCmdMembers;i++){ // Loop for command members to execute with pipes and fork (n members)
        currentMember = my_cmd->cmdMembersArgs[i];
        pipe(tube);
        pid = fork();
        if(pid == 0){ //Child process
            if(i!=0){
                dup2(in,0);
                close(in); //Close pipe output (to open entry)
            }
            if(i!=my_cmd->nbCmdMembers-1)
                dup2(tube[1],1);
            return execvp(currentMember[0], currentMember);

        }
        else if(pid == -1){
            printf("FATAL ERROR : fork() failed\n");
        }
        wait(&pid);
        close(tube[1]); //Close pipe entry (to open output)
        in=tube[0]; //Store the output of the old pipe to reuse it on the next pipe
    }
    close(tube[0]);

    //Last command to execute without pipes
    /*currentMember = my_cmd->cmdMembersArgs[my_cmd->nbCmdMembers-1];
    pipe(tube);
    pid = fork();
    if(pid == 0){ //Child process
        dup2(in, 0);
        return execvp(currentMember[0], currentMember);
    }
    else if(pid == -1){
        printf("FATAL ERROR : fork() failed\n");
        return -1;
    }
    wait(pid);*/
    //return 0;
}
