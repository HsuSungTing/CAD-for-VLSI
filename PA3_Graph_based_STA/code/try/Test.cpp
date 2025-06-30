#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <queue>

using namespace std;
//應該有更好的方法，和昀翰討論

struct gate_node {
    int delay;
    int start_time;
    int indegree;
    string gate_name;
    unordered_set<struct gate_node*> predecessor_set;  //可以用vector
    unordered_set<struct gate_node*> successor_set;    //可以用vector
};

void make_succeccor(unordered_map<string, struct gate_node*> &Gate_Netlist){
    for(auto& it:Gate_Netlist){
        struct gate_node* gate_node=it.second;
        for(auto& pair:it.second->predecessor_set){
            Gate_Netlist[pair->gate_name]->successor_set.insert(gate_node);
        }
    }
}

void bulid_Gate_Netlist(unordered_map<string, struct gate_node*> &Gate_Netlist){
    vector<string> gate_names = {"G1", "G2", "G3", "G4", "G5", "G6", "G7", "G8"};

    for (int i=0; i<gate_names.size(); i++){
        gate_node* gate = new gate_node();
        gate->gate_name=gate_names[i];
        gate->start_time=-1;
        gate->indegree=0;
        gate->delay=(i + 1)*2;
        Gate_Netlist[gate->gate_name]=gate;
    }
    Gate_Netlist["G1"]->indegree=0;

    Gate_Netlist["G2"]->predecessor_set.insert(Gate_Netlist["G1"]);
    Gate_Netlist["G2"]->indegree=1;

    Gate_Netlist["G3"]->predecessor_set.insert(Gate_Netlist["G2"]);
    Gate_Netlist["G3"]->predecessor_set.insert(Gate_Netlist["G1"]);
    Gate_Netlist["G3"]->indegree=2;

    Gate_Netlist["G4"]->predecessor_set.insert(Gate_Netlist["G3"]);
    Gate_Netlist["G4"]->predecessor_set.insert(Gate_Netlist["G1"]);
    Gate_Netlist["G4"]->indegree=2;

    Gate_Netlist["G5"]->predecessor_set.insert(Gate_Netlist["G2"]);
    Gate_Netlist["G5"]->predecessor_set.insert(Gate_Netlist["G4"]);
    Gate_Netlist["G5"]->indegree=2;

    Gate_Netlist["G6"]->predecessor_set.insert(Gate_Netlist["G3"]);
    Gate_Netlist["G6"]->predecessor_set.insert(Gate_Netlist["G5"]);
    Gate_Netlist["G6"]->indegree=2;

    Gate_Netlist["G7"]->predecessor_set.insert(Gate_Netlist["G4"]);
    Gate_Netlist["G7"]->predecessor_set.insert(Gate_Netlist["G6"]);
    Gate_Netlist["G7"]->indegree=2;

    Gate_Netlist["G8"]->predecessor_set.insert(Gate_Netlist["G4"]);
    Gate_Netlist["G8"]->predecessor_set.insert(Gate_Netlist["G7"]);
    Gate_Netlist["G8"]->indegree=2;

    make_succeccor(Gate_Netlist);
}

void show_Netlist(unordered_map<string, struct gate_node*> Gate_Netlist){
    for (const auto& pair : Gate_Netlist) {
        cout << "Gate: "<<pair.first<<", delay: "<<pair.second->delay<<", start_time: "<<pair.second->start_time<<", predecessor: ";
        for (auto it : pair.second->predecessor_set) {
            cout << it->gate_name<< " ";
        }
        // cout<<"successor: ";
        // for (auto  it: pair.second->successor_set) {
        //     cout << it->gate_name << " ";
        // }
        cout << endl;
    }
}

void find_start_time(unordered_map<string, struct gate_node*> &Gate_Netlist, unordered_map<struct gate_node*, struct gate_node*> &longest_path, struct gate_node* &final_node){
    queue<struct gate_node*> ASAP_Q;

    for(const auto& it:Gate_Netlist){
        if(it.second->predecessor_set.size()==0){
            it.second->start_time=0;    //放入queue，start_time=0;
            ASAP_Q.push(it.second);
            final_node=it.second;
        }
    }

    while (ASAP_Q.empty()==false){      //每次從queue中pop elements
        struct gate_node* gate=ASAP_Q.front();
        ASAP_Q.pop();

        //該element被schedule，所以從該gate所有sucessor的predessor中消失
        for(auto it:gate->successor_set){
            struct gate_node* successor_node=it;
            if(successor_node->indegree>0){//理論上一定會成立
                successor_node->indegree--; 

                if(successor_node->indegree==0){//放入Queue並計算start_time=max(successor.start_time+successor.delay)
                    int max_start_time=0;
                    for(auto pre:successor_node->predecessor_set){
                        if(max_start_time < pre->start_time+pre->delay){
                            longest_path[successor_node]=pre;
                            max_start_time=pre->start_time+pre->delay;
                        }
                    }
                    successor_node->start_time=max_start_time;
                    ASAP_Q.push(successor_node);

                    //更新最後的gate node
                    if(final_node->start_time+final_node->delay<successor_node->start_time+successor_node->delay){
                        final_node=successor_node;
                    }
                }
            }
        }
    }

}

void print_longest_path(unordered_map<struct gate_node*, struct gate_node*>longest_path,struct gate_node* final_node){
    cout<<"final node is: "<<final_node->gate_name<<endl;
    for(auto it:longest_path){
        cout<<"current node: "<<it.first->gate_name<<"  previous node: "<<it.second->gate_name<<endl;
    }
}

int main(){
    unordered_map<string, struct gate_node*> Gate_Netlist;
    bulid_Gate_Netlist(Gate_Netlist);
    unordered_map<struct gate_node*, struct gate_node*>longest_path;
    struct gate_node* final_node;       //找到longest path的結尾
    find_start_time(Gate_Netlist,longest_path,final_node);
    
    show_Netlist(Gate_Netlist);
    print_longest_path(longest_path,final_node);
    return 0;
}
