#ifndef AST_H
#define AST_H

#include <iostream>
#include <string>
#include <utility>   
#include <unordered_map>
#include <list>
#include <algorithm>
#include <vector>
#include <limits>

using namespace std;

struct AST_node {
    string Type;        //可以是SIGNAL, NUM, OP
    string Name_or_Op;
    AST_node* cond;     //只有MUX和NOT才會用到
    AST_node* left;
    AST_node* right;

    AST_node() : cond(nullptr), left(nullptr), right(nullptr) {}
};

struct AST {
    AST_node* root;
    list<AST_node> lst;
};

// === AST 建構與輔助功能 ===
AST_node* Build_AST(string input, list<AST_node>& lst);
string normalize_AST(AST_node* node);
bool isSameAST(AST_node* a, AST_node* b);
void print_AST(AST_node* node, int indent = 0);

// === 字串處理輔助函式 ===
bool parseTernary(const string expr, string& condition, string& true_expr, string& false_expr);
bool hasOuterParentheses(const string expr);
string stripOuterParentheses(const string& expr);
bool containsAnyOperator(const string& expr);
bool start_with_not(const string& str);
bool splitByRange(const string& expr, int start, int end, string& left, string& middle, string& right);
pair<int, int> find_depth_zero_op(const string& expr);

#endif // AST_H
