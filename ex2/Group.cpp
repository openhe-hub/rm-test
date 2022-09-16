//
// Created by 62552 on 9/16/2022.
//

#include "Group.h"
#include <iostream>
#include <algorithm>

using namespace std;



void Group::print() {
    for (int i = 0; i < graph[patterns[0].id].size(); ++i) {
        Component res= generate(graph[patterns[0].id][i]);
        for (int j = 0; j < res.height; ++j) {
            cout<<res.str[j]<<endl;
        }
        cout<<endl;
    }
}

void Group::add(const Pattern& p) {
    patterns.insert(make_pair(p.id,p));
    graph[p.father].push_back(p);
    size+=1;
}

Group::Component Group::generate(Pattern root) {
    if (graph[root.id].empty()){
        int min_height=3;
        int min_width=5;
        string* res=new string[min_height];
        for (int i = 0; i < min_height; ++i) {
            if (i==1){
                for (int j = 0; j < min_width; ++j) {
                    if (j==0||j==min_width-1) res[i]+="|";
                    else if (j==1) res[i]+=to_string(root.id);
                    else res[i]+=' ';
                }
            }else{
                for (int j = 0; j < min_width; ++j) {
                    if (j==0||j==min_width-1) res[i]+=root.pattern;
                    else res[i].append("-");
                }
            }
        }
        return {res,min_width,min_height};
    }else{
        vector<Component> components;
        int height=0,width=0,num=graph[root.id].size();
        for (int i = 0; i < num; ++i) {
            Component tmp=generate(graph[root.id][i]);
            height+=tmp.height;
            width= max(width,tmp.width);
            components.push_back(tmp);
        }
        height+=num+3;
        width+=4;
        string* res=new string[height];

        for (int i = 0; i < width; ++i) {
            if (i==0||i==width-1) res[0]+=root.pattern;
            else res[0].append("-");
        }
        for (int i = 0; i < width; ++i) {
            if (i==0||i==width-1) res[1].append("|");
            else if (i==1)  res[1].append(to_string(root.id));
            else res[1]+=" ";
        }
        int cnt=2;
        for (int i = 0; i < num; ++i) {
            for (int j = 0; j < components[i].height; ++j) {
                for (int k = 0; k < width; ++k) {
                    if (k==0||k==width-1) res[cnt+j].append("|");
                    else if (k==1||k==width-2) res[cnt+j].append(" ");
                    else if (k>=2&&k<=components[i].width+1) res[cnt+j]+=components[i].str[j][k-2];
                }
            }
            cnt+=components[i].height;
            for (int j = 0; j < width; ++j) {
                if (j==0||j==width-1) res[cnt].append("|");
                else res[cnt].append(" ");;
            }
            cnt++;
        }
        for (int i = 0; i < width; ++i) {
            if (i==0||i==width-1) res[height-1]+=root.pattern;
            else res[height-1].append("-");
        }
        return {res,width,height};
    }
}

void Group::remove(int id) {
    int father=patterns[id].father;
    for(auto it=graph[father].begin();it!=graph[father].end();it++){
        if (it->id==id){
            graph[father].erase(it);
            break;
        }
    }
    patterns.erase(patterns.find(id));
    graph[id].clear();
}

void Group::set(int id, char ch) {
    patterns[id].pattern=ch;
    int father=patterns[id].father;
    for(auto it=graph[father].begin();it!=graph[father].end();it++){
        if (it->id==id){
            it->pattern=ch;
            break;
        }
    }
}

void Group::move(int id, int move_id) {
    Pattern copy=patterns[id];
    remove(id);
    copy.father=move_id;
    add(copy);
}
