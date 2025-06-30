#include "AST.h"
#include <iostream>
#include <limits>

//for Build_AST()
bool parseTernary(const string expr, string& condition, string& true_expr, string& false_expr){
    int depth = 0, q_index = -1, colon_index = -1;
    for (int i = 0; i < expr.length(); i++) {
        char c = expr[i];
        if (c == '(') depth++;
        else if (c == ')') depth--;
        else if (c == '?' && depth == 0 && q_index == -1) {
            q_index = i;
        } else if (c == ':' && depth == 0 && q_index != -1) {
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

//for stripOuterParentheses()
bool hasOuterParentheses(const string expr) {
    if (expr.front() != '(' || expr.back() != ')') return false;
    int depth = 0;
    for (size_t i = 0; i < expr.length(); ++i) {
        if (expr[i] == '(') depth++;
        else if (expr[i] == ')') depth--;
        if (depth == 0 && i != expr.length() - 1) return false;
    }
    return depth == 0;
}

//for Build_AST()
string stripOuterParentheses(const string& expr) {
    if (hasOuterParentheses(expr)) {
        return expr.substr(1, expr.length() - 2);
    }
    return expr;
}

//for Build_AST()
bool containsAnyOperator(const string& expr) {
    vector<string> operators = {
        "&&", "||", ">=", "<=", "==", "!=", ">", "<",
        "&", "|", "^", "+", "-", "*", "/", "~"
    };
    for (const auto& op : operators) {
        if (expr.find(op) != string::npos) return true;
    }
    return false;
}

//for Build_AST()
bool start_with_not(const string& str) {
    return !str.empty() && str[0] == '~';
}

//for Build_AST()
bool splitByRange(const string& expr, int start, int end, string& left, string& middle, string& right) {
    if (start < 0 || end < start || end >= static_cast<int>(expr.length())) return false;
    left = expr.substr(0, start);
    middle = expr.substr(start, end - start + 1);
    right = expr.substr(end + 1);
    return true;
}

//for Build_AST()
pair<int, int> find_depth_zero_op(const string& expr) {
    unordered_map<string, int> opPrecedence = {
        {"&&", 0}, {"||", 0}, {">=", 1}, {"<=", 1}, {"==", 1}, {"!=", 1}, {">", 1}, {"<", 1},
        {"&", 2}, {"|", 2}, {"^", 2}, {"+", 3}, {"-", 3}, {"*", 4}, {"/", 4}, {"~", 5}
    };
    int depth = 0;
    pair<int, int> bestIndex = {-1, -1};
    int bestPrecedence = numeric_limits<int>::max();
    for (size_t i = 0; i < expr.size(); ++i) {
        char c = expr[i];
        if (c == '(') { depth++; continue; }
        if (c == ')') { depth--; continue; }
        if (depth != 0) continue;
        if (i + 1 < expr.size()) {
            string twoCharOp = expr.substr(i, 2);
            auto it = opPrecedence.find(twoCharOp);
            if (it != opPrecedence.end()) {
                int prec = it->second;
                if (prec < bestPrecedence || (prec == bestPrecedence && (int)i < bestIndex.first)) {
                    bestPrecedence = prec;
                    bestIndex = {static_cast<int>(i), static_cast<int>(i + 1)};
                }
                i++;
                continue;
            }
        }
        string oneCharOp(1, c);
        auto it = opPrecedence.find(oneCharOp);
        if (it != opPrecedence.end()) {
            int prec = it->second;
            if (prec < bestPrecedence || (prec == bestPrecedence && (int)i < bestIndex.first)) {
                bestPrecedence = prec;
                bestIndex = {static_cast<int>(i), static_cast<int>(i)};
            }
        }
    }
    return bestIndex;
}

AST_node* Build_AST(string input, list<AST_node>& lst) {
    string condition, true_expr, false_expr;
    bool mux_bool = parseTernary(input, condition, true_expr, false_expr);
    AST_node cur_node;

    if (mux_bool) {
        cur_node.Name_or_Op = "MUX"; cur_node.Type = "OP";
        cur_node.cond = Build_AST(condition, lst);
        cur_node.left = Build_AST(true_expr, lst);
        cur_node.right = Build_AST(false_expr, lst);
    } else {
        pair<int, int> op_idx = find_depth_zero_op(input);
        if (op_idx.first != -1) {
            string left_str, op_str, right_str;
            splitByRange(input, op_idx.first, op_idx.second, left_str, op_str, right_str);
            cur_node.Name_or_Op = op_str; cur_node.Type = "OP";
            if (op_str == "~") {
                input.erase(0, 1);
                cur_node.cond = Build_AST(input, lst);
            } else {
                cur_node.left = Build_AST(left_str, lst);
                cur_node.right = Build_AST(right_str, lst);
            }
        } else {
            if (containsAnyOperator(input)) {
                input = stripOuterParentheses(input);
                pair<int, int> op_idx = find_depth_zero_op(input);
                string left_str, op_str, right_str;
                splitByRange(input, op_idx.first, op_idx.second, left_str, op_str, right_str);
                cur_node.Name_or_Op = op_str; cur_node.Type = "OP";
                if (op_str == "~") {
                    input.erase(0, 1);
                    cur_node.cond = Build_AST(input, lst);
                } else {
                    cur_node.left = Build_AST(left_str, lst);
                    cur_node.right = Build_AST(right_str, lst);
                }
            } else {
                cur_node.Name_or_Op = input; cur_node.Type = "SIGNAL";
            }
        }
    }

    lst.push_back(cur_node);
    return &lst.back();
}

void print_AST(AST_node* node, int indent) {
    if (!node) return;
    string space(indent, ' ');
    if (node->Name_or_Op == "MUX") {
        cout << space << "MUX" << endl;
        cout << space << "  Cond:" << endl;
        print_AST(node->cond, indent + 4);
        cout << space << "  True:" << endl;
        print_AST(node->left, indent + 4);
        cout << space << "  False:" << endl;
        print_AST(node->right, indent + 4);
    } else {
        cout << space << "Name: " << node->Name_or_Op << endl;
        if (node->cond) {
            cout << space << "Cond:" << endl;
            print_AST(node->cond, indent + 4);
        }
        if (node->left) {
            cout << space << "Left:" << endl;
            print_AST(node->left, indent + 4);
        }
        if (node->right) {
            cout << space << "Right:" << endl;
            print_AST(node->right, indent + 4);
        }
    }
}
