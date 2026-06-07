#ifndef BORROWRECORD_H
#define BORROWRECORD_H
#include <string>

struct BorrowRecord {
    int record_id;      // 借阅记录ID
    int reader_id;      // 读者ID
    std::string reader_name; // 读者姓名
    int book_id;        // 图书ID
    std::string book_name;   // 图书名称
    std::string borrow_date; // 借阅日期
    std::string return_date; // 归还日期
};

#endif 