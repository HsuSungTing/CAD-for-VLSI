#include "AST.h"
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <limits>
#include <set>

using namespace std;

// === Auxilary Functions Implementation ===
bool parseTernary(const string expr, string& condition, string& true_expr, string& false_expr){
    int depth = 0, q_index = -1, colon_index = -1;
    for (int i = 0; i < expr.length(); i++) {
        char c = expr[i];
        if (c == '(') depth++;
        else if (c == ')') depth--;
        else if (c == '?' && depth == 0 && q_index == -1) q_index = i;
        else if (c == ':' && depth == 0 && q_index != -1) {
            colon_index = i;
            break;
        }
    }
    if (q_index != -1 && colon_index != -1) {
        condition = expr.substr(0, q_index);
        true_expr = expr.substr(q_index + 1, colon_index - q_index - 1);
        false_expr = expr.substr(colon_index + 1);
        return true;
    }
    return false;
}

bool hasValidOuterParentheses(const string& expr){
    if (expr.empty() || expr.front() != '(' || expr.back() != ')') return false;
    int depth = 0;
    for (size_t i = 0; i < expr.length(); ++i) {
        if (expr[i] == '(') depth++;
        else if (expr[i] == ')') depth--;
        if (depth < 0) return false;
        if (depth == 0 && i != expr.length() - 1) return false;
    }
    return depth == 0;
}

string stripOuterParentheses(const string& expr){
    return expr.substr(1, expr.length() - 2);
}

bool start_with_not(const string& str){
    return !str.empty() && str[0] == '~';
}

vector<string> split_string_excluding_ranges(const string& expr, vector<pair<int, int>> skipRanges) {
    vector<string> result;
    sort(skipRanges.begin(), skipRanges.end());
    int last = 0;
    for (const auto& p : skipRanges) {
        int start = p.first, end = p.second;
        if (last < start) result.push_back(expr.substr(last, start - last));
        last = max(last, end + 1);
    }
    if (last < expr.size()) result.push_back(expr.substr(last));
    return result;
}

void sort_children_by_name(AST_node* node){
    if (!node) return;
    sort(node->child.begin(), node->child.end(), [](AST_node* a, AST_node* b) {
        return a->Name_or_Op < b->Name_or_Op;
    });
}

// === Parser Functions Implementation ===
bool splitByRange(const string& expr, int start, int end, string& left, string& middle, string& right){
    if (start < 0 || end < start || end >= static_cast<int>(expr.length())) return false;
    left = expr.substr(0, start);
    middle = expr.substr(start, end - start + 1);
    right = expr.substr(end + 1);
    return true;
}

vector<pair<int, int>> find_all_depth_zero_op(const string& expr) {
    unordered_map<string, int> opPrecedence = {
        {"||", 0}, {"&&", 1}, {">=", 2}, {"<=", 3}, {"==", 4}, {"!=", 5}, {">", 6}, {"<", 7},
        {"|", 8}, {"&", 9}, {"^", 10}, {"+", 11}, {"-", 12}, {"*", 13}, {"/", 14}, {"<<", 15}, {">>", 16}, {"~", 17}
    };
    int depth = 0;
    vector<pair<int, int>> ans;
    int bestPrecedence = numeric_limits<int>::max();
    bool tildeAdded = false;

    for (size_t i = 0; i < expr.size(); ++i) {
        char c = expr[i];
        if (c == '(') { depth++; continue; }
        if (c == ')') { depth--; continue; }

        if (depth == 0) {
            if (i + 1 < expr.size()) {
                string twoCharOp = expr.substr(i, 2);
                if (opPrecedence.count(twoCharOp)) {
                    int prec = opPrecedence[twoCharOp];
                    if (prec < bestPrecedence) {
                        ans.clear();
                        ans.push_back({static_cast<int>(i), static_cast<int>(i + 1)});
                        bestPrecedence = prec;
                        tildeAdded = false;
                    } else if (prec == bestPrecedence) {
                        ans.push_back({static_cast<int>(i), static_cast<int>(i + 1)});
                    }
                    i++;
                    continue;
                }
            }
            string oneCharOp(1, c);
            if (opPrecedence.count(oneCharOp)) {
                int prec = opPrecedence[oneCharOp];
                if (prec < bestPrecedence) {
                    ans.clear();
                    ans.push_back({static_cast<int>(i), static_cast<int>(i)});
                    bestPrecedence = prec;
                    tildeAdded = (oneCharOp == "~");
                } else if (prec == bestPrecedence) {
                    if (oneCharOp == "~" && !tildeAdded) {
                        ans.push_back({static_cast<int>(i), static_cast<int>(i)});
                        tildeAdded = true;
                    } else if (oneCharOp != "~") {
                        ans.push_back({static_cast<int>(i), static_cast<int>(i)});
                    }
                }
            }
        }
    }
    return ans;
}

// === AST Builder ===
AST_node* Build_AST(string input, list<AST_node>& lst) {
    string condition, true_expr, false_expr;
    while (hasValidOuterParentheses(input)) {
        input = stripOuterParentheses(input);
    }
    bool mux_bool = parseTernary(input, condition, true_expr, false_expr);
    AST_node cur_node;

    if (mux_bool) {
        cur_node.Name_or_Op = "MUX"; cur_node.Type = "OP";
        cur_node.child.resize(3);
        cur_node.child[0] = Build_AST(true_expr, lst);
        cur_node.child[1] = Build_AST(condition, lst);
        cur_node.child[2] = Build_AST(false_expr, lst);
    } else {
        auto op_idx = find_all_depth_zero_op(input);
        if (!op_idx.empty()) {
            for (auto& idx : op_idx) {
                string left_str, op_str, right_str;
                splitByRange(input, idx.first, idx.second, left_str, op_str, right_str);
                cur_node.Name_or_Op = op_str; cur_node.Type = "OP";
                if (op_str == "~") {
                    input.erase(0, 1);
                    cur_node.child.resize(1);
                    cur_node.child[0] = Build_AST(input, lst);
                } else {
                    vector<string> parsed_str = split_string_excluding_ranges(input, op_idx);
                    cur_node.child.resize(parsed_str.size());
                    for (int i = 0; i < parsed_str.size(); ++i) {
                        cur_node.child[i] = Build_AST(parsed_str[i], lst);
                    }
                }
            }
        } else {
            cur_node.Name_or_Op = input;
            cur_node.Type = "SIGNAL";
            cur_node.child.clear();
        }
    }
    lst.push_back(cur_node);
    return &(lst.back());
}

// === AST Operations ===
void print_AST(AST_node* node, int indent) {
    if (!node) return;
    string space(indent, ' ');
    if (node->Name_or_Op == "MUX") {
        cout << space << "MUX" << endl;
        cout << space << "  Cond:" << endl;
        print_AST(node->child[1], indent + 4);
        cout << space << "  True:" << endl;
        print_AST(node->child[0], indent + 4);
        cout << space << "  False:" << endl;
        print_AST(node->child[2], indent + 4);
    } else {
        cout << space << "Name: " << node->Name_or_Op << endl;
        for (auto* child : node->child) {
            print_AST(child, indent + 4);
        }
    }
}

string canonical_string(AST_node* node) {
    if (!node) return "";
    if (node->Type == "SIGNAL" || node->Type == "NUM") return node->Name_or_Op;

    vector<string> child_reprs;
    for (auto* c : node->child) {
        child_reprs.push_back(canonical_string(c));
    }
    set<string> commutative_ops = {"&&", "||", "==", "!=", "&", "|", "^", "+", "*"};
    if (commutative_ops.count(node->Name_or_Op)) {
        sort(child_reprs.begin(), child_reprs.end());
    }
    string repr = node->Name_or_Op + "(";
    for (size_t i = 0; i < child_reprs.size(); ++i) {
        repr += child_reprs[i];
        if (i < child_reprs.size() - 1) repr += ",";
    }
    repr += ")";
    return repr;
}

void normalize_AST(AST_node* node) {
    if (!node) return;
    for (auto* child : node->child) {
        normalize_AST(child);
    }
    set<string> commutative_ops = {"&&", "||", "==", "!=", "&", "|", "^", "+", "*"};
    if (commutative_ops.count(node->Name_or_Op)) {
        sort_children_by_name(node);
    }
}

bool isSameAST(AST_node* a, AST_node* b) {
    return canonical_string(a) == canonical_string(b);
}