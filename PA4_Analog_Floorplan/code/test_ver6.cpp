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
    node_ptr M0=make_shared<node>();
    M0->id=0; M0->name="MM0"; M0->candidate_idx=0; M0->candidate.push_back(build_macro(4.99 ,2.12, 4, 1));
    node_vec.push_back(M0);

    node_ptr M1=make_shared<node>();
    M1->id=1; M1->name="MM1"; M1->candidate_idx=0; M1->candidate.push_back(build_macro(4.99,2.12,4,1));
    node_vec.push_back(M1);

    node_ptr M2=make_shared<node>();
    M2->id=2; M2->name="MM2"; M2->candidate_idx=0; M2->candidate.push_back(build_macro(1.9, 4.74,1,1));
    node_vec.push_back(M2);

    node_ptr M3=make_shared<node>();
    M3->id=3; M3->name="MM3"; M3->candidate_idx=0; M3->candidate.push_back(build_macro(1.9, 4.74,1,1));
    node_vec.push_back(M3);

    node_ptr M4=make_shared<node>();
    M4->id=4; M4->name="MM4"; M4->candidate_idx=0; M4->candidate.push_back(build_macro(1.9,22.16, 1, 4));
    node_vec.push_back(M4);
    
    return node_vec;
}

// vector<node_ptr> initialization(){
//     vector<node_ptr> node_vec;
//     node_vec.reserve(10000);

//     random_device rd;   //使用C++<random>隨機產生器
//     mt19937 gen(rd());
//     uniform_real_distribution<> dist_width(1.0, 20.0);
//     uniform_real_distribution<> dist_height(1.0, 20.0);
//     uniform_int_distribution<> dist_multiple(1, 5);

//     for(int i=0;i<10000;++i){
//         double w= dist_width(gen);
//         double h= dist_height(gen);
//         int col_mul= dist_multiple(gen);
//         int row_mul= dist_multiple(gen);

//         node_ptr n= make_shared<node>();
//         n->id= i;
//         n->name= "MM" + to_string(i);
//         n->candidate_idx= 0;
//         n->candidate.push_back(build_macro(w, h, col_mul, row_mul));
//         node_vec.push_back(n);
//     }
//     return node_vec;
// } 

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
    cout<<"up_bound: "<<my_placement.up_bound<<" right_bound: "<<my_placement.right_bound<<endl;
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
    cout<<"INL: "<<my_placement.INL<<endl;
    cout<<"B tree: "<<endl;
    show_tree(my_placement.root);
}

int main(){
    //初始化一個vector<node*>
    placement my_placement;
    my_placement.node_vec= initialization();
    sort_node_by_area(my_placement.node_vec);//有sort還是比較好，尤其是形狀差異越大的case

    //建一個tree(太寬就往上擺)
    double xk_sum=0,yk_sum=0;
    my_placement.root = make_Btree(my_placement.node_vec, my_placement.height_vec,my_placement.up_bound,my_placement.right_bound,xk_sum,yk_sum);
    find_centroid(xk_sum,yk_sum,my_placement);
    find_INL(my_placement);

    show_placement(my_placement);
    
    return 0;
}
