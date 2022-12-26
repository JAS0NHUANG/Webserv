Configuration file
------------------

### Explanation

- The configuration file must have atleast one server{}.
- If the same directives is used multiple time in the same context, an overwrite can occur if a parameter is already set.
- Using `#` is considered as a start of a comment and then the rest of the line will not be considered.
- You can use `webserv -t configuration\_file` to check if the configuration is valid.

### Directives list
Syntax allow\_method method ... ;  
Default: allow\_method GET POST DELETE ;  
Context: server, location  

Syntax: cgi extension cgi\_binary\_path ;  
Default: none  
Context: server, location  

Syntax: listen address:port ;  
Default: listen \*:80;  
Context: server  

Syntax : server\_name name ... ;  
Default : server\_name "" ;  
Context : server  

Syntax: error\_page code ... uri;  
Default: none  
Context: server, location  

Syntax: client\_max\_body\_size size ;  
Default: client\_max\_body\_size 1m ;  
Context: server, location  

Syntax: location uri { ... }  
Default: none  
Context: server  

Syntax: return URL ;  
Default: none  
Context: server, location  

Syntax: root path ;  
Default: root /var/www/html ;  
Context: server, location  

Syntax: autoindex on | off ;  
Default: autoindex off ;  
Context: server, location  

Syntax: index file ... ;  
Default: index index.html ;  
Context: server, location  

Syntax: upload\_store directory ;  
Default: none  
Context: server, location  

