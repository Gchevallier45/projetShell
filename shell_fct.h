#include "cmd.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
//Your imports come here

//Terminate shell
#define MYSHELL_FCT_EXIT 1

//Execute a command
int exec_command(cmd *c);
