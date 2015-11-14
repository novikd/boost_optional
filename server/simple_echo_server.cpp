//
//  server.cpp
//  Simple Server
//
//  Created by Дмитрий Новик on 31.10.15.
//  Copyright © 2015 Дмитрий Новик. All rights reserved.
//

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/event.h>
#include <sys/time.h>

int init(int port) {
    sockaddr_in server;
    
    int main_socket = socket(PF_INET, SOCK_STREAM, 0);
    
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);
    
    int binded = bind(main_socket, (const sockaddr*) &server, sizeof(server));
    
    if (binded == -1) {
        perror("Binding error uccured!\n");
    }
    
    listen(main_socket, SOMAXCONN);
    
    return main_socket;
}

struct event_queue {
    event_queue(int socket):
        main_socket(socket),
        kq(kqueue()),
        addr(),
        socketlen(sizeof(addr))
    {
        EV_SET(&change, main_socket, EVFILT_READ, EV_ADD, 0, 0, NULL);
        
        if (kevent(kq, &change, 1, NULL, 0, NULL) == -1) {
            perror("Can't listen\n");
        }
    }
    
    int get_new_event() {
        return kevent(kq, NULL, 0, evList, SOMAXCONN, NULL);
    }
    
    bool is_eof(int i) {
        return evList[i].flags & EV_EOF;
    }
    
    void disconnect(int i) {
        puts("Disconnect\n");
        int fd = static_cast<int>(evList[i].ident);
        EV_SET(&event, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
        if (kevent(kq, &event, 1, NULL, 0, NULL) == -1) {
            perror("Kevent deleting error\n");
        }
        close(fd);
    }
    
    bool is_connect(int i) {
        return evList[i].ident == main_socket;
    }
    
    void connect(int i) {
        int fd = accept(static_cast<int>(evList[i].ident), (sockaddr*) &addr, &socketlen);
        if (fd == -1) {
            perror("Accept error\n");
        } else {
            puts("Connect\n");
        }
        EV_SET(&event, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
        kevent(kq, &event, 1, NULL, 0, NULL);
        send(fd, "Welcome!\n", 9, 0);
    }
    
    bool is_echo(int i) {
        return evList[i].flags & EVFILT_READ;
    }
    
    void echo(int i) {
        char* buffer = new char[255];
        int fd = static_cast<int>(evList[i].ident);
        size_t size = recv(fd, buffer, sizeof(buffer), 0);
        if (size == -1) {
            perror("Recv error!\n");
        } else {
            send(fd, buffer, size, 0);
        }
        delete [] buffer;
    }
    
    ~event_queue() {
        if (main_socket != -1)
            close(main_socket);
    }
    
private:
    struct sockaddr_storage addr;
    socklen_t socketlen;
    int main_socket, kq;
    struct kevent change, event, evList[SOMAXCONN];
};

int main() {
    int main_socket = init(2539);
    
    event_queue queue(main_socket);
    
    while (true) {
        int new_event = queue.get_new_event();
        if (new_event == -1) {
            perror("It's a pitty\n");
        }
        for (int i = 0; i < new_event; ++i) {
            if (queue.is_eof(i))
                queue.disconnect(i);
            else if (queue.is_connect(i))
                queue.connect(i);
            else if (queue.is_echo(i))
                queue.echo(i);
        }
    }
    
    return 0;
}
