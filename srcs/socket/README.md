# ip
`man ip`! Read it. ^_^  

# socket

## prototype:
`int socket(int domain, int type, int protocol);`

## parameters
- domain: `PF_LOCAL`, `PF_INET`, `PF_INET6`...etc.  
- type: `SOCK_STREAM`, `SOCK_DGRAM`, `SOCK_RAW`
- protocol: see protocol numbers in `/etc/protocols` file.
