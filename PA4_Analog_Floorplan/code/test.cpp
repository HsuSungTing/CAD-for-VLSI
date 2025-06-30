#include <iostream>
#include <cmath>  // 包含 sqrt()
#include <algorithm> // std::max
#include <vector>
#include <memory>  

using namespace std;

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
    int level;    //目前node被擺在第幾層(從第0層開始)
    double x,y;   //一個block左下角的座標
    double Xc,Yc; //一個block的質心座標
    node_ptr left;  //left child，右邊的cell
    node_ptr right; //right child，上方的cell
    int candidate_idx;
    vector<macro_info>candidate;
};

struct height_info{
    double start_x,end_x; //起始與終止的橫坐標
    double max_y_pos; //高度(下一層在擺放的時候要遷就這個高度)
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
    M0->id=0; M0->name="MM0"; M0->candidate_idx=0; M0->candidate.push_back(build_macro(1.1,1.1,1,1));
    node_vec.push_back(M0);

    node_ptr M1=make_shared<node>();
    M1->id=1; M1->name="MM1"; M1->candidate_idx=0; M1->candidate.push_back(build_macro(1.5,2.2,2,1));
    node_vec.push_back(M1);

    node_ptr M2=make_shared<node>();
    M2->id=2; M2->name="MM2"; M2->candidate_idx=0; M2->candidate.push_back(build_macro(1.7,3.3,1,2));
    node_vec.push_back(M2);

    node_ptr M3=make_shared<node>();
    M3->id=3; M3->name="MM3"; M3->candidate_idx=0; M3->candidate.push_back(build_macro(3.1,1.5,2,1));
    node_vec.push_back(M3);

    node_ptr M4=make_shared<node>();
    M4->id=4; M4->name="MM4"; M4->candidate_idx=0; M4->candidate.push_back(build_macro(2.93,11.08,2,2));
    node_vec.push_back(M4);

    node_ptr M5 = make_shared<node>();
    M5->id = 5; M5->name = "MM5"; M5->candidate_idx = 0;
    M5->candidate.push_back(build_macro(3.5, 3.5, 1, 1));
    node_vec.push_back(M5);

    node_ptr M6 =make_shared<node>();
    M6->id = 6; M6->name = "MM6"; M6->candidate_idx = 0;
    M6->candidate.push_back(build_macro(2.2, 16.3, 1, 2));
    node_vec.push_back(M6);

    node_ptr M7 =make_shared<node>();
    M7->id = 7; M7->name = "MM7"; M7->candidate_idx = 0;
    M7->candidate.push_back(build_macro(8.1, 1.5, 5, 1));
    node_vec.push_back(M7);

    node_ptr M8 =make_shared<node>();
    M8->id = 7; M8->name = "MM8"; M8->candidate_idx = 0;
    M8->candidate.push_back(build_macro(18.1, 5.1, 5, 1));
    node_vec.push_back(M8);

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

node_ptr make_Btree(vector<node_ptr>node_vec,int idx, double cur_x, int cur_level, double area, vector<vector<height_info>> &height_vec,double &up_bound,double &right_bound){
    //cur_x是擺放目前block的位置(左邊)
    if(idx<node_vec.size()){
        node_ptr cur_node= node_vec[idx];
        double node_width= cur_node->candidate[0].width;
        double node_height=cur_node->candidate[0].height;

        cur_node->x=cur_x;  //把node往右對齊
        cur_node->level=cur_level;
        cur_node->y=find_valid_y(cur_node,cur_level,height_vec);
        update_height_vec(cur_node,cur_level, height_vec);
        right_bound=max(right_bound,cur_x+node_width);
        up_bound=max(up_bound,cur_node->y+node_height);

        cur_node->left = nullptr;
        cur_node->right = nullptr;

        if(cur_x > sqrt(area)){//如果目前擺放cell的位置已經過於右邊
            //往上擺
            cur_node->right=make_Btree(node_vec,idx+1, 0,cur_level+1,area+node_width*node_height,height_vec,up_bound,right_bound);
        }
        else{ 
            //往右擺
            cur_node->left=make_Btree(node_vec,idx+1,cur_x+node_width,cur_level,area+node_width*node_height,height_vec,up_bound,right_bound);
        }
        return cur_node;
    }
    return nullptr;
}

void show_tree(node_ptr root){
    if(root){
        cout<<"name: "<<root->name<<endl;
        cout<<"level: "<<root->level<<endl;
        cout<<"x: "<<root->x<<" y: "<<root->y<<endl;
        cout<<"width: "<<root->candidate[0].width<<" height: "<<root->candidate[0].height<<endl;
        if(root->right)show_tree(root->right);
        if(root->left)show_tree(root->left);
    }
}
int main(){
    //初始化一個vector<node*>
    vector<node_ptr> node_vec = initialization();
    vector<vector<height_info>> height_vec;
    //建一個tree(太寬就往上擺)
    double up_bound=0,right_bound=0;
    node_ptr root = make_Btree(node_vec, 0, 0, 0, 0.0, height_vec,up_bound,right_bound);
    cout<<"up_bound: "<<up_bound<<" right_bound: "<<right_bound<<endl;
    show_tree(root);

    return 0;
}
