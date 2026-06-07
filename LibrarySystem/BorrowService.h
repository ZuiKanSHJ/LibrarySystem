#ifndef BORROWSERVICE_H
#define BORROWSERVICE_H
#include "BorrowRecord.h"
#include <vector>

class BorrowService {
public:
    // 借阅
    bool borrowBook(int reader_id, int book_id);

    // 归还
    bool returnBook(int borrow_id);

    // 按读者ID查询借阅记录
    std::vector<BorrowRecord> findRecordsByReaderId(int reader_id);

    // 按图书ID查询借阅记录
    std::vector<BorrowRecord> findRecordsByBookId(int book_id);

    // 查询所有未归还记录
    std::vector<BorrowRecord> findAllUnreturned();

    // 查询所有借阅记录
    std::vector<BorrowRecord> findAllRecords();

    // 按记录ID查询单条记录
    BorrowRecord findRecordById(int borrow_id);
};

#endif