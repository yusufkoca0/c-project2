
#ifndef UNTITLED1_STACK_H
#define UNTITLED1_STACK_H


#include <vector>
#include <string>

class Stack {
    std::vector<std::string> stack;
    int top{};

public:
    Stack();

    void push(std::string s);
    void pop();
    std::string peek();
    bool isEmpty();
    void clear();
    void print(std::string output);
};


#endif //UNTITLED1_STACK_H
