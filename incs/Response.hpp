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
        bool send_response();

    private:
        Client						_client;
        Location					_location;
		Config						_conf;
        bool						_if_location;
        std::string					_extension;
        std::map<int, std::string>	_status_code_list;
        int							_status_code;
        std::string					_header_fields;
        std::string					_body;
		std::string					_path;
		std::string					_syscall_error;
        std::string					content_mime_type(std::string extension);
        void						init_code_msg();
        void						set_body();
        void						set_autoindex_body();
        void						delete_file();
        bool						post_body();
        std::string					get_code_msg();
		std::string					get_file_content(std::string content);
        bool						send_cgi_response(std::string body);
        void						set_header_fields(int cont_Leng) ;
		bool						send_error_response();
		bool						send_successful_response();
		bool						is_redirected();
		bool						set_defined_error_page();
		void						set_default_error_page();
};

#endif
