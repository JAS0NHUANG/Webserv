#ifndef RESPONSE_HPP
# define RESPONSE_HPP

class Response {
    public:

        Response(Client client);	
		~Response(void);
        std::string get_code_msg(int status_code);


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