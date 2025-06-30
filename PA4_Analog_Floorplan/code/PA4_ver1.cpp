#include "Parser.h"
#include <iostream>
#include <cmath>  // 包含 sqrt()
#include <algorithm> // std::max
#include <vector>
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

void sort_node_by_area(vector<node_ptr>& node_vec){
    sort(node_vec.begin(), node_vec.end(), [](const node_ptr& a, const node_ptr& b) {
        double area_a = a->candidate[a->candidate_idx].width * a->candidate[a->candidate_idx].height;
        double area_b = b->candidate[b->candidate_idx].width * b->candidate[b->candidate_idx].height;
        return area_a > area_b; // 由大排到小
    });
}

void update_height_vec(node_ptr cur_node,int cur_level, vector<vector<height_info>>&height_vec){
    height_info cur_info;
    cur_info.start_x=cur_node->x;
    cur_info.end_x=cur_node->x+cur_node->candidate[0].width;
    cur_info.max_y_pos=cur_node->y+cur_node->candidate[0].height;

    if(cur_level==height_vec.size()){   //代表要增加新的一個vector<height_info>
        vector<height_info> sub_vec;
        sub_vec.push_back(cur_info);
        height_vec.push_back(sub_vec);
    }
    else{
        height_vec[cur_level].push_back(cur_info);
    }
}

double find_valid_y(node_ptr cur_node, int cur_level, vector<vector<height_info>>height_vec){//從前一個level找出valid_y
    double node_start_x=cur_node->x;
    double node_end_x=cur_node->x+cur_node->candidate[0].width;
    double valid_y=0;

    if(height_vec.size()<=1&&cur_level==0) return 0;   //代表目前還沒有鋪滿一層，valid_y就從最底部開始即可
    else{  
        //從前一個level找出valid_y
        vector<height_info> sub_vec=height_vec[cur_level-1];
        
        for(int i=0; i<sub_vec.size(); i++){
            double cur_start_x=sub_vec[i].start_x;
            double cur_end_x=sub_vec[i].end_x;
            double cur_max_y_pos=sub_vec[i].max_y_pos;

            if((cur_start_x<=node_start_x&& node_start_x<=cur_end_x)||(node_start_x<=cur_start_x&&cur_end_x<=node_end_x)||(cur_start_x<=node_end_x && node_end_x<=cur_end_x)){
                valid_y=max(valid_y,cur_max_y_pos);
            }
            else if(node_end_x<cur_start_x) break;//已經超出需要比較的範圍了
        }
        return valid_y;
    }
}

node_ptr make_Btree(vector<node_ptr> node_vec, vector<vector<height_info>>& height_vec,
    double&up_bound,double& right_bound,double& xk_sum, double& yk_sum,double block_area_sum){
    if (node_vec.empty()) return nullptr;

    typedef tuple<int, double, int, node_ptr*, bool> State;
    stack<State> stk;
    node_ptr root = nullptr;
    double area = 0;

    stk.push(State(0, 0.0, 0, &root, false));

    while (!stk.empty()) {
        State s = stk.top();
        stk.pop();

        int idx        = get<0>(s);
        double cur_x   = get<1>(s);
        int cur_level  = get<2>(s);
        node_ptr* parent_link = get<3>(s);
        bool is_left_child    = get<4>(s);

        if (idx >= node_vec.size()) {
            *parent_link = nullptr;
            continue;
        }

        node_ptr cur_node = node_vec[idx];
        double node_width = cur_node->candidate[0].width;
        double node_height = cur_node->candidate[0].height;

        cur_node->x = cur_x;
        cur_node->level = cur_level;
        cur_node->y = find_valid_y(cur_node, cur_level, height_vec);
        cur_node->xk = cur_node->x + node_width / 2.0;
        cur_node->yk = cur_node->y + node_height / 2.0;

        xk_sum += cur_node->xk;
        yk_sum += cur_node->yk;
        update_height_vec(cur_node, cur_level, height_vec);

        right_bound = max(right_bound, cur_node->x + node_width);
        up_bound = max(up_bound, cur_node->y + node_height);

        *parent_link = cur_node;
        cur_node->left = nullptr;
        cur_node->right = nullptr;

        area += node_width * node_height;

        if (cur_x + node_width >= sqrt(block_area_sum)) {
            stk.push(State(idx + 1, 0.0, cur_level + 1, &cur_node->right, false));
        } else {
            stk.push(State(idx + 1, cur_x + node_width, cur_level, &cur_node->left, true));
        }
    }

    return root;
}


void find_centroid(double xk_sum,double yk_sum,placement &my_placement){
    int block_num=my_placement.node_vec.size();
    my_placement.Xc=xk_sum/block_num;
    my_placement.Yc=yk_sum/block_num;
}

vector<node_ptr>sort_nodes_by_id(const vector<node_ptr>& node_vec){
    vector<node_ptr> sorted_vec= node_vec; //複製原本vector，不影響原始資料
    sort(sorted_vec.begin(),sorted_vec.end(), [](const node_ptr& a,const node_ptr& b){
        return a->id < b->id;              //按照id由小到大排序
    });
    return sorted_vec;                     //回傳排序後的新vector
}

void make_S_actual(placement &my_placement){
    vector<node_ptr> sorted_vec= sort_nodes_by_id(my_placement.node_vec);
    my_placement.S_actual.resize(sorted_vec.size());
    double Xc=my_placement.Xc;
    double Yc=my_placement.Yc;
    my_placement.S_actual[0]=(sorted_vec[0]->xk-Xc)*(sorted_vec[0]->xk-Xc)+(sorted_vec[0]->yk-Yc)*(sorted_vec[0]->yk-Yc);

    for(int i=1;i<sorted_vec.size();i++){
        my_placement.S_actual[i]= my_placement.S_actual[i-1]+
        (sorted_vec[i]->xk-Xc)*(sorted_vec[i]->xk-Xc)+(sorted_vec[i]->yk-Yc)*(sorted_vec[i]->yk-Yc);
    }
}

void make_S_ideal(placement &my_placement){
    //sum_x_sqr是xi平方的總和
    double sum_x=0,sum_y=0,sum_x_sqr=0,sum_xy=0;
    int N=my_placement.S_actual.size();
    for(int i=0;i<N; i++){
        sum_x+=i;
        sum_y+=my_placement.S_actual[i];
        sum_x_sqr+=i*i;
        sum_xy +=i*my_placement.S_actual[i];
    }
    
    double a=(N*sum_xy - sum_x*sum_y) / (N*sum_x_sqr - sum_x*sum_x);
    double b=(sum_y-a*sum_x) / N;

    my_placement.S_ideal.resize(N);
    for (int i = 0;i< N; ++i) {
        my_placement.S_ideal[i]=a*i+b;
    }
}

void find_INL(placement &my_placement){
    make_S_actual(my_placement);
    make_S_ideal(my_placement);
    my_placement.INL=0;
    for(int i=0;i<my_placement.S_actual.size();i++){
        my_placement.INL=max(my_placement.INL, abs(my_placement.S_actual[i]-my_placement.S_ideal[i]));
    }
}

void show_tree(node_ptr root){
    if(!root) return;
    stack<node_ptr> stk;
    stk.push(root);

    while(!stk.empty()){
        node_ptr current=stk.top();
        stk.pop();
        cout<<"name: "<<current->name<<endl;
        cout<<"level: "<<current->level<<endl;
        cout<<"x: "<<current->x<<" y: "<<current->y<<endl;
        cout<<"width: "<<current->candidate[0].width<<" height: "<<current->candidate[0].height<<endl;

        //為了讓left優先處理，我們要先把right推進stack
        if(current->left)stk.push(current->left);
        if(current->right)stk.push(current->right);
    }
}

void show_placement(placement my_placement){
    cout<<"Xc: "<<my_placement.Xc<<" Yc "<<my_placement.Yc<<endl;
    cout<<"S_actual: ";
    for(int i=0;i<my_placement.S_actual.size();i++){
        cout<<my_placement.S_actual[i]<<" ";
    }
    cout<<endl;
    cout<<"S_ideal: ";
    for(int i=0;i<my_placement.S_ideal.size();i++){
        cout<<my_placement.S_ideal[i]<<" ";
    }
    cout<<endl;
    cout<<"B tree: "<<endl;
    show_tree(my_placement.root);

    cout<<"up_bound: "<<my_placement.up_bound<<" right_bound: "<<my_placement.right_bound<<endl;
    cout<<"INL: "<<my_placement.INL<<endl;
}

int main(int argc, char *argv[]){
    string input_file_name = argv[1];             
    string output_file_name= argv[2]; 

    placement my_placement;                     //初始化一個vector<node*>
    my_placement.node_vec= initialize_node_vec(my_placement,input_file_name);
    sort_node_by_area(my_placement.node_vec);   //有sort還是比較好，尤其是形狀差異越大的case

    //建一個tree(太寬就往上擺)
    double xk_sum=0,yk_sum=0;
    my_placement.root = make_Btree(my_placement.node_vec, my_placement.height_vec,my_placement.up_bound,my_placement.right_bound,xk_sum,yk_sum,my_placement.block_area_sum);
    find_centroid(xk_sum,yk_sum,my_placement);
    find_INL(my_placement);
    show_placement(my_placement);
    
    return 0;
}
