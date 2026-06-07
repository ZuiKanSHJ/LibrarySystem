#include "DBHelper.h"
using namespace std;

// 数据库配置
const string DBHelper::URL = "tcp://127.0.0.1:3307";//端口
const string DBHelper::USER = "root";
const string DBHelper::PASSWORD = "";//密码
const string DBHelper::DATABASE = "library_manage";


sql::Connection* DBHelper::getConn() {
    try {
        sql::Driver* driver = get_driver_instance();
        sql::Connection* conn = driver->connect(URL, USER, PASSWORD);
        conn->setSchema(DATABASE); 
        return conn;
    }
    catch (sql::SQLException& e) {
        cerr << "数据库连接失败：" << e.what() << endl;
        return nullptr;
    }
}


void DBHelper::closeRes(sql::ResultSet* res, sql::Statement* stmt, sql::Connection* conn) {
    if (res) delete res;
    if (stmt) delete stmt;
    if (conn) {
        try { conn->close(); }
        catch (...) {} 
        delete conn;
    }
}

void DBHelper::closeRes(sql::ResultSet* res, sql::PreparedStatement* pstmt, sql::Connection* conn) {
    if (res) delete res;
    if (pstmt) delete pstmt;
    if (conn) {
        try { conn->close(); }
        catch (...) {} 
        delete conn;
    }
}