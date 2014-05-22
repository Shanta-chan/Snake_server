//
//  main.cpp
//  Snake_server
//
//  Created by Ringo on 21.05.14.
//  Copyright (c) 2014 Ringo. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>


bool shouldStop = false;
int clients = 0;
int client1 = 0;
int client2 = 0;
int listener;
int msg1, msg2;

void* f1(void *param)
{
    while (!shouldStop)
    {
        if (!client1)
        {
            client1 = accept(listener, NULL, NULL);
            clients++;
            printf("Clients: %d", clients);
        }
        recv(client1, &msg1, sizeof(int), 0);
        switch (msg1)
        {
            case -1:
            {
                //shutdown(client1, 2);
                close(client1);
                --clients;
                client1 = accept(listener, NULL, NULL);
                clients++;
                printf("Clients: %d", clients);
                break;
            }
            default:
            {
                int doubleMsg = msg1 * 2;
                send(client2, &doubleMsg, sizeof(int), 0);
                send(client1, &msg1, sizeof(int), 0);
                break;
            }
        }
    }
    return 0;
}

void* f2(void *param)
{
    while (!shouldStop)
    {
        if (!client2)
        {
            client2 = accept(listener, NULL, NULL);
            clients++;
            printf("Clients: %d", clients);
        }
        recv(client1, &msg2, sizeof(int), 0);
        switch (msg1)
        {
            case -1:
            {
                //shutdown(client2, 2);
                close(client2);
                --clients;
                client1 = accept(listener, NULL, NULL);
                clients++;
                printf("Clients: %d", clients);
                break;
            }
            default:
            {
                int doubleMsg = msg2 * 2;
                send(client1, &doubleMsg, sizeof(int), 0);
                send(client2, &msg2, sizeof(int), 0);
                break;
            }
        }
    }

    return 0;
}

void start()
{
    struct sockaddr_in addr;
    
    if((listener = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket");
        exit(1);
    }
    
    listener = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8081);
    addr.sin_addr.s_addr = ntohl(INADDR_ANY);
    
    if( bind(listener,(struct sockaddr*) &addr, sizeof(addr)) < 0)
    {
        perror("bind");
        exit(2);
    }
    listen(listener, 1);
    
    pthread_t p1;
    pthread_t p2;
    
    pthread_create(&p1, NULL, f1, NULL);
    pthread_create(&p2, NULL, f2, NULL);
    
    void *ret;
    
    pthread_join(p1, &ret);
    pthread_join(p2, &ret);
}


int main()
{
    start();
}

