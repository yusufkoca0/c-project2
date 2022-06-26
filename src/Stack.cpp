
#include <fstream>
#include <iostream>
#include "Stack.h"


Stack::Stack() : top(-1) {}

void Stack::push(std::string s){
    stack.push_back(s);
    top++;
}
void Stack::pop(){
    stack.pop_back();
    top--;
}
std::string Stack::peek(){
    if (top > -1) {
        return stack.at(top);
    }
    return "";
}
bool Stack::isEmpty(){
    return stack.size() == 0;
}
void Stack::print(std::string output){
    std::ofstream ofs;
    ofs.open(output, std::fstream::app);
    if (stack.size() > 0) {
        for (int i = 0; i < stack.size()-1 ; ++i) {
            ofs << stack.at(i) << ",";
        }
        ofs << stack.at(stack.size() - 1);
    }
    ofs << "\n";
    ofs.close();
}
void Stack::clear() {
    for (int i = top; i > -1 ; --i) {
        stack.pop_back();
        top--;
    }
}
