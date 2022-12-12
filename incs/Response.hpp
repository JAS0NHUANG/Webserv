#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <iostream>
#include <fstream>
#include <string>   ///read file
#include "webserv.hpp"
#include "Cgi.hpp"

class Response {
    public:

        Response(Client client);	
		~Response(void);
        std::string get_code_msg(int status_code) const;
        bool send_response();
        void set_header_fields(int cont_Leng, Location check_location) ;
        void check_setting_location(Config conf);
        bool								send_error_response(Location check_location);
		bool								send_successful_response() ;
        bool send_cgi_response(std::string body) const ;


    private:
        Client client;
        Location location;
        std::string extension;
        std::map<int, std::string> init_code_msg();
        std::string http_version;
        std::map<int, std::string> status_code_list;
        int status_code;
        std::string header_fields;
        std::string body;
        bool set_body();
        bool set_autoindex_body();
        bool delete_file();
        bool post_body();
};


#endif
