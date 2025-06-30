#include "Parser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include <cctype> // for isdigit, isspace

using namespace std;

vector<string> read_file(const string& filename) {
    vector<string> lines;
    ifstream infile(filename);
    string line;
    if (!infile) {
        cout << "can't open file: " << filename << endl;
        return lines;
    }
    while (getline(infile, line)) {
        lines.push_back(line);
    }
    infile.close();
    return lines;
}

vector<string> splitByParentheses(const string& input) {
    vector<string> result;
    string token;
    bool inParentheses = false;

    for (size_t i = 0; i < input.size(); ++i) {
        char c = input[i];
        if (c == '(') {
            if (!token.empty()) {
                result.push_back(token);
                token.clear();
            }
            inParentheses = true;
            token += c;
        } else if (c == ')') {
            token += c;
            result.push_back(token);
            token.clear();
            inParentheses = false;
        } else if (inParentheses) {
            token += c;
        } else {
            if (!isspace(c)) {
                token += c;
            } else if (!token.empty()) {
                result.push_back(token);
                token.clear();
            }
        }
    }

    if (!token.empty()) {
        result.push_back(token);
    }
    return result;
}

vector<string> splitInsideParentheses(const string& input) {
    vector<string> result;
    string inner = input.substr(1, input.size() - 2);  // remove '(' and ')'
    istringstream iss(inner);
    string token;
    while (iss >> token) {
        result.push_back(token);
    }
    return result;
}

macro_info build_macro(double width, double height, int col_multiple, int row_multiple) {
    macro_info my_macro;
    my_macro.col_multiple = col_multiple;
    my_macro.row_multiple = row_multiple;
    my_macro.height = height;
    my_macro.width = width;
    return my_macro;
}

int extract_Int(const string& str) {
    string numberStr;
    for (char c : str) {
        if (isdigit(c)) {
            numberStr += c;
        }
    }
    return numberStr.empty() ? 0 : stoi(numberStr);
}

double roundTo2DecimalPlaces(double value){
    return round(value*100.0)/100.0;
}
