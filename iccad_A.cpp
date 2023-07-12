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

map<int, string> logic_map_rev{
	{0, "not"},
	{1, "and"},
	{2, "or"},
	{3, "nor"},
	{4, "xnor"},
	{5, "xor"},
	{6, "buf"},
	{7, "nand"},
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
    Gate(const Gate &g)
    {
    	this->logic = g.logic;
    	this->name = g.name;
    	this->input1 = g.input1;
    	this->input2 = g.input2;
    	this->output = g.output;
	}
    Gate() {}
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

struct RNode
{
	Gate gate;
	RNode *last1;
	RNode *last2;
	RNode(Gate gate)
	{
		this->gate = gate;
		this->last1 = NULL;
		this->last2 = NULL;
	}
};

// Given the output name, find where the output comes from
int find_gateind(vector<RNode*> gates, string output)
{
	for(int i=0 ; i<gates.size() ; i++)
	{
		if(gates[i]->gate.output == output) return i;
	}
	return -1;
}

RNode* create_graph_recur(string, vector<RNode*>, bool*, vector<string>);

vector<RNode*> create_graph(vector<Gate> gates_t, vector<string> inputs, vector<string> outputs)
{
	// Use RNode to store the gates information
	vector<RNode*> gates;
	int n = 0;
	for(Gate g:gates_t)
	{
		RNode *temp = new RNode(g);
		gates.push_back(temp);
		n++;
	}
	
	// using 'used' to store which gate has been used
	bool *used = new bool [n];
	for(int i=0 ; i<n ; i++)
		used[i] = false;
	
	// initialize the roots of the diagram
	vector<RNode*> roots;
	for(int i=0 ; i<outputs.size() ; i++)
	{
//		cout<<"|"<<outputs[i]<<"|"<<endl;
		if(find_gateind(gates, outputs[i]) != -1)
			roots.push_back(gates[find_gateind(gates, outputs[i])]);
//		else cout<<"Find gate index equal to -1.";
	}
	
	//if the input of gate is exactly the input of circuit, then point to NULL.
	//otherwise, point to the gate which generated that input.
	for(int i=0 ; i<n ; i++)
	{
		int ind1_t = find_gateind(gates, gates[i]->gate.input1);
		int ind2_t = find_gateind(gates, gates[i]->gate.input2);
//		cout<<ind1_t<<" "<<ind2_t<<endl;
		
		if(ind1_t == -1) gates[i]->last1 = NULL;
		else gates[i]->last1 = gates[ind1_t];
		if(ind2_t == -1) gates[i]->last2 = NULL;
		else gates[i]->last2 = gates[ind2_t];
	}
	
	return roots;
}

string graph2func(RNode *root, vector<string> inputs)
{
	int logic = root->gate.logic;
	string first = "";	
	string second = "";
	
	for(int i=0 ; i<inputs.size() ; i++)
	{
		if(root->gate.input1 == inputs[i]) first = inputs[i];
		if(root->gate.input2 == inputs[i]) second = inputs[i];
	}
	
	if(first == "")
		first = graph2func(root->last1, inputs);
	if(second == "" && logic != 0)
		second = graph2func(root->last2, inputs);
	
	string ans = "";
	if(logic != 0)
	{
		ans += "(";
		ans += first; ans += " ";
		ans += logic_map_rev[logic]; ans += " ";
		ans += second;
		ans += ")";	
	}
	else
	{
		ans += "(not ";
		ans += first;
		ans += ")"; 
	}
		
	return ans;
}
Circuit loadCircuit(string file_name){
    string temp_k;
    string input_t, temp_t, output_t, wire_t;
    Circuit circuit;
    ifstream net_f(file_name);
    while(net_f>>temp_k){
        if(temp_k == "input")
            break;
    }
    while(net_f >> input_t>>temp_t){
        circuit.input.push_back(input_t);
        if(temp_t == ";")
            break;
    }
    net_f>>temp_k;
    while(net_f >> output_t>>temp_t){
        circuit.output.push_back(output_t);
        if(temp_t == ";")
            break;
    }
    net_f>>temp_k;
    while(net_f >> wire_t>>temp_t){
        circuit.wire.push_back(wire_t);
        if(temp_t == ";")
            break;
    }
    while(net_f >> temp_k){
        string gate_name="";
        string input_1="";
        string input_2="";
        string output="";
        if(temp_k == "endmodule")
            break;
        switch (logic_map[temp_k])
        {
            case 0:
                net_f>>gate_name>>temp_k>>output>>temp_k>>input_1>>temp_k>>temp_k;
                circuit.gates.push_back(Gate(0,gate_name,input_1,input_2,output));
                break;
            case 1:
                net_f>>gate_name>>temp_k>>output>>temp_k>>input_1>>temp_k>>input_2>>temp_k>>temp_k;
                circuit.gates.push_back(Gate(1,gate_name,input_1,input_2,output));
                break;
            case 2:
                net_f>>gate_name>>temp_k>>output>>temp_k>>input_1>>temp_k>>input_2>>temp_k>>temp_k;
                circuit.gates.push_back(Gate(2,gate_name,input_1,input_2,output));
                break;
            case 3:
                net_f>>gate_name>>temp_k>>output>>temp_k>>input_1>>temp_k>>input_2>>temp_k>>temp_k;
                circuit.gates.push_back(Gate(3,gate_name,input_1,input_2,output));
                break;
            case 4:
                net_f>>gate_name>>temp_k>>output>>temp_k>>input_1>>temp_k>>input_2>>temp_k>>temp_k;
                circuit.gates.push_back(Gate(4,gate_name,input_1,input_2,output));
                break;
            case 5:
                net_f>>gate_name>>temp_k>>output>>temp_k>>input_1>>temp_k>>input_2>>temp_k>>temp_k;
                circuit.gates.push_back(Gate(5,gate_name,input_1,input_2,output));
                break;
            case 6:
                net_f>>gate_name>>temp_k>>output>>temp_k>>input_1>>temp_k>>temp_k;
                circuit.gates.push_back(Gate(6,gate_name,input_1,input_2,output));
                break;
            case 7:
                net_f>>gate_name>>temp_k>>output>>temp_k>>input_1>>temp_k>>input_2>>temp_k>>temp_k;
                circuit.gates.push_back(Gate(7,gate_name,input_1,input_2,output));
                break;
            default:
                break;
        }
    }
    net_f.close();

    return circuit;
}
int main() {
    string input_name;
//    cout << "Please input your input ford name:";
//    cin>>input_name;
	input_name = "1";
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

    //Load verilog data
    Circuit first_circuit, second_circuit;
    first_circuit = loadCircuit(input_name+"/"+first_net);
    second_circuit = loadCircuit(input_name+"/"+second_net);

  
    cout<<endl<<"First Circuit logic gate:"<<endl;
    
    for(Gate g:first_circuit.gates){
        cout<<g.name<<" "<<logic_map_rev[g.logic]<<" "<<g.input1<<" "<<g.input2<<" "<<g.output<<endl;
    }

    cout<<endl<<"Second Circuit logic gate:"<<endl;
    for(Gate g:second_circuit.gates){
        cout<<g.name<<" "<<logic_map_rev[g.logic]<<" "<<g.input1<<" "<<g.input2<<" "<<g.output<<endl;
    }
    cout<<endl;
    
    vector<RNode*> first_circuit_roots = create_graph(first_circuit.gates, first_circuit.input, first_circuit.output);
    vector<RNode*> second_circuit_roots = create_graph(second_circuit.gates, second_circuit.input, second_circuit.output);
    
	cout<<"Circuit 1"<<endl;
	for(int i=0 ; i<first_circuit_roots.size() ; i++)
    	cout<<first_circuit_roots[i]->gate.output<<" = "<<graph2func(first_circuit_roots[i], first_circuit.input)<<endl;
    cout<<endl;
    
    cout<<"Circuit 2:"<<endl;
    for(int i=0 ; i<second_circuit_roots.size() ; i++)
    	cout<<second_circuit_roots[i]->gate.output<<" = "<<graph2func(second_circuit_roots[i], second_circuit.input)<<endl;
    cout<<endl;
    
    return 0;
}




