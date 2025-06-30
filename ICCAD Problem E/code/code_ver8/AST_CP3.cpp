#include <iostream>
#include <string>
#include <utility>   
#include <unordered_map>
#include <list>
#include <algorithm>
#include <vector>
#include <limits>

using namespace std;

struct AST_node{
    string Type;        //可以是SIGNAL,NUM,OP  
    string Name_or_Op;
    vector<AST_node*> child;     //只有MUX和NOT才有改成vec<AST_node*>
};

struct AST{
    AST_node* root;     //改成vec<AST_node*>
    list<AST_node> lst;
};

//===Auxilary Function===
bool parseTernary(const string expr, string& condition, string& true_expr, string& false_expr){
    int depth=0; int q_index=-1; int colon_index=-1;
    for (int i =0; i<expr.length();i++){
        char c = expr[i];
        if(c=='(') depth++;
        else if(c==')') depth--;
        else if(c=='?'&&depth==0 && q_index == -1) {
            q_index= i;
        } 
        else if(c==':'&&depth==0 && q_index!=-1) {
            colon_index= i;
            break;
        }
    }
    if (q_index!=-1 && colon_index!=-1){
        condition= expr.substr(0, q_index);
        true_expr= expr.substr(q_index + 1, colon_index - q_index-1);
        false_expr= expr.substr(colon_index + 1);
        return true;
    }
    return false;   //Not a mux expression
}

bool hasValidOuterParentheses(const string& expr){
    if (expr.empty() || expr.front() != '(' || expr.back() != ')') return false;
    int depth = 0;
    for (size_t i = 0; i < expr.length(); ++i) {
        if (expr[i] == '(') depth++;
        else if (expr[i] == ')') depth--;
        if (depth < 0) return false;//如果中途出現未匹配的右括號，表示不是合法算式
        //如果在不是最後一個字元前就 depth 回到 0，表示外層括號不是唯一一對
        if (depth == 0 && i != expr.length() - 1) return false;
    }
    //結尾時 depth 必須為 0，代表所有括號都匹配了
    return depth == 0;
}

string stripOuterParentheses(const string& expr){   //去除最外層括號（如果有）
    return expr.substr(1, expr.length() - 2);
}

bool start_with_not(const string& str){
    return !str.empty() && str[0] == '~';
}

vector<string> split_string_excluding_ranges(const string& expr, vector<pair<int, int>> skipRanges) {
    vector<string> result;
    sort(skipRanges.begin(), skipRanges.end());//先依照起始 index 排序，確保由小到大處理

    int last = 0;
    for (const auto& p : skipRanges) {
        int start = p.first;
        int end = p.second;
        if (last < start) {
            result.push_back(expr.substr(last, start - last));
        }
        last = max(last, end + 1); // 更新 last，跳過這段
    }

    if (last < expr.size()) {// 最後一段 (如果還有剩)
        result.push_back(expr.substr(last));
    }
    return result;
}

void sort_children_by_name(AST_node* node){
    if (!node) return;
    sort(node->child.begin(), node->child.end(), [](AST_node* a, AST_node* b) {
        return a->Name_or_Op < b->Name_or_Op;
    });
}
//===End of Auxilary Function===

//===Parser Function===
bool splitByRange(const string& expr,int start,int end,string& left,string& middle,string& right){
    if (start < 0||end<start || end >= static_cast<int>(expr.length())){
        return false; //index無效
    }
    left = expr.substr(0, start);
    middle = expr.substr(start, end - start + 1);
    right = expr.substr(end + 1);
    return true;
}

vector<pair<int, int>> find_all_depth_zero_op(const string& expr) {
    unordered_map<string, int> opPrecedence = {
        {"||", 0}, {"&&", 1}, {">=", 2}, {"<=", 3}, {"==", 4}, {"!=", 5}, {">", 6}, {"<", 7},
        {"|", 8}, {"&", 9}, {"^", 10}, {"+", 11}, {"-", 12}, {"*", 13}, {"/", 14},{"<<",15},{">>",16}, {"~", 17}
    };
    int depth = 0;
    vector<pair<int, int>> ans;
    int bestPrecedence = numeric_limits<int>::max();
    bool tildeAdded = false; // 紀錄是否已加入過 ~
    for (size_t i = 0; i < expr.size(); ++i) {
        char c = expr[i];
        if (c == '(') {
            depth++;
            continue;
        } else if (c == ')') {
            depth--;
            continue;
        }
        if (depth == 0) {
            if (i + 1 < expr.size()) {
                string twoCharOp = expr.substr(i, 2);
                auto it = opPrecedence.find(twoCharOp);
                if (it != opPrecedence.end()) {
                    int prec = it->second;
                    if (prec < bestPrecedence) {
                        ans.clear();
                        ans.push_back({static_cast<int>(i), static_cast<int>(i + 1)});
                        bestPrecedence = prec;
                        tildeAdded = false; // 清空時也要重設
                    } else if (prec == bestPrecedence) {
                        ans.push_back({static_cast<int>(i), static_cast<int>(i + 1)});
                    }
                    i++; // skip second character
                    continue;
                }
            }
            string oneCharOp(1, c);
            auto it = opPrecedence.find(oneCharOp);
            if (it != opPrecedence.end()) {
                int prec = it->second;
                if (prec < bestPrecedence) {
                    ans.clear();
                    ans.push_back({static_cast<int>(i), static_cast<int>(i)});
                    bestPrecedence = prec;
                    tildeAdded = (oneCharOp == "~"); // 如果是~則記錄
                } else if (prec == bestPrecedence) {
                    if (oneCharOp == "~") {
                        if (!tildeAdded) {
                            ans.push_back({static_cast<int>(i), static_cast<int>(i)});
                            tildeAdded = true;
                        }
                    } else {
                        ans.push_back({static_cast<int>(i), static_cast<int>(i)});
                    }
                }
            }
        }
    }
    return ans;
}
//===end of parser logic===

AST_node* Build_AST(string input,list<AST_node> &lst){ //之後一個AST_node都要被Signal指到
    //===for debug===
    //cout<<"input: "<<input<<endl;
    //===============
    string condition,true_expr,false_expr;
    while(hasValidOuterParentheses(input)){
        input=stripOuterParentheses(input);
    }
    bool mux_bool=parseTernary(input,condition,true_expr,false_expr);
    AST_node cur_node;

    if(mux_bool){                         //檢查是否有depth=0的MUX
        cur_node.Name_or_Op="MUX"; cur_node.Type="OP";
        cur_node.child.resize(3);
        cur_node.child[0]=Build_AST(true_expr,lst); //left
        cur_node.child[1]=Build_AST(condition,lst); //cond
        cur_node.child[2]=Build_AST(false_expr,lst);//right
    }   
    else{
        vector<pair<int, int>>op_idx=find_all_depth_zero_op(input);
        if(op_idx.size()!=0){               //存在depth=0的OP(把NOT和其他OP分開解決)
            for(int i=0;i<op_idx.size();i++){
                vector<string> parsed_str=split_string_excluding_ranges(input,op_idx);
                string left_str,op_str,right_str;
                splitByRange(input,op_idx[i].first,op_idx[i].second,left_str,op_str,right_str);
                cur_node.Name_or_Op=op_str; cur_node.Type="OP";
                if(op_str=="~"){
                    input.erase(0,1);
                    cur_node.child.resize(1);
                    cur_node.child[0]=Build_AST(input,lst); //~只會有一個child
                }
                else{                       //merge起來，op只放一個
                    cur_node.child.resize(parsed_str.size());
                    for(int i=0;i<parsed_str.size();i++){
                        cur_node.child[i]=Build_AST(parsed_str[i],lst);
                    }
                }
            }
        }
        else{                              //沒有任何operator了
            cur_node.Name_or_Op=input; 
            cur_node.Type="SIGNAL";
            cur_node.child.resize(0);
        }
    }
    lst.push_back(cur_node);
    return &(lst.back());
}

void print_AST(AST_node* node,int indent=0){
    if(!node) return;
    string space(indent, ' ');
    if(node->Name_or_Op=="MUX"){
        cout<<space<<"MUX" << endl;
        cout<<space<<"  Cond:"<<endl;
        print_AST(node->child[1], indent + 4);
        cout<<space<<"  True:"<<endl;
        print_AST(node->child[0], indent + 4);
        cout<<space<<"  False:"<<endl;
        print_AST(node->child[2], indent + 4);
    } 
    else if(node->Name_or_Op!="None"){
        cout<<space<<"Name: "<<node->Name_or_Op<<endl;
        if(node->child.size()>0){
            for(int i=0;i<node->child.size();i++){
                print_AST(node->child[i], indent + 4);
            }
        }
    } 
    else cout<<space<<"Signal: "<<node->Name_or_Op<<endl;
}

string canonical_string(AST_node* node) {
    if (!node) return "";
    if (node->Type == "SIGNAL" || node->Type == "NUM") {
        return node->Name_or_Op;
    }
    vector<string> child_reprs;
    for (auto* c : node->child) {
        child_reprs.push_back(canonical_string(c));
    }
    vector<string> commutative_ops = {"&&", "||", "==", "!=", "&", "|", "^", "+", "*"};
    if (find(commutative_ops.begin(), commutative_ops.end(), node->Name_or_Op) != commutative_ops.end()) {
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

    vector<string> commutative_ops = {"&&", "||", "==", "!=", "&", "|", "^", "+", "*"};
    if (find(commutative_ops.begin(), commutative_ops.end(), node->Name_or_Op) != commutative_ops.end()) {
        sort_children_by_name(node); // 照 AST_node->Name_or_Op 排
    }
}

bool isSameAST(AST_node* a, AST_node* b) {
    return canonical_string(a) == canonical_string(b);
}


int main(){
    //記得去除空格和assign和=和output signal
    string input_1="((control1&control2)?(input_a+input_b):(control1&~control2)?(input_a-input_b):(~control1&control2)?(input_a|input_b):((input_a&input_b)))";
    string input_2="(control2&control1)?(input_b+input_a):(control1&~control2)?(input_a-input_b):(~control1&control2)?(input_b|input_a):(input_b&input_a)";
    string input_3="(x2&&~x3||(x1&&~x2&&x3))?(in2+in3):(in1+in2)";
    string input_4="in1+3*~in3*(~in2)+in4+(in77+~in5+in19)+5*(in_8<<8)";
    string input_5="5*(in_8<<8)+in4+3*(~in2)*~in3*+in1+(in19+~in5+in77)";
    string input_6="in3+3*~in4*(~in2)+in1+(in19+~in5+in77)+5*(in_8<<8)";
    
    AST AST_1; 
    AST_1.root=Build_AST(input_1,AST_1.lst);
    normalize_AST(AST_1.root);
    print_AST(AST_1.root);

    AST AST_2; 
    AST_2.root=Build_AST(input_2,AST_2.lst);
    normalize_AST(AST_2.root);
    print_AST(AST_2.root);

    cout<<isSameAST(AST_1.root, AST_2.root);
}