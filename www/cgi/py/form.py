import cgi, cgitb 

form = cgi.FieldStorage() 

first_name = form.getvalue('first_name')
last_name  = form.getvalue('last_name')

print ("Content-type:text/html; charset=utf-8\r\n\r\n")
print ("<html>")
print ("<head>")
print ("<title>Hello - Basic CGI Program</title>")
print ("</head>")
print ("<body>")
print (f'<h2>Hello {first_name} {last_name}</h2>')
print ("</body>")
print ("</html>")
