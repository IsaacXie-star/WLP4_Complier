#include <iostream>
#include <algorithm>
#include <cstring>
#include <vector>
#include <queue>
#include <array>
#include <list>
#include <sstream>
#include <stack>
#include <unordered_map>
#include <map>

using namespace std;

const int N = 200010;

list<string> input;
vector<string> read;
vector<vector<string>> rules;
struct State {
    unordered_map<string, int> trans;
    unordered_map<string, int> reduce;
} states[N];
stack<int> stateStk;
int k = 1;
string ACCEPT = ".ACCEPT";
bool globalErrorCheck = false;

void addtrans(int from, string& symb, int to) {
    states[from].trans[symb] = to;
}

void addreduce(int stateId, int ruleId, string& nextSymb) {
    states[stateId].reduce[nextSymb] = ruleId;
}

int getNextState(int from, string& symb) {
    if (states[from].trans.count(symb) == 0) {
        globalErrorCheck = true;
        return 0;
    }
    return states[from].trans[symb];
}

int getReduceRule(int curState, string& symb) {
    if (states[curState].reduce.count(symb) == 0) {
        globalErrorCheck = true;
        return 0;
    }
    return states[curState].reduce[symb];
}

bool check(int curState, string& nextInput) {
    if (states[curState].reduce.count(nextInput) != 0) {
        return 1;
    }
    return 0;
}

void printProgress() {
    bool first = true;
    for (size_t i = 0; i < read.size(); i ++) {
        if (first) {
            first = false;
            cout << read[i];
        } else {
            cout << ' ' << read[i];
        }
    }
    if (first) {
        cout << ".";
        first = false;
    } else {
        cout << " .";
    }
    
    for (auto i = input.begin(); i != input.end(); ++ i) {
        if (first) {
            cout << *i;
            first = false;
        } else {
            cout << ' ' << *i;
        }
    }
    cout << endl;
}

void shift(int stateMove) {
    read.push_back(input.front());
    input.pop_front();
    stateStk.push(stateMove);
}

void apply(int k) {
    // apply the kth rule
    int pop_sz = rules[k].size() - 1;
    string replace = rules[k][0];
    for (int i = 1; i < rules[k].size(); i ++) {
        if (rules[k][i] == ".EMPTY") pop_sz --;
    }
    while (pop_sz --) {
        read.pop_back();
        stateStk.pop();
    }
    read.push_back(replace);
    if (check(stateStk.top(), replace) == 0) {
        stateStk.push(getNextState(stateStk.top(), replace));
    } else {
        globalErrorCheck = true;
    }
}

int main() {
    
    string line;
    stringstream ss;
    string str;
    getline(cin, line); // get .CFG
    while (getline(cin, line)) {
        if (line == ".INPUT") break;
        // read CFGs
        ss << line;
        vector<string> rule;
        while (ss >> str) {
            rule.push_back(str);
        }
        rules.push_back(rule);
        ss.clear();
    }
    while (getline(cin, line)) {
        if (line == ".TRANSITIONS") break;
        // read input
        ss << line;
        while (ss >> str) {
            input.push_back(str);
        }
        ss.clear();
    }
    while (getline(cin, line)) {
        if (line == ".REDUCTIONS") break;
        // read transitions
        ss << line;
        int from, to;
        string symb;
        ss >> from >> symb >> to;
        addtrans(from, symb, to);
        ss.clear();
    }
    while (getline(cin, line)) {
        if (line == ".END") break;
        // read reductions
        ss << line;
        int stateId, ruleId;
        string nextSymb;
        ss >> stateId >> ruleId >> nextSymb;
        addreduce(stateId, ruleId, nextSymb);
        ss.clear();
    }

    // process input
    stateStk.push(0);
    printProgress();
    while (input.size()) {  
        int curState = stateStk.top();
        string nextInput = input.front();
        int signal = check(curState, nextInput); // 0 shift, 1 reduce, 2 error
        if (globalErrorCheck == true) {
            cerr << "ERROR at " << k << endl;
            return 0;
        }
        if (signal == 0) {
            shift(getNextState(curState, nextInput));
            if (globalErrorCheck == true) {
                cerr << "ERROR at " << k << endl;
                return 0;
            }
            ++ k;
        } else if (signal == 1) {
            apply(getReduceRule(curState, nextInput));
            if (globalErrorCheck == true) {
                cerr << "ERROR at " << k << endl;
                return 0;
            }
        } else {
            // error
            cerr << "ERROR at " << k << endl;
            break;
        }
        printProgress();
    }
    if (check(stateStk.top(), ACCEPT) == 1) {
        apply(getReduceRule(stateStk.top(), ACCEPT));
        printProgress();
    } else {
        cerr << "ERROR at " << k << endl;
    }

    return 0;
}