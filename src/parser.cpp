#include "parser.h"

using namespace std;

pair<string_view, string_view> pchar1(char charToMatch, string_view input){
    if(input.empty()){
        return {"No more input"s, input};
    } else if(input[0] == charToMatch){
        input = input.substr(1);
        return {format("Found {}", charToMatch), input};
    } else {
        return {format("Expecting '{}'. Got '{}'", charToMatch, input[0]), input}; 
    }
}

ParseResult<char> pchar(char charToMatch, string_view input){
    if(input.empty()){
        return "No more input"s;
    } else if(input[0] == charToMatch){
        input = input.substr(1);
        Success res{charToMatch, input};
        return res;
    } else {
        return {format("Expecting '{}'. Got '{}'", charToMatch, input[0])};
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
