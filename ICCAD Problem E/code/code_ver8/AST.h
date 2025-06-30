#ifndef AST_H
#define AST_H

#include <string>
#include <list>
#include <vector>

using namespace std;

struct AST_node {
    string Type;        // "SIGNAL", "NUM", "OP"
    string Name_or_Op;
    vector<AST_node*> child;
};

struct AST {
    AST_node* root;
    list<AST_node> lst;
};

// === Auxilary Functions ===
bool parseTernary(const string expr, string& condition, string& true_expr, string& false_expr);
bool hasValidOuterParentheses(const string& expr);
string stripOuterParentheses(const string& expr);
bool start_with_not(const string& str);
vector<string> split_string_excluding_ranges(const string& expr, vector<pair<int, int>> skipRanges);
void sort_children_by_name(AST_node* node);

// === Parser Functions ===
bool splitByRange(const string& expr, int start, int end, string& left, string& middle, string& right);
vector<pair<int, int>> find_all_depth_zero_op(const string& expr);
AST_node* Build_AST(string input, list<AST_node>& lst);

// === AST Operations ===
void print_AST(AST_node* node, int indent = 0);
string canonical_string(AST_node* node);
void normalize_AST(AST_node* node);
bool isSameAST(AST_node* a, AST_node* b);

#endif