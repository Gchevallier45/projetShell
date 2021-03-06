#include "cmd.h"

//Prints the command
void printCmd(cmd *cmd){
	printf("|||||||||||- PARSER -||||||||||\n");

	printf("INITIAL COMMAND \n");
	printf("%s \n",cmd->initCmd);

    printf("\nSPLIT STRING \n");
	for(int i=0;i<cmd->nbCmdMembers;i++)
        printf ("%s\n",cmd->cmdMembers[i]);

    printf("\nSPLIT MEMBERS \n");
    for(int i=0;i<cmd->nbCmdMembers;i++){
        for(int j=0;j<cmd->nbMembersArgs[i];j++){
            printf ("[%d][%d] = %s\n",i,j,cmd->cmdMembersArgs[i][j]);
        }
    }

    printf("\nGET REDIRECTIONS \n");
    for(int i=0;i<cmd->nbCmdMembers;i++){
        for(int j=0;j<3;j++){
            if(strcmp(cmd->redirection[i][j],"") != 0){
                printf ("[%d][%d] = %s ",i,j,cmd->redirection[i][j]);
                if(j!=0)
                    printf("| type %d",cmd->redirectionType[i][j]);
                printf("\n");
            }
        }
    }

    printf("|||||||||||||||||||||||||||||||\n\n");
}

//Parse the command
void parseMembers(char *inputString,cmd *cmd){
    if(inputString != NULL){
        //Copy raw string
        size_t inputStringSize = strlen(inputString);
        cmd->initCmd = malloc((inputStringSize+1)*sizeof(char));
        strcpy(cmd->initCmd,inputString);

        //Split string into members
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
            substr = substrInit;
            substr = strtok (NULL, "|");
        }

        //Split members into args
        cmd->nbMembersArgs = malloc(cmd->nbCmdMembers*sizeof(unsigned int));
        cmd->cmdMembersArgs = malloc(cmd->nbCmdMembers*sizeof(char**));
        for(int i=0;i<cmd->nbCmdMembers;i++){
            cmd->nbMembersArgs[i] = 0;
            cmd->cmdMembersArgs[i] = malloc((cmd->nbMembersArgs[i]+1)*sizeof(char*)); //+1 to have NULL at the end of array

            char *tmpCmdMember = malloc((strlen(cmd->cmdMembers[i])+1)*sizeof(char));
            strcpy(tmpCmdMember,cmd->cmdMembers[i]); //Copy for strtok
            substr = strtok (tmpCmdMember," ");

            while (substr != NULL && *substr != '<' && *substr != '>' && !(*substr == '2' && *(substr+1) == '>'))
            {
                cmd->nbMembersArgs[i]++;
                cmd->cmdMembersArgs[i] = realloc(cmd->cmdMembersArgs[i],(cmd->nbMembersArgs[i]+1)*sizeof(char*));
                char *substrInit = substr;
                while(*substr == ' ') //Delete spaces
                    substr++;
                cmd->cmdMembersArgs[i][cmd->nbMembersArgs[i]-1] = malloc((strlen(substr)+1)*sizeof(char));
                strcpy(cmd->cmdMembersArgs[i][cmd->nbMembersArgs[i]-1],substr);
                substr = substrInit;
                substr = strtok (NULL, " ");
            }

            cmd->cmdMembersArgs[i][cmd->nbMembersArgs[i]]=NULL;
            free(tmpCmdMember);
        }

        //Get redirections
        cmd->redirection = malloc(cmd->nbCmdMembers*sizeof(char**));
        cmd->redirectionType = malloc(cmd->nbCmdMembers*sizeof(int*));
        for(int i=0;i<cmd->nbCmdMembers;i++){
            cmd->redirection[i] = malloc(3*sizeof(char*));
            cmd->redirectionType[i] = malloc(3*sizeof(int));
            cmd->redirection[i][0] = strdup("");
            cmd->redirection[i][1] = strdup("");
            cmd->redirection[i][2] = strdup("");
            char *member = cmd->cmdMembers[i];

            while(*member != '\0' && *member!= '<' && *member != '>'){
                member++;
            }

            switch(*member){
            case '<': //Redirect STDIN
                member++;
                while(*member == ' ') //Delete spaces before text
                    member++;
                size_t substrSize = strlen(member);
                char *endmember = member + substrSize -1;
                while(*endmember == ' '){ //Delete spaces after text
                    endmember--;
                }

                cmd->redirection[i][0] = realloc(cmd->redirection[i][0],(endmember-member+2)*sizeof(char));
                strncpy(cmd->redirection[i][0],member,endmember-member+1);
                cmd->redirection[i][0][endmember-member+1] = '\0';
                break;
            case '>': //Redirect STDOUT OR STDERR
                member++;
                if(*(member-2) == '2'){ //Redirect STDERR (2> or 2>>)
                    if(*member == '>'){ // Append
                        member+=2;
                        cmd->redirectionType[i][2] = APPEND;
                    }
                    else{ // Overwrite
                        member++;
                        cmd->redirectionType[i][2] = OVERRIDE;
                    }

                    while(*member == ' ') //Delete spaces before text
                        member++;
                    size_t substrSize = strlen(member);
                    char *endmember = member + substrSize -1;
                    while(*endmember == ' '){ //Delete spaces after text
                        endmember--;
                    }

                    cmd->redirection[i][2] = realloc(cmd->redirection[i][2],(endmember-member+2)*sizeof(char));
                    strncpy(cmd->redirection[i][2],member,endmember-member+1);
                    cmd->redirection[i][2][endmember-member+1] = '\0';
                }
                else{ //Redirect STDOUT (> or >>)
                    if(*member == '>'){ // Append
                        member+=2;
                        cmd->redirectionType[i][1] = APPEND;
                    }
                    else{ // Overwrite
                        member++;
                        cmd->redirectionType[i][1] = OVERRIDE;
                    }

                    while(*member == ' ') //Delete spaces before text
                        member++;
                    size_t substrSize = strlen(member);
                    char *endmember = member + substrSize -1;
                    while(*endmember == ' '){ //Delete spaces after text
                        endmember--;
                    }

                    cmd->redirection[i][1] = realloc(cmd->redirection[i][1],(endmember-member+2)*sizeof(char));
                    strncpy(cmd->redirection[i][1],member,endmember-member+1);
                    cmd->redirection[i][1][endmember-member+1] = '\0';
                }
                break;
            }
        }
    }
}

//Frees memory associated to a cmd
void freeCmd(cmd  * cmd){
    free(cmd->initCmd);

    for(int i=0;i<cmd->nbCmdMembers;i++){

        //free memberArgs
        for(int j=0;j<cmd->nbMembersArgs[i];j++)
            free(cmd->cmdMembersArgs[i][j]);

        //free redirection
        for(int j=0;j<3;j++){
                free(cmd->redirection[i][j]);
        }

        if(cmd->redirectionType[i]!=NULL)
            free(cmd->redirectionType[i]);

        free(cmd->cmdMembers[i]);
        free(cmd->cmdMembersArgs[i]);
        free(cmd->redirection[i]);
    }

    free(cmd->cmdMembers);
    free(cmd->cmdMembersArgs);
    free(cmd->redirection);
    free(cmd->nbMembersArgs);
    free(cmd->redirectionType);
}
