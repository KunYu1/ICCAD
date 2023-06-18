#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
using namespace std;

map<string, int> logic_map{
    {"not",  0},
    {"and",  1},
    {"or",   2},
    {"nor",  3},
    {"xnor", 4},
    {"xor",  5},
    {"buf",  6},
    {"nand", 7},
};

struct Gate {
    int logic;
    string name;
    string input1;
    string input2;
    string output;
    Gate(int logic, string name,string input1,string input2,string output){
        this->logic = logic;
        this->name = name;
        this->input1 = input1;
        this->input2 = input2;
        this->output = output;
    }
};

struct Module {
    string name;
    vector<string> inputs;
    vector<Gate> gates;
};
struct Node_group
{
    int n;
    vector<string> nodes;
};
struct  Circuit
{
    vector<string> input;
    vector<string> output;
    vector<string> wire;
    vector<Gate> gates;
};


int main() {
    string input_name;
    cout << "Please input your input ford name:";
    cin>>input_name;
    ifstream file(input_name+"/input");
    if (!file) {
        cout << "Can't open input file!" << endl;
        return 1;
    }
    vector<Node_group> node_groups_first;
    vector<Node_group> node_groups_second;
    Module top_module;
    string line;
    string first_net;
    string second_net;
    int cnt;
    file >> first_net;
    file >> cnt;
    for(int i = 0;i<cnt;i++){
        int cnt_g;
        Node_group new_group;
        new_group.n = cnt_g;
        file>>cnt_g;
        for(int j = 0;j<cnt_g;j++){
            string node;
            file>>node;
            new_group.nodes.push_back(node);
        }
        node_groups_first.push_back(new_group);
    }
    file >> second_net;
    file >> cnt;
    for(int i = 0;i<cnt;i++){
        int cnt_g;
        Node_group new_group;
        new_group.n = cnt_g;
        file>>cnt_g;
        for(int j = 0;j<cnt_g;j++){
            string node;
            file>>node;
            new_group.nodes.push_back(node);
        }
        node_groups_second.push_back(new_group);
    }
    // cout<<second_net<<endl;
    // for(Node_group k:node_groups_second){
    //     cout<<k.n<<" ";
    //     for(string i:k.nodes)
    //         cout<<i<<" ";
    //     cout<<endl;
    // }

    ifstream first_net_f(input_name+"/"+first_net);
    ifstream second_net_f(input_name+"/"+second_net);
    string temp_k;
    string input_t, temp_t, output_t, wire_t;
    //first
    Circuit first_circuit;
    while(first_net_f>>temp_k){
        if(temp_k == "input")
            break;
    }
    while(first_net_f >> input_t>>temp_t){
        first_circuit.input.push_back(input_t);
        if(temp_t == ";")
            break;
    }
    first_net_f>>temp_k;
    while(first_net_f >> output_t>>temp_t){
        first_circuit.output.push_back(output_t);
        if(temp_t == ";")
            break;
    }
    first_net_f>>temp_k;
    while(first_net_f >> wire_t>>temp_t){
        first_circuit.wire.push_back(wire_t);
        if(temp_t == ";")
            break;
    }
    while(first_net_f >> temp_k){
        string gate_name="";
        string input_1="";
        string input_2="";
        string output="";
        if(temp_k == "endmodule")
            break;
        switch (logic_map[temp_k])
        {
            case 0:
                first_net_f>>gate_name>>temp_k>>input_1>>temp_k>>output>>temp_k>>temp_k;
                first_circuit.gates.push_back(Gate(0,gate_name,input_1,input_2,output));
                break;
            case 1:
                first_net_f>>gate_name>>temp_k>>input_1>>temp_k>>input_2>>temp_k>>output>>temp_k>>temp_k;
                first_circuit.gates.push_back(Gate(1,gate_name,input_1,input_2,output));
                break;
            case 2:
                first_net_f>>gate_name>>temp_k>>input_1>>temp_k>>input_2>>temp_k>>output>>temp_k>>temp_k;
                first_circuit.gates.push_back(Gate(2,gate_name,input_1,input_2,output));
                break;
            case 3:
                first_net_f>>gate_name>>temp_k>>input_1>>temp_k>>input_2>>temp_k>>output>>temp_k>>temp_k;
                first_circuit.gates.push_back(Gate(3,gate_name,input_1,input_2,output));
                break;
            case 4:
                first_net_f>>gate_name>>temp_k>>input_1>>temp_k>>input_2>>temp_k>>output>>temp_k>>temp_k;
                first_circuit.gates.push_back(Gate(4,gate_name,input_1,input_2,output));
                break;
            case 5:
                first_net_f>>gate_name>>temp_k>>input_1>>temp_k>>input_2>>temp_k>>output>>temp_k>>temp_k;
                first_circuit.gates.push_back(Gate(5,gate_name,input_1,input_2,output));
                break;
            case 6:
                first_net_f>>gate_name>>temp_k>>input_1>>temp_k>>output>>temp_k>>temp_k;
                first_circuit.gates.push_back(Gate(6,gate_name,input_1,input_2,output));
                break;
            case 7:
                first_net_f>>gate_name>>temp_k>>input_1>>temp_k>>input_2>>temp_k>>output>>temp_k>>temp_k;
                first_circuit.gates.push_back(Gate(7,gate_name,input_1,input_2,output));
                break;
            default:
                break;
        }
    }

    //second
    Circuit second_circuit;
    while(second_net_f>>temp_k){
        if(temp_k == "input")
            break;
    }
    while(second_net_f >> input_t>>temp_t){
        second_circuit.input.push_back(input_t);
        if(temp_t == ";")
            break;
    }
    second_net_f>>temp_k;
    while(second_net_f >> output_t>>temp_t){
        second_circuit.output.push_back(output_t);
        if(temp_t == ";")
            break;
    }
    second_net_f>>temp_k;
    while(second_net_f >> wire_t>>temp_t){
        second_circuit.wire.push_back(wire_t);
        if(temp_t == ";")
            break;
    }
    while(second_net_f >> temp_k){
        string gate_name="";
        string input_1="";
        string input_2="";
        string output="";
        if(temp_k == "endmodule")
            break;
        switch (logic_map[temp_k])
        {
            case 0:
                second_net_f>>gate_name>>temp_k>>input_1>>temp_k>>output>>temp_k>>temp_k;
                second_circuit.gates.push_back(Gate(0,gate_name,input_1,input_2,output));
                break;
            case 1:
                second_net_f>>gate_name>>temp_k>>input_1>>temp_k>>input_2>>temp_k>>output>>temp_k>>temp_k;
                second_circuit.gates.push_back(Gate(1,gate_name,input_1,input_2,output));
                break;
            case 2:
                second_net_f>>gate_name>>temp_k>>input_1>>temp_k>>input_2>>temp_k>>output>>temp_k>>temp_k;
                second_circuit.gates.push_back(Gate(2,gate_name,input_1,input_2,output));
                break;
            case 3:
                second_net_f>>gate_name>>temp_k>>input_1>>temp_k>>input_2>>temp_k>>output>>temp_k>>temp_k;
                second_circuit.gates.push_back(Gate(3,gate_name,input_1,input_2,output));
                break;
            case 4:
                second_net_f>>gate_name>>temp_k>>input_1>>temp_k>>input_2>>temp_k>>output>>temp_k>>temp_k;
                second_circuit.gates.push_back(Gate(4,gate_name,input_1,input_2,output));
                break;
            case 5:
                second_net_f>>gate_name>>temp_k>>input_1>>temp_k>>input_2>>temp_k>>output>>temp_k>>temp_k;
                second_circuit.gates.push_back(Gate(5,gate_name,input_1,input_2,output));
                break;
            case 6:
                second_net_f>>gate_name>>temp_k>>input_1>>temp_k>>output>>temp_k>>temp_k;
                second_circuit.gates.push_back(Gate(6,gate_name,input_1,input_2,output));
                break;
            case 7:
                second_net_f>>gate_name>>temp_k>>input_1>>temp_k>>input_2>>temp_k>>output>>temp_k>>temp_k;
                second_circuit.gates.push_back(Gate(7,gate_name,input_1,input_2,output));
                break;
            default:
                break;
        }
    }
    cout<<"First Circuit logic gate:"<<endl;
    for(Gate g:first_circuit.gates){
        cout<<g.name<<" "<<g.logic<<" "<<g.input1<<" "<<g.input2<<" "<<g.output<<endl;
    }

    cout<<"Second Circuit logic gate:"<<endl;
    for(Gate g:second_circuit.gates){
        cout<<g.name<<" "<<g.logic<<" "<<g.input1<<" "<<g.input2<<" "<<g.output<<endl;
    }
    first_net_f.close();
    second_net_f.close();
    return 0;
}




