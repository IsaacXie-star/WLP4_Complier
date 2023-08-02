#include <iostream>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <vector>
#include <set>
#include <unordered_set>
#include <memory>
#include <sstream>

using namespace std;

/*
$0 always 0 (or false)
$1 wain's 1st argument (a1)
$2 wain's 2nd argument (a2)
$3 result (and intermediate result) of calculations
$4 constant 4, useful for pushing and poping the stack
$5 previous intermediate results
$11 always 1
$29 frame pointer (fp)
$30 stack pointer (sp)
$31 return address (ra)

=============================================
Program Prolog:
- initialize constants (store 4 in $4, and 1 in $11)
- store return address ($31) on stack
- initialize frame pointer ($29) and create stack frame
- store arguments ($1 and $2) in stack frame

Program Body:
- initialize local variables in stack frame
- generate code for body of function

Program Epilog:
- pop stack frame
- restore previous return address to $31
*/

enum Vtype {
    INT = 1,
    INT_STAR,
    NA
};
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
    Node(int _node_type, Token _token, string& _prod, int _id) {
        id = _id;
        node_type = _node_type;
        token = _token;
        prod = _prod;
        parent = -1;
        children.clear();
    }
    int id;
    int node_type;
    Vtype value_type;
    Token token; // type 1
    string prod; // type 2
    vector<int> children;
    int parent;
    bool is_leaf = false;
};

int root_id = 0;
string procedure_track = "NA";
map<string, unordered_map<string, int>> symbol_table;
int gb_offset = 0;
vector<Node> tree;
int if_counter = 0;
bool pos_offset = false;
int arg_count = 0;


bool check_leaf(vector<string> &content) {
    if (content.size() != 2 && content.size() != 4) return false;
    auto s = content[0];
    for (auto c : s) {
        if (c < 'A' || c > 'Z') return false;
    }
    return true;
}

Vtype obtain_vtype(vector<string> &content) {
    if (content.size() == 2) {
        return NA;
    }
    if (content[3] == "int") return INT;
    return INT_STAR;
}

int read_and_build(int parentNode) {
    string line;
    stringstream ss;
    if (!getline(cin, line)) {
        return -1;
    }
    ss << line;
    string str;
    vector<string> strs;
    while (ss >> str) {
        strs.push_back(str);
    }
    bool is_leaf = check_leaf(strs);
    if (is_leaf) {
        Token token = {strs[0], strs[1]};
        Vtype vtype = obtain_vtype(strs);
        Node root(1, token, line, tree.size());
        root.parent = parentNode;
        root.value_type = vtype;
        root.is_leaf = true;
        if (parentNode != -1) tree[parentNode].children.push_back(root.id);
        tree.push_back(root);
        return root.id;
    }
    Token token;
    Node root(2, token, line, tree.size());
    root.parent = parentNode;
    if (parentNode != -1) tree[parentNode].children.push_back(root.id);
    tree.push_back(root);
    // .EMPTY edge case
    if (strs.size() == 2 && strs[1] == ".EMPTY") {
        // return directly
        return root.id;
    }
    if (strs[strs.size() - 2] == ":" && strs[strs.size() - 1] == "int") {
        tree[root.id].value_type = INT;
        tree[root.id].prod = tree[root.id].prod.substr(0, tree[root.id].prod.size() - 6);
        strs.pop_back();
        strs.pop_back();
    }
    if (strs[strs.size() - 2] == ":" && strs[strs.size() - 1] == "int*") {
        tree[root.id].value_type = INT_STAR;
        tree[root.id].prod = tree[root.id].prod.substr(0, tree[root.id].prod.size() - 7);
        strs.pop_back();
        strs.pop_back();
    }
    for (int i = 1; i < strs.size(); i ++) {
        read_and_build(root.id);
    }
    return root.id;
}

void prolog_wain() {
    cout << ";;;prolog" << endl;
    cout << "lis $4" << endl;
    cout << ".word 4" << endl;
    cout << "lis $11" << endl;
    cout << ".word 1" << endl;
    cout << "sub $29, $30, $4 ; init fp" << endl;
    cout << ";;;prolog done" << endl;
    cout << endl;
}

void epilog_wain() {
    cout << endl;
    cout << ";;;epilog" << endl;
    cout << "add $30, $29, $4" << endl;
    cout << "jr $31" << endl;
    cout << ";;;epilog done" << endl;
    cout << endl;
}

void prolog_func() {
    cout << ";;;prolog func" << endl;
    cout << "sub $29, $30, $4 ; init fp" << endl;
    cout << ";;;prolog func done" << endl;
    cout << endl;
}

void epilog_func() {
    cout << endl;
    cout << ";;;epilog func" << endl;
    cout << "add $30, $29, $4" << endl;
    cout << "jr $31" << endl;
    cout << ";;;epilog func done" << endl;
    cout << endl;
}

void push(int reg) {
    cout << "sw $" << reg << ", -4($30)" << endl;
    cout << "sub $30, $30, $4" << endl;
}

void pure_pop() {
    cout << "add $30, $30, $4" << endl;
}

void pop(int reg) {
    cout << "add $30, $30, $4" << endl;
    cout << "lw $" << reg << ", -4($30)" << endl;
}

int get_offset(string var) {
    return symbol_table[procedure_track][var];
}

void code(string var) {
    cout << "lw $3, " << get_offset(var) << "($29)" << endl;
}

void store(string var) {
    cout << "sw $3, " << get_offset(var) << "($29)" << endl;
}

void code_const(string num) {
    cout << "lis $3" << endl;
    cout << ".word " << num << endl;
}

string gen_label() {
    return "label" + to_string(if_counter ++);
}

void add_symbol_table(string procedure, string var, int offset) {
    symbol_table[procedure][var] = offset;
}

void build_symbol_table(int root) {
    if (root == -1) return;
    // update procedure_track
    string pre_procedure = procedure_track;
    if (tree[root].prod == "main INT WAIN LPAREN dcl COMMA dcl RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE") {
        procedure_track = "wain";
    }
    if (tree[root].prod == "procedure INT ID LPAREN params RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE") {
        procedure_track = tree[tree[root].children[1]].token.getLexeme();
    }

    // update symbol_table
    if (tree[root].prod == "main INT WAIN LPAREN dcl COMMA dcl RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE") {
        gb_offset = 0;
        build_symbol_table(tree[root].children[3]);
        build_symbol_table(tree[root].children[5]);
        build_symbol_table(tree[root].children[8]);
    } else if (tree[root].prod == "dcl type ID") {
        add_symbol_table(procedure_track, tree[tree[root].children[1]].token.getLexeme(), gb_offset);
        if (pos_offset) gb_offset += 4;
        else gb_offset -= 4;
    } else if (tree[root].prod == "start BOF procedures EOF") {
        build_symbol_table(tree[root].children[1]);
    } else if (tree[root].prod == "procedures main") {
        build_symbol_table(tree[root].children[0]);
    } else if (tree[root].prod == "dcls dcls dcl BECOMES NUM SEMI") {
        build_symbol_table(tree[root].children[0]);
        build_symbol_table(tree[root].children[1]);
        // code_const(tree[tree[root].children[3]].token.getLexeme());
        // push(3);
    } else if (tree[root].prod == "procedures procedure procedures") {
        build_symbol_table(tree[root].children[0]);
        build_symbol_table(tree[root].children[1]);
    } else if (tree[root].prod == "procedure INT ID LPAREN params RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE") {
        gb_offset = 4;
        pos_offset = true;
        build_symbol_table(tree[root].children[3]);
        pos_offset = false;
        gb_offset = 0;
        build_symbol_table(tree[root].children[6]);
    } else if (tree[root].prod == "params paramlist") {
        build_symbol_table(tree[root].children[0]);
    } else if (tree[root].prod == "paramlist dcl") {
        build_symbol_table(tree[root].children[0]);
    } else if (tree[root].prod == "paramlist dcl COMMA paramlist") {
        build_symbol_table(tree[root].children[2]);
        build_symbol_table(tree[root].children[0]);
    } else if (tree[root].prod == "dcls dcls dcl BECOMES NULL SEMI") {
        build_symbol_table(tree[root].children[0]);
        build_symbol_table(tree[root].children[1]);
    }

    procedure_track = pre_procedure;
}

bool check_lvalue(int root) {
    if (tree[root].prod == "lvalue LPAREN lvalue RPAREN") return check_lvalue(tree[root].children[1]);
    else if (tree[root].prod == "lvalue ID") return true;
    return false;
}

string get_lvalue_lexeme(int root) {
    if (tree[root].prod == "lvalue LPAREN lvalue RPAREN") return get_lvalue_lexeme(tree[root].children[1]);
    return tree[tree[root].children[0]].token.getLexeme();
}

void gencode(int root) {
    if (root == -1) return;
    // update procedure_track
    string pre_procedure = procedure_track;
    if (tree[root].prod == "main INT WAIN LPAREN dcl COMMA dcl RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE") {
        procedure_track = "wain";
    }
    if (tree[root].prod == "procedure INT ID LPAREN params RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE") {
        procedure_track = tree[tree[root].children[1]].token.getLexeme();
    }

    if (tree[root].prod == "start BOF procedures EOF") {
        gencode(tree[root].children[1]);
    } else if (tree[root].prod == "main INT WAIN LPAREN dcl COMMA dcl RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE") {
        cout << "Fwain:" << endl;
        prolog_wain();
        push(1);
        push(2);
        push(2);
        if (tree[tree[tree[root].children[3]].children[1]].value_type == INT) {
            cout << "lis $2" << endl;
            cout << ".word 0" << endl;
        }
        cout << "lis $5" << endl;
        cout << ".word init" << endl;
        push(31);
        cout << "jalr $5" << endl;
        pop(31);
        pop(2);
        gencode(tree[root].children[8]);
        gencode(tree[root].children[9]);
        gencode(tree[root].children[11]);
        epilog_wain();
    } else if (tree[root].prod == "procedures main" || tree[root].prod == "expr term" || tree[root].prod == "term factor") {
        gencode(tree[root].children[0]);
    } else if (tree[root].prod == "factor LPAREN expr RPAREN") {
        gencode(tree[root].children[1]);
    } else if (tree[root].prod == "factor ID" || tree[root].prod == "factor NUM") {
        gencode(tree[root].children[0]);
    } else if (tree[root].node_type == 1) {
        if (tree[root].token.getKind() == "ID") {
            code(tree[root].token.getLexeme());
        } else if (tree[root].token.getKind() == "NUM") {
            code_const(tree[root].token.getLexeme());
        }
    } else if (tree[root].prod == "statements statements statement") {
        gencode(tree[root].children[0]);
        gencode(tree[root].children[1]);
    } else if (tree[root].prod == "statement lvalue BECOMES expr SEMI") {
        if (tree[tree[root].children[0]].value_type == INT) {
            if (tree[tree[root].children[0]].prod == "lvalue ID") {
                gencode(tree[root].children[2]);
                store(tree[tree[tree[root].children[0]].children[0]].token.getLexeme());
            } else if (tree[tree[root].children[0]].prod == "lvalue STAR factor") {
                gencode(tree[root].children[2]);
                push(3);
                gencode(tree[root].children[0]);
                pop(5);
                cout << "sw $5, 0($3)" << endl;
            } else {
                // lvalue LPAREN lvalue RPAREN
                if (check_lvalue(tree[root].children[0])) {
                    gencode(tree[root].children[2]);
                    store(tree[tree[tree[root].children[0]].children[0]].token.getLexeme());
                } else {
                    gencode(tree[root].children[2]);
                    push(3);
                    gencode(tree[root].children[0]);
                    pop(5);
                    cout << "sw $5, 0($3)" << endl;
                }
            }
        } else if (tree[tree[root].children[0]].value_type == INT_STAR) {
            gencode(tree[root].children[2]);
            store(tree[tree[tree[root].children[0]].children[0]].token.getLexeme());
        }
    } else if (tree[root].prod == "lvalue ID") {

    } else if (tree[root].prod == "lvalue LPAREN lvalue RPAREN") {
        gencode(tree[root].children[1]);
    } else if (tree[root].prod == "expr expr PLUS term") {
        if (tree[tree[root].children[0]].value_type == INT && tree[tree[root].children[2]].value_type == INT) {
            gencode(tree[root].children[0]);
            push(3);
            gencode(tree[root].children[2]);
            pop(5);
            cout << "add $3, $3, $5" << endl;
        } else if (tree[tree[root].children[0]].value_type == INT_STAR && tree[tree[root].children[2]].value_type == INT) {
            gencode(tree[root].children[0]);
            push(3);
            gencode(tree[root].children[2]);
            cout << "mult $3, $4" << endl;
            cout << "mflo $3" << endl;
            pop(5);
            cout << "add $3, $5, $3" << endl;
        } else if (tree[tree[root].children[0]].value_type == INT && tree[tree[root].children[2]].value_type == INT_STAR) {
            gencode(tree[root].children[2]);
            push(3);
            gencode(tree[root].children[0]);
            cout << "mult $3, $4" << endl;
            cout << "mflo $3" << endl;
            pop(5);
            cout << "add $3, $5, $3" << endl;
        }
    } else if (tree[root].prod == "expr expr MINUS term") {
        if (tree[tree[root].children[0]].value_type == INT && tree[tree[root].children[2]].value_type == INT) {
            gencode(tree[root].children[0]);
            push(3);
            gencode(tree[root].children[2]);
            pop(5);
            cout << "sub $3, $5, $3" << endl;
        } else if (tree[tree[root].children[0]].value_type == INT_STAR && tree[tree[root].children[2]].value_type == INT) {
            gencode(tree[root].children[0]);
            push(3);
            gencode(tree[root].children[2]);
            cout << "mult $3, $4" << endl;
            cout << "mflo $3" << endl;
            pop(5);
            cout << "sub $3, $5, $3" << endl;
        } else if (tree[tree[root].children[0]].value_type == INT_STAR && tree[tree[root].children[2]].value_type == INT_STAR) {
            gencode(tree[root].children[0]);
            push(3);
            gencode(tree[root].children[2]);
            pop(5);
            cout << "sub $3, $5, $3" << endl;
            cout << "div $3, $4" << endl;
            cout << "mflo $3" << endl;
        }
        
    } else if (tree[root].prod == "term term STAR factor") {
        gencode(tree[root].children[0]);
        push(3);
        gencode(tree[root].children[2]);
        pop(5);
        cout << "mult $5, $3" << endl;
        cout << "mflo $3" << endl;
    } else if (tree[root].prod == "term term SLASH factor") {
        gencode(tree[root].children[0]);
        push(3);
        gencode(tree[root].children[2]);
        pop(5);
        cout << "div $5, $3" << endl;
        cout << "mflo $3" << endl;
    } else if (tree[root].prod == "term term PCT factor") {
        gencode(tree[root].children[0]);
        push(3);
        gencode(tree[root].children[2]);
        pop(5);
        cout << "div $5, $3" << endl;
        cout << "mfhi $3" << endl;
    } else if (tree[root].prod == "statement IF LPAREN test RPAREN LBRACE statements RBRACE ELSE LBRACE statements RBRACE") {
        gencode(tree[root].children[2]);
        string label1 = gen_label();
        string label2 = gen_label();
        cout << "beq $3, $0, " << label1 << endl;
        gencode(tree[root].children[5]);
        cout << "beq $0, $0, " << label2 << endl;
        cout << label1 << ":" << endl;
        gencode(tree[root].children[9]);
        cout << label2 << ":" << endl;
    } else if (tree[root].prod == "statement WHILE LPAREN test RPAREN LBRACE statements RBRACE") {
        string label1 = gen_label();
        string label2 = gen_label();
        cout << label1 << ":" << endl;
        gencode(tree[root].children[2]);
        cout << "beq $3, $0, " << label2 << endl;
        gencode(tree[root].children[5]);
        cout << "beq $0, $0, " << label1 << endl;
        cout << label2 << ":" << endl;

    } else if (tree[root].prod == "test expr EQ expr") {
        gencode(tree[root].children[0]);
        push(3);
        gencode(tree[root].children[2]);
        pop(5);
        if (tree[tree[root].children[0]].value_type == INT) {
            cout << "slt $6, $3, $5" << endl;
            cout << "slt $7, $5, $3" << endl;
        } else if (tree[tree[root].children[0]].value_type == INT_STAR) {
            cout << "sltu $6, $3, $5" << endl;
            cout << "sltu $7, $5, $3" << endl;
        }
        cout << "add $3, $6, $7" << endl;
        cout << "sub $3, $11, $3" << endl;
    } else if (tree[root].prod == "test expr NE expr") {
        gencode(tree[root].children[0]);
        push(3);
        gencode(tree[root].children[2]);
        pop(5);
        if (tree[tree[root].children[0]].value_type == INT) {
            cout << "slt $6, $3, $5" << endl;
            cout << "slt $7, $5, $3" << endl;
        } else if (tree[tree[root].children[0]].value_type == INT_STAR) {
            cout << "sltu $6, $3, $5" << endl;
            cout << "sltu $7, $5, $3" << endl;
        }
        cout << "add $3, $6, $7" << endl;
    } else if (tree[root].prod == "test expr LT expr") {
        gencode(tree[root].children[0]);
        push(3);
        gencode(tree[root].children[2]);
        pop(5);
        if (tree[tree[root].children[0]].value_type == INT) {
            cout << "slt $3, $5, $3" << endl;
        } else if (tree[tree[root].children[0]].value_type == INT_STAR) {
            cout << "sltu $3, $5, $3" << endl;
        }
    } else if (tree[root].prod == "test expr LE expr") {
        gencode(tree[root].children[0]);
        push(3);
        gencode(tree[root].children[2]);
        pop(5);
        if (tree[tree[root].children[0]].value_type == INT) {
            cout << "slt $3, $3, $5" << endl;
        } else if (tree[tree[root].children[0]].value_type == INT_STAR) {
            cout << "sltu $3, $3, $5" << endl;
        }
        cout << "sub $3, $11, $3" << endl;
    } else if (tree[root].prod == "test expr GE expr") {
        gencode(tree[root].children[0]);
        push(3);
        gencode(tree[root].children[2]);
        pop(5);
        if (tree[tree[root].children[0]].value_type == INT) {
            cout << "slt $3, $5, $3" << endl;
        } else if (tree[tree[root].children[0]].value_type == INT_STAR) {
            cout << "sltu $3, $5, $3" << endl;
        }
        cout << "sub $3, $11, $3" << endl;
    } else if (tree[root].prod == "test expr GT expr") {
        gencode(tree[root].children[0]);
        push(3);
        gencode(tree[root].children[2]);
        pop(5);
        if (tree[tree[root].children[0]].value_type == INT) {
            cout << "slt $3, $3, $5" << endl;
        } else if (tree[tree[root].children[0]].value_type == INT_STAR) {
            cout << "sltu $3, $3, $5" << endl;
        }
    } else if (tree[root].prod == "statement PRINTLN LPAREN expr RPAREN SEMI") {
        gencode(tree[root].children[2]);
        push(1);
        cout << "add $1, $0, $3" << endl;
        push(31);
        cout << "lis $5" << endl;
        cout << ".word print" << endl;
        cout << "jalr $5" << endl;
        pop(31);
        pop(1);
    } else if (tree[root].prod == "procedures procedure procedures") {
        gencode(tree[root].children[0]);
        gencode(tree[root].children[1]);
    } else if (tree[root].prod == "procedure INT ID LPAREN params RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE") {
        cout << "F" << tree[tree[root].children[1]].token.getLexeme() << ":" << endl;
        prolog_func();
        gencode(tree[root].children[6]);
        push(1), push(2), push(5), push(6), push(7), push(11);
        gencode(tree[root].children[7]);
        gencode(tree[root].children[9]);
        pop(11), pop(7), pop(6), pop(5), pop(2), pop(1);
        epilog_func();
    } else if (tree[root].prod == "factor ID LPAREN RPAREN") {
        push(31);
        push(29);
        cout << "lis $5" << endl;
        cout << ".word F" << tree[tree[root].children[0]].token.getLexeme() << endl;
        cout << "jalr $5" << endl;
        pop(29);
        pop(31);
    } else if (tree[root].prod == "factor ID LPAREN arglist RPAREN") {
        push(31);
        push(29);
        int pre_arg_count = arg_count;
        gencode(tree[root].children[2]);
        cout << "lis $5" << endl;
        cout << ".word F" << tree[tree[root].children[0]].token.getLexeme() << endl;
        cout << "jalr $5" << endl;
        while (arg_count != pre_arg_count) pure_pop(), arg_count --;
        pop(29);
        pop(31);
    } else if (tree[root].prod == "arglist expr") {
        gencode(tree[root].children[0]);
        push(3);
        arg_count ++;
    } else if (tree[root].prod == "arglist expr COMMA arglist") {
        gencode(tree[root].children[0]);
        push(3);
        arg_count ++;
        gencode(tree[root].children[2]);
    } else if (tree[root].prod == "dcls dcls dcl BECOMES NUM SEMI") {
        gencode(tree[root].children[0]);
        gencode(tree[root].children[1]);
        code_const(tree[tree[root].children[3]].token.getLexeme());
        push(3);
    } else if (tree[root].prod == "type INT STAR") {

    } else if (tree[root].prod == "dcls dcls dcl BECOMES NULL SEMI") {
        gencode(tree[root].children[0]);
        gencode(tree[root].children[1]);
        code_const("1");
        push(3);
    } else if (tree[root].prod == "factor NULL") {
        cout << "lis $3" << endl;
        cout << ".word 0x1" << endl;
    } else if (tree[root].prod == "factor AMP lvalue") {
        if (tree[tree[root].children[1]].prod == "lvalue ID") {
            cout << "lis $3" << endl;
            cout << ".word " << get_offset(tree[tree[tree[root].children[1]].children[0]].token.getLexeme()) << endl;
            cout << "add $3, $3, $29" << endl;
        } else if (tree[tree[root].children[1]].prod == "lvalue STAR factor") {
            gencode(tree[tree[root].children[1]].children[1]);
        } else if (tree[tree[root].children[1]].prod == "lvalue LPAREN lvalue RPAREN") {
            if (check_lvalue(tree[root].children[1])) {
                string lexeme = get_lvalue_lexeme(tree[root].children[1]);
                cout << "lis $3" << endl;
                cout << ".word " << get_offset(lexeme) << endl;
                cout << "add $3, $3, $29" << endl;
            } else {
                gencode(tree[tree[root].children[1]].children[1]);
            }
        }
    } else if (tree[root].prod == "factor STAR factor") {
        gencode(tree[root].children[1]);
        cout << "lw $3, 0($3)" << endl;
    } else if (tree[root].prod == "lvalue STAR factor") {
        gencode(tree[root].children[1]);
    } else if (tree[root].prod == "factor NEW INT LBRACK expr RBRACK") {
        gencode(tree[root].children[3]);
        cout << "add $1, $3, $0" << endl;
        cout << "lis $3" << endl;
        cout << ".word new" << endl;
        push(31);
        cout << "jalr $3" << endl;
        pop(31);
        cout << "bne $3, $0, 1" << endl;
        cout << "add $3, $0, $11" << endl;
    } else if (tree[root].prod == "statement DELETE LBRACK RBRACK expr SEMI") {
        gencode(tree[root].children[3]);
        string label = gen_label();
        cout << "beq $3, $11, " << label << endl;
        cout << "add $1, $3, $0" << endl;
        cout << "lis $3" << endl;
        cout << ".word delete" << endl;
        push(31);
        cout << "jalr $3" << endl;
        pop(31);
        cout << label << ":" << endl;
    }

    procedure_track = pre_procedure;
}

int main() {

    root_id = read_and_build(-1);
    build_symbol_table(root_id);
    cout << ".import print" << endl;
    cout << ".import init" << endl;
    cout << ".import new" << endl;
    cout << ".import delete" << endl;
    cout << "lis $5" << endl;
    cout << ".word Fwain" << endl;
    cout << "jr $5" << endl; // jump to wain procedure
    cout << endl;
    gencode(root_id);

    return 0;
}