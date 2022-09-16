//
// Created by 62552 on 9/16/2022.
//

#ifndef RM_TEST_GROUP_H
#define RM_TEST_GROUP_H
#include <vector>
#include <string>
#include <map>
#include "Pattern.h"

using namespace std;


class Group {
public:
    int size=0;
    vector<vector<Pattern>> graph;
    map<int,Pattern> patterns;
    void print();
    void add(const Pattern& b);
    void remove(int id);
    void set(int id,char ch);
    void move(int id,int move_id);
private:
    struct Component{
        string* str;
        int width;
        int height;
    };
    Component generate(Pattern root);
};


#endif //RM_TEST_GROUP_H
