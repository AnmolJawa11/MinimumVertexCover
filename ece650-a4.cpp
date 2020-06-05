//AUTHOR : ANMOL JAWA  a2jawa@uwaterloo.ca
#include <iostream>
#include "Validator.cpp"
#include "GraphProps.cpp"
#include "GeneralUtil.cpp"
#include <minisat/core/Solver.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <list>
#include <memory>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <thread>
#include <pthread.h>
// C++ library for multithreading
#include <time.h>
#include <iomanip>
using std::cin;
using std::cout;
using std::endl;
using std::ifstream;
using std::cerr;

#define handle_error(msg) \
               do { perror(msg); exit(EXIT_FAILURE); } while (0)
#define handle_error_en(en, msg) \
               do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)
#define RUNNING true
#define FINISHED false               
bool threadStatus, checkIfError = false;
bool timeoutForMiniSat=false;

//for analysis only 
bool analysis_mode=true;
long double algo1_time;
long double algo2_time;
long double minisat_time;
double algo1_ratio;
double algo2_ratio;



//the end

int total_vertices_in_graph;
bool res;
pthread_mutex_t lockp;

vector<int> vc_algo1;
vector<int> vc_algo2;
vector<int> vc_mini;
vector<int> edge;
vector<vector<int>> theUltimateGraph;

Validator validationUtil;
GraphUtil util;
GraphAnalyser* graphAnalyser = new GraphAnalyser();





static void pclock(char *msg, clockid_t cid){
    pthread_mutex_lock(&lockp);
    struct timespec ts;

    printf("%s", msg);
    if (clock_gettime(cid, &ts) == -1)
    handle_error("clock_gettime");
    printf("%4ld.%06ld\n", ts.tv_sec, ts.tv_nsec / 1000);
    pthread_mutex_unlock(&lockp);
       }


int mode_highest_inc (vector<int> e){
    int g=0;
    //using a histogram here to find highest incedence
vector<int> histogram(total_vertices_in_graph,0);
vector<int> temp;
vector<int>::iterator it = e.begin();
vector<int> temp2;

//iterate it throguhout
while(it != e.end())  histogram[*it++]++;
return max_element( histogram.begin(), histogram.end() ) - histogram.begin();
}

int randomize(int limitingV){
	ifstream urandom("/dev/urandom");
	int charac = 1;
	urandom.read((char *) &charac, 1);
	urandom.close();
    //limiting random by taking mod
	int rand = charac % limitingV ;
	if(rand%2 != 0){rand = rand -1; }
	return rand;
}

void mapMe(vector<int> ex){
   //for algo 1 and 2 
     int size_ex = ex.size();
     //for loop todo
    for (int m=0;m<size_ex;m++){
         if(m==size_ex -1){cout<<ex[m];}
        else{cout<<ex[m]<<",";}
        }  
cout<<endl;
}
void mapMe2(vector<int> ex){
    //for minisat alone
//todo , prepare abstract method
    if(timeoutForMiniSat==true){
        cout<<"timeout \n";
        return;
    }
    int temp=0;
     int size_ex = ex.size();
    for (int m=0;m<size_ex;m++){
         if(m==size_ex -1){cout<<ex[m];}
        else{cout<<ex[m]<<",";}
        }  
cout<<endl;
}

vector<int> sort (vector<int> a){
sort(a.begin(),a.end());
return a;
}


bool Isseperatpr(char ch)
{
    //checl whether that symbol qualifies to be included
    switch(ch)
    {
    case '}':
    case '>':
    case ',':
    case '<':
    case '{':

        return true;
    default:
        return false;
    }
}


void update_v(string po_E)
{
   edge.clear();
   string number ;
   for(char i : po_E) {
    if (Isseperatpr(i)== false){
        string I(1,i);
        number = number+I;
    }
    else{
        int result = atoi(number.c_str());
        if(!number.empty()){
        edge.push_back(result);
        number.clear();
        }
    }
   }
}

static void *algo1(void *arg){
        clockid_t cid_a1;

    vc_algo1.clear(); 
    int comp = 0;
    int sgp;
    vector<int> vc_single_set = edge;
    while(!vc_single_set.empty()){
        int modes = mode_highest_inc(vc_single_set);
      //  int modes=1;
        vc_algo1.push_back(modes);
        //highest precedence gone within vector
        for(int comp = 0 ;comp< (int)vc_single_set.size();){
            if((comp+1)>vc_single_set.size()) break;
            //todo - compare size
            if((vc_single_set[comp]==modes) || (vc_single_set[comp+1]==modes)){
                vc_single_set.erase(vc_single_set.begin()+comp,vc_single_set.begin()+comp+2);
            }
            else{ comp+=2;}
         }
    }
    if(analysis_mode){
        //probably not required in submission
        sgp = pthread_getcpuclockid(pthread_self(), &cid_a1);
        if (sgp != 0)
  	    handle_error_en(sgp, "pthread_getcpuclockid");
       // pclock("algo1 running time", cid);
           struct timespec ts_struct;
           clock_gettime(cid_a1, &ts_struct) ;
           algo1_time= (ts_struct.tv_sec*1000000)+(ts_struct.tv_nsec/1000);
        
    }

}




static void *algo2(void *arg){
    vc_algo2.clear();
    int comp=0;
    clockid_t cid_a2;
    int gp;
    vector<int> vc_single_set = edge;

    while(!vc_single_set.empty()){
        int m = randomize(vc_single_set.size());
        //int m = 0;
        vc_algo2.push_back(vc_single_set[m]);
        vc_algo2.push_back(vc_single_set[m+1]);
        int gp = vc_algo2.size();
        //compute size again for comparision
        for(comp=0;comp<(int)vc_single_set.size();){
            if((vc_single_set[comp]==vc_algo2[gp-1]) || (vc_single_set[comp+1]==vc_algo2[gp-1])||(vc_single_set[comp+1]==vc_algo2[gp-2])||(vc_single_set[comp]==vc_algo2[gp-2])){
                vc_single_set.erase(vc_single_set.begin()+comp,vc_single_set.begin()+comp+2);
        }
            else{comp+=2;}
        }
}
    if(analysis_mode){
        gp = pthread_getcpuclockid(pthread_self(), &cid_a2);
        if (gp != 0)
  	    handle_error_en(gp, "pthread_getcpuclockid");
        //pclock("algo2 running time", cid);
        struct timespec ts_struct;
        clock_gettime(cid_a2, &ts_struct) ;
        algo2_time= (ts_struct.tv_sec*1000000)+(ts_struct.tv_nsec/1000);
    }
}


void *AC_MS(void *arg){
        clockid_t cid_sat;
    	threadStatus = RUNNING;

    int gp;
    vc_mini.clear();
    vc_mini= graphAnalyser->vertex_cover();
	pthread_getcpuclockid(pthread_self(), &cid_sat);

    gp = pthread_getcpuclockid(pthread_self(), &cid_sat);
    threadStatus = FINISHED;
    if(analysis_mode){
        if (gp != 0)
  	    handle_error_en(gp, "pthread_getcpuclockid");
        // pclock("algo3=minisat running time", cid);
        struct timespec tspecStruct;
        clock_gettime(cid_sat, &tspecStruct) ;
        minisat_time= (tspecStruct.tv_sec*1000000)+(tspecStruct.tv_nsec/1000);
    }
}


void timeOut(pthread_t threadId, int timeLimit)
{

    clockid_t cid;
    struct timespec tspecStruct;
    tspecStruct.tv_sec=0;
    // cout<<tspec.tv_sec<<"  : was the previous set time \n";
    if (pthread_getcpuclockid(threadId,&cid)){
        cerr<<"Error: can't get the clock"<<endl;
    }
    while(threadStatus == RUNNING)
    {
         //cout<<"waiting "<<timeLimit<<"\n";
        clock_gettime(cid,&tspecStruct);
        //cout<<"and .. "<<tspec.tv_sec<<"\n";
        if(tspecStruct.tv_sec > timeLimit)
        {
            timeoutForMiniSat=true;
            graphAnalyser->solver->interrupt();
                        sleep(2);

            graphAnalyser->solver2->interrupt();

          //  break;
            //minisat_time=15;
            //return;
        }
    }
    return ;
}

//  std::ofstream timeAnalysis;
//  std::ofstream correctnessAnalysis;
void *actor(void * args)
{
    int oldVerts=0;

    try
    {
        
    int entered_verts=0;
    string entered_edges;
    char input_option;
   // int source_vertex, destination_vertex;
    GraphImpl* initGraph=new GraphImpl(0);

    //infinite menu loop 
    while (cin >> input_option) {
       
        input_option =(char) toupper(input_option);
        vector<vector<int>> input_graph; 
        switch (input_option) {
        case 'V':
            cin >> entered_verts;
            if (!validationUtil.validateInputVerticesNum(entered_verts)) {
                cout << "Error: vertices cannot be less than or equal to 0 \n";
            }
            else {
                //start with a new set of graph implementation
                delete initGraph;
                initGraph = new GraphImpl(entered_verts);
            }
            
      
            total_vertices_in_graph=entered_verts;

            break;

        case 'E':
            cin >> entered_edges;
            timeoutForMiniSat=false;
            vc_mini.clear();
            vc_algo1.clear();
            vc_algo2.clear();
            input_graph = util.formatInputEdges(entered_edges);

            if (!validationUtil.validateEnteredEdges(input_graph,entered_verts)) {
                cout << "Error: unsupported edges entered \n";
                break;
            }
            update_v(entered_edges);
            delete graphAnalyser;
            initGraph->formGraph(input_graph);
            graphAnalyser = new GraphAnalyser();
            theUltimateGraph= initGraph->getGraph();
            graphAnalyser->initializeGraph(entered_verts, initGraph->getGraph());
            graphAnalyser->setInputGraph(input_graph);

            //TODO

            if(entered_verts > 0 && theUltimateGraph.size() > 0){
                int s;
	            pthread_t thread[3];
                clockid_t cid;
                clockid_t aid;  

                s = pthread_create(&thread[0], NULL, algo1, NULL);
                s = pthread_create(&thread[1], NULL, algo2, NULL);
                s = pthread_create(&thread[2], NULL, AC_MS, NULL); 
                threadStatus = RUNNING;

               timeOut(thread[2], 15);


                // cout<<"came out";
                //  struct timespec ts2;
                //  ts2.tv_sec=20000000;
                //  int q= pthread_timedjoin_np(thread[2], NULL,&ts2);


                pthread_join(thread[2],NULL);
                pthread_join(thread[0],NULL);
                pthread_join(thread[1],NULL);
                // struct timespec ts;

                // ts.tv_sec += 5;
                // int st = pthread_timedjoin_np(thread[2], NULL, &ts);

                //double ratio1 = (double(vc_algo1.size()))/vc_mini.size();
	            //double ratio2 = (double(vc_algo2.size()))/vc_mini.size();
                // timeAnalysis<<"1,2,3 \n";
                // timeAnalysis << algo1_time <<","<<algo2_time<<","<<minisat_time<<"\n";
                if(timeoutForMiniSat){
                    minisat_time=-1;
                }
               

	            cout<<"CNF-SAT-VC:"<<" ";
                mapMe2(vc_mini);
                
                cout<<"APPROX-VC-1:"<<" ";
                mapMe(sort(vc_algo1));
                cout<<"APPROX-VC-2:"<<" ";
                mapMe(sort(vc_algo2));
                //cout<<"Approxratio 1 :"<<setprecision(3)<<ratio1<<endl;
                //cout<<"Approxratio 2 :"<<setprecision(3)<<ratio2<<endl;
                edge.clear();
                total_vertices_in_graph = 0;
                res = 0;
                vc_mini.clear();
                vc_algo1.clear();
                vc_algo2.clear();   
 	        }            





            //end




            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            break;

        // default:
        //     std::cin.clear();

        };
    }
    //timeAnalysis.close();
    delete initGraph;
    delete graphAnalyser;

    }
    catch (const std::exception& ex)
    {
        std::cout << "Exception was thrown: " << ex.what() << std::endl;
    }
}


int main(){
            pthread_t input;
          int s;
          s = pthread_create(&input, NULL, actor, NULL);
          pthread_join(input,NULL);
          pthread_exit(NULL);
         // timeAnalysis.close();
          return 0;
}