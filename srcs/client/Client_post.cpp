#include "Client.hpp"

bool Client::upload_file(std::string &raw_request) {
	std::string upload_path;
	if (_if_location && _location.get_upload_store().first == true)
		upload_path = create_path(_location.get_upload_store().second);
	else if (_conf.get_upload_store().first == true)
		upload_path = create_path(_conf.get_upload_store().second);

	while (raw_request.find("filename") != std::string::npos) {
		if (!_conf.get_upload_store().second.empty()) {
			if (access(upload_path.c_str(), X_OK) != 0) {
				return false;
			}
		}
		int filename_index = 0;
		if (raw_request.find("filename=") != std::string::npos)
			filename_index = raw_request.find("filename=");

		try {
			raw_request.erase(0, filename_index);
		}
		catch (std::exception &e) {
			std::cout << "First catch" << std::endl;
		}

		int n = 0;
		if (raw_request.find("\n") != std::string::npos)
			n = raw_request.find("\n");

		// get the file name
		std::string filename = raw_request.substr(10, n - 12 );
		// std::cout << BLU << "FILENAME : " << filename << std::endl;

		int content_start_index = 0;
		if (raw_request.find("\r\n\r\n") != std::string::npos)
			content_start_index = raw_request.find("\r\n\r\n");

		try {
			raw_request.erase(0, content_start_index + 4);
		}
		catch (std::exception &e) {
			std::cout << "Second catch" << std::endl;
		}


		int content_end_index = 0;
		if(raw_request.find(_body_boundary) != std::string::npos)
			content_end_index = raw_request.find(_body_boundary);

		try {
			// Sometimes this `erase` throw because the first parameter is negative,
			// leading to server crash.
			std::cout << "content_end_index - 5 = " << content_end_index - 5 << 
				" | _body_boundary.size =  " << +_body_boundary.size() << std::endl; 
			raw_request.erase(content_end_index - 5, _body_boundary.size() + 8);
		}
		catch (std::exception &e) {
			std::cout << "Third catch : " << e.what() << std::endl;
		}

		std::ofstream temp_file((upload_path + "/" + filename).c_str());
		temp_file << raw_request;
		temp_file.close();
	}
	return true;
}

void Client::process_body(std::string &raw_request) {
	// The presence of a message body in a request is signaled
	// by a Content-Length or Transfer-Encoding header field.

	// NOTE : Each time this function is called, need to check max_body_size

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
	// int has_boundary = false; // -> variable ‘has_boundary’ set but not used 
	// if (_headers["content-type"].find("boundary") != std::string::npos)
	// 	has_boundary = true;

	// seperate body from headers
	if (raw_request.find("\r\n\r\n") != std::string::npos) {
		int body_start_index = raw_request.find("\r\n\r\n");
		raw_request.erase(0, body_start_index + 4);
	}

	_body.append(raw_request);
	upload_file(raw_request);
	_request_is_complete = true;
	return ;
}
