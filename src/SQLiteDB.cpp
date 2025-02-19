#include "SQLiteDB.h"

SQLiteDB::SQLiteDB()
{
}

SQLiteDB::~SQLiteDB()
{
	if (DB_Open == 1)
	{
		sqlite3_close(db);
	}
}

int SQLiteDB::Open_DB(std::string path) // вернём 0 в случае успешного открытия
{
	statusDB = sqlite3_open_v2(path.c_str(), &db, SQLITE_OPEN_READWRITE, nullptr);
	if (statusDB == SQLITE_OK)
	{
		std::cout << "DB open!" << std::endl;
		DB_Open = 1;
		return 0;
	}
	else if (statusDB == SQLITE_CANTOPEN)
	{
		std::cout << "DB is missing!" << std::endl;
		return statusDB;
	}
	else
	{
		std::cerr << "Error open DB: " << sqlite3_errmsg(db) << std::endl;
		return statusDB;
	}
}

std::string SQLiteDB::Add_Any_URL(std::string Full_URL) // возвращает короткий URL в случае успеха, "0" в случае неудачи
{
	std::string New_Short_URL = Get_New_Short_URL(Get_Last_Short_URL()); // получение нового неиспользованного короткого URL

	const char* SQL = "INSERT INTO ShortURL (Custom, Short_URL_KEY, Full_URL) VALUES (0, ?, ?);"; // основа запроса, значение Custom = 0
	sqlite3_stmt* stmt;

	statusDB = sqlite3_prepare_v2(db, SQL, -1, &stmt, nullptr); // подготовка запроса
	if (statusDB != SQLITE_OK) {
		std::cerr << "Error prepare: " << sqlite3_errmsg(db) << std::endl;
		return "0";
	}

	sqlite3_bind_text(stmt, 1, New_Short_URL.c_str(), -1, SQLITE_STATIC); // привязка параметров
	sqlite3_bind_text(stmt, 2, Full_URL.c_str(), -1, SQLITE_STATIC);

	statusDB = sqlite3_step(stmt); // выполнение запроса
	if (statusDB == SQLITE_DONE) {
		sqlite3_finalize(stmt);
		return New_Short_URL;
	}
	else
	{
		std::cerr << "Error insert: " << sqlite3_errmsg(db) << std::endl;
		sqlite3_finalize(stmt);
		return "0";
	}
}

int SQLiteDB::Add_Custom_URL(std::string Full_URL, std::string Short_URL) // возвращает 0 в случае успеха
{
	const char* SQL = "INSERT INTO ShortURL (Custom, Short_URL_KEY, Full_URL) VALUES (1, ?, ?);"; // основа запроса, в добавлении пользвательского URL Custom = 1
	sqlite3_stmt* stmt;
	statusDB = sqlite3_prepare_v2(db, SQL, -1, &stmt, nullptr); // подготовка запроса
	if (statusDB != SQLITE_OK) {
		std::cerr << "Error prepare: " << sqlite3_errmsg(db) << std::endl;
		return statusDB;
	}

	sqlite3_bind_text(stmt, 1, Short_URL.c_str(), -1, SQLITE_STATIC); // привязка параметров
	sqlite3_bind_text(stmt, 2, Full_URL.c_str(), -1, SQLITE_STATIC);

	statusDB = sqlite3_step(stmt); // выполнение запроса
	if (statusDB != SQLITE_DONE) {
		std::cerr << "Error insert: " << sqlite3_errmsg(db) << std::endl;
		if (statusDB == 19)
		{
			std::cerr << "Yes not unik: " << sqlite3_errmsg(db) << std::endl;
		}
		sqlite3_finalize(stmt);
		return statusDB;
	}
	sqlite3_finalize(stmt);
	return 0;
}

std::string SQLiteDB::Get_Full_URL(std::string Short_URL)
{
	const char* SQL = "SELECT Full_URL FROM ShortURL WHERE Short_URL_KEY = ?;"; // основа запроса
	sqlite3_stmt* stmt;
	statusDB = sqlite3_prepare_v2(db, SQL, -1, &stmt, nullptr); // подготовка запроса
	if (statusDB != SQLITE_OK) {
		std::cerr << "Error prepare: " << sqlite3_errmsg(db) << std::endl;
		return "0";
	}

	sqlite3_bind_text(stmt, 1, Short_URL.c_str(), -1, SQLITE_STATIC); // привязка параметров

	statusDB = sqlite3_step(stmt); // выполнение запроса
	if (statusDB == SQLITE_ROW)
	{
		std::string result = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
		sqlite3_finalize(stmt);
		return result;
	}
	else if (statusDB == SQLITE_DONE)
	{
		sqlite3_finalize(stmt);
		return "0";
	}
	else
	{
		sqlite3_finalize(stmt);
		return "0";
	}
}

std::string SQLiteDB::Get_Last_Short_URL()
{
	const char* SQL = "SELECT Short_URL_KEY FROM ShortURL WHERE Custom = 0 ORDER BY KEY DESC LIMIT 1;"; // основа запроса, достать 1 последнюю запись
	sqlite3_stmt* stmt;
	statusDB = sqlite3_prepare_v2(db, SQL, -1, &stmt, nullptr); // подготовка запроса
	if (statusDB != SQLITE_OK) {
		std::cerr << "Error prepare: " << sqlite3_errmsg(db) << std::endl;
		return "0";
	}

	statusDB = sqlite3_step(stmt); // выполнение запроса
	if (statusDB == SQLITE_ROW)
	{
		std::string result = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
		sqlite3_finalize(stmt);
		return result;
	}
	else
	{
		sqlite3_finalize(stmt);
		return "0";
	}
}

std::string SQLiteDB::Get_New_Short_URL(std::string Last_Short_URL)
{
	char lastSymbol = Last_Short_URL.back();
	std::string result = Last_Short_URL;
	if (lastSymbol == 'Z')
	{
		result.push_back('A');
		return result;
	}
	else
	{
		result.pop_back();
		result.push_back(lastSymbol + 1);
		return result;
	}
}