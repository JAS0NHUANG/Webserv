#include "TestServer.hpp"

TestServer::TestServer():SimpleServer(AF_INET, SOCK_STREAM, 0, 4241, INADDR_ANY, 20){
    launch();
}

void TestServer::accepter(){
    struct sockaddr_in addr = get_socket()->get_addr();
    int addr_len = sizeof(addr);
    std::cout << "here1" << std::endl;
    new_socket = accept(get_socket()->get_sock(), (struct sockaddr *)&addr , (socklen_t *)&addr_len);
    if (new_socket < 0)
    {
        perror("In accept");            
        exit(EXIT_FAILURE); 
    }
    std::cout << "here2" << std::endl;
    read(new_socket, buffer, 3000);
}

void TestServer::handler(){
    std::cout << "| " << buffer << "|" << std::endl;
}

 void TestServer::responder(){
     char *reponse= "hello from server!";
     write(new_socket, reponse, strlen(reponse));
     close(new_socket);
     //delete socket?? 
 }

 void TestServer::launch(){
     while(1){
         std::cout <<" waiting~~~~" <<std::endl;
         accepter();
         handler();
         responder();
         std::cout << "done~~~~" <<std::endl;

     }
 }