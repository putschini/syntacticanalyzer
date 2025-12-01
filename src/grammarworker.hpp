#ifndef GRAMMAR_WORKER_HPP
#define GRAMMAR_WORKER_HPP

#include <vector>
#include <set>
#include <map>
#include <string>
#include <iostream>
#include <fstream>

class GrammarWorker {

    std::string first_symbol_;
    std::map < std::string, std::set< std::vector< std::string > > > grammar_;
    std::set < std::string > terminals_;

    std::map < std::string, std::set< std::string > > first_;
    std::map < std::string, std::set< std::string > > follow_;

    std::vector< std::string > tokenize( std::string in );

    void createNonTerminal( std::vector< std::string > non_terminal_and_derivations );

public:

    std::string epsilon_ = "@";

    std::string final_symbol_ = "$";

    GrammarWorker(  );

    void buildFirst();

    void buildFollow();

    std::map < std::string, std::map < std::string, std::vector< std::string > > > buildLL1Table();

    bool isTerminal( std::string symbol );

    std::string getFirstSymbol();

    void buildGrammar();

    void printGrammar();

    void printFirst();

    void printFollow();

};

#endif //GRAMMAR_WORKER_HPP
