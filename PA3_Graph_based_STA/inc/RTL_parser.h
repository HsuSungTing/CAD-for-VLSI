#ifndef RTL_PARSER_H
#define RTL_PARSER_H

#include <string>
#include <vector>
#include <iomanip>
#include <cmath> // for round
#include <sstream>

using namespace std;

// 判斷是否為 // 註解開頭
bool check_two_slash(const string line);

// 判斷是否為 /* 註解開頭
bool check_slash_star(const string line);

//* 判斷是否為*/ 結尾
bool endsWithBlockCommentEnd(string line);

// 判斷是否為空行
bool is_blank_line(const string line);

// 拆解形如 mod(.A1(n1), .A2(n2)) 的外層括號
void split_by_outer_parentheses(string line, string &part_1, string &part_2);

// 移除空白和最外層括號
string remove_spaces_and_outer_parentheses(const string& input);

// 根據分隔符拆解字串
vector<string> split_by_delimiter(const string input, char delimiter);

// 拆解 .A1(n11) 成 signal 和 wire
void split_signal_and_wire(const string input, string &signal, string &wire);

// 擷取首個 token
string get_first_token(string line);

// 從字串中提取int，ex: rwerwe2131，會提取2131
int extract_int_from_string(const string input);

// 四捨五入到小數點以下第六位
double roundToSixDecimalPlaces(double value);

// 無條件捨去到小數點以下第六位
double truncateToSixDecimalPlaces(double value);

string formatToSixDecimalPlaces(double value);

//去除字串中所有空格
string removeSpaces(const string& str);

#endif // RTL_PARSER_H
