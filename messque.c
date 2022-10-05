#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){

    struct mq_attr attr;

    char *queueName = "/testQueue";
    int flags = O_CREAT | O_RDWR;
    mode_t mode = S_IRUSR | S_IWUSR;
    errno = 0;
    mqd_t messageQueue = mq_open(queueName, flags, mode, NULL);
    mq_getattr(messageQueue, &attr);


    if (fork() == 0){                                                 //child
        void *buffer = malloc(9000);
        size_t msg_len = 9000;
        int wordCount = 0;
        char old = ' ', *cur = &old;

        while (mq_receive(messageQueue, buffer, msg_len, NULL) != -1){                                             //loop recieves from messageQueue
            cur = (char *) buffer;
            while (*cur != '\0' && *cur != EOF){
                if ((old == '\n' || old == '\r' || old == ' ' || old == '\t') && (*cur != '\n' && *cur != '\r' && *cur != ' ' && *cur != '\t'))
                    wordCount++;
                old = *cur;
                cur++;
            }

        }
        printf("wordcount: %d \n", wordCount);

        mq_unlink(queueName);
        exit(EXIT_SUCCESS);
    }

    /*parent*/

    FILE *fp;

    if ((fp = fopen(argv[1], "r")) == NULL){
        printf("could not open file %s", argv[1]);
        exit(EXIT_FAILURE);
    }

    int ch;
    size_t msg_len = attr.mq_msgsize;
    char chArray[attr.mq_msgsize];
    int index = 0;

    while ((ch = getc(fp)) != EOF){                             //loop sends to messageQueue
        chArray[index++] = ch;
        if (index == attr.mq_msgsize -2){
            chArray[attr.mq_msgsize-1] = '\0';
            mq_send(messageQueue, chArray, index +1, 1);
            index = 0;
        }
    }
    chArray[index] = '\0';
  
    int p = mq_send(messageQueue, chArray, index+1, 1);
    attr.mq_flags |= O_NONBLOCK;
    mq_setattr(messageQueue, &attr, NULL);
    

    fclose(fp);
    mq_unlink(queueName);
    wait(NULL);
    exit(EXIT_SUCCESS);
}