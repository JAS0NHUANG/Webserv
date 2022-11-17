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
### prototype
`int socket(int domain, int type, int protocol);`

### parameters
- domain: `PF_LOCAL`, `PF_INET`, `PF_INET6`...etc.  
- type: `SOCK_STREAM`, `SOCK_DGRAM`, `SOCK_RAW`
- protocol: see protocol numbers in `/etc/protocols` file.


### Usage

## 2. setsockopt, sockaddr_in

