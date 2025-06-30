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
#include <ctime>
#include <cstdlib> // for rand(), srand()
#include <fstream>
#include <iomanip>  // for std::setprecision

using namespace std;
//開始加上質心
//有加入sort的版本
//有更新Xc和Yc
//加入一點Perturb
//確定初始化正確
//目前做法是邊界有交界，如果不允許就改回來就好，一定正確
//邏輯幾乎正確了，但因為SA的過程中，vector和tree都是利用pointer操作node，必定會修改到node
//要讓每個placement都有自己的一份node的copy，用Pointer都只改在原本的node上，會亂掉
//在gpt的幫助下修改正確

struct node{
    string name;
    int id;
    int level;      //目前node被擺在第幾層(從第0層開始)
    double x,y;     //一個block左下角的座標
    double xk,yk;   //一個block的質心座標
    node* left;  //left child，右邊的cell
    node* right; //right child，上方的cell
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
    node* root;        //Btree的root
    double Xc,Yc;         //質心座標
    vector<node> node_vec;
    vector<vector<height_info>> height_vec;
    double up_bound;
    double right_bound;
    vector<double>S_actual;
    vector<double>S_ideal;
    double INL;
    double cost;
};

vector<node> initialize_node_vec(placement &my_placement,string input_file){
    my_placement.block_area_sum=0;
    vector<string>lines= read_file(input_file);
    vector<node>node_vec;
    node_vec.resize(0);
    for(int i=0;i<lines.size();i++){
        node my_node;
        vector<string>results=splitByParentheses(lines[i]);
        my_node.name=results[0];
        my_node.candidate_idx=0;
        my_node.candidate.resize(0);
        my_node.id=extract_Int(results[0]);

        for(int i=1;i<results.size();i++){
            vector<string> values=splitInsideParentheses(results[i]);
            my_node.candidate.push_back(build_macro(stod(values[0]),stod(values[1]),stoi(values[2]),stoi(values[3])));
        }
        my_placement.block_area_sum+= my_node.candidate[0].height*my_node.candidate[0].width;
        node_vec.push_back(my_node);
    }

    return node_vec;
} 

void sort_node_by_area(vector<node>& node_vec){
    sort(node_vec.begin(), node_vec.end(), [](const node& a, const node& b) {
        double area_a = a.candidate[a.candidate_idx].width * a.candidate[a.candidate_idx].height;
        double area_b = b.candidate[b.candidate_idx].width * b.candidate[b.candidate_idx].height;
        return area_a > area_b; // 由大排到小
    });
}

void update_height_vec(node cur_node,int cur_level, vector<vector<height_info>>&height_vec){
    height_info cur_info;
    int candidate_idx=cur_node.candidate_idx;
    cur_info.start_x=cur_node.x;
    cur_info.end_x=cur_node.x+cur_node.candidate[candidate_idx].width;
    cur_info.max_y_pos=cur_node.y+cur_node.candidate[candidate_idx].height;

    if(cur_level==height_vec.size()){   //代表要增加新的一個vector<height_info>
        vector<height_info> sub_vec;
        sub_vec.push_back(cur_info);
        height_vec.push_back(sub_vec);
    }
    else{
        height_vec[cur_level].push_back(cur_info);
    }
}

double find_valid_y(node cur_node, int cur_level, vector<vector<height_info>>height_vec){//從前一個level找出valid_y
    double node_start_x=cur_node.x;
    int candidate_idx=cur_node.candidate_idx;
    double node_end_x=cur_node.x+cur_node.candidate[candidate_idx].width;
    double valid_y=0;

    if(height_vec.size()<=1&&cur_level==0) return 0;   //代表目前還沒有鋪滿一層，valid_y就從最底部開始即可
    else{  
        //從前一個level找出valid_y
        vector<height_info> sub_vec=height_vec[cur_level-1];
        
        for(int i=0; i<sub_vec.size(); i++){
            double cur_start_x=sub_vec[i].start_x;
            double cur_end_x=sub_vec[i].end_x;
            double cur_max_y_pos=sub_vec[i].max_y_pos;
            //絕對可以work，但可以稍微塞好一點
            if(node_end_x<=cur_start_x) break; //已經超出需要比較的範圍了
            else if((cur_start_x<=node_start_x&&node_start_x<cur_end_x)||(node_start_x<=cur_start_x&&cur_end_x<=node_end_x)||(cur_start_x<=node_end_x && node_end_x<=cur_end_x)){
                valid_y=max(valid_y,cur_max_y_pos);
            }
        }
        return valid_y;
    }
}

node* make_Btree(vector<node>& node_vec, vector<vector<height_info>>& height_vec,
                double& up_bound, double& right_bound, double block_area_sum) {
    if (node_vec.empty()) return nullptr;

    typedef tuple<int, double, int, node**, bool> State;
    stack<State> stk;
    node* root = nullptr;
    stk.push(State(0, 0.0, 0, &root, false));

    while (!stk.empty()) {
        State s = stk.top();
        stk.pop();

        int idx = get<0>(s);
        double cur_x = get<1>(s);
        int cur_level = get<2>(s);
        node** prev_node_link = get<3>(s);

        if (idx >= node_vec.size()) {
            *prev_node_link = nullptr;
            continue;
        }

        node* cur_node = &node_vec[idx]; // 注意這裡取 reference 的 pointer

        double node_width = cur_node->candidate[cur_node->candidate_idx].width;
        double node_height = cur_node->candidate[cur_node->candidate_idx].height;

        cur_node->x = cur_x;
        cur_node->level = cur_level;
        cur_node->y = find_valid_y(*cur_node, cur_level, height_vec);
        cur_node->xk = cur_node->x + node_width / 2.0;
        cur_node->yk = cur_node->y + node_height / 2.0;

        update_height_vec(*cur_node, cur_level, height_vec);

        right_bound = max(right_bound, cur_node->x + node_width);
        up_bound = max(up_bound, cur_node->y + node_height);

        //===for debug===
        //cout<<"block: "<<cur_node->name<<endl;
        //cout<<"right_bound: "<<right_bound<<" should>="<<cur_node->x + node_width<<endl;
        //cout<<"up_bound: "<<up_bound<<" should>="<<cur_node->y + node_height<<endl;
        //===============

        *prev_node_link = cur_node;
        cur_node->left = nullptr;
        cur_node->right = nullptr;

        if(cur_x + node_width >= sqrt(block_area_sum)){
            stk.push(State(idx + 1, 0.0, cur_level + 1, &cur_node->right, false));
        } 
        else{
            stk.push(State(idx + 1, cur_x + node_width, cur_level, &cur_node->left, true));
        }
    }
    return root;
}


void find_centroid(placement &my_placement){
    int block_num=my_placement.node_vec.size();
    my_placement.Xc=my_placement.right_bound/2;
    my_placement.Yc=my_placement.up_bound/2;
}

vector<node>sort_nodes_by_id(const vector<node>& node_vec){
    vector<node> sorted_vec= node_vec; //複製原本vector，不影響原始資料
    sort(sorted_vec.begin(),sorted_vec.end(), [](const node& a,const node& b){
        return a.id < b.id;              //按照id由小到大排序
    });
    return sorted_vec;                     //回傳排序後的新vector
}

void make_S_actual(placement &my_placement){
    vector<node> sorted_vec= sort_nodes_by_id(my_placement.node_vec);
    my_placement.S_actual.resize(sorted_vec.size());
    double Xc=my_placement.Xc;
    double Yc=my_placement.Yc;
    my_placement.S_actual[0]=(sorted_vec[0].xk-Xc)*(sorted_vec[0].xk-Xc)+(sorted_vec[0].yk-Yc)*(sorted_vec[0].yk-Yc);

    for(int i=1;i<sorted_vec.size();i++){
        my_placement.S_actual[i]= my_placement.S_actual[i-1]+
        (sorted_vec[i].xk-Xc)*(sorted_vec[i].xk-Xc)+(sorted_vec[i].yk-Yc)*(sorted_vec[i].yk-Yc);
    }
}

void make_S_ideal(placement &my_placement){
    double sum_x=0,sum_y=0,sum_x_sqr=0,sum_xy=0;//sum_x_sqr是xi平方的總和
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

void show_tree(node* root){
    if(!root) return;
    stack<node*> stk;
    stk.push(root);

    while(!stk.empty()){
        node* current=stk.top();
        stk.pop();
        int candidate_idx=current->candidate_idx;
        cout<<"name: "<<current->name<<endl;
        cout<<"candidate_idx: "<<candidate_idx<<endl;
        cout<<"level: "<<current->level<<endl;
        cout<<"x: "<<current->x<<" y: "<<current->y<<endl;
        cout<<"width: "<<current->candidate[candidate_idx].width<<" height: "<<current->candidate[candidate_idx].height<<endl;

        if(current->left)stk.push(current->left);//為了讓left優先處理，我們要先把right推進stack
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
    cout<<"cost: "<<my_placement.cost<<endl;
}

void write_file(const placement &my_placement,const string& filename){
    ofstream outFile(filename);
    if(!outFile){
        cout<<"can't open: "<<filename<<endl;
        return;
    }
    outFile << fixed;
    double rounded_width=my_placement.right_bound;
    double rounded_height=my_placement.up_bound;
    outFile<<setprecision(4)<<rounded_width*rounded_height<<endl;
    //width and height
    outFile<<setprecision(2)<<rounded_width<<" "<<rounded_height<<endl;
    outFile<<setprecision(2)<<my_placement.INL<<endl;
    vector<node> sorted_vec= sort_nodes_by_id(my_placement.node_vec);
    for(int i=0;i<sorted_vec.size();i++){
        outFile<<sorted_vec[i].name<<" "<<sorted_vec[i].x<<" "<<sorted_vec[i].y<<" (";
        macro_info info=sorted_vec[i].candidate[sorted_vec[i].candidate_idx];
        outFile<<info.width<<" "<<info.height<<" "<<info.col_multiple<<" "<<info.row_multiple<<")"<<endl;
    }
}

//===SA Related===
void copy_placement(const placement &old_placement,placement &new_placement){
    new_placement=old_placement; //gpt說可以
}

void swap_two_random_node(vector<node> &node_vec){
    int n=node_vec.size();
    if (n< 2) return;      //如果不足兩個元素就不交換
    int i= rand()%n, j= rand()%n;
    if(j==i&&i==n-1) j=i-1; // 確保選到兩個不同的索引
    else if(j==i)j=i+1; 

    swap(node_vec[i], node_vec[j]);
}

void resize_random_node(placement &cur_placement){
    int n=cur_placement.node_vec.size();
    int selected_idx=0;
    for(int i=0;i<n;i++){
        selected_idx=rand()%n;
        //確保選定的node有不只一個candidate
        if(cur_placement.node_vec[selected_idx].candidate.size()>1)break;
    }
    node my_node=cur_placement.node_vec[selected_idx];
    int prev_idx=my_node.candidate_idx;
    //扣除舊面積
    cur_placement.block_area_sum-= my_node.candidate[prev_idx].height * my_node.candidate[prev_idx].width;
    int cur_idx=rand()%my_node.candidate.size();
    while(cur_idx==prev_idx){           //確保選定的idx和原本不同
        cur_idx=rand()%my_node.candidate.size();              
    }
    my_node.candidate_idx=cur_idx;
    //加上新面積
    cur_placement.block_area_sum+=my_node.candidate[cur_idx].height* my_node.candidate[cur_idx].width;
}

void clean_placement(placement &cur_placement){
    cur_placement.height_vec.clear();
    cur_placement.height_vec.resize(0); //清空height_vec
    cur_placement.INL=0;
    cur_placement.right_bound=0;
    cur_placement.up_bound=0;
    // for(int i=0;i<cur_placement.node_vec.size();i++){
    //     cur_placement.node_vec[i]->x=0;cur_placement.node_vec[i]->y=0;
    //     cur_placement.node_vec[i]->xk=0;cur_placement.node_vec[i]->yk=0;
    //     cur_placement.node_vec[i]->left=nullptr;cur_placement.node_vec[i]->right=nullptr;
    // }
}

void Perturb_node_vec(placement &cur_placement){     //80%機率swap node，20%機率改長寬
    int r= rand()%100;                      //產生0~99的整數
    if(r<80){
        swap_two_random_node(cur_placement.node_vec);//目前正確
    }
    else{
        resize_random_node(cur_placement);//目前正確(包含面積計算)
    }
}

double find_cost(const placement &my_placement){
    double AR=max(my_placement.up_bound/my_placement.right_bound, my_placement.right_bound/my_placement.up_bound);
    double F_AR;
    if(AR>2)F_AR=AR-2;
    else if(AR<0.5)F_AR=2*(AR-0.5);
    else F_AR=0.0;
    return my_placement.up_bound*my_placement.right_bound*(1+F_AR)+8*my_placement.INL;
}

placement SA_placement(placement &my_placement){
    placement cur_placement;                    //目前placement
    copy_placement(my_placement, cur_placement);
    double temp=1000.0,cooling_rate=0.99;       //初始溫度和冷卻率

    for(int i=0;i<10000;i++){
        //step 1: 80%機率swap node_vec中相鄰的node，20%機率改變長寬，重新建立tree和placement
        //===for debug===
        // cout<<"before Perturb: "<<endl;
        // cout<<"block area sum: "<<cur_placement.block_area_sum<<endl;
        // for(int i=0;i<cur_placement.node_vec.size();i++){
        //     cout<<"block name: "<<cur_placement.node_vec[i]->name<<"candidate_idx: "<<cur_placement.node_vec[i]->candidate_idx<<endl;
        // }
        //===============
        placement new_placement;
        copy_placement(cur_placement, new_placement);//new_placement=cur_placement
        Perturb_node_vec(new_placement);
        //===for debug===
        // cout<<"after Perturb: "<<endl;
        // cout<<"block area sum: "<<cur_placement.block_area_sum<<endl;
        // for(int i=0;i<cur_placement.node_vec.size();i++){
        //     cout<<"block name: "<<cur_placement.node_vec[i]->name<<"candidate_idx: "<<cur_placement.node_vec[i]->candidate_idx<<endl;
        // }
        //===============

        //===for debug===
        //cout<<"here 4"<<endl;
        //===============
        clean_placement(new_placement);
        new_placement.root=make_Btree(new_placement.node_vec, new_placement.height_vec,new_placement.up_bound,new_placement.right_bound,new_placement.block_area_sum);
        //===for debug===
        //(到這邊都還是對的)
        //cout<<"new_placement upbound: "<<new_placement.up_bound<<" new_placement right_bound: "<<new_placement.right_bound<<endl;
        //===============

        find_centroid(new_placement);
        find_INL(new_placement);
        new_placement.cost=find_cost(new_placement);
        
        //step 2:計算cost，如果cost較低 or 抽到特定機率就把cur_placement更新
        double delta=new_placement.cost-cur_placement.cost;
        if((delta<0)||(exp(-delta / temp) > ((double)rand() / RAND_MAX))){//接受新解
            //===for debug===
            //cout<<"new cost: "<<new_placement.cost<<" cur cost: "<<cur_placement.cost<<endl;
            //===============
            copy_placement(new_placement, cur_placement);
            //===for debug===
            //cout<<"update placement: "<<endl;
            //cout<<"cur_placement right_bound: "<<cur_placement.right_bound<<endl;
            //cout<<"cur_placement up_bound: "<<cur_placement.up_bound<<endl;
            //cout<<"new_placement right_bound: "<<new_placement.right_bound<<endl;
            //cout<<"new_placement up_bound: "<<new_placement.up_bound<<endl;
            //===============
        }
        temp*=cooling_rate;
    }
    return cur_placement;
    //copy_placement(cur_placement, my_placement);
    //show_placement(my_placement);
}
//============

int main(int argc, char *argv[]){
    string input_file_name = argv[1];             
    string output_file_name= argv[2]; 
    srand(time(0));
    
    //step 1:初始化一個vector<node>
    placement my_placement;                     
    my_placement.node_vec= initialize_node_vec(my_placement,input_file_name); 
    sort_node_by_area(my_placement.node_vec);   //有sort還是比較好，尤其是形狀差異越大的case

    //step 2:建立初始解的placement(確定正確，我手畫過也用python比對過)
    //建一個tree(太寬就往上擺)
    my_placement.root= make_Btree(my_placement.node_vec, my_placement.height_vec,my_placement.up_bound,my_placement.right_bound,my_placement.block_area_sum);
    find_centroid(my_placement);
    find_INL(my_placement);
    my_placement.cost=find_cost(my_placement);
    show_placement(my_placement);
    //write_file(my_placement,output_file_name);
    
    //step 3 以SA找尋更好的解
    placement cur_placement=SA_placement(my_placement);
    write_file(cur_placement,output_file_name);
    show_placement(cur_placement);

    return 0;
}
