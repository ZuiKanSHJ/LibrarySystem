#ifndef BOOK_H
#define BOOK_H
#include <string>

// 图书类
struct Book {
    int book_id;          // 图书ID
    std::string book_name;// 书名
    std::string author;   // 作者
    double price;         // 价格
    int stock;            // 库存
};

#endif 