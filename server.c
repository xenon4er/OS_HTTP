#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

/*
struct in_addr
{
    in_addr_t s_addr;
};

struct sockaddr_in
{
   short int          sin_family;
   unsigned short int sin_port;
   struct in_addr     sin_addr;
   //unsigned long s_addr;
};
*/
struct TaskManager
{
    pthread_t *threads;
    int sock_id;

    int taskcounter;

    //struct sockaddr_in addr;

    pthread_mutex_t taskqueuemutex;
    pthread_mutex_t waitmutex;
    pthread_mutex_t finishmutex;

    pthread_cond_t taskcomplete;
    pthread_cond_t finishwork;



    int num_of_threads;
    int finish;
};

void *threadwork(void *data)
{

    //printf("threadwork\n");

    char buf[1024];
    char filename[12];
    FILE *file;

    struct TaskManager *manager = (struct TaskManager *)data;
    printf("sock_id = %i\n",manager->sock_id);
    //printf("before while\n");

    while(1)
    {
        //printf("1\n");

        int sock = accept(manager->sock_id, NULL, NULL);
        if(sock < 0)
        {
            perror("accept");
            exit(1);
        }
         printf("%i\n",sock);

            char c;
            int i = 5;
            memset(buf,0,1024);
            memset(filename,0,12);
            printf("memset filename\n");

            recv(sock, buf, 1024,0);

            int j =0;
            while(buf[i]!=' ')
            {
                filename[j] = buf[i];
                j++;
                i++;
            }

            printf("%s\n",buf);
            //memset(buf,'d',15);
            //strcpy(filename,buf);
            memset(buf,0,1024);

            printf("\n%s\n",filename);
            //printf("%lu\n",sizeof(buf));
            //printf("%lu\n",sizeof(filename));


            printf("before while\n");

            memcpy(buf,"HTTP/1.1 200 OK \nContent-Type: text/html; charset=utf-8 \n Connection: close\n\n",1024);
            send(sock, buf, sizeof(buf),0);
            //memset(buf,0,1024);
            //send(sock,buf,1024,0);
            i=0;
            if((file = fopen(filename, "r")) == NULL)
            {
                printf("oops!\n",fflush);
                memset(buf,0,1024);
                memcpy(buf, "file not found\n",sizeof(buf));
                send(sock, buf, sizeof(buf),0);
                close(sock);

                //exit(2);
            }
            else
            {
                memset(buf,0,1024);
                while((c = fgetc(file)) != EOF)
                {
                    buf[i] = c;
                    i++;

                    if((i == 1023))
                    {

                        printf("send\n");
                        send(sock, buf, sizeof(buf), 0);
                        //sleep(10);
                        //printf("%i\n",i);
                        i=0;
                        memset(buf,0,sizeof(buf));

                    }


                }
                if(i!=0)
                {
                    send(sock, buf, sizeof(buf), 0);
                }
                fclose(file);


                close(sock);
                printf("sock close\n");
            }
            //free(filename);
    }

}


void InitManager(struct TaskManager *manager)
{


    manager->sock_id = socket(AF_INET, SOCK_STREAM, 0);
    if(manager->sock_id < 0)
    {
        perror("socket");
        exit(1);
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(3420);
    addr.sin_addr.s_addr = (((((3 << 8) | 0) << 8) | 0) << 8) | 127; //htonl(INADDR_ANY);//

    if(bind( manager->sock_id, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        exit(2);
    }


    listen(manager->sock_id, manager->num_of_threads);

    int t;
    //printf("%i\n",manager->num_of_threads);
    for (t = 0; t < manager->num_of_threads; t++)
    {
        printf("%i\n",t);
        manager->threads = (pthread_t *)calloc(t+1,sizeof(pthread_t));
        pthread_create(&manager->threads[t], NULL, threadwork, (void *) manager);

    }

    printf("init complited\n");

}




int main(int argc, char** argv)
{
   struct TaskManager manager, *man;
   printf("sd\n");

   if(argc < 2)
   {
       printf("usage:  ./server num_of_threads\n");
       return;
   }

   manager.num_of_threads = atoi(argv[1]);
   printf("sd2\n");

   man = &manager;
   InitManager(man);
   int t=0;

   for(t = 0; t<manager.num_of_threads;t++)
   {
       pthread_join(manager.threads[t], NULL);
   }



}
