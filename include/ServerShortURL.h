#pragma once

#include <iostream>
#include <string>

#include "SQLiteDB.h"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <nlohmann/json.hpp>

namespace beast = boost::beast;
namespace http = boost::beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;
using json = nlohmann::json;

class ServerShortURL
{
public:
	ServerShortURL(std::string, unsigned short, SQLiteDB&);
	~ServerShortURL();
	void run();

private:
	void handle_request(http::request<http::string_body>&, http::response<http::string_body>&);

	SQLiteDB db;
	std::string address;
	unsigned short port;
};