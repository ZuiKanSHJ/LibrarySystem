#include "BookService.h"
#include "DBHelper.h"
#include <iostream>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>

// 新增图书
bool BookService::addBook(const Book& bk) {
    sql::Connection* conn = DBHelper::getConn();
    if (!conn) return false;

    sql::PreparedStatement* pstmt = nullptr;
    try {
        std::string sql = "INSERT INTO book (book_name, author, price, quantity) VALUES (?, ?, ?, ?)";
        pstmt = conn->prepareStatement(sql);
        pstmt->setString(1, bk.book_name);
        pstmt->setString(2, bk.author);
        pstmt->setDouble(3, bk.price);
        pstmt->setInt(4, bk.stock);

        int affect = pstmt->executeUpdate();
        DBHelper::closeRes(nullptr, pstmt, conn);
        return affect > 0;
    }
    catch (sql::SQLException& e) {
        std::cerr << "新增失败:" << e.what() << std::endl;
        DBHelper::closeRes(nullptr, pstmt, conn);
        return false;
    }
}

// 删除图书
bool BookService::delBook(int bid) {
    sql::Connection* conn = DBHelper::getConn();
    if (!conn) return false;

    sql::PreparedStatement* pstmt = nullptr;
    try {
        std::string sql = "DELETE FROM book WHERE id = ?";
        pstmt = conn->prepareStatement(sql);
        pstmt->setInt(1, bid);

        int affect = pstmt->executeUpdate();
        DBHelper::closeRes(nullptr, pstmt, conn);
        return affect > 0;
    }
    catch (sql::SQLException& e) {
        std::cerr << "删除失败:" << e.what() << std::endl;
        DBHelper::closeRes(nullptr, pstmt, conn);
        return false;
    }
}

// 修改图书
bool BookService::updateBook(const Book& bk) {
    sql::Connection* conn = DBHelper::getConn();
    if (!conn) return false;

    sql::PreparedStatement* pstmt = nullptr;
    try {
        std::string sql = "UPDATE book SET book_name=?, author=?, price=?, quantity=? WHERE id=?";
        pstmt = conn->prepareStatement(sql);
        pstmt->setString(1, bk.book_name);
        pstmt->setString(2, bk.author);
        pstmt->setDouble(3, bk.price);
        pstmt->setInt(4, bk.stock);
        pstmt->setInt(5, bk.book_id);

        int affect = pstmt->executeUpdate();
        DBHelper::closeRes(nullptr, pstmt, conn);
        return affect > 0;
    }
    catch (sql::SQLException& e) {
        std::cerr << "修改失败:" << e.what() << std::endl;
        DBHelper::closeRes(nullptr, pstmt, conn);
        return false;
    }
}

// 按ID查询
Book BookService::findById(int bid) {
    Book emptyBook = { 0, "", "", 0.0, 0 }; 
    sql::Connection* conn = DBHelper::getConn();
    if (!conn) return emptyBook;

    sql::PreparedStatement* pstmt = nullptr;
    sql::ResultSet* res = nullptr;
    try {
        std::string sql = "SELECT * FROM book WHERE id = ?";
        pstmt = conn->prepareStatement(sql);
        pstmt->setInt(1, bid);
        res = pstmt->executeQuery();

        if (res->next()) {
            Book bk;
            bk.book_id = res->getInt("id");
            bk.book_name = res->getString("book_name");
            bk.author = res->getString("author");
            bk.price = res->getDouble("price");
            bk.stock = res->getInt("quantity");
            DBHelper::closeRes(res, pstmt, conn);
            return bk;
        }
        else {
            DBHelper::closeRes(res, pstmt, conn);
            return emptyBook;
        }
    }
    catch (sql::SQLException& e) {
        std::cerr << "查询失败:" << e.what() << std::endl;
        DBHelper::closeRes(res, pstmt, conn);
        return emptyBook;
    }
}

// 查询所有图书
std::vector<Book> BookService::findAll() {
    std::vector<Book> bookList;
    sql::Connection* conn = DBHelper::getConn();
    if (!conn) return bookList;

    sql::Statement* stmt = nullptr;
    sql::ResultSet* res = nullptr;
    try {
        stmt = conn->createStatement();
        res = stmt->executeQuery("SELECT * FROM book ORDER BY id");

        while (res->next()) {
            Book bk;
            bk.book_id = res->getInt("id");
            bk.book_name = res->getString("book_name");
            bk.author = res->getString("author");
            bk.price = res->getDouble("price");
            bk.stock = res->getInt("quantity");
            bookList.push_back(bk);
        }
        DBHelper::closeRes(res, stmt, conn);
        return bookList;
    }
    catch (sql::SQLException& e) {
        std::cerr << "全查失败:" << e.what() << std::endl;
        DBHelper::closeRes(res, stmt, conn);
        return bookList;
    }
}
//检查图书是否重复
bool BookService::existsBook(const std::string& book_name, const std::string& author, double price) {
    sql::Connection* conn = DBHelper::getConn();
    if (!conn) return false;
    sql::PreparedStatement* pstmt = nullptr;
    sql::ResultSet* res = nullptr;

    try {
        pstmt = conn->prepareStatement(
            "SELECT id FROM book WHERE book_name=? AND author=? AND price=?"
        );
        pstmt->setString(1, book_name);
        pstmt->setString(2, author);
        pstmt->setDouble(3, price);
        res = pstmt->executeQuery();
        bool exist = res->next();

        DBHelper::closeRes(res, pstmt, conn);
        return exist;
    }
    catch (...) {
        DBHelper::closeRes(res, pstmt, conn);
        return false;
    }
}

//合并库存数量
bool BookService::updateBookQuantity(const std::string& book_name, const std::string& author, double price, int add_num) {
    sql::Connection* conn = DBHelper::getConn();
    if (!conn) return false;
    sql::PreparedStatement* pstmt = nullptr;

    try {
        pstmt = conn->prepareStatement(
            "UPDATE book SET quantity=quantity+? WHERE book_name=? AND author=? AND price=?"
        );
        pstmt->setInt(1, add_num);
        pstmt->setString(2, book_name);
        pstmt->setString(3, author);
        pstmt->setDouble(4, price);
        int rows = pstmt->executeUpdate();

        DBHelper::closeRes(nullptr, pstmt, conn);
        return rows > 0;
    }
    catch (...) {
        DBHelper::closeRes(nullptr, pstmt, conn);
        return false;
    }
}