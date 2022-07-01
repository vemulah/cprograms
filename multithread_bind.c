#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <pthread.h>

void* do_work(void *arg)
{
    int *port = (int *) arg;

    int listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    int one = 1;
    setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &one, sizeof(one));

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(*port);

    int ret = bind(listen_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if (ret == -1)
    {
        printf("Failed to bind to port\n");
        return NULL;
    }

    listen(listen_socket, 5);

    struct sockaddr_in cli_addr;
    memset(&cli_addr, 0, sizeof(cli_addr));
    int addr_length = sizeof(cli_addr);

    do
    {
        int cli_sock = accept(listen_socket, (struct sockaddr *) &cli_addr, (socklen_t *) &addr_length);
        close(cli_sock);
    } while (true);

    close(listen_socket);

    return 0;
}

int main(int ac, const char *av[])
{
    if (ac != 2)
    {
        printf("Usage: liser <port>\n");
        return 1;
    }

    int port = atoi(av[1]);

    const int MAX_THREADS = 10;
    pthread_t tid[MAX_THREADS];
    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_create(&tid[i], NULL, do_work, &port);
    }

    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_join(tid[i], NULL);
    }
    return 0;
}
