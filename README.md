# Webserv, a mini web server with C++

## Configuration file
The configuration file must have atleast one `server{}`.
If the same directives is used multiple time in the same context, an overwrite can occur if a parameter is already set.
Using `#` is considered as a start of a comment and then the rest of the line will not be considered.
You can use `webserv -t configuration_file`  to check if the configuration is valid.

### Directives list

Syntax **allow_method** _method_ ... ;
Default: GET POST DELETE
Context: server, location

Syntax: **cgi** _extension_ _cgi_binary_path_ ;
Default: _none_
Context: server, location

Syntax: **listen** *address*[:*port*] ;
**listen** *port* ;
Default: listen *:80 | *:8000;
Context: server

Syntax : **server_name** _name_ ... ;
Default : **server_name** "" ;
Context : server

Syntax: **error_page**  _code_ ... [=[_response_]] _uri_;
Default: _none_
Context: server, location

Syntax: **client_max_body_size** _size_ ;
Default: client_max_body_size 1m;
Context: server, location

Syntax: **location** [ = | ~ | ~* | ^~ ] _uri_ { ... }
**location** @_name_ { ... }
Default: _none_
Context: server, location

Syntax:**return** _code_ [_text_] ;
**return** _code_ _URL_;
**return** _URL_;
Default: _none_
Context: server, location

Syntax: **root** _path_ ;
Default: root html;
Context: server, location

Syntax: **autoindex** on | off ;
Default: autoindex off;
Context: server, location

Syntax: **index** _file_ ... ;
Default: index index.html;
Context: server, location

Syntax: **upload_store** _directory_ ... ;
Default: _none_
Context: server, location