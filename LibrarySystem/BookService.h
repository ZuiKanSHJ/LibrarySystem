#ifndef BOOKSERVICE_H
#define BOOKSERVICE_H
#include "Book.h"
#include <vector>

class BookService {
public:
    // 新增
    bool addBook(const Book& bk);
    // 删除
    bool delBook(int bid);
    // 修改
    bool updateBook(const Book& bk);
    // 查询
    Book findById(int bid);
     //查询全部
    std::vector<Book> findAll();
    // 检查图书是否已存在
    bool existsBook(const std::string& book_name, const std::string& author, double price);
    // 合并入库：给已存在的图书增加数量
    bool updateBookQuantity(const std::string& book_name, const std::string& author, double price, int add_num);
   
};

#endif 