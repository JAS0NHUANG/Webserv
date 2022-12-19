# References
- `man ip`! Read it. ^_^  
- `man socket`

# Steps
- Create `socket()`  
- `setsockopt()` and setup `sockaddr_in` struct
- `bind()` it to an address  
- `listen()` to new connection  
- deal with the operations inside `epoll()`

## 1. socket
The socket() system call creates a new socket. 

### prototype
`int socket(int domain, int type, int protocol);`

### parameters
- domain: `PF_LOCAL`, `PF_INET`, `PF_INET6`...etc.  
- type: `SOCK_STREAM`, `SOCK_DGRAM`, `SOCK_RAW`  
- protocol: see protocol numbers in `/etc/protocols` file.  

## 2. setsockopt, sockaddr_in

## 3. bind
The bind() system call binds a socket to an address.  

### prototype
`int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);`  

## 4. listen
The listen() system call marks the stream socket referred to by the fd sockfd as passive. The socket will subsequently be used to accept connection from (active sockets).

### prototype
`int listen (int sockfd, int backlog);`  

Notes: test backlog with multiple values  
Notes: what is the maximum backlog we will give to our server  
