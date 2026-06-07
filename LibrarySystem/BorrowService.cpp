#include "BorrowService.h"
#include "DBHelper.h"
#include <iostream>
#include <stdexcept>
#include <vector>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h> 


using namespace sql;
using namespace std;

bool BorrowService::borrowBook(int reader_id, int book_id) {
    Connection* conn = DBHelper::getConn();
    if (!conn) return false;

    conn->setAutoCommit(false);
    PreparedStatement* pstmt = nullptr;
    ResultSet* res = nullptr;

    try {
        // 检查读者
        pstmt = conn->prepareStatement("SELECT COUNT(*) FROM reader WHERE reader_id=?");
        pstmt->setInt(1, reader_id);
        res = pstmt->executeQuery();
        res->next();
        if (res->getInt(1) == 0) throw runtime_error("读者不存在");
        DBHelper::closeRes(res, pstmt, nullptr);

        // 检查库存
        pstmt = conn->prepareStatement("SELECT quantity FROM book WHERE id=?");
        pstmt->setInt(1, book_id);
        res = pstmt->executeQuery();
        if (!res->next() || res->getInt(1) <= 0) throw runtime_error("库存不足");
        DBHelper::closeRes(res, pstmt, nullptr);

        // 检查是否已借出
        pstmt = conn->prepareStatement("SELECT COUNT(*) FROM borrow WHERE book_id=? AND return_date IS NULL");
        pstmt->setInt(1, book_id);
        res = pstmt->executeQuery();
        res->next();
        if (res->getInt(1) > 0) throw runtime_error("图书已借出");
        DBHelper::closeRes(res, pstmt, nullptr);

        // 插入借阅记录
        pstmt = conn->prepareStatement(
            "INSERT INTO borrow(reader_id,book_id,borrow_date,due_date) VALUES(?,?,CURDATE(),CURDATE()+INTERVAL 30 DAY)"
        );
        pstmt->setInt(1, reader_id);
        pstmt->setInt(2, book_id);
        pstmt->executeUpdate();
        DBHelper::closeRes(nullptr, pstmt, nullptr);

        // 扣库存
        pstmt = conn->prepareStatement("UPDATE book SET quantity=quantity-1 WHERE id=?");
        pstmt->setInt(1, book_id);
        pstmt->executeUpdate();

        conn->commit();
        DBHelper::closeRes(nullptr, pstmt, conn);
        return true;
    }
    catch (SQLException& e) { 
        conn->rollback();
        cerr << "数据库错误：" << e.what() << endl;
        DBHelper::closeRes(res, pstmt, conn);
        return false;
    }
    catch (runtime_error& e) {
        conn->rollback();
        cerr << "业务错误：" << e.what() << endl;
        DBHelper::closeRes(res, pstmt, conn);
        return false;
    }
}

bool BorrowService::returnBook(int borrow_id) {
    Connection* conn = DBHelper::getConn();
    if (!conn) return false;

    conn->setAutoCommit(false);
    PreparedStatement* pstmt = nullptr;
    ResultSet* res = nullptr;

    try {
        pstmt = conn->prepareStatement("SELECT book_id FROM borrow WHERE borrow_id=? AND return_date IS NULL");
        pstmt->setInt(1, borrow_id);
        res = pstmt->executeQuery();
        if (!res->next()) throw runtime_error("记录不存在/已归还");
        int book_id = res->getInt(1);
        DBHelper::closeRes(res, pstmt, nullptr);

        pstmt = conn->prepareStatement("UPDATE borrow SET return_date=CURDATE() WHERE borrow_id=?");
        pstmt->setInt(1, borrow_id);
        pstmt->executeUpdate();
        DBHelper::closeRes(nullptr, pstmt, nullptr);

        pstmt = conn->prepareStatement("UPDATE book SET quantity=quantity+1 WHERE id=?");
        pstmt->setInt(1, book_id);
        pstmt->executeUpdate();

        conn->commit();
        DBHelper::closeRes(nullptr, pstmt, conn);
        return true;
    }
    catch (SQLException& e) {
        conn->rollback();
        cerr << "数据库错误：" << e.what() << endl;
        DBHelper::closeRes(res, pstmt, conn);
        return false;
    }
    catch (runtime_error& e) {
        conn->rollback();
        cerr << "业务错误：" << e.what() << endl;
        DBHelper::closeRes(res, pstmt, conn);
        return false;
    }
}

vector<BorrowRecord> BorrowService::findRecordsByReaderId(int reader_id) {
    vector<BorrowRecord> records;
    Connection* conn = DBHelper::getConn();
    if (!conn) return records;

    PreparedStatement* pstmt = nullptr;
    ResultSet* res = nullptr;

    try {
        string sql = R"(
            SELECT br.*, r.reader_name, b.book_name
            FROM borrow br
            JOIN reader r ON br.reader_id=r.reader_id
            JOIN book b ON br.book_id=b.id
            WHERE br.reader_id=?
        )";
        pstmt = conn->prepareStatement(sql);
        pstmt->setInt(1, reader_id);
        res = pstmt->executeQuery();

        while (res->next()) {
            BorrowRecord r{};
            r.record_id = res->getInt("borrow_id");
            r.reader_id = res->getInt("reader_id");
            r.reader_name = res->getString("reader_name");
            r.book_id = res->getInt("book_id");
            r.book_name = res->getString("book_name");
            r.borrow_date = res->getString("borrow_date");
            r.return_date = res->isNull("return_date") ? "未归还" : res->getString("return_date");
            records.push_back(r);
        }
    }
    catch (SQLException& e) {
        cerr << "查询错误：" << e.what() << endl;
    }
    DBHelper::closeRes(res, pstmt, conn);
    return records;
}

vector<BorrowRecord> BorrowService::findRecordsByBookId(int book_id) {
    vector<BorrowRecord> records;
    Connection* conn = DBHelper::getConn();
    if (!conn) return records;

    PreparedStatement* pstmt = nullptr;
    ResultSet* res = nullptr;

    try {
        string sql = R"(
            SELECT br.*, r.reader_name, b.book_name
            FROM borrow br
            JOIN reader r ON br.reader_id=r.reader_id
            JOIN book b ON br.book_id=b.id
            WHERE br.book_id=?
        )";
        pstmt = conn->prepareStatement(sql);
        pstmt->setInt(1, book_id);
        res = pstmt->executeQuery();
        res = pstmt->executeQuery();

        while (res->next()) {
            BorrowRecord r{};
            r.record_id = res->getInt("borrow_id");
            r.reader_id = res->getInt("reader_id");
            r.reader_name = res->getString("reader_name");
            r.book_id = res->getInt("book_id");
            r.book_name = res->getString("book_name");
            r.borrow_date = res->getString("borrow_date");
            r.return_date = res->isNull("return_date") ? "未归还" : res->getString("return_date");
            records.push_back(r);
        }
    }
    catch (SQLException& e) {
        cerr << "查询错误：" << e.what() << endl;
    }
    DBHelper::closeRes(res, pstmt, conn);
    return records;
}

vector<BorrowRecord> BorrowService::findAllUnreturned() {
    vector<BorrowRecord> records;
    Connection* conn = DBHelper::getConn();
    if (!conn) return records;

    Statement* stmt = nullptr;
    ResultSet* res = nullptr;

    try {
        string sql = R"(
            SELECT br.*, r.reader_name, b.book_name
            FROM borrow br
            JOIN reader r ON br.reader_id=r.reader_id
            JOIN book b ON br.book_id=b.id
            WHERE br.return_date IS NULL
        )";
        stmt = conn->createStatement();
        res = stmt->executeQuery(sql);

        while (res->next()) {
            BorrowRecord r{};
            r.record_id = res->getInt("borrow_id");
            r.reader_id = res->getInt("reader_id");
            r.reader_name = res->getString("reader_name");
            r.book_id = res->getInt("book_id");
            r.book_name = res->getString("book_name");
            r.borrow_date = res->getString("borrow_date");
            r.return_date = "未归还";
            records.push_back(r);
        }
    }
    catch (SQLException& e) {
        cerr << "查询错误：" << e.what() << endl;
    }
    DBHelper::closeRes(res, stmt, conn);
    return records;
}

vector<BorrowRecord> BorrowService::findAllRecords() {
    vector<BorrowRecord> records;
    Connection* conn = DBHelper::getConn();
    if (!conn) return records;

    Statement* stmt = nullptr;
    ResultSet* res = nullptr;

    try {
        string sql = R"(
            SELECT br.*, r.reader_name, b.book_name
            FROM borrow br
            JOIN reader r ON br.reader_id=r.reader_id
            JOIN book b ON br.book_id=b.id
        )";
        stmt = conn->createStatement();
        res = stmt->executeQuery(sql);

        while (res->next()) {
            BorrowRecord r{};
            r.record_id = res->getInt("borrow_id");
            r.reader_id = res->getInt("reader_id");
            r.reader_name = res->getString("reader_name");
            r.book_id = res->getInt("book_id");
            r.book_name = res->getString("book_name");
            r.borrow_date = res->getString("borrow_date");
            r.return_date = res->isNull("return_date") ? "未归还" : res->getString("return_date");
            records.push_back(r);
        }
    }
    catch (SQLException& e) {
        cerr << "查询错误：" << e.what() << endl;
    }
    DBHelper::closeRes(res, stmt, conn);
    return records;
}

BorrowRecord BorrowService::findRecordById(int borrow_id) {
    BorrowRecord record{};
    Connection* conn = DBHelper::getConn();
    if (!conn) return record;

    PreparedStatement* pstmt = nullptr;
    ResultSet* res = nullptr;

    try {
        string sql = R"(
            SELECT br.*, r.reader_name, b.book_name
            FROM borrow br
            JOIN reader r ON br.reader_id=r.reader_id
            JOIN book b ON br.book_id=b.id
            WHERE br.borrow_id=?
        )";
        pstmt = conn->prepareStatement(sql);
        pstmt->setInt(1, borrow_id);
        res = pstmt->executeQuery();

        if (res->next()) {
            record.record_id = res->getInt("borrow_id");
            record.reader_id = res->getInt("reader_id");
            record.reader_name = res->getString("reader_name");
            record.book_id = res->getInt("book_id");
            record.book_name = res->getString("book_name");
            record.borrow_date = res->getString("borrow_date");
            record.return_date = res->isNull("return_date") ? "未归还" : res->getString("return_date");
        }
    }
    catch (SQLException& e) {
        cerr << "查询错误：" << e.what() << endl;
    }
    DBHelper::closeRes(res, pstmt, conn);
    return record;
}