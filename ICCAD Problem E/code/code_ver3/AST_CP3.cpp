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
    AST_node* cond;     //只有MUX和NOT才有
    AST_node* left;
    AST_node* right;
};

struct AST{
    AST_node* root;
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

bool hasOuterParentheses(const string expr){    //檢查是否外層包覆括號，且括號是完整配對的
    if (expr.front()!='('||expr.back()!=')')return false;
    int depth=0;
    for(size_t i = 0; i < expr.length(); ++i){
        if(expr[i]=='(') depth++;
        else if(expr[i]==')') depth--;

        if(depth==0&&i!=expr.length()-1)return false;// 括號在中途就關閉，代表外層不是唯一一對
    }
    return depth==0;
}

string stripOuterParentheses(const string& expr){   //去除最外層括號（如果有）
    if (hasOuterParentheses(expr)){
        return expr.substr(1, expr.length() - 2);
    }
    return expr;
}

bool containsAnyOperator(const std::string& expr){
    vector<std::string> operators = {
        "&&", "||", ">=", "<=", "==", "!=", ">", "<",
        "&", "|", "^", "+", "-", "*", "/","~"};
    for (const auto& op : operators) {
        if (expr.find(op) != std::string::npos) {
            return true;
        }
    }
    return false;
}

bool start_with_not(const string& str){
    return !str.empty() && str[0] == '~';
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

pair<int, int> find_depth_zero_op(const string& expr) {
    unordered_map<string, int>opPrecedence={//運算子優先權，值越小表示優先順序越高
    {"&&", 0}, {"||", 0}, {">=", 1}, {"<=", 1}, {"==", 1}, {"!=", 1}, {">", 1}, {"<", 1},
    {"&", 2}, {"|", 2}, {"^", 2},{"+", 3}, {"-", 3},{"*", 4}, {"/", 4},  {"~", 5}};
    int depth = 0;
    pair<int, int> bestIndex = {-1, -1};
    int bestPrecedence = numeric_limits<int>::max();
    for (size_t i = 0; i < expr.size(); ++i){
        char c = expr[i];
        if (c == '('){
            depth++;
            continue;
        } else if (c == ')'){
            depth--;
            continue;
        }
        if(depth != 0) continue;
        if(i + 1 < expr.size()){//嘗試雙字元運算子
            string twoCharOp = expr.substr(i, 2);
            auto it = opPrecedence.find(twoCharOp);
            if (it != opPrecedence.end()) {
                int prec = it->second;
                if (prec < bestPrecedence || (prec == bestPrecedence && (int)i < bestIndex.first)) {
                    bestPrecedence = prec;
                    bestIndex = {static_cast<int>(i), static_cast<int>(i + 1)};
                }
                i++; //跳過第二字元
                continue;
            }
        }
        string oneCharOp(1, c); //嘗試單字元運算子
        auto it=opPrecedence.find(oneCharOp);
        if(it!=opPrecedence.end()){
            int prec = it->second;
            if (prec<bestPrecedence||(prec==bestPrecedence&&(int)i<bestIndex.first)) {
                bestPrecedence = prec;
                bestIndex={static_cast<int>(i), static_cast<int>(i)};
            }
        }
    }
    return bestIndex;
}
//===end of parser logic===

AST_node* Build_AST(string input,list<AST_node> &lst){ //之後一個AST_node都要被Signal指到
    //===for debug===
    //cout<<"input: "<<input<<endl;
    //===============
    string condition,true_expr,false_expr;
    bool mux_bool=parseTernary(input,condition,true_expr,false_expr);
    AST_node cur_node;

    if(mux_bool){                         //檢查是否有depth=0的MUX
        cur_node.Name_or_Op="MUX"; cur_node.Type="OP";
        cur_node.cond= Build_AST(condition,lst);
        cur_node.left= Build_AST(true_expr,lst);
        cur_node.right=Build_AST(false_expr,lst);
    }   
    else{
        pair<int, int>op_idx=find_depth_zero_op(input);
        if(op_idx.first!=-1){              //存在depth=0的OP(把NOT和其他OP分開解決)
            string left_str,op_str,right_str;
            splitByRange(input,op_idx.first,op_idx.second,left_str,op_str,right_str);
            cur_node.Name_or_Op=op_str; cur_node.Type="OP";
            if(op_str=="~"){
                input.erase(0,1);
                cur_node.cond=Build_AST(input,lst);
                cur_node.left=nullptr;cur_node.right=nullptr;
            }
            else{
                cur_node.cond=nullptr;
                cur_node.left= Build_AST(left_str,lst);
                cur_node.right=Build_AST(right_str,lst);
            }
        }
        else{
            if(containsAnyOperator(input)){//存在depth=0的OP(包含~)但需要先拆括號
                input=stripOuterParentheses(input);
                pair<int, int>op_idx=find_depth_zero_op(input);
                string left_str,op_str,right_str;
                splitByRange(input,op_idx.first,op_idx.second,left_str,op_str,right_str);
                cur_node.Name_or_Op=op_str; cur_node.Type="OP";
                if(op_str=="~"){
                    input.erase(0,1);
                    cur_node.cond=Build_AST(input,lst);
                    cur_node.left=nullptr;cur_node.right=nullptr;
                }
                else{
                    cur_node.cond=nullptr;
                    cur_node.left= Build_AST(left_str,lst);
                    cur_node.right=Build_AST(right_str,lst);
                }
            }
            else{                          //沒有任何operator了
                cur_node.Name_or_Op=input; cur_node.Type="SIGNAL";
                cur_node.cond=nullptr;cur_node.left=nullptr;cur_node.right=nullptr;
            }
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
        print_AST(node->cond, indent + 4);
        cout<<space<<"  True:"<<endl;
        print_AST(node->left, indent + 4);
        cout<<space<<"  False:"<<endl;
        print_AST(node->right, indent + 4);
    } 
    else if(node->Name_or_Op!="None"){
        cout<<space<<"Name: "<<node->Name_or_Op<<endl;
        if(node->cond){
            cout<<space<<"Cond:"<<endl;
            print_AST(node->cond, indent+4);
        }
        if(node->left){
            cout<<space<<"Left:"<<endl;
            print_AST(node->left, indent+4);
        }
        if(node->right){
            cout<<space<<"Right:"<<endl;
            print_AST(node->right,indent+4);
        }
    } 
    else cout<<space<<"Signal: "<<node->Name_or_Op<<endl;
}

//假設AST_node結構已定義好
string normalize_AST(AST_node* node) {
    if(!node) return "";
    if(node->Name_or_Op == "MUX") {
        string cond = normalize_AST(node->cond);
        string t_expr = normalize_AST(node->left);
        string f_expr = normalize_AST(node->right);
        return "MUX(" + cond + "," + t_expr + "," + f_expr + ")";
    } 
    else if (node->Type == "OP") {
        if (node->Name_or_Op == "~") {
            return "~" + normalize_AST(node->cond);
        }
        string left_str= normalize_AST(node->left);
        string right_str= normalize_AST(node->right);
        vector<string> commutative_ops={"&&", "||", "==", "!=", "&", "|", "^", "+", "*"};

        //對commutative operator做排序
        if(find(commutative_ops.begin(), commutative_ops.end(), node->Name_or_Op) != commutative_ops.end()) {
            if (left_str > right_str) {
                node->left->Name_or_Op=right_str;
                node->right->Name_or_Op=left_str;
            }
        }
        return "(" + left_str + node->Name_or_Op + right_str + ")";
    } 
    else{
        return node->Name_or_Op; // SIGNAL or NUM
    }
}

bool isSameAST(AST_node* a, AST_node* b) {
    if (a == nullptr && b == nullptr) return true; // 若兩個指標皆為 nullptr，表示相同
    if (a == nullptr || b == nullptr) return false;//若只有一個為 nullptr，表示不同

    if (a->Type != b->Type || a->Name_or_Op != b->Name_or_Op) return false;
    if (a->Type == "OP") {              //根據節點型別，遞迴比對子節點
        if (a->Name_or_Op == "MUX") {   //判斷 MUX 和 NOT 使用 cond
            return isSameAST(a->cond,b->cond)&&isSameAST(a->left,b->left)&&isSameAST(a->right,b->right);
        } 
        else if (a->Name_or_Op == "~") {
            return isSameAST(a->cond, b->cond);
        } 
        else {
            return isSameAST(a->left, b->left) &&isSameAST(a->right, b->right);
        }
    }
    return true;
}

int main(){
    //記得去除空格和assign和=和output signal
    string input_1="(control1&control2)?(input_a+input_b):(control1&~control2)?(input_a-input_b):(~control1&control2)?(input_a|input_b):(input_a&input_b)";
    string input_2="(control2&control1)?(input_b+input_a):(control1&~control2)?(input_a-input_b):(~control1&control2)?(input_b|input_a):(input_b&input_a)";
    string input_3="(x2&&~x3||(x1&&~x2&&x3))?(in2+in3):(in1+in2)";
    string input_4="in1+3*~in3*(~in2)";
    
    AST AST_1; 
    AST_1.root=Build_AST(input_1,AST_1.lst);
    normalize_AST(AST_1.root);
    print_AST(AST_1.root);

    AST AST_2; 
    AST_2.root=Build_AST(input_2,AST_2.lst);
    normalize_AST(AST_2.root);
    print_AST(AST_2.root);

    cout<<isSameAST(AST_1.root, AST_2.root);
    //print_AST(root_1);
}