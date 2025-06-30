#include <iostream>
#include <cmath>  // 包含 sqrt()
#include <algorithm> // std::max
#include <vector>
#include <stack>
#include <tuple>
#include <memory>  
#include <string>
#include <random>

using namespace std;
//開始加上質心

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
    double area;
    node_ptr root;        //Btree的root
    double Xc,Yc;         //質心座標
    vector<node_ptr> node_vec;
    vector<vector<height_info>> height_vec;
    double up_bound;
    double right_bound;
};

macro_info build_macro(double width, double height, int col_multiple, int row_multiple){
    macro_info my_macro;
    my_macro.col_multiple=col_multiple;
    my_macro.row_multiple=row_multiple;
    my_macro.height=height;
    my_macro.width=width;
    return my_macro;
}

vector<node_ptr> initialization(){
    vector<node_ptr> node_vec;
    node_vec.reserve(10000);

    random_device rd;   //使用C++<random>隨機產生器
    mt19937 gen(rd());
    uniform_real_distribution<> dist_width(1.0, 20.0);
    uniform_real_distribution<> dist_height(1.0, 20.0);
    uniform_int_distribution<> dist_multiple(1, 5);

    for(int i=0;i<10000;++i){
        double w= dist_width(gen);
        double h= dist_height(gen);
        int col_mul= dist_multiple(gen);
        int row_mul= dist_multiple(gen);

        node_ptr n= make_shared<node>();
        n->id= i;
        n->name= "MM" + to_string(i);
        n->candidate_idx= 0;
        n->candidate.push_back(build_macro(w, h, col_mul, row_mul));
        node_vec.push_back(n);
    }
    return node_vec;
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

node_ptr make_Btree(vector<node_ptr>node_vec,vector<vector<height_info>>& height_vec,double& up_bound, double& right_bound, double& xk_sum, double& yk_sum) {
    if (node_vec.empty()) return nullptr;

    using State = tuple<int, double, int, node_ptr*, bool>;
    // idx, cur_x, cur_level, pointer to parent node's left/right, is_left_child

    stack<State> stk;
    node_ptr root = nullptr;
    double area = 0;
    stk.emplace(0, 0.0, 0, &root, false);

    while (!stk.empty()) {
        auto [idx, cur_x, cur_level, parent_link, is_left_child] = stk.top();
        stk.pop();

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

        //連接到parent的left或right
        *parent_link = cur_node;
        cur_node->left = nullptr;
        cur_node->right = nullptr;
        area += node_width * node_height;

        //遞迴順序為:先處理右邊(所以先壓入右邊，後壓入左邊)
        if (cur_x > sqrt(area)){
            stk.emplace(idx + 1, 0.0, cur_level + 1, &cur_node->right, false);
        } 
        else{
            stk.emplace(idx + 1, cur_x + node_width, cur_level, &cur_node->left, true);
        }
    }
    return root;
}

void find_centroid(double xk_sum,double yk_sum,placement &my_placement){
    int block_num=my_placement.node_vec.size();
    my_placement.Xc=xk_sum/block_num;
    my_placement.Yc=yk_sum/block_num;
}

void show_tree(node_ptr root){
    if (!root) return;
    stack<node_ptr> stk;
    stk.push(root);

    while(!stk.empty()){
        node_ptr current=stk.top();
        stk.pop();
        cout<<"name: "<<current->name<<endl;
        cout<<"level: "<<current->level<<endl;
        cout<<"x: "<<current->x<<" y: "<<current->y<<endl;
        cout<<"width: "<<current->candidate[0].width<<" height: "<<current->candidate[0].height<<endl;

        // 注意：為了讓 left 優先處理，我們要先把 right 推進 stack
        if(current->left)stk.push(current->left);
        if(current->right)stk.push(current->right);
    }
}


void show_placement(placement my_placement){
    cout<<"up_bound: "<<my_placement.up_bound<<" right_bound: "<<my_placement.right_bound<<endl;
    cout<<"Xc: "<<my_placement.Xc<<" Yc "<<my_placement.Yc<<endl;
    cout<<"B tree: "<<endl;
    show_tree(my_placement.root);
}

int main(){
    //初始化一個vector<node*>
    placement my_placement;
    my_placement.node_vec = initialization();
    
    //建一個tree(太寬就往上擺)
    double xk_sum=0,yk_sum=0;
    my_placement.root = make_Btree(my_placement.node_vec, my_placement.height_vec,my_placement.up_bound,my_placement.right_bound,xk_sum,yk_sum);
    find_centroid(xk_sum,yk_sum,my_placement);
    show_placement(my_placement);
    
    return 0;
}
