<h1>Webserv, a mini web server with C++</h1>

<h2>Configuration file</h2>

<h3>Explanation</h3>

<p>The configuration file must have atleast one server{}.</p>
<p>If the same directives is used multiple time in the same context, an overwrite can occur if a parameter is already set.</p>
<p>Using # is considered as a start of a comment and then the rest of the line will not be considered.</p>
<p>You can use webserv -t configuration_file to check if the configuration is valid.</p>

<h3>Directives list</h3>

<p>Syntax allow_method method ... ;</p>
<p>Default: allow_method GET POST DELETE ;</p>
<p>Context: server, location</p>
<p>&nbsp;</p>

<p>Syntax: cgi extension cgi_binary_path ;</p>
<p>Default: none</p>
<p>Context: server, location</p>
<p>&nbsp;</p>

<p>Syntax: listen address:port ;</p>
<p>Default: listen *:80;</p>
<p>Context: server</p>
<p>&nbsp;</p>

<p>Syntax : server_name name ... ;</p>
<p>Default : server_name "" ;</p>
<p>Context : server</p>
<p>&nbsp;</p>

<p>Syntax: error_page code ... uri;</p>
<p>Default: none</p>
<p>Context: server, location</p>
<p>&nbsp;</p>

<p>Syntax: client_max_body_size size ;</p>
<p>Default: client_max_body_size 1m ;</p>
<p>Context: server, location</p>
<p>&nbsp;</p>

<p>Syntax: location uri { ... }</p>
<p>Default: none</p>
<p>Context: server</p>
<p>&nbsp;</p>

<p>Syntax: return URL ;</p>
<p>Default: none</p>
<p>Context: server, location</p>
<p>&nbsp;</p>

<p>Syntax: root path ;</p>
<p>Default: root /var/www/html ;</p>
<p>Context: server, location</p>
<p>&nbsp;</p>

<p>Syntax: autoindex on | off ;</p>
<p>Default: autoindex off ;</p>
<p>Context: server, location</p>
<p>&nbsp;</p>

<p>Syntax: index file ... ;</p>
<p>Default: index index.html ;</p>
<p>Context: server, location</p>
<p>&nbsp;</p>

<p>Syntax: upload_store directory ;</p>
<p>Default: none</p>
<p>Context: server, location</p>
