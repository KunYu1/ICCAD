#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <sstream>
#include<cstdlib>
#include<time.h>
#include<iomanip>
#include <unistd.h>
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

struct IOnode{
    int circuit;
    bool positvie;
    string name;
};
struct IOgroup
{
    vector<IOnode> node_list;
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

struct  Circuit
{
    vector<string> input;
    vector<string> output;
    vector<string> wire;
    vector<Gate> gates;
    vector<RNode*> roots;
};

class IOMap
{
    public:
    map<string, set<string>> iomap;
    IOMap(vector<string> iostring)
    {
        for(string str: iostring)
        {
            iomap[str] = {};
        }
    }
};

struct Group
{
	vector<string> first;
	vector<string> second;
	vector<vector<string>> groups;
	vector<vector<pair<string, bool>>> groups_sign;
	vector<pair<string, bool>> first_value;
	vector<pair<string, bool>> second_value;
	
	void assign(vector<string> a, vector<string> b, vector<vector<string>> c)
    {
        this->first = a; 
        this->second = b; 
        this->groups = c;
    }
};

struct ComparePair
{
    vector<vector<string>> first;
    vector<vector<string>> second;
};


struct ComparePairGroup
{
    ComparePair inout_org;
    ComparePair input_sup;
    ComparePair output_sup;
    map<string, int> first_map_org;
    map<string, int> second_map_org;
    map<string, int> first_map_supout;
    map<string, int> second_map_supout;
    map<string, int> first_map_supin;
    map<string, int> second_map_supin;
    void construct_map()
    {
        // Construct origin pair map
        for(int i = 0; i < this->inout_org.first.size();i++)
        {
            for(int j = 0; j < this->inout_org.first[i].size(); j++)
            {
                this->first_map_org[this->inout_org.first[i][j]] = i;
            }
        }
        for(int i = 0; i < this->inout_org.second.size();i++)
        {
            for(int j = 0; j < this->inout_org.second[i].size(); j++)
            {
                this->second_map_org[this->inout_org.second[i][j]] = i;
            }
        }

        // Construct intput support pair map
        for(int i = 0; i < this->input_sup.first.size();i++)
        {
            for(int j = 0; j < this->input_sup.first[i].size(); j++)
            {
                this->first_map_supin[this->input_sup.first[i][j]] = i;
            }
        }
        for(int i = 0; i < this->input_sup.second.size();i++)
        {
            for(int j = 0; j < this->input_sup.second[i].size(); j++)
            {
                this->second_map_supin[this->input_sup.second[i][j]] = i;
            }
        }

        // Construct output support pair map
        for(int i = 0; i < this->output_sup.first.size();i++)
        {
            for(int j = 0; j < this->output_sup.first[i].size(); j++)
            {
                this->first_map_supout[this->output_sup.first[i][j]] = i;
            }
        }
        for(int i = 0; i < this->output_sup.second.size();i++)
        {
            for(int j = 0; j < this->output_sup.second[i].size(); j++)
            {
                this->second_map_supout[this->output_sup.second[i][j]] = i;
            }
        }
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
		if(find_gateind(gates, outputs[i]) != -1)
			roots.push_back(gates[find_gateind(gates, outputs[i])]);
	}
	
	//if the input of gate is exactly the input of circuit, then point to NULL.
	//otherwise, point to the gate which generated that input.
	for(int i=0 ; i<n ; i++)
	{
		int ind1_t = find_gateind(gates, gates[i]->gate.input1);
		int ind2_t = find_gateind(gates, gates[i]->gate.input2);
		
		if(ind1_t == -1) 
            gates[i]->last1 = NULL;
		else 
            gates[i]->last1 = gates[ind1_t];

		if(ind2_t == -1) 
            gates[i]->last2 = NULL;
		else 
            gates[i]->last2 = gates[ind2_t];
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
	{
		first = graph2func(root->last1, inputs);
	}
	if(second == "" && logic != 0 && logic != 6)
	{
		second = graph2func(root->last2, inputs);
	}
	
	string ans = "";
	if(logic != 0 && logic != 6)
	{
		ans += "(";
		ans += first; ans += " ";
		ans += logic_map_rev[logic]; 
        ans += " ";
		ans += second;
		ans += ")";	
	}
	else if(logic == 0)
	{
		ans += "(not ";
		ans += first;
		ans += ")"; 
	}
	else if(logic == 6)
	{
		ans += "(buf ";
		ans += first;
		ans += ")";
	}
	
	return ans;
}

bool gate_operate(int logic, bool a, bool b)
{
	switch(logic)
	{
		case 0: // not
			return !a;
		case 1: // and
			return a & b;
		case 2: // or
			return a | b;
		case 3: // nor
			return !(a | b);
		case 4: // xnor
			return !(a != b);
		case 5: // xor
			return a != b;
		case 6: //buf
			return b;
		case 7: //nand
			return !(a & b);
		default:
			return false;
	}
}

bool circuit_operate(RNode* root, vector<pair<string, bool>> inputs)
{
	bool input1, input2, flag1, flag2;
	flag1 = false; 
    flag2 = false;
	bool ifinput2 = !(root->gate.logic==0 || root->gate.logic==6);
	
	for(int i=0 ; i<inputs.size() ; i++)
		if(root->gate.input1 == inputs[i].first)
		{
			input1 = inputs[i].second;
			flag1 = true;
		}
	for(int i=0 ; i<inputs.size() && ifinput2 ; i++)
		if(root->gate.input2 == inputs[i].first)
		{
			input2 = inputs[i].second;
			flag2 = true;
		}
	
	if(!flag1) input1 = circuit_operate(root->last1, inputs);
	if(!flag2 && ifinput2) input2 = circuit_operate(root->last2, inputs);
	if(! ifinput2) input2 = false; 

	return gate_operate(root->gate.logic, input1, input2);
}

void print_circuit(const Circuit first_circuit, const Circuit second_circuit)
{
    cout<<endl<<"First Circuit logic gate:"<<endl;
    
    for(Gate g:first_circuit.gates){
        cout<<g.name<<" "<<logic_map_rev[g.logic]<<" "<<g.input1<<" "<<g.input2<<" "<<g.output<<endl;
    }

    cout<<endl<<"Second Circuit logic gate:"<<endl;
    for(Gate g:second_circuit.gates){
        cout<<g.name<<" "<<logic_map_rev[g.logic]<<" "<<g.input1<<" "<<g.input2<<" "<<g.output<<endl;
    }
    cout<<endl;
}

void print_booleanfunc(const Circuit first_circuit, const Circuit second_circuit,const vector<RNode*> first_circuit_roots,const vector<RNode*> second_circuit_roots)
{
    cout<<"Circuit 1"<<endl;
	for(int i=0 ; i<first_circuit_roots.size() ; i++)
    	cout<<first_circuit_roots[i]->gate.output<<" = "<<graph2func(first_circuit_roots[i], first_circuit.input)<<endl;
    cout<<endl;
    
    cout<<"Circuit 2:"<<endl;
    for(int i=0 ; i<second_circuit_roots.size() ; i++)
    	cout<<second_circuit_roots[i]->gate.output<<" = "<<graph2func(second_circuit_roots[i], second_circuit.input)<<endl;
    cout<<endl;
}

void print_map(const IOMap target_map)
{
    cout<<"-----------------------------------------"<<endl;
    for(const auto& ele: target_map.iomap)
    {
        cout<<ele.first<<": ";
        for(string const& k:ele.second)
            cout<<k<<" ";
        cout<<endl<<endl;
    }
    cout<<endl;
}

void find_leave(string root, const RNode* node, IOMap& inmap, IOMap& outmap)
{
    if(node->last1 != NULL)
        find_leave(root, node->last1, inmap, outmap);
    else{
        if(inmap.iomap.find(node->gate.input1)!=inmap.iomap.end())
        {
            inmap.iomap[node->gate.input1].insert(root);
            outmap.iomap[root].insert(node->gate.input1);
        }
    }

    if(node->last2 != NULL)
        find_leave(root, node->last2, inmap, outmap);
    else{
        if(inmap.iomap.find(node->gate.input2)!=inmap.iomap.end())
        {
            inmap.iomap[node->gate.input2].insert(root);
            outmap.iomap[root].insert(node->gate.input2);
        }
    }
}

void construct_supmap(const vector<RNode*> all_root, IOMap& inmap, IOMap& outmap)
{
    for(RNode* root: all_root)
    {
        find_leave(root->gate.output, root, inmap, outmap);
    }
}

bool obsCal(vector<RNode*> circuit_root,vector<string> input, string input_a, string input_b)
{
    vector<pair<string, bool>> inputv_origin;
    vector<pair<string, bool>> inputv_a;
    vector<pair<string, bool>> inputv_b;
	for(int i=0 ; i<input.size() ; i++)
	{
        inputv_origin.push_back(make_pair(input[i], false));
        if(input[i] != input_a && input[i] != input_b)
        {
		    inputv_a.push_back(make_pair(input[i], false));
            inputv_b.push_back(make_pair(input[i], false));
        } else if(input[i] == input_a)
        {
		    inputv_a.push_back(make_pair(input[i], true));
            inputv_b.push_back(make_pair(input[i], false));            
        } else if(input[i] == input_b)
        {
		    inputv_a.push_back(make_pair(input[i], false));
            inputv_b.push_back(make_pair(input[i], true)); 
        }
	}
    // for(const auto& k: inputv_a)
    //     cout<<k.first<<" "<<k.second<<endl;
    // cout<<"Size: "<<circuit_root.size()<<endl;
	// for(int i=0 ; i<circuit_root.size() ; i++)
    // {
    //     cout<<i<<endl;
	// 	cout<<circuit_root[i]->gate.output<<" = "<<circuit_operate(circuit_root[i], inputv_a)<<endl;
    // }
    int obs_a = 0;
    int obs_b = 0;
    for(int i=0 ; i<circuit_root.size() ; i++)
    {
        bool ori = circuit_operate(circuit_root[i], inputv_origin);
        bool a_result = circuit_operate(circuit_root[i], inputv_a);
        bool b_result = circuit_operate(circuit_root[i], inputv_b);
        obs_a += (a_result == ori)? 0:1;
        obs_b += (b_result == ori)? 0:1;
    }
    return obs_a < obs_b;
}

bool compareStrings(const string& str1, const string& str2, const IOMap& iomap, map<string, vector<int>> sup_map, vector<RNode*> circuit_root, vector<string> nodes, int inout)
{
    if(iomap.iomap.at(str1).size() != iomap.iomap.at(str2).size())
    {
        return iomap.iomap.at(str1).size() < iomap.iomap.at(str2).size();
    }
    else    
    {
        for(int i = 0; i < sup_map[str1].size(); i++)
        {
            if(sup_map[str1][i] != sup_map[str2][i])
                return sup_map[str1][i] < sup_map[str2][i];
        }
        if(inout == 0)
            return obsCal(circuit_root, nodes, str1, str2);
        else
            return false;
    }
}

bool equalsup(const string& str1,const string& str2, const IOMap& iomap, map<string, vector<int>> sup_map){
    if(iomap.iomap.at(str1).size() != iomap.iomap.at(str2).size())
        return false;
    else    
    {
        for(int i = 0; i < sup_map[str1].size(); i++)
        {
            if(sup_map[str1][i] != sup_map[str2][i])
                return false;
        }
    }
    return true;
}

bool equalobs(vector<RNode*> circuit_root,vector<string> input, string input_a, string input_b){
    vector<pair<string, bool>> inputv_origin;
    vector<pair<string, bool>> inputv_a;
    vector<pair<string, bool>> inputv_b;
	for(int i=0 ; i<input.size() ; i++)
	{
        inputv_origin.push_back(make_pair(input[i], false));
        if(input[i] != input_a && input[i] != input_b)
        {
		    inputv_a.push_back(make_pair(input[i], false));
            inputv_b.push_back(make_pair(input[i], false));
        } else if(input[i] == input_a)
        {
		    inputv_a.push_back(make_pair(input[i], true));
            inputv_b.push_back(make_pair(input[i], false));            
        } else if(input[i] == input_b)
        {
		    inputv_a.push_back(make_pair(input[i], false));
            inputv_b.push_back(make_pair(input[i], true)); 
        }
	}
    int obs_a = 0;
    int obs_b = 0;
    for(int i=0 ; i<circuit_root.size() ; i++)
    {
        bool ori = circuit_operate(circuit_root[i], inputv_origin);
        bool a_result = circuit_operate(circuit_root[i], inputv_a);
        bool b_result = circuit_operate(circuit_root[i], inputv_b);
        obs_a += (a_result == ori)? 0:1;
        obs_b += (b_result == ori)? 0:1;
    }
    return obs_a == obs_b;
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
    string str;
    getline(net_f, str);
    while(getline(net_f, str)){
        string gate_name="";
        string input_1="";
        string input_2="";
        string output="";
        if(str == "endmodule")
            break;
        stringstream ins(str);
        ins>>temp_k;
        switch (logic_map[temp_k])
        {
            case 0:
                ins>>gate_name>>temp_k>>output>>temp_k>>input_1>>temp_k;
                circuit.gates.push_back(Gate(0,gate_name,input_1,input_2,output));
                break;
            case 1:
                ins>>gate_name>>temp_k>>output>>temp_k>>input_1>>temp_k>>input_2>>temp_k>>temp_k;
                circuit.gates.push_back(Gate(1,gate_name,input_1,input_2,output));
                break;
            case 2:
                ins>>gate_name>>temp_k>>output>>temp_k>>input_1>>temp_k>>input_2>>temp_k>>temp_k;
                circuit.gates.push_back(Gate(2,gate_name,input_1,input_2,output));
                break;
            case 3:
                ins>>gate_name>>temp_k>>output>>temp_k>>input_1>>temp_k>>input_2>>temp_k>>temp_k;
                circuit.gates.push_back(Gate(3,gate_name,input_1,input_2,output));
                break;
            case 4:
                ins>>gate_name>>temp_k>>output>>temp_k>>input_1>>temp_k>>input_2>>temp_k>>temp_k;
                circuit.gates.push_back(Gate(4,gate_name,input_1,input_2,output));
                break;
            case 5:
                ins>>gate_name>>temp_k>>output>>temp_k>>input_1>>temp_k>>input_2>>temp_k>>temp_k;
                circuit.gates.push_back(Gate(5,gate_name,input_1,input_2,output));
                break;
            case 6:
                ins>>gate_name>>temp_k>>output>>temp_k>>input_1>>temp_k>>temp_k;
                circuit.gates.push_back(Gate(6,gate_name,input_1,input_2,output));
                break;
            case 7:
                ins>>gate_name>>temp_k>>output>>temp_k>>input_1>>temp_k>>input_2>>temp_k>>temp_k;
                circuit.gates.push_back(Gate(7,gate_name,input_1,input_2,output));
                break;
            default:
                break;
        }
    }
    net_f.close();

    return circuit;
}

// inout = in:0, out:1 
vector<vector<string>> sort_sup(vector<RNode*> circuit_root,vector<string> nodes,IOMap primaryMap, IOMap secondaryMap, int inout){
    vector<vector<string>> sup;
    vector<string> input;
    map<string, vector<int>> node_supmap;
    for(string k:input)
        cout<<k<<endl;
    for(const auto& ele: primaryMap.iomap)
    {
        for(string k: ele.second)
            node_supmap[ele.first].push_back(secondaryMap.iomap[k].size());
        sort(node_supmap[ele.first].begin(),node_supmap[ele.first].end());    
    }

    input.assign(nodes.begin(), nodes.end());
    sort(input.begin(),input.end(), [&](string str1, string str2){
        return compareStrings(str1,str2, primaryMap, node_supmap, circuit_root, nodes, inout);
    });

// Debug
/*
    for(string k :input){
        cout<<k<<" "<<primaryMap.iomap[k].size()<<": ";
        for(int i: node_supmap[k])
            cout<<i<<" ";
        cout<<endl;
    }
*/
    vector<string> currentGroup;
    currentGroup.push_back(input[0]);
    for(int i = 1; i < input.size(); i++)
    {
        const std::string& currentString = input[i];
        const std::string& previousString = currentGroup.back();
        if(inout = 0)
        {
            if (equalsup(currentString, previousString, primaryMap,node_supmap) && equalobs(circuit_root,nodes, currentString,previousString)) {
                currentGroup.push_back(currentString);
            } else {
                sup.push_back(currentGroup);
                currentGroup.clear();
                currentGroup.push_back(currentString);
            }
        } else
        {
            if (equalsup(currentString, previousString, primaryMap,node_supmap)) {
                currentGroup.push_back(currentString);
            } else {
                sup.push_back(currentGroup);
                currentGroup.clear();
                currentGroup.push_back(currentString);
            }
        }

    }
    sup.push_back(currentGroup);
    return sup;
}

void print_sup(vector<vector<string>> sup){
    cout<<"-----------------------------------------"<<endl;
    for(int i = 0; i< sup.size();i++){
        cout << i <<" { ";
        for(string k: sup[i])
            cout<< k <<" ";
        cout<<"}"<<endl; 
    }
    cout<<endl;
}

bool verify(RNode* output1, RNode* output2, vector<pair<string, bool>> inputs)
{
	return circuit_operate(output1, inputs) == circuit_operate(output2, inputs);
}


bool make_ingroups(Circuit, Circuit, Group, Group , ComparePairGroup, map<int ,int>);
bool make_outgroups(Circuit, Circuit, Group, Group, ComparePairGroup, map<int ,int>);
bool make_sign(Circuit, Circuit, Group, Group);
bool give_value(Circuit, Circuit, Group, Group, int);
void print_matching(Group, Group);

bool make_ingroups(Circuit first_circuit, Circuit second_circuit, Group Input, Group Output, ComparePairGroup compareGroup, map<int, int> check_map)
{
	vector<string> input1 = Input.first;
	vector<string> input2 = Input.second;
	vector<vector<string>> in_groups = Input.groups;
	
	if(input1.empty() && input2.empty())
	{
		Output.groups.clear();
		return make_outgroups(first_circuit, second_circuit, Input, Output, compareGroup,check_map);
	}
	
	// try all possible matching of two circuits
	
	string in2 = input2[0];
	input2.erase(input2.begin());
	
	for(int i=0 ; i<input1.size() ; i++)
	{
        map<int, int> map_temp = check_map;
		cout<<"Make Input for: "<<i<<endl;

		vector<string> input1_t = input1;
		vector<vector<string>> in_groups_t = in_groups;
		
		vector<string> temp;
		temp.push_back(input1_t[i]);
		temp.push_back(in2);
		input1_t.erase(input1_t.begin()+i);
        // Whether the condition matches the support pair
        if(compareGroup.first_map_supin[temp[0]] != compareGroup.second_map_supin[temp[1]]){
            cout<<"error support pair!"<<endl;
            continue;            
        }

        // Whether the condition matches the comparison pair
        
        if(map_temp.find(compareGroup.first_map_org[temp[0]]) != map_temp.end())
        {
            if(map_temp[compareGroup.first_map_org[temp[0]]] != compareGroup.second_map_org[temp[1]]){
                cout<<"error comparison pair!"<<endl;
                continue;
            }
        }	else
        {
            map_temp[compareGroup.first_map_org[temp[0]]] = compareGroup.second_map_org[temp[1]];
        }

		in_groups_t.push_back(temp);

		
		Group I;
		I.assign(input1_t, input2, in_groups_t);
		if(make_ingroups(first_circuit, second_circuit, I, Output, compareGroup, map_temp)) return true;
	}
	
	return false;
}

bool make_outgroups(Circuit first_circuit, Circuit second_circuit, Group Input, Group Output, ComparePairGroup compareGroup, map<int, int> check_map)
{
	vector<string> output1 = Output.first;
	vector<string> output2 = Output.second;
	vector<vector<string>> out_groups = Output.groups;
	
	if(output1.empty() && output2.empty())
	{
		Output.groups_sign.clear();
		return make_sign(first_circuit, second_circuit, Input, Output);
	}
	
	string out2 = output2[0];
	output2.erase(output2.begin());
	
	for(int i=0 ; i<output1.size() ; i++)
	{
        map<int, int> map_temp = check_map;
		vector<string> output1_t = output1;
		vector<vector<string>> out_groups_t = out_groups;
		
		vector<string> temp;
		temp.push_back(output1_t[i]);
		temp.push_back(out2);
		output1_t.erase(output1_t.begin()+i);
		
        // Whether the condition matches the support pair
        if(compareGroup.first_map_supout[temp[0]] != compareGroup.second_map_supout[temp[1]])
            continue;

        // Whether the condition matches the comparison pair
        if(map_temp.find(compareGroup.first_map_org[temp[0]]) != map_temp.end())
        {
            if(map_temp[compareGroup.first_map_org[temp[0]]] != compareGroup.second_map_org[temp[1]]){
                cout<<"error comparison pair!"<<endl;
                continue;
            }
        }	else
        {
            map_temp[compareGroup.first_map_org[temp[0]]] = compareGroup.second_map_org[temp[1]];
        }

		out_groups_t.push_back(temp);
		
		Group O;
		O.assign(output1_t, output2, out_groups_t);
		
		if(make_outgroups(first_circuit, second_circuit, Input, O, compareGroup, check_map)) return true;
	}
	
	return false;
}

bool make_sign(Circuit first_circuit, Circuit second_circuit, Group Input, Group Output)
{
	vector<vector<string>> in_groups_t = Input.groups;
	vector<vector<string>> out_groups_t = Output.groups;
	vector<vector<pair<string, bool>>> in_groups = Input.groups_sign;
	vector<vector<pair<string, bool>>> out_groups = Output.groups_sign;
	
	if(in_groups_t.empty() && out_groups_t.empty())
	{
		Output.first = first_circuit.output;
		Output.second = second_circuit.output;
		return give_value(first_circuit, second_circuit, Input, Output, 0);
	}
	
	else if(!in_groups_t.empty())
		for(int i=0 ; i<2 ; i++)
		{
			vector<pair<string, bool>> temp;
			pair<string, bool> *t1 = new pair<string, bool>;
			pair<string, bool> *t2 = new pair<string, bool>;
			t1->first = in_groups_t[0][0];
			t2->first = in_groups_t[0][1];
			t1->second = true;
			t2->second = i==0? true : false;
			temp.push_back(*t1);
			temp.push_back(*t2);
			
			vector<vector<pair<string, bool>>> tg = in_groups;
			tg.push_back(temp);
			
			Group I = Input;
			I.groups_sign = tg;
			I.groups.erase(I.groups.begin());
			
			if(make_sign(first_circuit, second_circuit, I, Output)) return true;
		}
	
	else if(!out_groups_t.empty())
	{
		for(int i=0 ; i<2 ; i++)
		{
			vector<pair<string, bool>> temp;
			pair<string, bool> *t1 = new pair<string, bool>;
			pair<string, bool> *t2 = new pair<string, bool>;
			t1->first = out_groups_t[0][0];
			t2->first = out_groups_t[0][1];
			t1->second = true;
			t2->second = i==0 ? true : false;
			temp.push_back(*t1);
			temp.push_back(*t2);
			
			vector<vector<pair<string, bool>>> tg = out_groups;
			tg.push_back(temp);
			
			Group O = Output;
			O.groups_sign = tg;
			O.groups.erase(O.groups.begin());
			
			if(make_sign(first_circuit, second_circuit, Input, O)) return true;
		}
	}
	else
		cout<<"Error when making sign."<<endl;
	
	
	return false;
}

bool give_value(Circuit first_circuit, Circuit second_circuit, Group Input, Group Output, int step=0)
{
    static int count = 0;

	vector<vector<pair<string, bool>>> in_groups = Input.groups_sign;
	if(step == in_groups.size())
	{
    	count++;
		cout<<"Count: "<<count<<endl;
		print_matching(Input, Output);
		for(int i=0 ; i<Output.first.size() ; i++)
		{
			RNode *node1 = first_circuit.roots[find_gateind(first_circuit.roots, Output.groups_sign[i][0].first)];
			RNode *node2 = second_circuit.roots[find_gateind(second_circuit.roots, Output.groups_sign[i][1].first)];
			bool same = (Output.groups_sign[i][0].second == Output.groups_sign[i][1].second);
			
			bool result1 = circuit_operate(node1, Input.first_value);
			bool result2 = circuit_operate(node2, Input.second_value);
			
			cout<<"Result(give value): "<<result1<<" "<<result2<<"   Same = "<<same<<endl;
			if(same)
			{
				if(result1 != result2) return false;
			}
			else
			{
				if(result1 == result2) return false;
			}
		}
		print_matching(Input, Output);
		return true;
	}
	
	vector<pair<string, bool>> group = in_groups[step];
	for(int i=0 ; i<2 ; i++)
	{
		bool value = i==0? true : false;
		
		Group I = Input;
		pair<string, bool> *t1 = new pair<string, bool>;
		pair<string, bool> *t2 = new pair<string, bool>;
		t1->first = group[0].first;
		t2->first = group[1].first;
		t1->second = value;
		t2->second = group[0].second == group[1].second? value : !value;
		
		I.first_value.push_back(*t1);
		I.second_value.push_back(*t2);
		
		if(!give_value(first_circuit, second_circuit, I, Output, step+1)) return false;
	}
	
	return true;
}

void print_matching(Group Input, Group Output)
{
	vector<vector<pair<string, bool>>> in_groups = Input.groups_sign;
	vector<vector<pair<string, bool>>> out_groups = Output.groups_sign;
	
	cout<<endl<<"Input matching: "<<endl;
	for(int i=0 ; i<in_groups.size() ; i++)
	{
		string first_sign = in_groups[i][0].second ? "+" : "-";
		string second_sign = in_groups[i][1].second ? "+" : "-";
		cout<<first_sign<<setw(5)<<in_groups[i][0].first<<"  <->  "<<second_sign<<setw(5)<<in_groups[i][1].first<<endl;
	}
	cout<<endl;
	
	cout<<"Output matching: "<<endl;
	for(int i=0 ; i<out_groups.size() ; i++)
	{
		string first_sign = out_groups[i][0].second ? "+" : "-";
		string second_sign = out_groups[i][1].second ? "+" : "-";
		cout<<first_sign<<out_groups[i][0].first<<" <-> "<<second_sign<<out_groups[i][1].first<<endl;
	}
	cout<<endl;
}

int main(int argc, char *argv[]) {
    string input_name = argv[1];
    string output_name = argv[2];
    ifstream file(input_name);
    if (!file) {
        cout << "Can't open input file!" << endl;
        return 1;
    }
    ComparePairGroup compareGroup;
    // vector<Node_group> node_groups_first;
    // vector<Node_group> node_groups_second;
    Module top_module;
    string line;
    string first_net;
    string second_net;
    int cnt;
    file >> first_net;
    file >> cnt;
    for(int i = 0;i<cnt;i++){
        int cnt_g;
        vector<string> new_group;
        file>>cnt_g;
        for(int j = 0;j<cnt_g;j++){
            string node;
            file>>node;
            new_group.push_back(node);
        }
        compareGroup.inout_org.first.push_back(new_group);
    }
    file >> second_net;
    file >> cnt;
    for(int i = 0;i<cnt;i++){
        int cnt_g;
        vector<string> new_group;
        file>>cnt_g;
        for(int j = 0;j<cnt_g;j++){
            string node;
            file>>node;
            new_group.push_back(node);
        }
        compareGroup.inout_org.second.push_back(new_group);
    }

    //Load verilog data
    Circuit first_circuit, second_circuit;
    first_circuit = loadCircuit(first_net);
    second_circuit = loadCircuit(second_net);

    // print_circuit(first_circuit, second_circuit);

    IOMap first_inout(first_circuit.input);
    IOMap first_outin(first_circuit.output);
    IOMap second_inout(second_circuit.input);
    IOMap second_outin(second_circuit.output);
    
    vector<RNode*> first_circuit_roots = create_graph(first_circuit.gates, first_circuit.input, first_circuit.output);
    vector<RNode*> second_circuit_roots = create_graph(second_circuit.gates, second_circuit.input, second_circuit.output);
	
    first_circuit.roots = first_circuit_roots;
    second_circuit.roots = second_circuit_roots;

//  Debug Boolean function

/*
    print_booleanfunc(first_circuit, second_circuit,first_circuit_roots, second_circuit_roots);
*/
    print_booleanfunc(first_circuit, second_circuit,first_circuit_roots, second_circuit_roots);
    construct_supmap(first_circuit_roots, first_inout, first_outin);
    construct_supmap(second_circuit_roots, second_inout, second_outin);
    
    // obsCal(first_circuit, first_circuit_roots, "n4", "n5");
//  Debug in/out map

    // cout<<"Circuit1"<<endl;
    // print_map(first_inout);
    // print_map(first_outin);
    // cout<<"Circuit2"<<endl;
    // print_map(second_inout);
    // print_map(second_outin);


    cout<<"Circuit1"<<endl;
    vector<vector<string>> first_insup = sort_sup(first_circuit_roots,first_circuit.input, first_inout, first_outin, 1);
    print_sup(first_insup);
    vector<vector<string>> first_outsup = sort_sup(first_circuit_roots,first_circuit.output, first_outin, first_inout, 1);
    print_sup(first_outsup);
    cout<<"Circuit2"<<endl;
    vector<vector<string>> second_insup = sort_sup(second_circuit_roots,second_circuit.input, second_inout, second_outin, 1);
    print_sup(second_insup);
    vector<vector<string>> second_outsup = sort_sup(second_circuit_roots,second_circuit.output, second_outin, second_inout, 1);
    print_sup(second_outsup);
    
    compareGroup.input_sup.first = first_insup;
    compareGroup.input_sup.second = second_insup;
    compareGroup.output_sup.first = first_outsup;
    compareGroup.output_sup.second = second_outsup;
    compareGroup.construct_map();

// 	Boolean function matching

    int conv = strtol(argv[3],NULL, 10);
    if(conv)
    {
        cout<<"Comparing..."<<endl;
        
        Group Input, Output;
        map<int, int> org_map;
        vector<vector<string>> at, bt;
        Input.assign(first_circuit.input, second_circuit.input, at);
        Output.assign(first_circuit.output, second_circuit.output, bt);
        bool ans = make_ingroups(first_circuit, second_circuit, Input, Output, compareGroup, org_map);
        
        cout<<"Result = "<<ans<<endl;
    }



// Output file
    vector<IOgroup> outGrouplist;
    vector<IOgroup> inGrouplist;
    vector<IOgroup> constGrouplist;
    
    ofstream outfile;
    outfile.open(output_name);
    for(IOgroup k: inGrouplist)
    {
        outfile<<"INGROUP"<<endl;
        for(IOnode n: k.node_list)
        {
            if(n.positvie)
                outfile<<n.circuit<<" + "<<n.name<<endl;
            else    
                outfile<<n.circuit<<" - "<<n.name<<endl;
        }
        outfile<<"END"<<endl;
    }

    for(IOgroup k: outGrouplist)
    {
        outfile<<"OUTGROUP"<<endl;
        for(IOnode n: k.node_list)
        {
            if(n.positvie)
                outfile<<n.circuit<<" + "<<n.name<<endl;
            else    
                outfile<<n.circuit<<" - "<<n.name<<endl;
        }
    }

    for(IOgroup k: constGrouplist)
    {
        outfile<<"CONSTGROUP"<<endl;
        for(IOnode n: k.node_list)
        {
            if(n.positvie)
                outfile<<" + "<<n.name<<endl;
            else    
                outfile<<" - "<<n.name<<endl;
        }
        outfile<<"END"<<endl;
    }
    outfile.close();


    cout<<"Finished."<<endl;
        
    return 0;
}




