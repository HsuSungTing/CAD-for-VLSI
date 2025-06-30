#include <iostream>
#include <cmath>  // 包含 sqrt()
#include <algorithm> // std::max
#include <vector>
#include <sstream>
#include <stack>
#include <tuple>
#include <memory>  
#include <string>
#include <random>
#include <fstream>

using namespace std;
//開始加上質心
//有加入sort的版本

struct node;
typedef shared_ptr<node> node_ptr;  // 放在這裡，這樣 node_ptr 可以被整個程式識別

struct macro_info{
    double width;
    double height;
    int col_multiple;
    int row_multiple;
};

struct node{
    string name;
    int id;
    int level;      //目前node被擺在第幾層(從第0層開始)
    double x,y;     //一個block左下角的座標
    double xk,yk;   //一個block的質心座標
    node_ptr left;  //left child，右邊的cell
    node_ptr right; //right child，上方的cell
    int candidate_idx;
    vector<macro_info>candidate;
};

struct height_info{
    double start_x,end_x; //起始與終止的橫坐標
    double max_y_pos;     //高度(下一層在擺放的時候要遷就這個高度)
};

struct placement{
    double block_area_sum;
    double area;
    node_ptr root;        //Btree的root
    double Xc,Yc;         //質心座標
    vector<node_ptr> node_vec;
    vector<vector<height_info>> height_vec;
    double up_bound;
    double right_bound;
    vector<double>S_actual;
    vector<double>S_ideal;
    double INL;
};

vector<string> read_file(const string& filename) {
    vector<string> lines;
    ifstream infile(filename);
    string line;
    if(!infile){
        cout<<"can't open file:"<<filename<<endl;
        return lines;
    }
    while(getline(infile, line)){
        lines.push_back(line);
    }
    infile.close();
    return lines;
}

vector<string> splitByParentheses(const string& input){
    //依照()拆分字串，MM4 (1.9 22.16 1 4) (2.93 11.08 2 2)拆成MM4、(1.9 22.16 1 4)、(2.93 11.08 2 2)
    vector<string> result;
    string token;
    bool inParentheses = false;
    for (size_t i = 0; i < input.size(); ++i) {
        char c = input[i];
        if (c == '(') {             //儲存括號前的token(如果非空)
            if (!token.empty()) {
                result.push_back(token);
                token.clear();
            }
            inParentheses = true;
            token += c;
        } 
        else if(c == ')'){
            token += c;
            result.push_back(token);
            token.clear();
            inParentheses = false;
        } 
        else if(inParentheses){
            token += c;
        } 
        else{
            if(!isspace(c)){
                token += c;
            } 
            else if(!token.empty()){  //如果遇到空白且 token 非空，代表 token 結束
                result.push_back(token);
                token.clear();
            }
        }
    }
    if(!token.empty()){               //最後還有token(無括號的情況)
        result.push_back(token);
    }
    return result;
}

vector<string> splitInsideParentheses(const string& input){
    //把(1.9 22.16 1 4)拆分成"1.9" 、"22.16"、"1"、"4"
    vector<string> result;

    string inner= input.substr(1, input.size() - 2);   //去除開頭的 '(' 和結尾的 ')'
    istringstream iss(inner);
    string token;
    while (iss >> token) {
        result.push_back(token);
    }
    return result;
}

macro_info build_macro(double width, double height, int col_multiple, int row_multiple){
    macro_info my_macro;
    my_macro.col_multiple=col_multiple;
    my_macro.row_multiple=row_multiple;
    my_macro.height=height;
    my_macro.width=width;
    return my_macro;
}

int extract_Int(const string& str){//把字串中結尾的數字找出來
    string numberStr;
    for (char c : str) {
        if (isdigit(c)) {
            numberStr += c;
        }
    }
    return numberStr.empty()? 0:stoi(numberStr);
}

vector<node_ptr> initialize_node_vec(placement &my_placement,string input_file){
    my_placement.block_area_sum=0;
    vector<string>lines= read_file(input_file);
    vector<node_ptr>node_vec;
    for(int i=0;i<lines.size();i++){
        node_ptr my_node=make_shared<node>();
        vector<string>results=splitByParentheses(lines[i]);
        my_node->name=results[0];
        my_node->candidate_idx=0;
        my_node->candidate.resize(0);
        my_node->id=extract_Int(results[0]);

        for(int i=1;i<results.size();i++){
            vector<string> values=splitInsideParentheses(results[i]);
            my_node->candidate.push_back(build_macro(stod(values[0]),stod(values[1]),stoi(values[2]),stoi(values[3])));
        }
        my_placement.block_area_sum+= my_node->candidate[0].height*my_node->candidate[0].width;
        node_vec.push_back(my_node);
    }

    return node_vec;
} 

void show_initialization(placement my_placement){
    cout<<"block_area_sum: "<<my_placement.block_area_sum<<endl;
    cout<<"node_vec: "<<endl;
    for(int i=0;i<my_placement.node_vec.size();i++){
        cout<<"block name: "<<my_placement.node_vec[i]->name<<" Id: "<<my_placement.node_vec[i]->id<<endl;
        cout<<"macro info: ";
        vector<macro_info>candidate_vec=my_placement.node_vec[i]->candidate;
        for(int j=0;j<candidate_vec.size();j++){
            cout<<"("<<candidate_vec[j].width<<", "<<candidate_vec[j].height<<", "<<candidate_vec[j].col_multiple<<", "<<candidate_vec[j].row_multiple<<") ";
        }
        cout<<endl;
    }
}

int main(int argc, char *argv[]){
    string input_file_name=argv[1];             
    string output_file_name=argv[2]; 
    placement my_placement;
    my_placement.node_vec=initialize_node_vec(my_placement, input_file_name);
    show_initialization(my_placement);
    
    return 0;
}
