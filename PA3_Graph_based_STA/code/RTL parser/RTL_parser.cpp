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

bool endsWithBlockCommentEnd(string line){
    if (line.length() < 2) return false;
    return line[line.length() - 2] == '*' && line[line.length() - 1] == '/';
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

int extract_int_from_string(const string input){
    regex number_regex("\\d+"); // \d+ 表示一串數字
    smatch match;

    if(regex_search(input, match, number_regex)){
        return stoi(match.str()); // 將擷取到的數字字串轉為 int
    } 
    else{
        return -1; // 若無數字，回傳 -1 或其他你希望的錯誤值
    }
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