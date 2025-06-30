#include "RTL_parser.h"

bool check_two_slash(const string line){
    size_t i = 0;
    while (i < line.size() && isspace(line[i])) {
        ++i;
    }
    return (i+1 < line.size() && line[i] == '/' && line[i+1] == '/');
}

bool check_slash_star(const string line){
    size_t i = 0;
    while (i < line.size() && isspace(line[i])) {
        ++i;
    }
    return (i+1 < line.size() && line[i] == '/' && line[i+1] == '*');
}

bool endsWithBlockCommentEnd(string line) {
    // 先從最後一個非空格字元開始往回找
    int i = line.length() - 1;
    while (i >= 0 && isspace(line[i])) {
        i--;
    }
    if (i < 1) return false; // 需要至少兩個有效字元
    return line[i - 1] == '*' && line[i] == '/';
}

bool is_blank_line(const string line){
    for(char ch : line) {
        if(!isspace(static_cast<unsigned char>(ch))) {
            return false;
        }
    }
    return true;
}

void split_by_outer_parentheses(string line, string &part_1, string &part_2){
    size_t start = line.find('(');
    if (start == string::npos) return;
    int level = 0;
    size_t end = start;
    for (; end < line.size(); ++end) {
        if (line[end] == '(') ++level;
        else if (line[end] == ')') --level;
        if (level == 0 && line[end] == ')') {
            ++end;
            break;
        }
    }
    part_1 = line.substr(0, start);
    part_2 = line.substr(start, end - start);
}

string remove_spaces_and_outer_parentheses(const string& input) {
    string no_space;
    for (char c : input) {
        if (!isspace(static_cast<unsigned char>(c))) {
            no_space += c;
        }
    }
    size_t start = no_space.find('(');
    if (start == string::npos) return no_space;
    int balance = 0;
    size_t end = string::npos;
    for (size_t i = start; i < no_space.size(); ++i) {
        if (no_space[i] == '(') ++balance;
        else if (no_space[i] == ')') {
            --balance;
            if (balance == 0) {
                end = i;
                break;
            }
        }
    }
    if (end != string::npos && end > start) {
        return no_space.substr(start + 1, end - start - 1);
    } else {
        return "";
    }
}

vector<string> split_by_delimiter(const string input, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream iss(input);
    if (delimiter == ' ') {
        while (iss >> token){
            tokens.push_back(token);
        }
    } else {
        while(getline(iss, token, delimiter)){
            if (!token.empty()){
                tokens.push_back(token);
            }
        }
    }
    return tokens;
}

void split_signal_and_wire(const string input, string &signal, string &wire){
    size_t start = input.find('(');
    size_t end = input.find(')');
    if (start == string::npos || end == string::npos || start < 1 || end <= start + 1) {
        return;
    }
    signal = input.substr(1, start - 1);
    wire = input.substr(start + 1, end - start - 1);
}

string get_first_token(string line){
    size_t i = 0;
    while (i < line.size() && isspace(line[i])){
        ++i;
    }
    size_t start = i;
    while (i < line.size() && !isspace(line[i])){
        ++i;
    }
    return line.substr(start, i - start);
}

int extract_int_from_string(const string input) {
    int result = 0;
    bool found_number = false;

    for (char ch : input) {
        if (isdigit(ch)) {
            result = result * 10 + (ch - '0');
            found_number = true;
        }
        else if (found_number) {
            break; // 假如已經找到數字，遇到非數字字符就停止
        }
    }

    return found_number ? result : -1; // 若有找到數字則回傳結果，否則回傳 -1
}

double roundToSixDecimalPlaces(double value){
    return round(value * 1e6) / 1e6;
}

double truncateToSixDecimalPlaces(double value){
    return static_cast<long long>(value * 1e6) / 1e6;
}

string formatToSixDecimalPlaces(double value) {
    ostringstream oss;
    oss << fixed << setprecision(6) << value;
    return oss.str();
}

string removeSpaces(const string& str) {
    string result;
    for (char ch : str) {
        if (ch != ' ') {
            result += ch;
        }
    }
    return result;
}