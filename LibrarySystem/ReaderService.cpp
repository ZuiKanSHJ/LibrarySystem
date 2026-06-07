#include "ReaderService.h"
#include "DBHelper.h"
#include <iostream>
#include <cppconn/prepared_statement.h>
#include "Reader.h"
//添加读者
bool ReaderService::addReader(const Reader& reader) {
    sql::Connection* conn = DBHelper::getConn();
    if (!conn) return false;

    sql::PreparedStatement* pstmt = nullptr;
    try {
        std::string sql = "INSERT INTO reader(reader_name, phone) VALUES(?,?)";
        pstmt = conn->prepareStatement(sql);
        pstmt->setString(1, reader.name);
        pstmt->setString(2, reader.phone);

        int rows = pstmt->executeUpdate();
        DBHelper::closeRes(nullptr, pstmt, conn);
        return rows > 0;
    }
    catch (sql::SQLException& e) {
        std::cerr << "添加读者失败：" << e.what() << std::endl;
        DBHelper::closeRes(nullptr, pstmt, conn);
        return false;
    }
}

//删除读者
bool ReaderService::deleteReader(int readerId) {
    sql::Connection* conn = DBHelper::getConn();
    if (!conn) return false;

    sql::PreparedStatement* pstmt = nullptr;
    try {
        std::string sql = "DELETE FROM reader WHERE reader_id=?";
        pstmt = conn->prepareStatement(sql);
        pstmt->setInt(1, readerId);

        int rows = pstmt->executeUpdate();
        DBHelper::closeRes(nullptr, pstmt, conn);
        return rows > 0;
    }
    catch (sql::SQLException& e) {
        std::cerr << "删除读者失败：" << e.what() << std::endl;
        DBHelper::closeRes(nullptr, pstmt, conn);
        return false;
    }
}

//修改读者
bool ReaderService::updateReader(const Reader& reader) {
    sql::Connection* conn = DBHelper::getConn();
    if (!conn) return false;

    sql::PreparedStatement* pstmt = nullptr;
    try {
        std::string sql = "UPDATE reader SET reader_name=?, phone=? WHERE reader_id=?";
        pstmt = conn->prepareStatement(sql);
        pstmt->setString(1, reader.name);
        pstmt->setString(2, reader.phone);
        pstmt->setInt(3, reader.reader_id);

        int rows = pstmt->executeUpdate();
        DBHelper::closeRes(nullptr, pstmt, conn);
        return rows > 0;
    }
    catch (sql::SQLException& e) {
        std::cerr << "修改读者失败：" << e.what() << std::endl;
        DBHelper::closeRes(nullptr, pstmt, conn);
        return false;
    }
}

//按ID查询读者
Reader ReaderService::findReaderById(int readerId) {
    Reader emptyReader = { 0, "", "" };
    sql::Connection* conn = DBHelper::getConn();
    if (!conn) return emptyReader;

    sql::PreparedStatement* pstmt = nullptr;
    sql::ResultSet* res = nullptr;
    try {
        std::string sql = "SELECT * FROM reader WHERE reader_id=?";
        pstmt = conn->prepareStatement(sql);
        pstmt->setInt(1, readerId);
        res = pstmt->executeQuery();

        if (res->next()) {
            Reader r;
            r.reader_id = res->getInt("reader_id");
            r.name = res->getString("reader_name");
            r.phone = res->getString("phone");
            DBHelper::closeRes(res, pstmt, conn);
            return r;
        }
        DBHelper::closeRes(res, pstmt, conn);
        return emptyReader;
    }
    catch (sql::SQLException& e) {
        std::cerr << "查询读者失败：" << e.what() << std::endl;
        DBHelper::closeRes(res, pstmt, conn);
        return emptyReader;
    }
}

//查询所有读者
std::vector<Reader> ReaderService::findAllReaders() {
    std::vector<Reader> list;
    sql::Connection* conn = DBHelper::getConn();
    if (!conn) return list;

    sql::Statement* stmt = nullptr;
    sql::ResultSet* res = nullptr;
    try {
        stmt = conn->createStatement();
        res = stmt->executeQuery("SELECT * FROM reader ORDER BY reader_id");

        while (res->next()) {
            Reader r;
            r.reader_id = res->getInt("reader_id");
            r.name = res->getString("reader_name");
            r.phone = res->getString("phone");
            list.push_back(r);
        }
        DBHelper::closeRes(res, stmt, conn);
        return list;
    }
    catch (sql::SQLException& e) {
        std::cerr << "查询所有读者失败：" << e.what() << std::endl;
        DBHelper::closeRes(res, stmt, conn);
        return list;
    }
}