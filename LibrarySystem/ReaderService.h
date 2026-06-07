#ifndef READERSERVICE_H
#define READERSERVICE_H

#include "Reader.h"
#include <vector>

class ReaderService {
public:
    //添加读者
    bool addReader(const Reader& reader);
    //删除读者（按ID）
    bool deleteReader(int readerId);
    //修改读者信息
    bool updateReader(const Reader& reader);
    //按ID查询读者
    Reader findReaderById(int readerId);
    //查询所有读者
    std::vector<Reader> findAllReaders();
};

#endif 
