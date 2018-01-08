#include "cmd.h"

//Prints the command
void printCmd(cmd *cmd){
	printf("%s",cmd->initCmd);
}

//Initializes the initial_cmd, membres_cmd et nb_membres fields
void parseMembers(char *inputString,cmd *cmd){
    if(inputString != NULL){
        //Copy raw string
        size_t inputStringSize = strlen(inputString);
        cmd->initCmd = malloc((inputStringSize+1)*sizeof(char));
        strcpy(cmd->initCmd,inputString);

        //Split string into members
        printf("\nSPLIT STRING \n");
        cmd->nbCmdMembers = 0;
        cmd->cmdMembers = malloc(cmd->nbCmdMembers*sizeof(char*));
        char *substr;
        substr = strtok (inputString,"|");
        while (substr != NULL)
        {
            cmd->nbCmdMembers++;
            cmd->cmdMembers = realloc(cmd->cmdMembers,cmd->nbCmdMembers*sizeof(char*));
            char *substrInit = substr;
            while(*substr == ' ') //Delete spaces
                substr++;
            cmd->cmdMembers[cmd->nbCmdMembers-1] = malloc((strlen(substr)+1)*sizeof(char));
            strcpy(cmd->cmdMembers[cmd->nbCmdMembers-1],substr);
            printf ("%s\n",cmd->cmdMembers[cmd->nbCmdMembers-1]);
            substr = substrInit;
            substr = strtok (NULL, "|");
        }

        //Split members into args
        printf("\nSPLIT MEMBERS \n");
        cmd->nbMembersArgs = malloc(cmd->nbCmdMembers*sizeof(unsigned int));
        cmd->cmdMembersArgs = malloc(cmd->nbCmdMembers*sizeof(char**));
        for(int i=0;i<cmd->nbCmdMembers;i++){
            cmd->nbMembersArgs[i] = 0;
            cmd->cmdMembersArgs[i] = malloc((cmd->nbMembersArgs[i]+1)*sizeof(char*)); //+1 to have NULL at the end of array

            substr = strtok (cmd->cmdMembers[i]," ");
            while (substr != NULL && *substr != '<' && *substr != '>')
            {
                cmd->nbMembersArgs[i]++;
                cmd->cmdMembersArgs[i] = realloc(cmd->cmdMembersArgs[i],(cmd->nbMembersArgs[i]+1)*sizeof(char*));
                char *substrInit = substr;
                while(*substr == ' ') //Delete spaces
                    substr++;
                cmd->cmdMembersArgs[i][cmd->nbMembersArgs[i]-1] = malloc((strlen(substr)+1)*sizeof(char));
                strcpy(cmd->cmdMembersArgs[i][cmd->nbMembersArgs[i]-1],substr);
                printf ("[%d][%d] = %s\n",i,cmd->nbMembersArgs[i]-1,cmd->cmdMembersArgs[i][cmd->nbMembersArgs[i]-1]);
                substr = substrInit;
                substr = strtok (NULL, " ");
            }
        }

        //Get redirections
        for(int i=0;i<cmd->nbCmdMembers;i++){
            cmd->redirection[i] = (3*sizeof(char*));
            char *member = cmd->cmdMembers[i];
            while(*member != '\0' && *member!= '<' && *member != '>'){
                *member++;
            }
            switch(*member){
            case '<':
                if(*substr+1 == ' '){
                    substr++;
                    while(*substr == ' ') //Delete spaces
                        substr++;
                }
                size_t substrSize = strlen(substr);
                cmd->redirection[i][0] = malloc(substrSize*sizeof(char));
                strcpy(cmd->redirection[i][0],substr);
                break;
            }
        }

    }
}

//Frees memory associated to a cmd
void freeCmd(cmd  * cmd){

}
