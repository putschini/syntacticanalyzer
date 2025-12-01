#ifndef SIN_HPP
#define SIN_HPP

#include <vector>
#include <set>
#include <map>
#include <string>
#include <stack>
#include <iostream>
#include <fstream>

#include "grammarworker.hpp"
#include "lexicanalyzer.hpp"

class SyntacticAnalyzer {
    
    std::map < std::string, std::map < std::string, std::vector< std::string > > > ll1_table_;

    GrammarWorker gw;

    LexicAnalyzer lex;

public:

    SyntacticAnalyzer(  );

	void analyze();

	void printLL1Table();

};

#endif //SIN_HPP
