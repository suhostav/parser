#include "parser.h"

using namespace std;

pair<string_view, string_view> pchar1(char charToMatch, string_view input){
    if(input.empty()){
        return {statics::msg_no_input, input};
    } else if(input[0] == charToMatch){
        input = input.substr(1);
        return {format(statics::msg_found, charToMatch), input};
    } else {
        return {format(statics::msg_expecting, charToMatch, input[0]), input}; 
    }
}

ParseResult<char> pchar(char charToMatch, string_view input){
    if(input.empty()){
        return string{statics::msg_no_input};
    } else if(input[0] == charToMatch){
        input = input.substr(1);
        Success res{charToMatch, input};
        return res;
    } else {
        return {format(statics::msg_expecting, charToMatch, input[0])};
    }
}

Parser<char> pchar(char charToMatch){
    return satisfy<char>([charToMatch](char c){return c == charToMatch;});
}

Parser<char> anyOf(string listOfChars){
    vector<Parser<char>> parsers;
    for(auto c : listOfChars){
       parsers.emplace_back(pchar(c));
    }
    Parser<char> parser = choice<char>(parsers);
    return parser;
}

int plus2(int x, int y){
    return x + y;
}

// Parser<int> addP(Parser<int> x, Parser<int> y){
//     std::function<int(int,int)>f(plus2);
//     Parser<int> parser = lift2<int,int,int>(f, x, y);
//     return parser;
// }
