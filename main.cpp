#include <iostream>

#include "syntacticanalyzer.hpp"
#include "grammarworker.hpp"


int main( int argc, char* argv[] ){
	
    GrammarWorker gw = GrammarWorker();

	gw.buildGrammar();
    
    gw.buildFirst();
    std::cout << "FIRST :: " << std::endl;
    gw.printFirst();
    std::cout << "===========================" << std::endl << std::endl;
    
    gw.buildFollow();
    std::cout << "FOLLOW :: " << std::endl;
    gw.printFollow();
    std::cout << "===========================" << std::endl << std::endl;

    SyntacticAnalyzer sin = SyntacticAnalyzer();

    std::cout << "TABLE :: " << std::endl;
    sin.printLL1Table();
    std::cout << "===========================" << std::endl << std::endl;

    sin.analyze();

	return 0;
}