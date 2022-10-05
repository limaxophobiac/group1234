#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

int main(){
    int pfd[2];
    if (pipe(pfd) == -1)                            //Create pipe
        exit(EXIT_FAILURE);                         //exit if it fails

    if (fork() == 0) {                              //create fork, if statement true in child, false in parent

        if (close(pfd[0]) == -1)                    //close pipe read end
            exit(EXIT_FAILURE);                     //exit if it fails

        if (dup2(pfd[1], STDOUT_FILENO) == -1)      //bind stdout to write end
            exit(EXIT_FAILURE);
        if (close(pfd[1]) == -1)
            exit(EXIT_FAILURE);

        execlp("ls", "ls", "/", (char *) NULL);     //execute ls with "/" argument
        exit(EXIT_FAILURE);                         //child should never get here

    }
    /*Parent continues here*/
    wait(NULL);
    if (close(pfd[1]) == -1)                        //close pipe write end
            exit(EXIT_FAILURE);                     //exit if it fails
        
                       
    if (dup2(pfd[0], STDIN_FILENO) == -1)           //bind stdin to read end
        exit(EXIT_FAILURE);
    if (close(pfd[0]) == -1)
        exit(EXIT_FAILURE);

    execlp("wc", "wc", "-l", (char *) NULL);        //execute wc with "-l" argument
    exit(EXIT_FAILURE);                             //parent should never get here
}