#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <dirent.h>
#include <ctype.h>

void myPrint(char *msg)
{
    write(STDOUT_FILENO, msg, strlen(msg));
}

char semiColon[]   = ";\t\n";
char newLine[]   = "\n";
char seps[]   = " +<>,\t\n";
char leftright[]   = "<>";
char leftrightplus[]   = "<>+";
char empty[]   = " \n";

char slash[] = "/";

char *newl = "\n";

char error_message[30] = "An error has occurred\n";\



int getType(char *cmd) {
    //gets the type of operation
    //returns 0 for none
    //returns 1 for >
    //returns 2 for >+
    if (strstr(cmd,">+")) {
        return 2;
    }
    if (strchr(cmd,'>') != NULL) {
        return 1;
    }
    if (strchr(cmd,'<') != NULL) {
        return 5;
    }

    return 0;
}

char **parseLine(char *str, char *delim) {
    char **ret = (char**)malloc(sizeof(char*) * 100);
    char *token = strtok(str, delim);
    int count = 0;
    while( token != NULL )
    {
        ret[count] = token;
        count++;
        token = strtok(NULL, delim);
    }

    return ret;

}

int validPath(char* path)
{
	FILE* file = fopen(path, "r");
  	if(file)
    {
  		fclose(file);
      	return 1;
    }
  	return 0;
}

void makeCopy (char *file) {
    //printf("making copy...\n");
    printf("making copy\n");

    //int fd1 = open(file, O_RDONLY);

    FILE *old = fopen(file,"r");
    if (old == NULL) {
        return;
    }

    FILE *temp = fopen("temp.txt","w");

    //printf("entering while\n");

    char ch;
    //int count;

    while (fscanf(old,"%c",&ch) > 0) {
        //printf("%d - %c\n",count++,ch);
        fprintf(temp,"%c",ch);
        //dprintf(fd1,"%c",ch);
    }  

    fclose(old);
    fclose(temp);

    //printf("copy made\n");
}

int checkEmpty (char *cmd) {
    int len = strlen(cmd);

    for (int i = 0; i < len; i++) {

        if (isspace(cmd[i]) == 0) {
            return 0;
        } else {
            
        }
    }
    //printf("HEYO!\n");
    return 1;

}

int checkEmpty2 (char *cmd) {
    int len = strlen(cmd);

    for (int i = 0; i < len; i++) {

        if (isspace(cmd[i]) == 0 && cmd[i] != '>' && cmd[i] != '+') {
            return 0;
        } else {
            
        }
    }
    //printf("HEYO!\n");
    return 1;

}

int emptyFile (char *file) {
    FILE *fp = fopen(file, "r");
    char c;
    char *contents = (char*)malloc(sizeof(char) * 9999);
    int s = 0;
    while (fscanf(fp,"%c",&c) != EOF) {
        contents[s] = c;
        s++;
    }

    if (strlen(contents) == 0) {
        return 1;
    }

    return 0;
}

char *copyFile (int fd) {
    char c;
    char *ret = (char*)malloc(sizeof(char) * 9999);
    int x = 0;
    while (read(fd,&c,1) > 0) {
        ret[x] = c;
        //printf("%c\n",c);
        x++;
    }

    return ret;

}

void remove_spaces(char* s) {
    char* d = s;
    do {
        while (*d == ' ' || isspace(*d) == 1) {
            ++d;
        }
    } while ((*s)++ == (*d)++);
}

int multiRedirect (char *cmd) {
    int len = strlen(cmd);
    int count = 0;

    for (int i = 0; i < len; i++) {
        if (cmd[i] == '>') {
            count++;
        }
    }
    //printf("count - %d\n",count);
    return count;
}

int checkOverflow (char *line) {
    if (strlen(line) > 514) {
        //printf("OVERFLOW!\n");
        return 1;
    }
    return 0;

}

int checkDirExistance(char *str) {
    char **ret = (char**)malloc(sizeof(char*) * 100);
    char *token = strtok(str, slash);
    int count = 0;
    while( token != NULL )
    {
        ret[count] = token;
        count++;
        token = strtok(NULL, slash);
    }

    for (int i = 0; i < count - 1; i++) {
        //printf("---- testing %s ----\n",ret[i]);
        DIR* dir = opendir(ret[i]);
        if (dir) {
            closedir(dir);
        } else {
           return 0;
        }
    }
    return 1;
}

int checkArg(char *str) {
    char *dup = strdup(str);
    remove_spaces(dup);

    if (strcmp(dup,">") == 0) {
        return 0;
    }

    if (strcmp(dup,">+") == 0) {
        return 0;
    }

    return 1;
}


//MAIN FUNCTION FOR BATCH MODE
void executeCommand (char **cmds, int numCommands) {
    
    int numArgs = 0;
    //char **cmdsDup = cmds;
    
    for(int i = 0; i < numCommands; i++) {
        char **args = (char**)malloc(sizeof(char*) * 512);
        char **leftArgs = (char**)malloc(sizeof(char*) * 512);
        char **rightArgs = (char**)malloc(sizeof(char*) * 512);
        char *currCommand = cmds[i];
        
        //memset(args, 0, numArgs);
        //memset(leftArgs, 0, LENGTH);
        
        //printf("==== NOW RUNNING [%s] ====\n",cmds[i]);
        if (checkEmpty(cmds[i]) == 1) {
            //printf("empty - %d\n",checkEmpty(cmds[i]));
            continue;
        }

        if (multiRedirect(currCommand) > 1) {
            //printf("SAOPRG\n");
            write(STDOUT_FILENO, error_message, strlen(error_message));
            continue;
        }

        //remove_spaces(currCommand);
        //printf("curr command: [%s]\n",currCommand);

        
/*
        if (
            (strcmp(currCommand,">") == 0) ||
            (strcmp(currCommand,"> ") == 0) ||
            (strcmp(currCommand,"	>") == 0) ||
            (strcmp(currCommand," > ") == 0)) {
            write(STDOUT_FILENO, error_message, strlen(error_message));
            continue;
        }

        if (
            (strcmp(currCommand,">+") == 0) ||
            (strcmp(currCommand,">+ ") == 0) ||
            (strcmp(currCommand,"	>+") == 0) ||
            (strcmp(currCommand," >+ ") == 0)) {
            write(STDOUT_FILENO, error_message, strlen(error_message));
            continue;
        }
        */

        if (checkEmpty2(currCommand) == 1) {
            write(STDOUT_FILENO, error_message, strlen(error_message));
            continue;
        }
        
        int type = getType(cmds[i]);
        /*
        if (type != 0) {
            if (checkArg(currCommand) == 0) {
                write(STDOUT_FILENO, error_message, strlen(error_message));
                continue; 
            }
        }*/




        
        int curr = 0;
        int suc = 0;
        
        if (type == 0) {
            char *token2 = strtok(cmds[i], seps);
            int y = 0;
            while( token2 != NULL ) {
                if (checkEmpty(token2) == 1) {
                    continue;
                }
                args[y] = token2;
                //printf("arg: %s\n",token2);
                y++;
                token2 = strtok(NULL, seps);
            }
            numArgs = y;
            
        } if (type != 0) {
            char *token3 = strtok(cmds[i], leftright);
            int y = 0;
            while( token3 != NULL ) {
                if (checkEmpty(token3) == 1) {
                    continue;
                }
                args[y] = token3;
                //printf("arg: [%s]\n",token3);
                y++;
                token3 = strtok(NULL, leftright);
            }

            if (y > 2) {
                write(STDOUT_FILENO, error_message, strlen(error_message));
                exit(0);
            }
        
            char *token4 = strtok(args[0], seps);
            y = 0;
            while( token4 != NULL ) {
                if (checkEmpty(token4) == 1) {
                    continue;
                }
                leftArgs[y] = token4;
                //printf("leftarg: %s\n",token4);
                y++;
                token4 = strtok(NULL, seps);
            }

            char *token5 = strtok(args[1], seps);
            y = 0;
            while( token5 != NULL ) {
                if (checkEmpty(token5) == 1) {
                    continue;
                }
                rightArgs[y] = token5;
                //printf("rightarg: %s\n",token5);
                y++;
                token5 = strtok(NULL, seps);
            }
        }

        //printf("here!!!!!\n");

        

        //printf("left args: %s - %d\n",leftArgs[0], strcmp(leftArgs[0],"cd"));

        // ==== checking for errors ====
        if ((type != 0 && strcmp(leftArgs[curr],"pwd") == 0 && type != 0) ||
            (strcmp(args[curr],"pwd") == 0 && type != 0) ||
            (strcmp(args[curr],"pwd") == 0 && numArgs > 1)) {
            write(STDOUT_FILENO, error_message, strlen(error_message));
            continue;
        }
        if ((type != 0 && strcmp(leftArgs[curr],"cd") == 0 && type != 0) ||
            (strcmp(args[curr],"cd") == 0 && type != 0) ||
            (strcmp(args[curr],"cd") == 0 && numArgs > 2) ) {
            write(STDOUT_FILENO, error_message, strlen(error_message));
            continue;
        }

        if ((type != 0 && strcmp(leftArgs[0],"exit") == 0) ||
            (numArgs > 1 && strcmp(args[0],"exit") == 0)
            
            
            ) {
                //printf("here!!!!!\n");
            write(STDOUT_FILENO, error_message, strlen(error_message));
            continue; 
        }


        //printf("got here\n");
        


        //==== built in commands ====

        if (type == 0) {
            

            if (strcmp(args[curr],"exit") == 0) {
                exit(0);
            }

            if (strcmp(args[curr],"cd") == 0) {
                
                
                if (numArgs > 1) {      
                        for (int j = 0; j < numArgs - 1; j++) {
                            //printf("current arg: %s\n",args[curr + 1]);
                            char path[100];
                            //chdir(args[curr + 1]);
                            if (chdir(args[curr + 1]) != 0) {
                                write(STDOUT_FILENO, error_message, strlen(error_message));
                                continue;
                            }
                            getcwd(path, 100);
                            curr++;
                    }
                } else {
                    chdir(getenv("HOME"));
                }

                suc = 1;
                
            }


            if (strcmp(args[curr],"pwd") == 0) {
                //printf("arg num: %d\n",numArgs);
                int pid = fork();
                if (pid < 0) write(STDOUT_FILENO, error_message, strlen(error_message));
                if (pid == 0) {
                    execlp(args[curr], "", NULL);
                    write(STDOUT_FILENO, error_message, strlen(error_message));
                    exit(0);
                } else {
                    waitpid(pid, 0, 0);
                    suc = 1;
                }
            }

            if (strcmp(args[curr],"ls") == 0) {
                int pid = fork();
                if (pid < 0) write(STDOUT_FILENO, error_message, strlen(error_message));
                if (pid == 0) {
                    execvp(args[0],args);
                    write(STDOUT_FILENO, error_message, strlen(error_message));
                    exit(0);
                } else {
                    waitpid(pid, 0, 0);
                    suc = 1;
                }
            }
        } //end of built in commands

            //==== redirection ====


            if (type == 2) {

 

                if (
                    (rightArgs[0] == NULL) ||
                    (checkDirExistance(rightArgs[0]) == 0)
                ) {
                    write(STDOUT_FILENO, error_message, strlen(error_message));
                    continue;
                }

                int pid = fork();
                if (pid < 0) write(STDOUT_FILENO, error_message, strlen(error_message));
                if (pid == 0) {
                    if (access(rightArgs[0], F_OK) != 0) {
                    int fd1 = creat(rightArgs[0], 0644);
                    dup2(fd1, STDOUT_FILENO);
                    close(fd1);
                } else {

                    FILE *tmp = tmpfile();
                    
                    dup2(fileno(tmp),STDOUT_FILENO);   

                    int f = f=open(rightArgs[0], O_RDONLY, 0777);

                    char ch;

                    fseek(tmp, 0, SEEK_END);
                    
                    while(read(f, &ch, 1) > 0) {
                        dprintf(fileno(tmp),"%c",ch);
                    }

                    close(f);

                    fseek(tmp, 0, SEEK_SET);

                    f =open(rightArgs[0], O_WRONLY, 0777);




                }
                    execvp(leftArgs[0], leftArgs);
                    write(STDOUT_FILENO, error_message, strlen(error_message));
                    exit(0);
                } else {
                    waitpid(pid, 0, 0);
                    suc = 1;
                }




            }


            
            if (type == 1) {

                if (
                    ((access(rightArgs[0], F_OK) == 0) && type == 1) ||
                    (rightArgs[0] == NULL) ||
                    (checkDirExistance(rightArgs[0]) == 0)
                ) {
                    write(STDOUT_FILENO, error_message, strlen(error_message));
                    continue;
                }



                int pid = fork();
                if (pid < 0) write(STDOUT_FILENO, error_message, strlen(error_message));
                if (pid == 0) {
                    if (type == 1) { //>
                        int fd1 = creat(rightArgs[0], 0644);
                        dup2(fd1, STDOUT_FILENO);
                        close(fd1);
                    }
                    if (type == 2) { //>+

                        if (access(rightArgs[0], F_OK) != 0) {
                            int fd1 = creat(rightArgs[0], 0644);
                            dup2(fd1, STDOUT_FILENO);
                            close(fd1);
                        } else {

                            char *text = (char*)malloc(sizeof(char) * 999);
                            int buff = 0;
                            char ch;
    
                            int fd = open(rightArgs[0], 0666);
                            int temp = creat("temp.txt", 0666);
                        
                            while(read(fd,&ch,1) > 0) {
                                text[buff] = ch;
                                buff++;
                            }

                            dprintf(temp,"%s",text);
                            close(temp);
                            
                            int fd1 = creat("temp2.txt", 0666);
                            dup2(fd1,STDOUT_FILENO);
                            close(fd1);

                            int temp2 = open("temp2.txt", 0666);
                            
                            char *text2 = (char*)malloc(sizeof(char) * 999);//
                            buff = 0;
                            char cha;
                            while(read(temp2,&cha,1) > 0) {
                                
                                text2[buff] = ch;
                                buff++;

                            }
                            
                            close(fd1);

                            remove(rightArgs[0]);
                            fd = creat(rightArgs[0], 0666);

                            dup2(fd,1);
                            dprintf(1,"%s",text);

                            dup2(fd,2);
                            dprintf(2,"%s",text2);

                            free(text);
                            free(text2);


                            
                            
                            //dprintf(fd,"%s%s",text2,text);
                    
                            //remove("temp.txt");
                            //remove("temp2.txt");

                            close(fd);

                            
                        }


                    }
                    execvp(leftArgs[0], leftArgs);
                    
                    write(STDOUT_FILENO, error_message, strlen(error_message));
                    exit(0);
                } else {
                    waitpid(pid, 0, 0);
                    suc = 1;
                }
            }
            

            if (suc == 0 && type == 0) {
                int pid = fork();
                if (pid < 0) write(STDOUT_FILENO, error_message, strlen(error_message));
                if (pid == 0) {
                    execvp(args[0],args);
                    write(STDOUT_FILENO, error_message, strlen(error_message));
                    exit(0);
                } else {
                    waitpid(pid, 0, 0);
                    suc = 1;
                }
                
            }
            

            //printf("%s\n",cmds[i]);


        } //for loop end
}




int main(int argc, char *argv[]) 
{

    char *token;

    char cmd_buff[100];
    char *pinput;

    char **cmds = (char**)malloc(sizeof(char) * 512);
    if (cmds == NULL) {
        write(STDOUT_FILENO, error_message, strlen(error_message));
        exit(0);
    }
    
    int batchMode = 0;
    //char *cd;
    
    if (argc == 2) {
    	batchMode = 1;
    } else if (argc > 2) {
        write(STDOUT_FILENO, error_message, strlen(error_message));
        exit(0);
    }
    
    while (1) {
        int x = 0;
        int numArgs = 0;
        //==== setup ====
        

        if (batchMode == 0) {
            // ==========================================================================================
            // ===========================================================================================
            // ===========================================================================================
            // ===========================================================================================
            // ===========================================================================================
            myPrint("myshell> ");
            pinput = fgets(cmd_buff, 100, stdin);
            token = strtok( pinput, semiColon );
            while( token != NULL )
            {
                cmds[x] = token;
                printf("new command: %s\n",cmds[x]);
                //printf("%s\n",token);
                x++;
                token = strtok( NULL, semiColon );
            }

        for(int i = 0; i <= x - 1; i++) {

            int suc = 0;

            int curr = 0;

            int type = getType(cmds[i]);

            printf("type: %d\n",type);

            printf("==== executing command %s ====\n",cmds[i]);


            //initializing the arguments

            char **args = (char**)malloc(sizeof(char*) * 100);
            char **leftArgs = (char**)malloc(sizeof(char*) * 100);
            char **rightArgs = (char**)malloc(sizeof(char*) * 100);

            if (type == 0) {
                char *token2 = strtok(cmds[i], seps);
                int y = 0;
                while( token2 != NULL ) {
                    args[y] = token2;
                    //printf("arg: %s\n",token2);
                    y++;
                    token2 = strtok(NULL, seps);
                }
                numArgs = y;
                
            } if (type != 0) {
                char *token3 = strtok(cmds[i], leftright);
                int y = 0;
                while( token3 != NULL ) {
                    args[y] = token3;
                    //printf("arg: %s\n",token3);
                    y++;
                    token3 = strtok(NULL, leftright);
                }

                if (y > 2) {
                    write(STDOUT_FILENO, error_message, strlen(error_message));
                    exit(0);
                }
            
                char *token4 = strtok(args[0], seps);
                y = 0;
                while( token4 != NULL ) {
                    leftArgs[y] = token4;
                    printf("leftarg: %s\n",token4);
                    y++;
                    token4 = strtok(NULL, seps);
                }

                char *token5 = strtok(args[1], seps);
                y = 0;
                while( token5 != NULL ) {
                    rightArgs[y] = token5;
                    printf("rightarg: %s\n",token5);
                    y++;
                    token5 = strtok(NULL, seps);
                }
            }

            // ==== checking for errors ====
            if (strcmp(args[curr],"pwd") == 0 && type != 0) {
                write(STDOUT_FILENO, error_message, strlen(error_message));
                break;
            }

            //==== built in commands ====

            if (type == 0) {
                
                if (!pinput) {
                    exit(0);
                }

                if (strcmp(args[curr],"exit") == 0) {
                    if (numArgs > 1) {
                        write(STDOUT_FILENO, error_message, strlen(error_message));
                        break;
                    } else {
                        exit(0);
                    }
                    
                }

                if (strcmp(args[curr],"cd") == 0) {

                    
                    if (numArgs > 1) {      
                            for (int j = 0; j < numArgs - 1; j++) {
                                //printf("current arg: %s\n",args[curr + 1]);
                                char path[100];
                                //chdir(args[curr + 1]);
                                if (chdir(args[curr + 1]) != 0) {
                                    write(STDOUT_FILENO, error_message, strlen(error_message));
                                    break;
                                }
                                getcwd(path, 100);
                                curr++;
                        }
                    } else {
                        chdir(getenv("HOME"));
                    }

                    suc = 1;
                    
                }


                if (strcmp(args[curr],"pwd") == 0) {
                    if (numArgs > 1) {
                        write(STDOUT_FILENO, error_message, strlen(error_message));
                        break;
                    }
                    int pid = fork();
                    if (pid < 0) write(STDOUT_FILENO, error_message, strlen(error_message));
                    if (pid == 0) {

                        execvp(args[0],args);
                        write(STDOUT_FILENO, error_message, strlen(error_message));
                        exit(0);
                    } else {
                        waitpid(pid, 0, 0);
                        suc = 1;
                    }
                }

                int ex = 1;

                if (strcmp(args[curr],"ls") == 0) {
                    printf("LS COMMAND\n");
                    //printf("val: %d\n",execvp(args[0], args));
                    
                    int pid = fork();
                    if (pid < 0) write(STDOUT_FILENO, error_message, strlen(error_message));
                    if (pid == 0) {
                        ex = execvp(args[0],args);
                        printf("testing...\n");
                        if (ex < 0) {
                            printf("HEY!\n");
                        }
                        write(STDOUT_FILENO, error_message, strlen(error_message));
                        exit(0);
                    } else {
                        waitpid(pid, 0, 0);
                        suc = 1;
                    }
                    
                }
                
 
            } //end of built in commands

                //==== redirection ====

                if (type == 2) {

                    printf("type 2\n");

                    FILE *tmp = tmpfile();
                    int tmpInt = fileno(tmp);
                    printf("duped\n");
                    dup2(tmpInt,STDOUT_FILENO);
                    printf("duped\n");

                    FILE*f = fopen(rightArgs[0], "rw");
                    char ch;
                    
                    printf("first loop\n");
                    while(fgetc(f) != EOF) {
                        fputc(ch,tmp);
                        ch = fgetc(f);
                    }

                    fseek(f, 0, SEEK_SET);
                    fseek(tmp, 0, SEEK_SET);

                    printf("second loop\n");
                    while(fgetc(tmp) != EOF) {
                        fputc(ch,f);
                        ch = fgetc(tmp);
                    }

                int pid = fork();
                if (pid < 0) write(STDOUT_FILENO, error_message, strlen(error_message));
                if (pid == 0) {
                    execvp(leftArgs[0], leftArgs);
                    write(STDOUT_FILENO, error_message, strlen(error_message));
                    exit(0);
                } else {
                    waitpid(pid, 0, 0);
                    suc = 1;
                }
            }


                
                if (type == 1) {
                    int pid = fork();
                    if (pid < 0) write(STDOUT_FILENO, error_message, strlen(error_message));
                    if (pid == 0) {
                        if (type == 1) { //>
                            int fd1 = creat(rightArgs[0], 0644);
                            dup2(fd1, STDOUT_FILENO);
                            close(fd1);
                        }
                        if (type == 2) { //>+
                            char *f = rightArgs[0];

                            makeCopy(f);
                            
                            int fd1 = creat(rightArgs[0], 0777);
                            

                            FILE *temp = fopen("temp.txt","r");

                            char ch;

                            dup2(fd1, STDOUT_FILENO);

                            lseek(fd1,0, SEEK_END);
                            
                            
                            while (fscanf(temp,"%c",&ch) != EOF) {
                                dprintf(fd1,"%c",ch);
                            }
                            
                            
                            

                            //remove("temp.txt");
                            fclose(temp);
                            close(fd1);
                            

                        }
                        execvp(leftArgs[0], leftArgs);
                        write(STDOUT_FILENO, error_message, strlen(error_message));
                        exit(0);
                    } else {
                        waitpid(pid, 0, 0);
                        suc = 1;
                    }
                }

                if (suc == 0) {
                    printf("LAST RESORT\n");
                    int pid = fork();
                    if (pid < 0) write(STDOUT_FILENO, error_message, strlen(error_message));
                    if (pid == 0) {
                        execvp(args[0],args);
                        write(STDOUT_FILENO, error_message, strlen(error_message));
                        exit(0);
                    } else {
                        waitpid(pid, 0, 0);
                        suc = 1;
                    }
                }

                //printf("%s\n",cmds[i]);


            } //for loop end



        } else {
            // ===========================================================================================
            // ===========================================================================================
            // ===========================================================================================
            // ===========================================================================================
            // ===========================================================================================
            //printf("batch mode\n");
            FILE *batch = fopen(argv[1],"r");
            if (batch == NULL) {
                write(STDOUT_FILENO, error_message, strlen(error_message));
                exit(0);
            }
            if( feof(batch) ) { 
                exit(0) ;
            }
            char c;
            int m = 0;
            char *command = (char*)malloc(sizeof(char) * 9999);
            if (command == NULL) {
                write(STDOUT_FILENO, error_message, strlen(error_message));
                break;
            }
            while (fscanf(batch,"%c",&c) != EOF) {
                command[m] = c;
                m++;
            }
            if (strlen(command) == 0 || command[0] == '\n') {
                exit(0);
            }
            if (m == 0) {
                exit(0);
            }
            pinput = command;//
            char **line = (char**)malloc(sizeof(char*) * 512);

            token = strtok( pinput, newLine );
            while( token != NULL )
            {
                line[x] = token;
                x++;
                token = strtok( NULL, newLine );
            }
            
            for (int i = 0; i < x; i++) {
                char *currLine = line[i];

                //printf("LINE: %s\n\n\n%d\n\n\n",currLine,checkOverflow(line[i]));

                if (checkEmpty(line[i]) == 0) {
                    //write(STDOUT_FILENO,"writing line - ", strlen("writing line - "));
                    write(STDOUT_FILENO,currLine,strlen(currLine));
                    write(STDOUT_FILENO,newl,strlen(newl));
                }

                if (checkOverflow(currLine) == 1) {
                    write(STDOUT_FILENO, error_message, strlen(error_message));
                    continue;
                }


                token = strtok( line[i], semiColon ); //hi
                int numCommands = 0;
                int count = 0;

                while( token != NULL )
                {
                    cmds[count] = token;
                    //printf("new command: %s\n",cmds[count]);
                    //printf("%s\n",token);
                    numCommands++;
                    count++;
                    token = strtok( NULL, semiColon );
                }


                //write(STDOUT_FILENO,newl,strlen(newl));
                executeCommand (cmds, numCommands);
            }

            batchMode = 0;
        }
    }
}
