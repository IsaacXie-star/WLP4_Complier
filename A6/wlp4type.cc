#include <iostream>
#include <algorithm>
#include <cstring>
#include <unordered_map>
#include <map>
#include <vector>
#include <sstream>
#include <unordered_set>
#include <set>

using namespace std;

enum DclType {
    INT = 1,
    INT_STAR,
    NA
};

map<string, unordered_map<string, DclType>> symbTable;
unordered_set<string> proSet;
map<string, vector<DclType> > proParamList;

string getDclTypeStr(DclType dclType) {
    if (dclType == INT) return "int";
    else if (dclType == INT_STAR) return "int*";
    return "NA";
}

string getNaTypeStr(DclType naType) {
    if (naType == INT) return "int";
    else if (naType == INT_STAR) return "int*";
    return "NA";
}

void addToSymbTable(string curProcedure, string idStr, DclType naType) {
    symbTable[curProcedure][idStr] = naType;
}

DclType getIDType(string curProcedure, string idStr) {
    return symbTable[curProcedure][idStr];
}

struct Token {
    string kind;
    string lexeme;

    string getKind() {
        return kind;
    }
    string getLexeme() {
        return lexeme;
    }
};
struct Node {
    Node(int _type, Token _token, vector<string>& _prod, int _id, DclType _dclType) {
        type = _type;
        token = _token;
        prod = _prod;
        parent = -1;
        children.clear();
        id = _id;
        dclType = _dclType;
    }
    int id;
    int type = 0;
    DclType dclType;
    DclType naType = NA; // only for check, not for print
    Token token; // type 1
    vector<string> prod; // type 2
    vector<int> children;
    int parent;
    bool isProcedureID = false;

    DclType getDclType() {
        return dclType;
    }
    DclType getNaType() {
        return naType;
    }
    bool isProID() {
        return isProcedureID;
    }
};
int idx = 0;
vector<Node> allNodes;
bool globalErrorCheck = false;

vector<DclType> argList;

bool checkLeaf(vector<string> &content) {
    if (content.size() != 2) return false;
    auto s = content[0];
    for (auto c : s) {
        if (c < 'A' || c > 'Z') return false;
    }
    return true;
}

// read wlp4ti file and construct the parse tree
void readAndBuild(int parentNode, string curProcedure) {
    // input: left-to-right preorder traversal
    string line;
    stringstream ss;
    if (!getline(cin, line)) {
        return;
    }
    ss << line;
    // cerr << line << endl;
    string str;
    vector<string> tmp;
    while (ss >> str) {
        tmp.push_back(str);
    }
    bool isLeaf = checkLeaf(tmp);
    int type = 0;
    if (isLeaf) {
        Token token = {tmp[0], tmp[1]};
        vector<string> tmpvec;
        DclType dclType = NA;
        if (token.getKind() == "NUM") {
            dclType = INT;
        } else if (token.getKind() == "NULL") {
            dclType = INT_STAR;
        }
        Node root(1, token, tmpvec, idx ++, dclType);
        if (parentNode != -1) {
            root.parent = allNodes[parentNode].id;
            allNodes[parentNode].children.push_back(idx - 1);
        }
        allNodes.push_back(root);
        return;
    }
    Token tmpToken;
    Node tmpNode(2, tmpToken, tmp, idx ++, NA);
    allNodes.push_back(tmpNode);
    int root = idx - 1;;
    if (parentNode != -1) {
        allNodes[parentNode].children.push_back(root);
    }
    // .EMPTY edge case
    if (allNodes[root].prod.size() == 2 && allNodes[root].prod[1] == ".EMPTY") {
        // return directly
        return;
    }
    string curNonterminal = allNodes[root].prod[0];
    int argListPreCount = argList.size();

    if (curProcedure != "NA") {
        for (int i = 1; i < allNodes[root].prod.size(); i ++) {
            readAndBuild(root, curProcedure);
        }
    } else {
        if (curNonterminal == "procedures" && allNodes[root].prod.size() == 2) {
            for (int i = 1; i < allNodes[root].prod.size(); i ++) {
                readAndBuild(root, allNodes[root].prod[1]);
            }
        } else if (curNonterminal == "procedure") {
            readAndBuild(root, "NA"); // special case for INT
            readAndBuild(root, "NA"); // special case for ID
            string proID = allNodes[allNodes[root].children[1]].token.getLexeme();
            allNodes[allNodes[root].children[1]].isProcedureID = true;
            if (proSet.count(proID)) {
                globalErrorCheck = true;
                return;
            }
            proSet.insert(proID);
            for (int i = 3; i < allNodes[root].prod.size(); i ++) {
                readAndBuild(root, allNodes[allNodes[root].children[1]].token.getLexeme());
            }
        } else {
            for (int i = 1; i < allNodes[root].prod.size(); i ++) {
                readAndBuild(root, "NA");
            }
        }
    }
    // check sub types correctness for current non-terminal
    if (curNonterminal == "main") {
        if (allNodes[allNodes[root].children[5]].getNaType() != INT || allNodes[allNodes[root].children[11]].getDclType() != INT) {
            globalErrorCheck = true;
            return;
        }
    } else if (curNonterminal == "dcls") {
        if (allNodes[root].children.size() == 5) {
            if (allNodes[allNodes[root].children[1]].getNaType() != allNodes[allNodes[root].children[3]].getDclType()) {
                globalErrorCheck = true;
                return;
            }
        }
    } else if (curNonterminal == "expr") {
        if (allNodes[root].children.size() == 3) {
            if (allNodes[allNodes[root].children[0]].getDclType() == INT_STAR && allNodes[allNodes[root].children[2]].getDclType() == INT_STAR) {
                if (allNodes[allNodes[root].children[1]].token.getKind() == "PLUS") {
                    globalErrorCheck = true;
                    return;
                }
            }
        }
    } else if (curNonterminal == "term") {
        if (allNodes[root].children.size() == 3) {
            if (allNodes[allNodes[root].children[0]].getDclType() == INT_STAR || allNodes[allNodes[root].children[2]].getDclType() == INT_STAR) {
                globalErrorCheck = true;
                return;
            }
        }
    } else if (curNonterminal == "factor") {
        int argListNewCount = argList.size();
        int newAdd = argListNewCount - argListPreCount;
        if (allNodes[root].children.size() == 2) {
            if (allNodes[allNodes[root].children[0]].token.getKind() == "AMP") {
                if (allNodes[allNodes[root].children[1]].getDclType() == INT_STAR) {
                    globalErrorCheck = true;
                    return;
                }
            }
            if (allNodes[allNodes[root].children[0]].token.getKind() == "STAR") {
                if (allNodes[allNodes[root].children[1]].getDclType() == INT) {
                    globalErrorCheck = true;
                    return;
                }
            }
        } else if (allNodes[root].children.size() == 4) {
            // factor → ID LPAREN arglist RPAREN
            if (proSet.count(allNodes[allNodes[root].children[0]].token.getLexeme()) == 0) {
                // used of undeclared procedure
                globalErrorCheck = true;
                return;
            }
            // argList now should store all the dcltypes of the argument lists
            string idStr = allNodes[allNodes[root].children[0]].token.getLexeme();
            if (proParamList[idStr].size() != newAdd) {
                globalErrorCheck = true;
                return;
            }
            for (int i = argListPreCount; i < argListNewCount; i ++) {
                if (argList[i] != proParamList[idStr][i - argListPreCount]) {
                    globalErrorCheck = true;
                    return;
                }
            }
            while (newAdd --) argList.pop_back();
        } else if (allNodes[root].children.size() == 3 && allNodes[allNodes[root].children[0]].token.getKind() == "ID") {
            if (proSet.count(allNodes[allNodes[root].children[0]].token.getLexeme()) == 0) {
                globalErrorCheck = true;
                return;
            }
            string idStr = allNodes[allNodes[root].children[0]].token.getLexeme();
            if (proParamList[idStr].size() != 0) {
                // paramlist should be empty
                globalErrorCheck = true;
                return;
            }
        }
    } else if (curNonterminal == "lvalue") {
        if (allNodes[root].children.size() == 2) {
            if (allNodes[allNodes[root].children[1]].getDclType() != INT_STAR) {
                globalErrorCheck = true;
                return;
            }
        } else if (allNodes[root].children.size() == 1) {
            string idStr = allNodes[allNodes[root].children[0]].token.getLexeme();
            if (symbTable.count(curProcedure) == 0) {
                globalErrorCheck = true;
                return;
            }
            if (symbTable[curProcedure].count(idStr) == 0) {
                globalErrorCheck = true;
                return;
            }
        }
    } else if (curNonterminal == "procedure") {
        // check for double declaration
        string proID = allNodes[allNodes[root].children[1]].token.getLexeme();
        // check return type
        if (allNodes[allNodes[root].children[9]].getDclType() != INT) {
            globalErrorCheck = true;
            return;
        }
    } else if (curNonterminal == "statement") {
        if (allNodes[root].children.size() == 4) {
            if (allNodes[allNodes[root].children[0]].getDclType() != allNodes[allNodes[root].children[2]].getDclType()) {
                globalErrorCheck = true;
                return;
            }
        } else if (allNodes[root].prod[1] == "PRINTLN") {
            if (allNodes[allNodes[root].children[2]].getDclType() != INT) {
                globalErrorCheck = true;
                return;
            }
        } else if (allNodes[root].prod[1] == "DELETE") {
            if (allNodes[allNodes[root].children[3]].getDclType() != INT_STAR) {
                globalErrorCheck = true;
                return;
            }
        }
    } else if (curNonterminal == "test") {
        if (allNodes[allNodes[root].children[0]].getDclType() != allNodes[allNodes[root].children[2]].getDclType()) {
            globalErrorCheck = true;
            return;
        }
    }

    // obtain expression type for current non-terminal
    if (curNonterminal == "factor") {
        if (allNodes[root].children.size() == 1) {
            if (allNodes[allNodes[root].children[0]].type == 1 && allNodes[allNodes[root].children[0]].token.getKind() == "ID") {
                // ID
                string idStr = allNodes[allNodes[root].children[0]].token.getLexeme();
                if (symbTable[curProcedure].count(idStr) == 0) {
                    // not declared
                    globalErrorCheck = true;
                    return;
                }
                allNodes[root].dclType = symbTable[curProcedure][idStr];
            } else {
                allNodes[root].dclType = allNodes[allNodes[root].children[0]].getDclType();
            }
        } else if (allNodes[root].children.size() == 3) {
            if (allNodes[allNodes[root].children[0]].token.getKind() == "ID") {
                allNodes[root].dclType = INT; // return type of procedure
            } else {
                allNodes[root].dclType = allNodes[allNodes[root].children[1]].getDclType();
            }
        } else if (allNodes[root].children.size() == 2) {
            if (allNodes[allNodes[root].children[0]].token.getKind() == "AMP") {
                allNodes[root].dclType = INT_STAR;
            }
            if (allNodes[allNodes[root].children[0]].token.getKind() == "STAR") {
                allNodes[root].dclType = INT;
            }
        } else if (allNodes[root].children.size() == 5) {
            allNodes[root].dclType = INT_STAR;
        } else if (allNodes[root].children.size() == 4) {
            allNodes[root].dclType = INT; // return type of procedure
        }
    } else if (curNonterminal == "term") {
        if (allNodes[root].children.size() == 1) {
            allNodes[root].dclType = allNodes[allNodes[root].children[0]].getDclType();
        } else if (allNodes[root].children.size() == 3) {
            allNodes[root].dclType = INT; // should be INT oper INT
        }
    } else if (curNonterminal == "expr") {
        if (allNodes[root].children.size() == 1) {
            allNodes[root].dclType = allNodes[allNodes[root].children[0]].getDclType();
            if (parentNode != -1 && allNodes[parentNode].prod[0] == "arglist") {
                argList.push_back(allNodes[root].getDclType());
            }
        } else if (allNodes[root].children.size() == 3) {
            if (allNodes[allNodes[root].children[0]].getDclType() == INT_STAR || allNodes[allNodes[root].children[2]].getDclType() == INT_STAR) {
                if (allNodes[allNodes[root].children[0]].getDclType() == INT_STAR && allNodes[allNodes[root].children[2]].getDclType() == INT_STAR) {
                    allNodes[root].dclType = INT;
                } else {
                    allNodes[root].dclType = INT_STAR;
                }
            } else {
                allNodes[root].dclType = INT;
            }
        }
    } else if (curNonterminal == "type") {
        if (allNodes[root].children.size() == 1) {
            // INT
            allNodes[root].naType = INT;
        } else {
            // INT STAR
            allNodes[root].naType = INT_STAR;
        }
    } else if (curNonterminal == "dcl") {
        string idStr = allNodes[allNodes[root].children[1]].token.getLexeme();
        if (symbTable[curProcedure].count(idStr)) {
            // double declared
            globalErrorCheck = true;
            return;
        }
        allNodes[root].naType = allNodes[allNodes[root].children[0]].getNaType();
        addToSymbTable(curProcedure, idStr, allNodes[root].getNaType());
    } else if (curNonterminal == "lvalue") {
        if (allNodes[root].children.size() == 1) {
            allNodes[root].dclType = getIDType(curProcedure, allNodes[allNodes[root].children[0]].token.getLexeme());
        } else if (allNodes[root].children.size() == 2) {
            allNodes[root].dclType = INT;
        } else if (allNodes[root].children.size() == 3) {
            allNodes[root].dclType = allNodes[allNodes[root].children[1]].getDclType();
        }
    }

    if (curNonterminal == "dcl" && parentNode != -1 && curProcedure != "NA" && allNodes[parentNode].prod[0] == "paramlist") {
        proParamList[curProcedure].push_back(allNodes[root].getNaType());        
    }
    return;
}

void output(Node& node, string curProcedure) {
    if (node.type == 1) {
        cout << (node.token).kind << ' ' << (node.token).lexeme;
    } else {
        for (int i = 0; i < (node.prod).size(); i ++) {
            if (!i) cout << (node.prod)[i];
            else cout << ' ' << (node.prod)[i];
        }
    }
    if (node.dclType != NA) {
        cout << " : " << getDclTypeStr(node.dclType) << endl;
    } else {
        if (node.type == 1 && node.token.getKind() == "ID" && symbTable[curProcedure].count(node.token.getLexeme())) {
            if (node.isProID()) {
                cout << endl;
            } else {
                cout << " : " << getDclTypeStr(getIDType(curProcedure, node.token.getLexeme())) << endl;
            }
        } else {
            cout << endl;
        }
    }
    if (curProcedure == "NA" && node.type == 2 && node.prod[0] == "procedures" && node.prod.size() == 2) {
        curProcedure = node.prod[1];
    } else if (curProcedure == "NA" && node.type == 2 && node.prod[0] == "procedure") {
        curProcedure = allNodes[node.children[1]].token.getLexeme();
    }
    for (int i = 0; i < (node.children).size(); i ++) {
        output(allNodes[(node.children)[i]], curProcedure);
    }
    return;
}

int main() {

    readAndBuild(-1, "NA");
    if (globalErrorCheck == true) {
        cerr << "ERROR" << endl;
        return 0;
    }
    output(allNodes[0], "NA");

    return 0;
}