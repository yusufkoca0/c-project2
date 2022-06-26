#include <iostream>
#include <fstream>
#include <vector>
#include "Stack.h"
#include <sstream>

struct transitionRule{
    transitionRule(const std::string &startState, const std::string &inputSymbol, const std::string &popSymbol,
                   const std::string &resultState, const std::string &pushSymbol) : startState(startState),
                                                                                    inputSymbol(inputSymbol),
                                                                                    popSymbol(popSymbol),
                                                                                    resultState(resultState),
                                                                                    pushSymbol(pushSymbol) {}

    std::string startState;
    std::string inputSymbol;
    std::string popSymbol;
    std::string resultState;
    std::string pushSymbol;
};

struct state{
    state(const std::string &name) : name(name) {}

    std::string name;
    int start;
    int final;
    std::vector<transitionRule> transitionRules;
};

struct DPDA{
    std::vector<state> states;
    std::vector<std::string> inputAlph;
    std::vector<std::string> stackAlph;
    Stack stack;
};

DPDA dpda;
Stack inputstack;
bool error;

void constructStackAlphabet(std::string Z);

void constructAlphabet(std::string A);

void constructTransitionRule(std::string T, std::string output);

void splitState(std::vector<std::string>& States, std::vector<std::string>& specialities);
void constructState(std::string Q);

void constructDPDA(std::vector<std::string>& dpdaInfo, std::string output);

void printResult(int i, std::string output);
void printTransition(transitionRule t, std::string output);
void splitInput(std::string input);
void executeTransition(transitionRule t, std::string output);
void transition(std::string s, const std::string& output);
void executeInput(std::string input, std::string output);

int main(int argc, const char * argv[]) {
    std::string output = argv[3];
    std::string str;
    std::vector<std::string> dpdaInfo;
    std::vector<std::string> inputInfo;

    //reading dpda
    std::ifstream in1(argv[1]);
    while (std::getline(in1, str))
    {
        if(!str.empty()) {
            dpdaInfo.push_back(str);
        }
    }
    in1.close();

    //reading input
    std::ifstream in2(argv[2]);
    while (std::getline(in2, str))
    {
        inputInfo.push_back(str);
    }
    in2.close();

    //clearing output file
    std::ofstream ofs;
    ofs.open(argv[3], std::ofstream::out | std::ofstream::trunc);
    ofs.close();

    constructDPDA(dpdaInfo, output);
    if (!error){
        return 0;
    }
    for (int i = 0; i < inputInfo.size(); ++i) {
        executeInput(inputInfo.at(i), output);
        inputstack.clear();
    }
}

//-----------------------DPDA----------------------------------------------------
//constructs DPDA by calling other functions that constructs the parts of DPDA
void constructDPDA(std::vector<std::string>& dpdaInfo, std::string output){
    constructState(dpdaInfo.at(0));
    constructAlphabet(dpdaInfo.at(1));
    constructStackAlphabet(dpdaInfo.at(2));
    for (int i = 3; i < dpdaInfo.size(); ++i) {
        constructTransitionRule(dpdaInfo.at(i), output);
        if (!error){
            return;
        }
    }

}
//function that splits state line to the correct format
void splitState(std::string Q, std::vector<std::string>& States, std::vector<std::string>& specialities){
    std::vector<std::string> tokens;
    std::string intermediate;

    Q.erase(0,1);
    Q.erase(0,1);

    // Tokenizing
    std::stringstream check1(Q);

    while(getline(check1, intermediate, '='))
    {
        tokens.push_back(intermediate);
    }

    std::stringstream check2(tokens.at(0));

    while(getline(check2, intermediate, ','))
    {
        States.push_back(intermediate);
    }

    tokens.at(1).erase(0,1);
    tokens.at(1).erase(0,1);

    std::stringstream check3(tokens.at(1));

    while(getline(check3, intermediate, ','))
    {
        specialities.push_back(intermediate);
    }

    std::string last = States.at(States.size()-1);
    last.erase(last.size()-1);
    States.pop_back();
    States.push_back(last);
    tokens.clear();


}
//constructs states with name, start and final values and adds them into states vector
void constructState(std::string Q){

    std::vector<std::string> specialities;
    std::vector<std::string> States;

    splitState(Q, States, specialities);

    //creates stats with their names and adds them to the states vector
    for (int i = 0; i < States.size(); ++i) {
        dpda.states.push_back(state(States.at(i)));
    }

    //assigns start or final values as 1 if a state is start or final
    for (int i = 0; i < specialities.size(); ++i) {
        if (specialities.at(i).rfind("(", 0) == 0){
            specialities.at(i).erase(0,1);
            specialities.at(i).erase(specialities.at(i).size()-1);
            for (int j = 0; j < dpda.states.size(); ++j) {
                if (dpda.states.at(j).name == specialities.at(i)){
                    dpda.states.at(j).start = 1;
                }
            }
        }
        else if (specialities.at(i).rfind("[", 0) == 0){

            specialities.at(i).erase(0,1);
            specialities.at(i).erase(specialities.at(i).size()-1);

            for (int j = 0; j < dpda.states.size(); ++j) {
                if (dpda.states.at(j).name == specialities.at(i)){
                    dpda.states.at(j).final = 1;
                }
            }
        }
    }

    //assigns start or final values as 0 if a state is not start or final
    for (int i = 0; i < dpda.states.size(); ++i) {
        if (dpda.states.at(i).start != 1){
            dpda.states.at(i).start = 0;
        }
        if (dpda.states.at(i).final != 1){
            dpda.states.at(i).final = 0;
        }
    }
    States.clear();
    specialities.clear();

}
//constructs input alphabet
void constructAlphabet(std::string A){
    std::string intermediate;

    A.erase(0,1);
    A.erase(0,1);

    // Tokenizing
    std::stringstream check1(A);

    while(getline(check1, intermediate, ','))
    {
        dpda.inputAlph.push_back(intermediate);
    }
}
//constructs stack alphabet
void constructStackAlphabet(std::string Z){
    std::string intermediate;

    Z.erase(0,1);
    Z.erase(0,1);

    // Tokenizing
    std::stringstream check1(Z);

    while(getline(check1, intermediate, ','))
    {
        dpda.stackAlph.push_back(intermediate);
    }
}
//constructs transition rules and adds them into correct states
void constructTransitionRule(std::string T, std::string output){
    std::vector<std::string> transitionRuleVector;
    std::string intermediate;
    error = 0;

    T.erase(0,1);
    T.erase(0,1);

    // Tokenizing
    std::stringstream check1(T);

    while(getline(check1, intermediate, ','))
    {
        transitionRuleVector.push_back(intermediate);
    }

    for (int i = 0; i < dpda.states.size(); ++i) {
        if (dpda.states.at(i).name == transitionRuleVector.at(0)){
            for (int j = 0; j < dpda.inputAlph.size(); ++j) {
                if (dpda.inputAlph.at(j) == transitionRuleVector.at(1) || transitionRuleVector.at(1) == "e"){
                    for (int k = 0; k < dpda.stackAlph.size(); ++k) {
                        if (dpda.stackAlph.at(k) == transitionRuleVector.at(2) || transitionRuleVector.at(2) == "e"){
                            for (int l = 0; l < dpda.states.size(); ++l) {
                                if (dpda.states.at(l).name == transitionRuleVector.at(3)){
                                    for (int m = 0; m < dpda.stackAlph.size(); ++m) {
                                        if (dpda.stackAlph.at(m) == transitionRuleVector.at(2) || transitionRuleVector.at(2) == "e"){
                                            dpda.states.at(i).transitionRules.push_back(transitionRule(transitionRuleVector.at(0),
                                                                                                       transitionRuleVector.at(1),
                                                                                                       transitionRuleVector.at(2),
                                                                                                       transitionRuleVector.at(3),
                                                                                                       transitionRuleVector.at(4)));
                                            error = 1;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    if (!error){
        std::ofstream ofs;
        ofs.open(output, std::fstream::app);
        ofs << "Error [1]:DPDA description is invalid!";
        ofs.close();
        return;
    }
    transitionRuleVector.clear();
}
//-----------------------DPDA----------------------------------------------------

//-----------------------Execution-----------------------------------------------
//prints the result as accept or reject
void printResult(int i, std::string output){
    if (i == 1 && inputstack.isEmpty()){
        std::ofstream ofs;
        ofs.open(output, std::fstream::app);
        ofs << "ACCEPT\n\n";
        ofs.close();
    }else{
        std::ofstream ofs;
        ofs.open(output, std::fstream::app);
        ofs << "REJECT\n\n";
        ofs.close();
    }
    dpda.stack.clear();
}
//prints the transition action
void printTransition(transitionRule t, std::string output){
    std::ofstream ofs;
    ofs.open(output, std::fstream::app);
    ofs << t.startState << "," << t.inputSymbol << "," << t.popSymbol << " => " << t.resultState << "," << t.pushSymbol << " [STACK]:";
    ofs.close();
    dpda.stack.print(output);
}
//splits input to a usable format
void splitInput(std::string input){
    std::string intermediate;
    std::vector<std::string> inputVector;

    // Tokenizing
    std::stringstream check1(input);

    while(getline(check1, intermediate, ','))
    {
        inputVector.push_back(intermediate);
    }

    for (int i = inputVector.size()-1; i > -1; --i) {
        inputstack.push(inputVector.at(i));
    }
    inputVector.clear();
}
//executes the transition action and send command for the next transition
void executeTransition(transitionRule t, std::string output){
    if (t.popSymbol != "e"){
        dpda.stack.pop();
    }
    if (t.pushSymbol != "e"){
        dpda.stack.push(t.pushSymbol);
    }
    printTransition(t, output);
    transition(t.resultState, output);
}
//checks if theres a valid transition, if theres it calls executeTransition function if theres not it stops the execution of that input
void transition(std::string s, const std::string& output){
    for (int i = 0; i < dpda.states.size(); ++i) {
        if (dpda.states.at(i).name == s){
            for (int j = 0; j < dpda.states.at(i).transitionRules.size(); ++j) {
                if ((dpda.states.at(i).transitionRules.at(j).inputSymbol == inputstack.peek() &&
                     dpda.states.at(i).transitionRules.at(j).popSymbol == dpda.stack.peek()) ||
                    (dpda.states.at(i).transitionRules.at(j).inputSymbol == inputstack.peek() &&
                     dpda.states.at(i).transitionRules.at(j).popSymbol == "e")){
                    inputstack.pop();
                    executeTransition(dpda.states.at(i).transitionRules.at(j),output);
                    return;
                }

            }
            for (int j = 0; j < dpda.states.at(i).transitionRules.size(); ++j) {
                if ((dpda.states.at(i).transitionRules.at(j).inputSymbol == "e" &&
                    dpda.states.at(i).transitionRules.at(j).popSymbol == "e") ||
                    (dpda.states.at(i).transitionRules.at(j).inputSymbol == "e" &&
                    dpda.states.at(i).transitionRules.at(j).popSymbol == dpda.stack.peek())){
                    executeTransition(dpda.states.at(i).transitionRules.at(j),output);
                    return;
                }
            }
            printResult(dpda.states.at(i).final, output);
            return;
        }
    }
}
//finds the start state and starts execution of the input
void executeInput(std::string input, std::string output){

    splitInput(input);
    for (int i = 0; i < dpda.states.size(); ++i) {
        if (dpda.states.at(i).start == 1){
            if (input.empty()){
                printResult(dpda.states.at(i).final, output);
                return;
            }
            transition(dpda.states.at(i).name, output);
        }
    }
}
//-----------------------Execution-----------------------------------------------
