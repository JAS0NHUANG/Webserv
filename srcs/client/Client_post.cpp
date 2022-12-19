#include "Client.hpp"

bool Client::upload_file(std::string &raw_request) {
	std::string upload_path;
	std::string filename;
	std::string file_body;

	if (_conf.get_upload_store().first == true)
		upload_path = _conf.get_upload_store().second;

	if (access(upload_path.c_str(), X_OK) != 0) {
		_status_code = 500;
		_request_is_complete = true;
		return false;
	}

	while (raw_request.find("filename") != std::string::npos) {

		// check if "filename" exist.
		int filename_index = 0;
		if (raw_request.find("filename=") != std::string::npos) {
			filename_index = raw_request.find("filename=");
			raw_request.erase(0, filename_index);
		}

		// get the filename
		int n = 0;
		if (raw_request.find("\n") != std::string::npos) {
			n = raw_request.find("\n");
			filename = raw_request.substr(10, n - 12 );
		}

		// remove the chars before the real content
		int content_start_index = 0;
		if (raw_request.find("\r\n\r\n") != std::string::npos) {
			content_start_index = raw_request.find("\r\n\r\n");
			raw_request.erase(0, content_start_index + 4);
		}

		// remove the end boundary
		int content_end_index = 0;
		if(raw_request.find(_body_boundary) != std::string::npos) {
			content_end_index = raw_request.find(_body_boundary);
			file_body = raw_request.substr(0, content_end_index - 5);
			raw_request.erase(0, content_end_index + _body_boundary.size() + 8);
		}

		std::ofstream temp_file((upload_path + "/" + filename).c_str());
		if (!temp_file.is_open()) {
			_status_code = 500;
			_request_is_complete = true;
			return false;
		}
		temp_file << file_body;
		temp_file.close();
		if (raw_request.empty() || \
			raw_request.find(_body_boundary) == std::string::npos) {
			break ;
		}
	}
	_request_is_complete = true;
	return true;
}

void Client::process_body(std::string &raw_request) {
	// The presence of a message body in a request is signaled
	// by a Content-Length or Transfer-Encoding header field.

	if (_request_is_complete == true || raw_request.empty())
		return ;

	// We dont expect receiving a body if the method is not POST
	if (_method != "POST") {
		_request_is_complete = true;
		return;
	}

	if (_headers.count("transfer-encoding") == 0 && \
		_headers.count("content-length") == 0) { // No body
		_request_is_complete = true;
		return ;
	}

	_body.append(raw_request);
	if (upload_file(raw_request))
		_request_is_complete = true;
	return ;
}
