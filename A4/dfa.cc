#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
const std::string ALPHABET    = ".ALPHABET";
const std::string STATES      = ".STATES";
const std::string TRANSITIONS = ".TRANSITIONS";
const std::string INPUT       = ".INPUT";
const std::string EMPTY       = ".EMPTY";

bool isChar(std::string s) {
  return s.length() == 1;
}
bool isRange(std::string s) {
  return s.length() == 3 && s[1] == '-';
}

// Locations in the program that you should modify to store the
// DFA information have been marked with four-slash comments:
//// (Four-slash comment)
const int N = 10010;
std::vector<char> alphabet;
std::unordered_map<char, int> charToPosMap;
std::unordered_map<int, char> posToCharMap;

void addToAlphabet(char c) {
    alphabet.push_back(c);
    charToPosMap[c] = alphabet.size() - 1;
    posToCharMap[alphabet.size() - 1] = c;
}

struct Node {
    int id; // node id
    std::vector<int> children;
    bool accept;

    void setId(int _id) {
        id = _id;
    }

    void initChildren(int len) {
        children.resize(len);
        for (size_t i = 0; i < len; i ++) {
            children[i] = 0;
        }
    }

    void addEdge(char transEdge, int toNode) {
        int edge = charToPosMap[transEdge];
        children[edge] = toNode;
    }

    void updateAccept(bool _accept) {
        accept = _accept;
    }

    bool isAccept() {
        return accept;
    }

} nodes[N];
int nodeCnt;
std::unordered_map<std::string, int> stateToNodeMap;
std::string initialState;

void addNode(std::string& state) {
    stateToNodeMap[state] = ++ nodeCnt;
    nodes[nodeCnt].setId(nodeCnt);
    nodes[nodeCnt].initChildren(alphabet.size());
    nodes[nodeCnt].updateAccept(false);
}

int getNode(std::string& state) {
    return stateToNodeMap[state];
}

void addTransition(std::string& state1, std::string& state2, char inputChar) {
    // node1 --inputChar-->  node2
    int node1 = getNode(state1), node2 = getNode(state2);
    nodes[node1].addEdge(inputChar, node2);
}

int getNext(int cur, char transEdge) {
    int edge = charToPosMap[transEdge];
    return nodes[cur].children[edge];
}

bool search(std::string& s) {
    int node = 1; // root
    for (char c : s) {
        int nxt = getNext(node, c);
        if (nxt == 0) {
            // no such transition
            return false;
        }
        node = nxt;
    }
    return nodes[node].isAccept();
}

int main() {
  std::istream& in = std::cin;
  std::string s;

  std::getline(in, s); // Alphabet section (skip header)
  // Read characters or ranges separated by whitespace
  while(in >> s) {
    if (s == STATES) { 
      break; 
    } else {
      if (isChar(s)) {
        //// Variable 's[0]' is an alphabet symbol
        addToAlphabet(s[0]);
      } else if (isRange(s)) {
        for(char c = s[0]; c <= s[2]; ++c) {
          //// Variable 'c' is an alphabet symbol
          addToAlphabet(c);
        }
      } 
    }
  }

  std::getline(in, s); // States section (skip header)
  // Read states separated by whitespace
  while(in >> s) {
    if (s == TRANSITIONS) { 
      break; 
    } else {
      static bool initial = true;
      bool accepting = false;
      if (s.back() == '!' && !isChar(s)) {
        accepting = true;
        s.pop_back();
      }
      //// Variable 's' contains the name of a state
      addNode(s);
      if (initial) {
        //// The state is initial
        initial = false;
        initialState = s;
      }
      if (accepting) {
        //// The state is accepting
        nodes[getNode(s)].updateAccept(true);
      }
    }
  }

  std::getline(in, s); // Transitions section (skip header)
  // Read transitions line-by-line
  while(std::getline(in, s)) {
    if (s == INPUT) { 
      // Note: Since we're reading line by line, once we encounter the
      // input header, we will already be on the line after the header
      break; 
    } else {
      std::string fromState, symbols, toState;
      std::istringstream line(s);
      line >> fromState;
      bool last;
      while(line >> s) {
        if(line.peek() == EOF) { // If we reached the last item on the line
          // Then it's the to-state
          toState = s;
        } else { // Otherwise, there is more stuff on the line
          // We expect a character or range
          if (isChar(s)) {
            symbols += s;
          } else if (isRange(s)) {
            for(char c = s[0]; c <= s[2]; ++c) {
              symbols += c;
            }
          }
        }
      }
      for ( char c : symbols ) {
        //// There is a transition from 'fromState' to 'toState' on 'c'
        addTransition(fromState, toState, c);
      }
    }
  }

  // Input section (already skipped header)
  while(in >> s) {
    //// Variable 's' contains an input string for the DFA
    bool accepted = search(s);
    if (accepted) {
        std::cout << s << " true" << std::endl;
    } else {
        std::cout << s << " false" << std::endl;
    }
  }
}