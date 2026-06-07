#include <iostream>
#include <windows.h>
#include <iomanip>  
#include <vector>
#include "BookService.h"
#include "Book.h"
#include "ReaderService.h"
#include "Reader.h"
#include "BorrowService.h"
#include "BorrowRecord.h"
#include <sstream>
#include <string>
#include <cstdio>
//UTF-8字符显示宽度计算
int getDisplayWidth(const std::string& str) {
    int width = 0;
    size_t i = 0;
    while (i < str.size()) {
        unsigned char c = static_cast<unsigned char>(str[i]);
        if (c < 0x80) {
            width += 1;
            i++;
        }
        else if (c >= 0xC0 && c < 0xE0) {
            width += 2;
            i += 2;
        }
        else if (c >= 0xE0 && c < 0xF0) {
            width += 2;
            i += 3;
        }
        else if (c >= 0xF0 && c < 0xF8) {
            width += 2;
            i += 4;
        }
        else {
           width += 1;
            i++;
        }
    }
    return width;
}

// 填充空格函数
std::string padToWidth(const std::string& str, int targetWidth, bool leftAlign = true) {
    int currentWidth = getDisplayWidth(str);
    if (currentWidth >= targetWidth) {
        
        return str.substr(0, str.size() - (currentWidth - targetWidth));
    }
    int pad = targetWidth - currentWidth;
    if (leftAlign) {
        return str + std::string(pad, ' ');
    }
    else {
        return std::string(pad, ' ') + str;
    }
}

// 按 '-' 分割字符串（用于批量入库）
std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        if (!token.empty()) tokens.push_back(token);
    }
    return tokens;
}

// 判断价格位置字符串是否为小数
bool isDouble(const std::string& s) {
    try { std::stod(s); return true; }
    catch (...) { return false; }
}

// 判断库存位置字符串是否为整数
bool isInt(const std::string& s) {
    try { std::stoi(s); return true; }
    catch (...) { return false; }
}


// 输出借阅记录列表
void showBorrowRecords(const std::vector<BorrowRecord>& records) {
    std::cout << "\n📋 借阅记录列表（共" << records.size() << "条）📋" << std::endl;

    
    const int COL1_START = 1;    // 记录ID
    const int COL2_START = 10;   // 读者ID
    const int COL3_START = 20;   // 读者姓名
    const int COL4_START = 35;   // 图书ID
    const int COL5_START = 45;   // 图书名称
    const int COL6_START = 70;   // 借阅日期
    const int COL7_START = 85;   // 归还日期

   
    const int COL1_WIDTH = COL2_START - COL1_START;
    const int COL2_WIDTH = COL3_START - COL2_START;
    const int COL3_WIDTH = COL4_START - COL3_START;
    const int COL4_WIDTH = COL5_START - COL4_START;
    const int COL5_WIDTH = COL6_START - COL5_START;
    const int COL6_WIDTH = COL7_START - COL6_START;
    const int COL7_WIDTH = 15;
    std::string line(COL7_START + COL7_WIDTH - 1, '=');
    std::cout << line << std::endl;

    //左对齐
    std::cout << padToWidth("记录ID", COL1_WIDTH)
        << padToWidth("读者ID", COL2_WIDTH)
        << padToWidth("读者姓名", COL3_WIDTH)
        << padToWidth("图书ID", COL4_WIDTH)
        << padToWidth("图书名称", COL5_WIDTH)
        << padToWidth("借阅日期", COL6_WIDTH)
        << padToWidth("归还日期", COL7_WIDTH)
        << std::endl;
    std::string divider(COL7_START + COL7_WIDTH - 1, '-');
    std::cout << divider << std::endl;
    for (const auto& record : records) {
        std::string rid = std::to_string(record.record_id);
        std::string uid = std::to_string(record.reader_id);
        std::string bid = std::to_string(record.book_id);

        std::cout << padToWidth(rid, COL1_WIDTH)
            << padToWidth(uid, COL2_WIDTH)
            << padToWidth(record.reader_name, COL3_WIDTH)
            << padToWidth(bid, COL4_WIDTH)
            << padToWidth(record.book_name, COL5_WIDTH)
            << padToWidth(record.borrow_date, COL6_WIDTH)
            << padToWidth(record.return_date, COL7_WIDTH)
            << std::endl;
    }

    std::cout << line << std::endl;
}

//借阅管理菜单
void borrowMenu() {
    BorrowService borrowService;
    int choice = -1;
    while (choice != 0) {
        std::cout << "\n===== 借阅管理 =====" << std::endl;
        std::cout << "1. 图书借阅" << std::endl;
        std::cout << "2. 图书归还" << std::endl;
        std::cout << "3. 按读者ID查询借阅记录" << std::endl;
        std::cout << "4. 按图书ID查询借阅记录" << std::endl;
        std::cout << "5. 查看所有未归还记录" << std::endl;
        std::cout << "6. 查看所有借阅记录" << std::endl;
        std::cout << "0. 返回主菜单" << std::endl;
        std::cout << "请选择：";
        std::cin >> choice;
        std::cin.ignore();

        switch (choice) {
        case 1: {
            //借阅
            int reader_id, book_id;
            std::cout << "请输入读者ID: ";
            std::cin >> reader_id;
            std::cout << "请输入图书ID: ";
            std::cin >> book_id;
            std::cin.ignore();

            if (borrowService.borrowBook(reader_id, book_id))
                std::cout << "✅ 借阅成功！" << std::endl;
            else
                std::cout << "❌ 借阅失败！" << std::endl;
            break;
        }
        case 2: {
            //归还
            int borrow_id;
            std::cout << "请输入要归还的借阅记录ID: ";
            std::cin >> borrow_id;

            BorrowRecord record = borrowService.findRecordById(borrow_id);
            if (record.record_id == 0) {
                std::cout << "❌ 未找到该借阅记录！" << std::endl;
                break;
            }

            if (record.return_date != "未归还") {
                std::cout << "❌ 该图书已经归还！" << std::endl;
                break;
            }

            if (borrowService.returnBook(borrow_id)) {
                std::cout << "✅ 归还成功！" << std::endl;
            }
            else {
                std::cout << "❌ 归还失败！" << std::endl;
            }
            break;
        }
        case 3: {
            //按读者ID查询
            int reader_id;
            std::cout << "请输入读者ID: ";
            std::cin >> reader_id;
            std::vector<BorrowRecord> records = borrowService.findRecordsByReaderId(reader_id);
            showBorrowRecords(records);
            break;
        }
        case 4: {
            //按图书ID查询
            int book_id;
            std::cout << "请输入图书ID: ";
            std::cin >> book_id;
            std::vector<BorrowRecord> records = borrowService.findRecordsByBookId(book_id);
            showBorrowRecords(records);
            break;
        }
        case 5: {
            //查看所有未归还记录
            std::vector<BorrowRecord> records = borrowService.findAllUnreturned();
            showBorrowRecords(records);
            break;
        }
        case 6: {
            //查看所有借阅记录
            std::vector<BorrowRecord> records = borrowService.findAllRecords();
            showBorrowRecords(records);
            break;
        }
        case 0:
            std::cout << "返回主菜单..." << std::endl;
            break;
        default:
            std::cout << "❌ 无效选项！请输入0-6之间的数字。" << std::endl;
        }
    }
}
//输出图书列表
void showBookList(const std::vector<Book>& list) {
    std::cout << "\n📚 图书列表（共 " << list.size() << " 本）" << std::endl;
    const int COL1_START = 1;    
    const int COL2_START = 10;   
    const int COL3_START = 50;   
    const int COL4_START = 80;   
    const int COL5_START = 95;   
    const int COL1_WIDTH = COL2_START - COL1_START;
    const int COL2_WIDTH = COL3_START - COL2_START;
    const int COL3_WIDTH = COL4_START - COL3_START;
    const int COL4_WIDTH = COL5_START - COL4_START;
    const int COL5_WIDTH = 8;

    std::string line(COL5_START + COL5_WIDTH - 1, '=');
    std::cout << line << std::endl;
    std::cout << std::left;
    std::cout << std::setw(COL1_WIDTH) << "图书ID"
        << std::setw(COL2_WIDTH) << "书名"
        << std::setw(COL3_WIDTH) << "作者"
        << std::setw(COL4_WIDTH) << "价格(元)"
        << std::setw(COL5_WIDTH) << "库存"
        << std::endl;

    std::string divider(COL5_START + COL5_WIDTH - 1, '-');
    std::cout << divider << std::endl;
    std::cout << std::fixed << std::setprecision(2);
    for (const auto& b : list) {
        std::string idStr = std::to_string(b.book_id);
        std::cout << padToWidth(idStr, COL1_WIDTH);
        std::cout << padToWidth(b.book_name, COL2_WIDTH);
        std::cout << padToWidth(b.author, COL3_WIDTH);
        std::string priceStr = std::to_string(b.price);
        std::cout << padToWidth(priceStr, COL4_WIDTH, true);
        std::string stockStr = std::to_string(b.stock);
        std::cout << padToWidth(stockStr, COL5_WIDTH) << std::endl;
    }

    std::cout << line << std::endl;
}

//输出读者列表
void showReaderList(const std::vector<Reader>& readers) {
    std::cout << "\n👤 读者列表（共" << readers.size() << "人）👤" << std::endl;
    const int COL1_START = 1;   
    const int COL2_START = 10;   
    const int COL3_START = 30;   
    const int COL1_WIDTH = COL2_START - COL1_START;
    const int COL2_WIDTH = COL3_START - COL2_START;
    const int COL3_WIDTH = 15;
    std::string line(COL3_START + COL3_WIDTH - 1, '=');
    std::cout << line << std::endl;
    std::cout << padToWidth("读者ID", COL1_WIDTH)
        << padToWidth("姓名", COL2_WIDTH)
        << padToWidth("电话", COL3_WIDTH)
        << std::endl;

    std::string divider(COL3_START + COL3_WIDTH - 1, '-');
    std::cout << divider << std::endl;

    for (const auto& r : readers) {
        std::string idStr = std::to_string(r.reader_id);
        std::cout << padToWidth(idStr, COL1_WIDTH)
            << padToWidth(r.name, COL2_WIDTH)
            << padToWidth(r.phone, COL3_WIDTH)
            << std::endl;
    }

    std::cout << line << std::endl;
}

// 图书管理菜单
void bookMenu() {
    BookService bs;
    int choice = -1;
    while (choice != 0) {
        std::cout << "\n===== 图书管理 =====" << std::endl;
        std::cout << "1. 新增图书" << std::endl;
        std::cout << "2. 删除图书" << std::endl;
        std::cout << "3. 修改图书" << std::endl;
        std::cout << "4. 查询单本" << std::endl;
        std::cout << "5. 查询全部" << std::endl;
        std::cout << "6. 批量入库" << std::endl;
        std::cout << "0. 返回主菜单" << std::endl;
        std::cout << "请选择：";
        std::cin >> choice;
        std::cin.ignore(); 

        switch (choice) {
        case 1: {
            Book bk;
            std::cout << "请输入书名: ";
            std::getline(std::cin, bk.book_name);
            std::cout << "请输入作者: ";
            std::getline(std::cin, bk.author);
            std::cout << "请输入价格: ";
            std::cin >> bk.price;
            std::cout << "请输入库存: ";
            std::cin >> bk.stock;

            if (bs.addBook(bk))
                std::cout << "✅ 新增成功！" << std::endl;
            else
                std::cout << "❌ 新增失败！" << std::endl;
            break;
        }
        case 2: {
            int id;
            std::cout << "请输入要删除的图书ID: ";
            std::cin >> id;
            if (bs.delBook(id))
                std::cout << "✅ 删除成功！" << std::endl;
            else
                std::cout << "❌ 删除失败！" << std::endl;
            break;
        }
        case 3: {
            Book bk;
            std::cout << "请输入要修改的图书ID: ";
            std::cin >> bk.book_id;
            std::cin.ignore();

            std::cout << "请输入新书名: ";
            std::getline(std::cin, bk.book_name);
            std::cout << "请输入新作者: ";
            std::getline(std::cin, bk.author);
            std::cout << "请输入新价格: ";
            std::cin >> bk.price;
            std::cout << "请输入新库存: ";
            std::cin >> bk.stock;

            if (bs.updateBook(bk))
                std::cout << "✅ 修改成功！" << std::endl;
            else
                std::cout << "❌ 修改失败！" << std::endl;
            break;
        }
        case 4: {
            int id;
            std::cout << "请输入要查询的图书ID: ";
            std::cin >> id;
            Book bk = bs.findById(id);

            if (bk.book_id != 0) { 
                std::cout << "\n===== 图书详情 =====" << std::endl;
                std::cout << "ID: " << bk.book_id << std::endl;
                std::cout << "书名: " << bk.book_name << std::endl;
                std::cout << "作者: " << bk.author << std::endl;
                std::cout << "价格: " << bk.price << std::endl;
                std::cout << "库存: " << bk.stock << std::endl;
                std::cout << "====================\n" << std::endl;
            }
            else {
                std::cout << "❌ 未找到ID为 " << id << " 的图书！" << std::endl;
            }
            break;
        }
        case 5: {
            showBookList(bs.findAll());
            break;
        }
        case 6: {
            std::cout << "\n===== 图书批量入库（格式录入） =====" << std::endl;
            std::cout << "【正确格式】书名-作者-价格-数量-书名-作者-价格-数量-..." << std::endl;
            std::cout << "【规则】格式错误自动停止；重复图书(书名+作者+价格)自动合并数量" << std::endl;
            std::cout << "请输入批量图书信息：";
            std::string input;
            std::getline(std::cin, input);

            //批量录入格式校验
            std::vector<std::string> parts = split(input, '-');
            //书名-作者-价格-数量
            if (parts.size() % 4 != 0) {
                std::cout << "\n❌ 格式错误！请严格按照：书名-作者-价格-数量- 格式输入！" << std::endl;
                break;
            }
            int total = 0, add_count = 0, merge_count = 0;
            for (int i = 0; i < parts.size(); i += 4) {
                std::string name = parts[i];
                std::string author = parts[i + 1];
                std::string price_str = parts[i + 2];
                std::string num_str = parts[i + 3];

                // 校验价格/数量必须是数字
                if (!isDouble(price_str) || !isInt(num_str)) {
                    std::cout << "\n❌ 格式错误：价格/数量必须为数字！停止录入！" << std::endl;
                    return;
                }

                double price = std::stod(price_str);
                int num = std::stoi(num_str);
                if (num <= 0) {
                    std::cout << "\n❌ 错误：数量必须大于0！停止录入！" << std::endl;
                    return;
                }

                
                if (bs.existsBook(name, author, price)) {
                    if (bs.updateBookQuantity(name, author, price, num)) {
                        merge_count++;
                        total++;
                        std::cout << "✅ 合并：" << name << "  +" << num << std::endl;
                    }
                }
                else {
                    Book bk;
                    bk.book_name = name;
                    bk.author = author;
                    bk.price = price;
                    bk.stock = num;
                    if (bs.addBook(bk)) {
                        add_count++;
                        total++;
                        std::cout << "✅ 新增：" << name << std::endl;
                    }
                }
            }

            //输出批量录入结果
            std::cout << "\n=====================================" << std::endl;
            std::cout << "📦 批量入库完成！" << std::endl;
            std::cout << "总处理：" << total << " 本" << std::endl;
            std::cout << "新增图书：" << add_count << " 本" << std::endl;
            std::cout << "合并库存：" << merge_count << " 本" << std::endl;
            break;
        }
        
        case 0:
            std::cout << "返回主菜单..." << std::endl;
            break;
        default:
            std::cout << "❌ 无效选项！请输入0-5之间的数字。" << std::endl;
        }
    }
}

// 读者菜单
void readerMenu() {
    ReaderService rs;
    int choice = -1;
    while (choice != 0) {
        std::cout << "\n===== 读者管理 =====" << std::endl;
        std::cout << "1. 新增读者" << std::endl;
        std::cout << "2. 删除读者" << std::endl;
        std::cout << "3. 修改读者" << std::endl;
        std::cout << "4. 查询读者" << std::endl;
        std::cout << "5. 查询全部" << std::endl;
        std::cout << "0. 返回主菜单" << std::endl;
        std::cout << "请选择：";
        std::cin >> choice;
        std::cin.ignore();

        switch (choice) {
        case 1: {
            Reader r;
            std::cout << "请输入读者姓名: ";
            std::getline(std::cin, r.name);
            std::cout << "请输入联系电话: ";
            std::getline(std::cin, r.phone);

            if (rs.addReader(r))
                std::cout << "✅ 新增成功！" << std::endl;
            else
                std::cout << "❌ 新增失败！" << std::endl;
            break;
        }
        case 2: {
            int id;
            std::cout << "请输入要删除的读者ID: ";
            std::cin >> id;
            if (rs.deleteReader(id))
                std::cout << "✅ 删除成功！" << std::endl;
            else
                std::cout << "❌ 删除失败！" << std::endl;
            break;
        }
        case 3: {
            Reader r;
            std::cout << "请输入要修改的读者ID: ";
            std::cin >> r.reader_id;
            std::cin.ignore();

            std::cout << "请输入新姓名: ";
            std::getline(std::cin, r.name);
            std::cout << "请输入新电话: ";
            std::getline(std::cin, r.phone);

            if (rs.updateReader(r))
                std::cout << "✅ 修改成功！" << std::endl;
            else
                std::cout << "❌ 修改失败！" << std::endl;
            break;
        }
        case 4: {
            int id;
            std::cout << "请输入要查询的读者ID: ";
            std::cin >> id;
            Reader r = rs.findReaderById(id);

            if (r.reader_id != 0) {
                std::cout << "\n===== 读者详情 =====" << std::endl;
                std::cout << "ID: " << r.reader_id << std::endl;
                std::cout << "姓名: " << r.name << std::endl;
                std::cout << "电话: " << r.phone << std::endl;
                std::cout << "====================\n" << std::endl;
            }
            else {
                std::cout << "❌ 未找到ID为 " << id << " 的读者！" << std::endl;
            }
            break;
        }
        case 5: {
            showReaderList(rs.findAllReaders());
            break;
        }
        case 0:
            std::cout << "返回主菜单..." << std::endl;
            break;
        default:
            std::cout << "❌ 无效选项！请输入0-5之间的数字。" << std::endl;
        }
    }
}
// 主菜单
void showMainMenu() {
    std::cout << "\n===== 图书管理系统 =====" << std::endl;
    std::cout << "1. 图书管理" << std::endl;
    std::cout << "2. 读者管理" << std::endl;
    std::cout << "3. 借阅管理" << std::endl;  
    std::cout << "0. 退出系统" << std::endl;
    std::cout << "========================\n";
    std::cout << "请选择：";
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    int choice;
    while (true) {
        showMainMenu();
        std::cin >> choice;
        if (choice == 1) {
            bookMenu();
        }
        else if (choice == 2) {
            readerMenu();
        }
        else if (choice == 3) { 
            borrowMenu();
        }
        else if (choice == 0) {
            std::cout << "👋 感谢使用，再见！" << std::endl;
            break;
        }
        else {
            std::cout << "❌ 无效选项，请重新输入！" << std::endl;
        }
    }
    return 0;
}