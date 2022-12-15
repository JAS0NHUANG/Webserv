#include "Client.hpp"

bool Client::upload_file(std::string &raw_request) {
	std::cout << "\n\n\n The recv len: |" << _recv_length << "|\n\n\n";
	std::cout << "\n\n\n The request size now: |" << raw_request.length() << "|\n\n\n";
	std::cout << "\n\n\n The content length: |" << _headers["content-length"] << "|\n\n\n";
	if (_recv_length < toInt(_headers["content-length"])) 
	{
		std::cout << "here\n";
		_request_is_complete = false;
		return  false;
	} else {
		std::cout << "handlinggggg\n";

		while ( raw_request.find("filename") != std::string::npos) {
			std::cout << "in the loop!!!!\n";
			if (!_conf.get_upload_store().second.empty()) {
				if (access(_conf.get_upload_store().second.c_str(), X_OK) != 0) {
					return false;
				}
			}
			int filename_index = 0;
			if (raw_request.find("filename=") != std::string::npos) {
				filename_index = raw_request.find("filename=");
				std::cout  << "Filename index: " << filename_index << "\n";
				raw_request.erase(0, filename_index);
			}
			int n = 0;
			if (raw_request.find("\n") != std::string::npos)
				n = raw_request.find("\n");

			// get the file name
			std::string filename = raw_request.substr(10, n - 12 );

			int content_start_index = 0;
			if (raw_request.find("\r\n\r\n") != std::string::npos) {
				content_start_index = raw_request.find("\r\n\r\n");
				raw_request.erase(0, content_start_index + 4);
			}

			int content_end_index = 0;
			if(raw_request.find(_body_boundary) != std::string::npos) {
				content_end_index = raw_request.find(_body_boundary);
				raw_request.erase(content_end_index - 5, _body_boundary.size() + 8);
			}

			std::ofstream temp_file((_conf.get_upload_store().second + "/" + filename).c_str());
			temp_file << raw_request;
			temp_file.close();
		}
		std::cout << "\n\n\n The request size now: |" << _raw_request.length() << "|\n\n\n";
		std::cout << "\n\n\n The content length: |" << _headers["content-length"] << "|\n\n\n";
		std::cout << "\n\n\n The request size now: |" << _raw_request.length() << "|\n\n\n";
		std::cout << "\n\n\n The content length: |" << _headers["content-length"] << "|\n\n\n";
		_request_is_complete = true;
		return true;
	}
}

void Client::process_body(std::string &raw_request) {
	// The presence of a message body in a request is signaled
	// by a Content-Length or Transfer-Encoding header field.

	// NOTE : Each time this function is called, need to check max_body_size
	if (_request_is_complete == true)
		return ;

	// We dont expect receiving a body if the method is not POST
	if (_method != "POST") {
		_request_is_complete = true;
		return;
	}

	if (raw_request.size() > _conf.get_client_max_body_size()) {
		_request_is_complete = true;
		throw 413;
	}

	if (_headers.count("transfer-encoding") == 0 && \
		_headers.count("content-length") == 0) { // No body
		_request_is_complete = true;
		return ;
	}

	// check if there's boundary
	int has_boundary = false;
	if (_headers["content-type"].find("boundary") != std::string::npos)
		has_boundary = true;

	// seperate body from headers
	if (raw_request.find("\r\n\r\n") != std::string::npos) {
		int body_start_index = raw_request.find("\r\n\r\n");
		raw_request.erase(0, body_start_index + 4);
	}

	_body.append(raw_request);
	std::cout << "raw_request added again!!!\n";
	std::cout << raw_request;
	if (upload_file(raw_request) || _request_is_complete)
		_request_is_complete = true;
	else
		_request_is_complete = false;
	return ;
}
