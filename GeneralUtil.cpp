#include<iostream>
#include<string>
#include<vector>
#include<regex>
#define digit_regex "[+-]?[0-9]+"
using std::regex;
using std::vector;
using std::pair;
using std::string;

class GraphUtil {
public:

    vector<vector<int> > formatInputEdges(string s) {
        vector<int> single_line;
        vector< vector<int> > input_edges;
        std::regex re(digit_regex);
            std::sregex_iterator next(s.begin(), s.end(), re);
            std::sregex_iterator end;
            std::smatch match1;
            std::smatch match2;
            while (next != end) {
                single_line.clear();
                match1 = *next;
                next++;
                if (next != end) {
                    match2 = *next;
                    single_line.push_back(std::stoi(match1.str()));
                    single_line.push_back(std::stoi(match2.str()));
                    input_edges.push_back(single_line);
                    next++;
                }
            }

        return input_edges;

    };
};