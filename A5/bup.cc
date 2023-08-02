#include <iostream>
#include <algorithm>
#include <cstring>
#include <vector>
#include <queue>
#include <array>
#include <list>
#include <sstream>
#include <stack>

using namespace std;

list<string> input;
vector<string> read;
vector<vector<string>> rules;

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

void shift() {
    read.push_back(input.front());
    input.pop_front();
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
    }
    read.push_back(replace);
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
        if (line == ".ACTIONS") break;
        // read input
        ss << line;
        while (ss >> str) {
            input.push_back(str);
        }
        ss.clear();
    }
    while (getline(cin, line)) {
        if (line == ".END") break;
        // read actions
        ss << line;
        ss >> str;
        if (str == "print") {
            printProgress();
        } else if (str == "shift") {
            shift();
        } else {
            // reduce
            int k;
            ss >> k;
            apply(k);
        }
        ss.clear();
    }

    return 0;
}