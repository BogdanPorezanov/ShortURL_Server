#include "ServerShortURL.h"

ServerShortURL::ServerShortURL(std::string address, unsigned short port, SQLiteDB& db)
{
	this->address = address;
	this->port = port;
	this->db = db;
}

ServerShortURL::~ServerShortURL()
{
}

void ServerShortURL::run()
{
	try
	{
		net::io_context ioc;

		tcp::endpoint endpoint(net::ip::make_address(address), port);

		tcp::acceptor acceptor(ioc, endpoint);

		std::cout << "Server is running on http://" << address << ":" << port << std::endl;

		while (true)
		{
			tcp::socket socket(ioc);
			acceptor.accept(socket);

			beast::flat_buffer buffer;

			http::request<http::string_body> req;

			http::read(socket, buffer, req);

			http::response<http::string_body> res;

			handle_request(req, res);

			http::write(socket, res);

			socket.shutdown(tcp::socket::shutdown_send);
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

void ServerShortURL::handle_request(http::request<http::string_body>& req, http::response<http::string_body>& res)
{
	res.version(req.version());

	res.set(http::field::content_type, "application/json");

	switch (req.method())
	{
	case http::verb::get:
	{
		std::string target = db.Get_Full_URL(req.target());
		if (target != "0")
		{
			res.result(http::status::moved_permanently); // передаём статус 301 Moved permanently
			res.set(http::field::location, target);
			res.body() = "Redirecting ...";
			res.prepare_payload(); // проверяем перед отправкой
		}
		else
		{
			res.result(http::status::not_found); // статус 404 Not found
			res.body() = "This short URL was not found or an error occurred";
			res.prepare_payload(); // проверяем перед отправкой
		}
		break;
	}
	case http::verb::post: {
		try
		{
			json body = json::parse(req.body());
			unsigned int body_size = body.size();
			if (body_size == 1)
			{
				std::string result = db.Add_Any_URL(body["full_url"]);
				if (result != "0")
				{
					std::string message = "Short URL successfully created for ";
					message.append(body["full_url"]);
					json response = {
						{"message", message},
						{"short_url", result}
					};
					res.result(http::status::ok);
					res.body() = response.dump();
				}
			}
			else if (body_size == 2)
			{
				int result = db.Add_Custom_URL(body["full_url"], body["short_url"]);
				if (result == 0)
				{
					std::string message = "For ";
					message.append(body["full_url"]);
					message.append(" successfully created ");
					message.append(body["short_url"]);
					json response = { {"message", message} };
					res.result(http::status::ok);
					res.body() = response.dump();
				}
			}
			else
			{
				res.result(http::status::bad_request);
				res.body() = json{ {"error", "Unsupported method or Invalid JSON"} }.dump();
			}

			json response = {

			};
		}
		catch (const json::parse_error& e)
		{
			res.result(http::status::bad_request);
			res.body() = json{ {"error", "Invalid JSON"} }.dump();
		}
		break;
	}
	default: { // остальные методы не поддерживаются
		res.result(http::status::bad_request);
		res.body() = json{ {"error", "Unsupported method"} }.dump();
		break;
	}
	}
	res.prepare_payload();
}