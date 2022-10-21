#include "TestServer.hpp"

TestServer::TestServer():SimpleServer(AF_INET, SOCK_STREAM, 0, 4242, INADDR_ANY, 5){
    launch();
}

void TestServer::accepter(){
    struct sockaddr_in addr = get_socket()->get_addr();
    int addr_len = sizeof(addr);
    new_socket = accept(get_socket()->get_sock(), (struct sockaddr *)&addr , (socklen_t *)&addr_len);
    if (new_socket < 0)
    {
        perror("In accept");            
        exit(EXIT_FAILURE); 
    }
    //read(new_socket, buffer, 3000);
}

void TestServer::handler(){
    std::cout << "| " << "|" << "|" << std::endl;
}

 void TestServer::responder(){
     char *reponse= "hello from server!";
     write(new_socket, reponse, strlen(reponse));
     close(new_socket);
     //delete socket?? 
 }

 void TestServer::launch(){
     struct kevent evSet;
     //struct	kevent chlist[128];	 //Event to monitor 
     struct	kevent evList[128];	 //Event triggered 
     int kq, nev;
     int localFd = get_socket()->get_sock();
     //int junk = open("some.big.file", O_RDONLY);
     uint64_t bytes_written = 0;
     
     kq = kqueue();
     if (kq == -1){
         perror("kqueue");            
         exit(EXIT_FAILURE); 
     }
     std::cout<<"localfd" << get_socket()->get_sock() <<std::endl;
     EV_SET(&evSet, localFd, EVFILT_READ, EV_ADD, 0, 0, NULL);
     assert(-1 != kevent(kq, &evSet, 1, NULL, 0, NULL));
     while(1){
         std::cout<<"here"<<std::endl;
         nev = kevent(kq, NULL, 0, evList, 128, NULL);
         std::cout<<"new:" << nev<<std::endl;
         for (int i = 0; i < nev; i++) {
            std::cout<<"in for"<<std::endl;
            int fd = (int)evList[i].ident;
            if (evList[i].flags & EV_EOF) {
                printf("Disconnect\n");
                close(fd);
                // Socket is automatically removed from the kq by the kernel.
            } else if (fd == localFd) {
                std::cout<<"new event"<<std::endl;
                struct sockaddr_storage addr;
                socklen_t socklen = sizeof(addr);
                int connfd = accept(fd, (struct sockaddr *)&addr, &socklen);
                //assert(connfd != -1);
                
                // Listen on the new socket
                EV_SET(&evSet, connfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
                kevent(kq, &evSet, 1, NULL, 0, NULL);
                printf("Got connection!\n");
                
                int flags = fcntl(connfd, F_GETFL, 0);
                assert(flags >= 0);
                fcntl(connfd, F_SETFL, flags | O_NONBLOCK);

                // schedule to send the file when we can write (first chunk should happen immediately)
                EV_SET(&evSet, connfd, EVFILT_WRITE, EV_ADD | EV_ONESHOT, 0, 0, NULL);
                kevent(kq, &evSet, 1, NULL, 0, NULL);
            } else if (evList[i].filter == EVFILT_READ) {
                std::cout<<"read"<<std::endl;
                // Read from socket.
                char buf[1024];
                size_t bytes_read = recv(fd, buf, sizeof(buf), 0);
                printf("read %zu bytes\n", bytes_read);
                printf("|%s|\n", buf);
            } else if (evList[i].filter == EVFILT_WRITE) {
                    std::cout<<"write"<<std::endl;
//                printf("Ok to write more!\n");
                 off_t offset = (off_t)evList[i].udata;
                 off_t len = 0;//evList[i].data;
//                 if (sendfile(junk, fd, offset, &len, NULL, 0) != 0) {
// //                    perror("sendfile");
// //                    printf("err %d\n", errno);
                    uintptr_t fd_client = evList[i].ident;
                    std::cout<<"write : fd"<< fd_client <<std::endl;
                    char *s ="hello from server";
                    usleep(3000);
                    if (int re = send(fd_client, s, 18, 0) != 0){
                        perror("sendfile");
                        printf("err : |%d|,  |%d|\n", re , errno);
                        if (errno == EAGAIN) {
                        // schedule to send the rest of the file
                        EV_SET(&evSet, fd, EVFILT_WRITE, EV_ADD | EV_ONESHOT, 0, 0, (void *)(offset + len));
                        kevent(kq, &evSet, 1, NULL, 0, NULL);
                        }
                    }
                    bytes_written += len;
                    printf("wrote %lld bytes, %lld total\n", len, bytes_written);
            }
        //  std::cout <<" waiting~~~~" <<std::endl;
        //  //accepter();
        //  //handler();
        //  //responder();
         }
        //  std::cout << "done~~~~" <<std::endl;
     }
     //close(junk);

 }