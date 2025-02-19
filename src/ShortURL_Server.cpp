#include "ShortURL_Server.h"

using namespace std;

int main(int argc, char* argv[])
{
	SQLiteDB db;
	if (argc == 3)
	{
		if (db.Open_DB("ShortURL.db") != 0)
		{
			return 0;
		}
	}
	else if (argc == 4)
	{
		if (db.Open_DB(argv[3]) != 0)
		{
			return 0;
		}
	}
	else
	{
		std::cerr << "Invalid input arguments" << std::endl;
		return 0;
	}

	std::string address = argv[1];
	std::string port_string = argv[2];
	unsigned short port;
	try {
		int port_int = std::stoi(port_string); // сначала преобразуем в int
		if (port_int >= 0 && port_int <= 65535) { // проверяем диапазон на соответствию unsigned short
			port = static_cast<unsigned short>(port_int);
		}
		else {
			std::cerr << "Error: the number is out of range unsigned short!" << std::endl;
			return 0;
		}
	}
	catch (const std::invalid_argument& e) {
		std::cerr << "Error: incorrect string format!" << std::endl;
		return 0;
	}
	catch (const std::out_of_range& e) {
		std::cerr << "Error: the number is out of range int!" << std::endl;
		return 0;
	}
	ServerShortURL Server(address, port, db);
	Server.run();
	return 0;
}
