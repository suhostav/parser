#pragma once

#include <format>
#include <functional>
#include <string>
#include <sstream>
#include <utility>
#include <variant>
#include <vector>

using std::pair, std::string, std::string_view, std::format, std::vector;
using namespace std::literals;

template <typename T>
using Success = pair<T, string_view>;
using Failure = string;
template <typename T>
using ParseResult = std::variant<Success<T>, Failure>;
template <typename T>
using Parser = std::function<ParseResult<T>(string_view)>;

class statics{
public:
    constexpr static string_view msg_no_input{"No more input"};
    constexpr static string_view msg_found{"Found {}"};
    constexpr static string_view msg_expecting{"Expecting '{}'. Got '{}'"};
    constexpr static string_view msg_unexpected{"Unexpected  '{}'"};
};

template <typename T>
bool IsSuccess(const ParseResult<T>& r){
    return r.index() == 0 ? true : false;
}

template <typename T>
string_view GetInputOrMessage(const ParseResult<T>& r){
    return IsSuccess(r) ? get<Success<T>>(r).second : get<Failure>(r);
}

template<typename T>
Parser<T> satisfy(std::function<bool(T)> predicate){
    auto innerFn = [predicate](string_view input){
        if(input.empty()){
            return ParseResult<T>{string{statics::msg_no_input}};
        } else if(predicate(input[0])){
            char c = input[0];
            input = input.substr(1);
            Success res{c, input};
            return ParseResult<T>{res};
        } else {
            return ParseResult<T>{Failure{format(statics::msg_unexpected, input[0])}};
        }
    };
    return innerFn;
}

Parser<char> pchar(char charToMatch);

template <typename T>
Parser<T> parseDigit (){
    return satisfy<T>([](char c){return isdigit(c);});
}

pair<string_view, string_view> pchar1(char charToMatch, string_view input);
ParseResult<char> pchar(char charToMatch, string_view input);

template <typename T>
ParseResult<T> run(Parser<T> parser, string_view input){
    return parser(input);
}

template <typename T, typename K>
Parser<std::pair<T,K>> andThen(Parser<T> parser1, Parser<K> parser2){
    auto innerFn = [parser1, parser2](string_view input) {
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

template <typename T, typename K>
Parser<std::pair<T,K>> operator>>(Parser<T> parser1, Parser<K> parser2){
    return andThen(parser1, parser2);
}

template <typename T>
Parser<T> orElse(Parser<T> parser1, Parser<T> parser2){
    auto innerFn = [parser1, parser2](string_view input){
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
Parser<T> operator|(Parser<T> parser1, Parser<T> parser2){
    return orElse(parser1, parser2);
}

template <typename T>
Parser<T> choice(vector<Parser<T>> listOfParsers){
    auto innerFn = [listOfParsers](string_view input){
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
    auto innerFn = [f, parser](string_view input){
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

template <typename A, typename B>
Parser<B> operator^(Parser<A> parser, std::function<B(A)> f){
    return mapP(f, parser);
}


template <typename A>
Parser<A> returnP (A x){
    auto innerFn = [x](string_view input){
        // return ParseResult<A>(Success<A>{x,input});
        return Success<A>{x,input};
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
    auto parser2 = mapP(f, parser);
    return parser2;  
}

template <typename A, typename B>
Parser<B> operator*(Parser<std::function<B(A)>> fP, Parser<A> xP){
    return applyP(fP, xP);
}

// template<typename A, typename B, typename C>
// Parser<C> lift2(std::function<C(A,B)> f, Parser<A> xP, Parser<B> yP){
//     auto parser1 = returnP(f);
//     auto parser2 = applyP(parser1, xP);
//     auto parser3 = applyP(parser2, yP);
//     // auto parser3 = returnP( f * xP * yP);
//     return parser3;
// }

// Parser<int> addP(Parser<int> x, Parser<int> y);