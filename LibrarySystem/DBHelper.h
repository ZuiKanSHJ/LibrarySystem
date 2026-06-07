#ifndef DBHELPER_H
#define DBHELPER_H

#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <exception>
#include <iostream>
#include <string>

class DBHelper {
public:
    static sql::Connection* getConn();
    static void closeRes(sql::ResultSet* res, sql::Statement* stmt, sql::Connection* conn);
    static void closeRes(sql::ResultSet* res, sql::PreparedStatement* pstmt, sql::Connection* conn);

private:
    // 数据库配置
    static const std::string URL;
    static const std::string USER;
    static const std::string PASSWORD;
    static const std::string DATABASE;
};

#endif