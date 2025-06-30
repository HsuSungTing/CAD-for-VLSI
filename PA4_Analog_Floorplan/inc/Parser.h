#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include <cmath>

using namespace std;

struct macro_info {
    double width;
    double height;
    int col_multiple;
    int row_multiple;
};

//讀取檔案內容並以行為單位回傳
vector<string> read_file(const string& filename);

//將輸入字串依照括號區塊拆分，例如 "MM4 (1.9 22.16) (2.93 11.08)" 拆成 "MM4", "(1.9 22.16)", "(2.93 11.08)"
vector<string> splitByParentheses(const string& input);

//拆分括號內的內容，例如 "(1.9 22.16 1 4)" 拆成 "1.9", "22.16", "1", "4"
vector<string> splitInsideParentheses(const string& input);

//建立macro_info結構
macro_info build_macro(double width, double height, int col_multiple, int row_multiple);

//從字串中提取數字(通常為尾端數字)，例如 "macro123" 會回傳 123
int extract_Int(const string& str);

//四捨五入到小數點第二位
double roundTo2DecimalPlaces(double value);

#endif // PARSER_H
