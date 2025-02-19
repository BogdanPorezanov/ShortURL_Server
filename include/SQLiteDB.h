#pragma once

#include <iostream>
#include <string>
#include "sqlite3.h"

class SQLiteDB
{
public:
	SQLiteDB();
	~SQLiteDB();
	int Open_DB(std::string); // возвращает 0 в случае успушного открытия базы данных
	std::string Add_Any_URL(std::string); // возвращает короткий URL в случае успеха, "0" в случае неудачи
	int Add_Custom_URL(std::string, std::string); // возвращает 0 в случае успеха, предназначена для пользовательского короткого URL
	std::string Get_Full_URL(std::string);


private:
	std::string Get_Last_Short_URL();
	std::string Get_New_Short_URL(std::string);

	sqlite3* db;
	char* errorMessage = nullptr;
	int statusDB;
	bool DB_Open = 0;
};