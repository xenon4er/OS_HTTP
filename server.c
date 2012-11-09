#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>


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

    printf("threadwork\n");

    char *buf;
    char *filename = "readme.txt";
    FILE *file;

    struct TaskManager *manager = (struct TaskManager *)data;
    printf("sock_id = %i\n",manager->sock_id);
    printf("before while\n");

    //while(1)
    {
        printf("1");

        int sock = accept(manager->sock_id, NULL, NULL);
            if(sock < 0)
            {
                perror("accept");
                exit(3);
            }

            file = fopen(filename, "r");
            char c;
            char tmp;
            int i = 0;
            while((c = fgetc(file)) != EOF)
            {
                //int bytes_read = recv(sock, buf, 1024, 0);

                //if(bytes_read <= 0) break;
                //buf = fgetc(file);
                buf[i] = c;
                i++;

                if((i == 1024))
                {
                    send(sock, buf, sizeof(buf), 0);
                    i=0;
                    printf("%i\n",i);
                    buf = "";
                }

            }
            send(sock, buf, sizeof(buf), 0);
            fclose(file);
            close(sock);
    }
    printf("qqqqqqqqqqqqqqqqqqqqqqqqqqqqqq");

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
    addr.sin_addr.s_addr = htonl(INADDR_ANY);//((((3 << 8) | 0) << 8) | 0) << 8) | 128;

    if(bind( manager->sock_id, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        exit(2);
    }


    listen(manager->sock_id, manager->num_of_threads);

    //manager->threads = pthread_t[manager->num_of_threads];
    //manager.finish = 0;
    int t;
    printf("%i\n",manager->num_of_threads);
    for (t = 0; t < manager->num_of_threads; t++)
    {
        printf("%i\n",t);
        manager->threads = (pthread_t *)calloc(t+1,sizeof(pthread_t));

        pthread_create(&manager->threads[t], NULL, threadwork, (void *) manager);
        //pthread_join(manager->threads[t], NULL);
    }

    printf("init complited\n");

}




int main(int argc, char** argv)
{
   struct TaskManager manager, *man;
   printf("sd\n");

   manager.num_of_threads = 1;//atoi(argv[3]);
   printf("sd2\n");

   man = &manager;
   InitManager(man);
   int t=0;

   for(t = 0; t<manager.num_of_threads;t++)
   {
       pthread_join(manager.threads[t], NULL);
   }



}
