#pragma once

#include <format>
#include <functional>
#include <string>
#include <sstream>
#include <utility>
#include <variant>
#include <vector>

template <typename T>
using Success = std::pair<T, std::string&>;
using Failure = std::string;
template <typename T>
using ParseResult = std::variant<Success<T>, Failure>;
template <typename T>
using Parser = std::function<ParseResult<T>(std::string&)>;
using std::pair, std::string, std::format, std::vector;
using namespace std::literals;

template <typename T>
bool IsSuccess(const ParseResult<T>& r){
    return r.index() == 0 ? true : false;
}

template <typename T>
string GetInputOrMessage(const ParseResult<T>& r){
    return IsSuccess(r) ? get<Success<T>>(r).second : get<Failure>(r);
}

template<typename T>
Parser<T> satisfy(std::function<bool(T)> predicate){
    auto innerFn = [predicate](string& input){
        if(input.empty()){
            return ParseResult<T>{"No more input"s};
        } else if(predicate(input[0])){
            char c = input[0];
            input.erase(0, 1);
            Success res{c, input};
            return ParseResult<T>{res};
        } else {
            return ParseResult<T>{format("Unexpected  '{}'", input[0])};
        }
    };
    return innerFn;
}

Parser<char> pchar(char charToMatch);

template <typename T>
Parser<T> parseDigit (){
    return satisfy<T>([](char c){return isdigit(c);});
}

std::pair<std::string, std::string&> pchar1(char charToMatch, std::string& input);
ParseResult<char> pchar(char charToMatch, std::string& input);

template <typename T>
ParseResult<T> run(Parser<T> parser, string& input){
    return parser(input);
}

template <typename T, typename K>
Parser<std::pair<T,K>> andThen(Parser<T> parser1, Parser<K> parser2){
    auto innerFn = [parser1, parser2](string& input) {
        auto res1 = parser1(input);
        if(!IsSuccess(res1)){
            return ParseResult<std::pair<T,K>>(std::get<Failure>(res1));
        }
        Success<T> s1 = std::get<Success<T>>(res1);
        auto res2 = parser2(s1.second);
        if(!IsSuccess(res2)){
            return ParseResult<std::pair<T,K>>(std::get<Failure>(res2));
        }
        Success<K> s2 = std::get<Success<K>>(res2);
        std::pair<T,K> res{s1.first, s2.first};
        Success s{res, s2.second};
        return ParseResult<std::pair<T,K>>(s);
    };
    return innerFn;
}

template <typename T>
Parser<T> orElse(Parser<T> parser1, Parser<T> parser2){
    auto innerFn = [parser1, parser2](string& input){
        auto res1 = parser1(input);
        if(IsSuccess(res1)){
            return ParseResult<T>(res1);
        }
        auto res2 = parser2(input);
        return ParseResult<T>(res2);
    };
    return innerFn;
}

template <typename T>
Parser<T> choice(vector<Parser<T>> listOfParsers){
    auto innerFn = [listOfParsers](string& input){
        Parser<T> parser = nullptr;
        bool first = true;
        for(auto p : listOfParsers){
            if(first){
                first = false;
                parser = p;
            } else {
                parser = orElse(parser, p);
            }
        }
        auto res = parser(input);
        return ParseResult<T>(res);
    };
    return innerFn;
}

Parser<char> anyOf(string listOfChars);

template <typename A, typename B>
Parser<B> mapP (std::function<B(A)> f, Parser<A> parser){
    auto innerFn = [f, parser](string& input){
        auto res = parser(input);
        if(IsSuccess(res)){
            auto [val, rem] = std::get<Success<A>>(res);
            B newVal = f(val);
            return ParseResult<B>(Success<B>{newVal,rem});
        }
        Failure err = std::get<Failure>(res);
        return ParseResult<B>(err);
    };
    return innerFn;
}

template <typename A>
Parser<A> returnP (A x){
    auto innerFn = [x](string& input){
        return ParseResult<A>(Success<A>{x,input});
    };
    return innerFn;
}

template <typename A, typename B>
Parser<B> applyP (Parser<std::function<B(A)>> fP, Parser<A> xP){
    auto parser = andThen(fP, xP);
    std::function<B(pair<std::function<B(A)>, A>)> f = 
        [](pair<std::function<B(A)>, A> fx){
            return fx.first(fx.second);
        };
    auto parser2 = mapX(f, parser);
    return parser2;  
}

template<typename A, typename B, typename C>
Parser<C> lift2(std::function<C(A,B)> f, Parser<A> xP, Parser<B> yP){
    auto parser1 = returnP(f);
    auto parser2 = applyP(parser1, xP);
    auto parser3 = applyP(parser2, yP);
    return parser3;
}