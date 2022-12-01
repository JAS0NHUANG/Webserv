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
        void set_header_fields(int cont_Leng, Location check_location);
        void check_setting_location(Location Location, Config conf);
        bool								send_client_error_response() const;
		bool								send_server_error_response() const;
		bool								send_successful_response() const;
		bool								send_redirection_message() const;
		bool								send_informational_response() const;
        bool send_cgi_response(std::string body) const ;


    private:
        Client client;
        std::string extension;
        std::map<int, std::string> init_code_msg();
        std::string http_version;
        std::map<int, std::string> status_code_list;
        int status_code;
        std::string header_fields;
        //std::map<std::string, std::string> re_header_fields;
        //std::map<std::string, std::string> entity_header;
        std::string body;
        void set_header_fields();
        void set_body();
        bool delete_file();


        //Transfer-Encoding  chunk
        //Content-Length
        //Content-Type


        //date
        //server
        //Content-Location:   post ?? redicetion?
        //allow 405 Method Not Allowed status code



};


#endif