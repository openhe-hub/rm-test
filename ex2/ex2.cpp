#include <iostream>
#include "Group.h"
#include "Pattern.h"

#define RECTANGLE "-r"
#define CIRCLE "-c"
#define STAR "-l"

#define CREATE "create"
#define DELETE "del"
#define MOVE "mv"
#define SET "set"
#define UNDO "undo"
#define REDO "redo"

using namespace std;

struct Token {
    string command;
    int int_arg1;
    int int_arg2;
    char ch_arg;
};

char transform(string type) {
    char pattern = ' ';
    if (type == RECTANGLE) pattern = '+';
    else if (type == CIRCLE) pattern = 'o';
    else if (type == STAR) pattern = '*';
    return pattern;
}

//the initial commands will be compiled to token(mainly deal with undo & redo)
vector<Token> compile() {
    string op;
    vector<Token> tokens;
    Token last_undo;
    vector<Token>::iterator last_undo_idx;
    while ((cin >> op) && (op != "exit")) {
        int int_arg1 = 0, int_arg2 = 0;
        string str_arg = " ";
        char ch_arg = ' ';

        if (op == CREATE) {
            cin >> int_arg1 >> int_arg2 >> str_arg;
            ch_arg = transform(str_arg);
        } else if (op == DELETE) {
            cin >> int_arg1;
        } else if (op == MOVE) {
            cin >> int_arg1 >> int_arg2;
        } else if (op == SET) {
            cin >> int_arg1 >> str_arg;
            ch_arg = transform(str_arg);
        } else if (op == UNDO) {
            last_undo=tokens[tokens.size()-1];
            last_undo_idx=tokens.end();
            tokens.erase(tokens.end());
            tokens.resize(tokens.size()+1);
            continue;
        } else if (op == REDO) {
            tokens.insert(last_undo_idx,last_undo);
            continue;
        }
        tokens.push_back({op, int_arg1, int_arg2, ch_arg});
    }
    return tokens;
}

int main() {
    //init
    Group group;
    group.graph.resize(10);
    Pattern p0 = {' ', 0, 0};
    group.patterns.insert(make_pair(0, p0));
    //input and compile
    cout << "NOTE:Please type 'exit' to end your input!!!" << endl;
    auto tokens = compile();
    //exec
    for (const auto &token: tokens) {
        int int_arg1 = token.int_arg1, int_arg2 = token.int_arg2;
        char ch_arg = token.ch_arg;
        string op = token.command;
        if (op == CREATE) {
            Pattern p = {ch_arg, int_arg1, int_arg2};
            group.add(p);
        } else if (op == DELETE) {
            group.remove(int_arg1);
        } else if (op == MOVE) {
            group.move(int_arg1, int_arg2);
        } else if (op == SET) {
            group.set(int_arg1, ch_arg);
        }
    }
    //output
    group.print();
    return 0;
}