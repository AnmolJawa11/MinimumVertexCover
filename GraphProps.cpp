#include <queue> 
#include<iostream>
#include<vector>
#include<climits>
#include<memory>
using std::vector;
using std::queue;
using std::cout;
#include <vector>
#include <algorithm>
#include <chrono>
#include "minisat/core/Solver.h"
#include "minisat/core/SolverTypes.h"
using std::unique_ptr;
class GraphImpl 
{
public:
	int graph_verts = 0;
	vector<vector<int>> formedGraph;
	GraphImpl() {}
	GraphImpl(int num) {
		formedGraph.resize(num);
		graph_verts = num;
		}
	vector<vector<int>> getGraph() {
		return this->formedGraph;
	}
	void addEdge(int from, int to) {
		formedGraph[from].push_back(to);
		formedGraph[to].push_back(from);
		return;
	}
	void formGraph(vector<vector<int>> edgePairs) {
		int size = (int)edgePairs.size();
		for (int i = 0; i < size; i++) {
			formedGraph[edgePairs[i][0]].push_back(edgePairs[i][1]);
			formedGraph[edgePairs[i][1]].push_back(edgePairs[i][0]);
		}

	}

	int vertsGetter() {
		return graph_verts;
	}
};

class GraphAnalyser : public GraphImpl {
private:
	vector<int> predecessor;
	int source=0;
	int destination=0;
	int verts = graph_verts;
	vector<vector<int>> input_graph;
	int vertices;
    

public:
	GraphAnalyser() {
	//empty constructor 
	}

	void setInputGraph(vector<vector<int>> input_graph){
		this->input_graph=input_graph;
	}
	GraphAnalyser(int source,int destination) {
		this->source = source;
		this->destination = destination;
	}
	void initializeGraph(int entered_verts, vector<vector<int>> formedGraph) {
		graph_verts = entered_verts;
        this->verts=entered_verts;
        this->vertices=entered_verts;
		vertices=entered_verts;
		this->predecessor.resize(graph_verts);
		this->formedGraph = formedGraph;
	}


    unique_ptr<Minisat::Solver> solver;
	unique_ptr<Minisat::Solver> solver2;

std::vector<int>  vertex_cover() {
    solver.reset(new Minisat::Solver());
    solver2.reset(new Minisat::Solver());
    int high_pt = verts;
    int mid_pt;
    int vc_results[verts];  
    std:vector<int> nullVector;
    if (formedGraph.empty()) {
        std::cerr << "Error: no graph, no result, niklo yaha se" << std::endl;
        return nullVector;
    }
    int low_pt = 0;
        
    std::vector<int> result_paths[verts];
    std::fill_n(vc_results, verts, -1);

    while (low_pt <= high_pt) {
        mid_pt = (low_pt+high_pt)/2;

        //Minisat::Solver solver2;
        auto start = std::chrono::system_clock::now();
        vc_results[mid_pt] = mysterySolver( mid_pt);
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> diff = end-start;

        if (vc_results[mid_pt]) {
            result_paths[mid_pt] = resolveSatPath(solver, mid_pt);
        }
          
        // // If UNSAT and result[k+1] are SAT, the minimum is found
        // if (results[mid_pt] == 0 && results[mid_pt+1] == 1 && mid_pt != vertices) {
        //     print_vector(result_paths[mid_pt+1]);
        //     break;
        // }
        // If SAT and result[k-1] are UNSAT, the minimum is found
        // if (results[mid_pt] == 1 && results[mid_pt-1] == 0 && mid_pt != 0) {
        //     print_vector(result_paths[mid_pt]);
        //     break;
        // }

      
        
        if (vc_results[mid_pt]) {
            high_pt = mid_pt - 1;
        } 
        else {
            low_pt = mid_pt + 1;
        }
    }

    std::fill_n(vc_results, verts, -1);
    for (int i=0; i <=vertices; i++) {
        //Minisat::Solver solver;
        solver2.reset(new Minisat::Solver());
        vc_results[i] = mysterySolver2( i);
        if (vc_results[i]) {
            result_paths[i] = resolveSatPath(solver2, i);
           // printVerts(result_paths[i]);
           std::vector<int> mini_result=result_paths[i];
           sort(mini_result.begin(),mini_result.end());
           return mini_result;
        } 
    }
}

void printVerts(std::vector<int> result_paths) {
    sort(result_paths.begin(), result_paths.end());
    
    for (auto v : result_paths) 
    {std::cout << v << " ";
    }
     std::cout << std::endl;
}

// converting to my single dimension list
Minisat::Var singleDIndex(int r_num, int c_num, int i) {
    return r_num * i + c_num;
}

void clause2_onlyOnce(unique_ptr<Minisat::Solver>& solver, int k) {
    for (int i = 0; i < vertices; i++) {   
        for (int q = 0; q < k; q++) {
            for (int p = 0; p < q; p++) {
                solver->addClause( ~Minisat::mkLit(singleDIndex(i,p,k)), ~Minisat::mkLit(singleDIndex(i,q,k)));  
            }   
        }        
    }
}


void clause3_onePCover(unique_ptr<Minisat::Solver>& solver, int k) {
    
    for (int i = 0; i < k; i++) {   
        for (int q = 0; q < vertices; q++) {
            for (int p = 0; p < q; p++) {
                solver->addClause( ~Minisat::mkLit(singleDIndex(p,i,k)), ~Minisat::mkLit(singleDIndex(q,i,k)));  
            }   
        }        
    }
}


void clause4_coverAllEdges(unique_ptr<Minisat::Solver>& solver, int k) {
    
    for ( auto& e : input_graph) {
        Minisat::vec<Minisat::Lit> literals;
        for (int i = 0; i < k; i++) {
            literals.push(Minisat::mkLit(singleDIndex(e[0], i, k)));
            literals.push(Minisat::mkLit(singleDIndex(e[1], i, k)));
            }
        solver->addClause(literals);
        }
}


bool mysterySolver( int k) {
    for (int r = 0; r < verts; r++) {
        for (int c = 0; c < k; c++) {
            //auto var = solver->newVar();
            solver->newVar();
        }
    }
    clause1_least1(solver, k);
    clause2_onlyOnce(solver, k);
    clause3_onePCover(solver, k);
    clause4_coverAllEdges(solver, k);
        Minisat::vec<Minisat::Lit> dummy;
           // cout<<"start mystery1 \n";

    Minisat::lbool sat = solver->solveLimited(dummy);
              //  cout<<"end mystery1 \n";

    return sat==Minisat::l_True;
}
bool mysterySolver2( int k) {
    for (int r = 0; r < verts; r++) {
        for (int c = 0; c < k; c++) {
            //auto var = solver->newVar();
            solver2->newVar();
        }
    }
    clause1_least1(solver2, k);
    clause2_onlyOnce(solver2, k);
    clause3_onePCover(solver2, k);
    clause4_coverAllEdges(solver2, k);
        Minisat::vec<Minisat::Lit> temp;
                    //cout<<"start mystery2 \n";

    Minisat::lbool sat = solver2->solveLimited(temp);
              // cout<<"start mystery2 \n";

    return sat==Minisat::l_True;
}
void clause1_least1(unique_ptr<Minisat::Solver>& solver, int k) {
    for (int i = 0; i < k; i++) {
        Minisat::vec<Minisat::Lit> clause;
        for (int n = 0; n < vertices; n++) {
            clause.push(Minisat::mkLit(singleDIndex(n,i,k)));
        }
        solver->addClause(clause);
    }
}

std::vector<int> resolveSatPath(unique_ptr<Minisat::Solver>& solver, int k) {
    
    //cout<<" \n resolving start \n";
    std::vector<int> path;
    for (int r = 0; r < vertices; r++) {
        for (int c = 0; c < k; c++) {
            if (solver->modelValue(singleDIndex(r,c,k)) == Minisat::toLbool(0)) {
                path.push_back(r);
            }
        }      
    }
   // cout<<" \n resolving end \n";
    std::sort(path.begin(), path.end());
    return path;
}



	void shortestPathAnalyzer() {
		vector<int> path;
		int current = destination;
		if (validateConnection() == false) {
			cout << "Error: the points aren't connected \n";
			return;
		}

		path.push_back(current);
		while (predecessor[current] != -1) {
			path.push_back(predecessor[current]);
			current = predecessor[current];
		}

		for (int i = (int) path.size() - 1; i >= 0; i--) {
			cout << path[i];
			if (i > 0) std::cout << "-";
		}
		cout << std::endl;

		return;
	}

	bool validateConnection() {
		queue<int> touched_nodes;
		vector<bool> visited_node(graph_verts);

		if (graph_verts < 1) {
			return false;
		}
		if (source >= graph_verts || destination>= graph_verts) {
			return false;
		}
		for (int i = 0; i < graph_verts; i++) {
			visited_node[i] = false;
		//	distance_unit[i] = INT_MAX;
			predecessor[i] = -1;
		}


		visited_node[source] = true;
		//distance_unit[source] = 0;
		touched_nodes.push(source);

		while (!touched_nodes.empty()) {
			int n = touched_nodes.front();
			touched_nodes.pop();
			for (size_t  i = 0; i < formedGraph[n].size(); i++) {
				if (visited_node[formedGraph[n][i]] == false)
				{
					visited_node[formedGraph[n][i]] = true;
				//	distance_unit[formedGraph[n][i]] = distance_unit[n] + 1;
					predecessor[formedGraph[n][i]] = n;
					touched_nodes.push(formedGraph[n][i]);
					if (formedGraph[n][i] == destination) {
						return true;
					}
				}
			}
		}
		return false;
	}

};
