#ifndef RESPONSE_HPP
# define RESPONSE_HPP
#include "incs/webserv.hpp"
#include "incs/Client.hpp"

#include <iostream>
#include <fstream>
#include <string>   ///read file

class Response {
    public:

        Response(Client client);	
		~Response(void);
        std::string get_code_msg(int status_code) const;
        bool send_response();
        bool								send_client_error_response() const;
		bool								send_server_error_response() const;
		bool								send_successful_response() const;
		bool								send_redirection_message() const;
		bool								send_informational_response() const;


    private:
        Client client;
        std::map<int, std::string> init_code_msg();
        std::string http_version;
        std::map<int, std::string> status_code_list;
        int status_code;
        std::map<std::string, std::string> ge_header_fields;
        std::map<std::string, std::string> re_header_fields;
        std::map<std::string, std::string> entity_header;
        std::string entity_body;

};


#endif