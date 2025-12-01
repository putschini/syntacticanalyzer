#include "syntacticanalyzer.hpp"

SyntacticAnalyzer::SyntacticAnalyzer(){
    gw = GrammarWorker();
    lex = LexicAnalyzer();
    // lex.getAutomaton().printState();
    ll1_table_ = gw.buildLL1Table();
}

void SyntacticAnalyzer::printLL1Table(){
    for( auto line : ll1_table_ ){
        std::cout << line.first  << " | ";
        for( auto collum : line.second ){
            std::cout << collum.first << " -> ";
            for ( auto symbol : collum.second ){
                std::cout << symbol;
            }
            std::cout << " | ";
        }
        std::cout << std::endl;
    }
}

void SyntacticAnalyzer::analyze(){
    std::stack < std::string > stack = std::stack< std::string >();
    stack.push( gw.final_symbol_ );
    stack.push( gw.getFirstSymbol() );

    std::string input_file = "test/program.txt";
    std::ifstream input_fstream;
    input_fstream.open( input_file );
    std::string line;
    bool get_next_token = true;
    Token result;
    while( std::getline(input_fstream, line ) && !stack.empty() ){
        lex.column_count_ = 1;
        // Clean spaces from line
        lex.removeSpacesFromBegin( line );
        while ( !line.empty() && !stack.empty() || !get_next_token ){
            if( get_next_token ){
                result = lex.getNextToken( line );
                get_next_token = false;
            }

            if( result.token.find( "error" ) != std::string::npos ){
                std::cout << "Error at Line: " << lex.line_count_<< " Token" << result.token<< " OBS: " << result.obs << ";" << std::endl;
                get_next_token = true;
            }else{

                std::string token = result.token;
                std::string stack_symbol = stack.top();

                std::cout << "Token : " << token << " Stack " << stack_symbol << std::endl;

                if( gw.isTerminal( stack_symbol ) ){
                    get_next_token = true;
                    if( token == stack_symbol ){
                        std::cout << "Removed " << stack_symbol << std::endl;
                        stack.pop();
                        // token_stack.pop();
                    }else{
                        std::cout << "Error1 at Line:" << lex.line_count_ << " Column:" << lex.column_count_ << " ; Expecting " << stack_symbol << " but get " << token << std::endl;
                    }
                }else{
                    if( ll1_table_.at( stack_symbol ).count( token ) ){
                        std::vector< std::string > next_stack_symbols = ll1_table_.at( stack_symbol ).at( token );
                        stack.pop();
                        if( next_stack_symbols[0] == gw.epsilon_ ){
                            std::cout << "Removed " << stack_symbol << std::endl;
                            continue;
                        }
                        std::cout << "Added to stack  "  ;
                        for( std::vector< std::string >::reverse_iterator next_stack_symbols_it = next_stack_symbols.rbegin(); next_stack_symbols_it != next_stack_symbols.rend(); next_stack_symbols_it++ ){
                            stack.push( *next_stack_symbols_it );
                            std::cout << " " << *next_stack_symbols_it;
                        }
                        std::cout << std::endl;
                    }else{
                        // List Follow of stack_symbol
                        std::cout << "Error2 at Line: " << lex.line_count_ << "Column:" << lex.column_count_ << "; Expecting ";
                        for ( auto symbol : ll1_table_.at( stack_symbol ) ){
                            std::cout << symbol.first << " ";
                        }
                        std::cout << " but get " << token << std::endl;
                        get_next_token = true;
                    }
                }
            }
            // Clean spaces from line
            lex.removeSpacesFromBegin( line );
        }
        lex.line_count_++;
    }
    input_fstream.close();

    if( stack.top() == gw.final_symbol_ ){
        std::cout << " Program is OK " << std::endl;
    }else{
        std::cout << " Failed " << std::endl;
    }
}
