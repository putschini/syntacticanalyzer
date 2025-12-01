#include "grammarworker.hpp"

GrammarWorker::GrammarWorker(){
    buildGrammar();
}

void GrammarWorker::buildFirst(){
    for( auto derivations : grammar_ ){
        first_.insert( std::pair< std::string, std::set < std::string > >( derivations.first, std::set < std::string >() )  );
        for ( auto rule : derivations.second ){
            if( rule[0] == epsilon_ )
                first_.at( derivations.first ).insert( epsilon_ );
        }
    }
    
    for( auto terminal : terminals_ ){
        first_.insert( std::pair< std::string, std::set < std::string > >( terminal, std::set < std::string >() )  );
        first_.at( terminal ).insert( terminal );
    }

    bool new_first_add = true;

    while ( new_first_add ){
        new_first_add = false;
        for( auto derivations : grammar_ ){
            bool first_epsilon = false;
            for ( auto rule : derivations.second ){
                if( rule[0] ==  epsilon_ ){
                    first_epsilon = true;
                    continue;
                }
                for( std::string first_from : first_.at( rule[0] ) ){
                    if( first_from == epsilon_ ){
                        first_epsilon = true;
                        continue;
                    }
                    if( !first_.at( derivations.first ).count( first_from ) ){
                        first_.at( derivations.first ).insert( first_from );
                        new_first_add = true;
                        // std::cout << first_from << " added by " << rule[0] << " in " << derivations.first <<  std::endl;
                    }
                }
                if ( first_epsilon ){
                    int k = 1;
                    while( k < rule.size() ){
                        if( !first_.at( rule[ k -1 ] ).count( epsilon_ ) || terminals_.count( rule[ k -1 ] ) )
                            break;
                        for( std::string first_from : first_.at( rule[k] ) ){
                            if( first_from == epsilon_ )
                                continue;
                            if( !first_.at( derivations.first ).count( first_from ) ){
                                first_.at( derivations.first ).insert( first_from );
                                new_first_add = true;
                                // std::cout << first_from << " added by " << rule[k -1] << " in " << derivations.first << std::endl;
                            }
                        }
                        k++;
                    }
                    if( k == rule.size() ){
                        if( !first_.at( derivations.first ).count( epsilon_ )  ){
                            first_.at( derivations.first ).insert( epsilon_ );
                            new_first_add = true;
                            // std::cout << " epsilon_ added by " << rule[0] << " in " << derivations.first << std::endl;
                        }
                    }
                }
            }
        }
    }
}

void GrammarWorker::buildFollow(){
    // Creat empty set's for every nonterminal
    for( auto derivations : grammar_ ){
        // std::cout << "empty set " << derivations.first << std::endl;
        follow_.insert( std::pair< std::string, std::set < std::string > >( derivations.first, std::set < std::string >() )  );
    }

    // Add end of stack at first symbol follow set
    follow_.at( first_symbol_ ).insert( final_symbol_ );

    bool new_follow_add = true;

    while ( new_follow_add ){
        new_follow_add = false;
        for( auto derivations : grammar_ ){ // loop por producões
            // std::cout << "Prodution : " << derivations.first << std::endl;
            for( auto derivation : derivations.second ){ // loop por regra


                for( int actual = 0; actual < derivation.size(); actual++ ){ // loop por simbolo
                    if ( terminals_.count( derivation[actual] ) || derivation[actual] == epsilon_ ) //  skip terminals they don't have follow
                        continue;
                    // std::cout << "Symbol : " << derivation[ actual ] << " SIZE " << follow_.at( derivation.at( actual ) ).size() << std::endl;
                    int next = actual + 1;
                    int epsilon_count = 0;
                    for( ; next < derivation.size(); next++ ){ // loop por proximo simbolo
                        if( derivation[actual] == derivation[next] )
                            continue;
                        // std::cout << "first_.at( derivation[next]  " << derivation[next] << std::endl;
                        int epsilon_count_before = epsilon_count;
                        for ( auto first : first_.at( derivation[next] ) ){
                            // std::cout << "First : " << first << std::endl;
                            if( first != epsilon_ ){
                                if ( !follow_.at( derivation.at( actual ) ).count( first ) ){
                                    // std::cout << "First : " << first << std::endl;
                                    follow_.at( derivation.at( actual ) ).insert( first );
                                    new_follow_add = true;
                                }
                            }else{
                                epsilon_count++;
                            }
                        }
                        if( terminals_.count( derivation[next] ) || epsilon_count_before == epsilon_count )
                            break;
                        // std::cout << " ----- " << std::endl;
                    }
                    // std::cout << "First OVER " << std::endl;
                    if( ( derivation.size() - next ) == epsilon_count ){
                         for ( auto follow : follow_.at( derivations.first ) ){
                            if( follow != epsilon_ ){
                            // std::cout << "First_EPSILON : " << first << std::endl;
                                if ( !follow_.at( derivation.at( actual ) ).count( follow ) ){
                                    follow_.at( derivation.at( actual ) ).insert( follow );
                                    new_follow_add = true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

std::map < std::string, std::map < std::string, std::vector< std::string > > > GrammarWorker::buildLL1Table(){
    
    buildFirst();

    buildFollow();

    std::map < std::string, std::map < std::string, std::vector< std::string > > > ll1_table;

    for ( auto prodution : grammar_ ){
        ll1_table.insert( std::pair< std::string, std::map < std::string, std::vector< std::string > > >( prodution.first, std::map < std::string, std::vector< std::string > >() )  );
    }

    first_.insert( std::pair< std::string, std::set < std::string > >( epsilon_, std::set < std::string >() )  );
    first_.at( epsilon_ ).insert( epsilon_ );

    for( auto prodution : grammar_ ){
        for( auto derivation : prodution.second ){
            int epsilon = 0;
            for( auto symbol : derivation ){

                bool epsilon_in_first = false;

                for( auto first : first_.at( symbol ) ){
                    if( first == epsilon_ ){
                        epsilon++;
                        epsilon_in_first = true;
                        continue;
                    }
                    // std::cout << prodution.first << std::endl;
                    ll1_table.at( prodution.first ).insert( std::pair< std::string, std::vector< std::string > >( first, derivation ) ); // Add all rule
                }

                if( terminals_.count( symbol ) || !epsilon_in_first )
                    break;
            }
            if( epsilon == derivation.size() ){
                for( auto follow : follow_.at( prodution.first ) ){
                    ll1_table.at( prodution.first ).insert( std::pair< std::string, std::vector< std::string > >( follow, derivation ) ); // Add all rule
                }                    
            }
        }
    }
    return ll1_table;
}

bool GrammarWorker::isTerminal( std::string symbol ){
    return terminals_.count( symbol );
}

std::string GrammarWorker::getFirstSymbol(){
    return first_symbol_;
}

std::vector< std::string > GrammarWorker::tokenize( std::string in ){
    std::vector< std::string > out;
    std::string::iterator pos = in.begin();
    std::string new_derivation = "";
    char separator = ',';
    char non_terminal_init = '<';
    char non_terminal_final = '>';

    bool derivation = false;
    while ( pos != in.end() ){
        char actual = *pos;
        pos++;
        // if( actual == ' ' )
            // continue;
        if(actual == separator || actual == non_terminal_final | actual == non_terminal_init || actual == ' ' ){
            if( derivation ){
                if ( actual == non_terminal_final )
                    new_derivation += actual;
                out.push_back( new_derivation );
                // std::cout << "new_derivation : " << new_derivation << std::endl;
                derivation = false;
                new_derivation = "";
            }else{
                if ( actual == non_terminal_final ){
                    new_derivation += actual;
                    out.push_back( new_derivation );
                    derivation = true;
                }
            }
            if( actual == non_terminal_init ){
                new_derivation += actual;
                derivation = true;
            }
            if( actual == separator ){
                std::string s = "";
                s += separator;
                out.push_back( s );
            }
        }else{
            new_derivation += actual;
            derivation = true;
//                std::cout << actual << std::endl;
        }
    }
    if( !new_derivation.empty() ){
        out.push_back(new_derivation);
        // std::cout << "new_derivation : " << new_derivation << std::endl;
    }

    return out;
}

void GrammarWorker::createNonTerminal( std::vector< std::string > non_terminal_and_derivations ){

    auto derivation = non_terminal_and_derivations.begin();
    std::string separator = ",";
    std::string non_terminal = non_terminal_and_derivations[0];

    if( first_symbol_.empty() )
        first_symbol_ = non_terminal;

    derivation++;

    grammar_.insert(  std::pair< std::string, std::set< std::vector< std::string > > >( non_terminal, std::set< std::vector< std::string > >() )) ;
    std::vector< std::string > next_derivation_vector;
    bool first = true;
    for ( ; derivation != non_terminal_and_derivations.end(); derivation++ ){
        // std::cout << "symbol " << *derivation << std::endl;
        if( *derivation == separator ){
            if( !next_derivation_vector.empty() ){
                grammar_.at( non_terminal ).insert( next_derivation_vector );
                next_derivation_vector = std::vector< std::string >();
            }
        }else{
            std::string next_derivation = *derivation;
            next_derivation_vector.push_back( next_derivation );
            if( ( next_derivation.find("<") == std::string::npos || next_derivation.find(">") == std::string::npos ) && next_derivation != epsilon_  )
                terminals_.insert( next_derivation );
        }
    }
    if( !next_derivation_vector.empty() ){
        grammar_.at( non_terminal ).insert( next_derivation_vector );
    }
    // std::cout << "NEW nonterminal  " << non_terminal << std::endl;
}

void GrammarWorker::buildGrammar(){

    std::ifstream inputFileGrammar;

    inputFileGrammar.open("test/syntactic_grammar.in");
    std::string line;
    while( std::getline(inputFileGrammar, line )){
        std::vector< std::string > tokens = tokenize( line );
        if( tokens.size() == 0 )
            continue;
        createNonTerminal( tokens );
    }
    inputFileGrammar.close();
}

void GrammarWorker::printGrammar(){
    for( auto non_terminal : grammar_ ){
        std::cout << non_terminal.first << ": ";
        for( auto derivation : non_terminal.second ){
            for( auto simbol : derivation ){
                std::cout << simbol << " ";
            }
            std::cout << " | ";
        }
        std::cout << std::endl;
    }
    std::cout << "First Symbol : " << first_symbol_ << std::endl;
    std::cout << "Terminals : " << std::endl;
    for ( auto terminal : terminals_){
        std::cout << terminal << std::endl;
    }
}

void GrammarWorker::printFirst(){
    for( auto f : first_ ){
        if ( terminals_.count( f.first ) )
            continue;

        std::cout << f.first << " : ";
        for( auto s : f.second ){
            std::cout << s << ", ";
        }
        std::cout << std::endl;
    }
}

void GrammarWorker::printFollow(){
    for( auto f : follow_ ){
        std::cout << f.first << " : ";
        for( auto s : f.second ){
            std::cout << s << ", ";
        }
        std::cout << std::endl;
    }
}
