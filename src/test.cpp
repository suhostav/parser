#include <iostream>
#include <fstream>

#include "parser.h"

using namespace std;

string GetInput(){
    string line;
    ifstream f{"../input.txt", };
    if(!f){
        std::cerr << "Error in opening file input.txt\n"s;
    }
    getline(f, line);
    return line;
}

pair<bool, string_view> parseA(string_view input){
    static string null_string{""};
    if (input.empty()){
        return {false, null_string};
    } else if(input[0] == 'A'){
        input = input.substr(1);
        return {true, input};
    } else {
        return {false, input};
    }
}

void Test1(){
    string input{"ABC"};
    auto res1 = parseA(input);
    cout << res1.first << ", " << res1.second << endl;
}

void Test2(){
    {
        string input{"ABC"};
        auto res = pchar1('A', input);
        cout << res.first << ", " << res.second << endl;
    }
    {
        string input{"YSF"};
        auto res = pchar1('A', input);
        cout << res.first << ", " << res.second << endl;
    }
    {
        string input{""};
        auto res = pchar1('A', input);
        cout << res.first << ", " << res.second << endl;
    }
}

void Test3(){
    {
        string input{"ABC"};
        auto res = pchar('A', input);
        cout << GetInputOrMessage(res) << endl;
    }
    {
        string input{"XYZ"};
        auto res = pchar('A', input);
        cout << GetInputOrMessage(res) << endl;
    }
    {
        string input{""};
        auto res = pchar('A', input);
        cout << GetInputOrMessage(res) << endl;
    }
}

void Test4(){
    Parser<char> pcharA = pchar('A');
    {
        string input{"ABC"};
        auto res = pcharA(input);
        cout << GetInputOrMessage(res) << endl;
    }
    {
        string input{"XYZ"};
        auto res = pcharA(input);
        cout << GetInputOrMessage(res) << endl;
    }
    {
        string input{""};
        auto res = pcharA(input);
        cout << GetInputOrMessage(res) << endl;
    }
}

void TestComposed(){
    {
        string input{"ABC"};
        Parser<char> pcharA = pchar('A');
        Parser<char> pcharB = pchar('B');
        // auto parser = andThen(pcharA, pcharB);
        auto parser = pcharA >> pcharB;
        auto res = parser(input);
        if(IsSuccess(res)){
            auto [p, rest] = std::get<0>(res);
            cout << p.first << ", " << p.second << ", rest = " << rest << endl;
        }
    }
    {
        string input {"BXY"};
        Parser<char> pcharA = pchar('A');
        Parser<char> pcharB = pchar('B');
        auto parser = orElse(pcharA, pcharB);
        auto res = parser(input);
        if(IsSuccess(res)){
            auto [c, rest] = std::get<0>(res);
            cout << c << ", " << rest << endl;
        } else {
            cout << GetInputOrMessage(res) << endl;
        }
    }
    {
        string input {"ACZQBZ"};
        auto aAndThenBorC = pchar('A') >> (pchar('B') | pchar('C'));
        auto res = aAndThenBorC(input);
        if(IsSuccess(res)){
            auto [p, rest] = std::get<0>(res);
            cout << p.first << ", " << p.second << ", rest = " << rest << endl;
        } else {
            cout << GetInputOrMessage(res) << endl;
        }
    }
}



void TestAnyOf(){
    {
        string input {"DQZ"};
        Parser<char> parseA = pchar('A');
        Parser<char> parseB = pchar('B');
        Parser<char> parseC = pchar('C');
        vector<Parser<char>> parsers{move(parseA), move(parseB), move(parseC)};
        Parser<char> parser = choice(parsers);
        auto res = parser(input);
        if(IsSuccess(res)){
            auto [c, rest] = std::get<0>(res);
            cout << c << ", " << rest << endl;
        } else {
            cout << GetInputOrMessage(res) << endl;
        }
    }
    {
        string input{"CBC"};
        Parser<char> parser = anyOf("ABC"s);
        auto res = parser(input);
        if(IsSuccess(res)){
            auto [c, rest] = std::get<0>(res);
            cout << c << ", " << rest << endl;
        } else {
            cout << GetInputOrMessage(res) << endl;
        }
    }
}

void Test3Digits(){
    {
        string input = GetInput();
        Parser<char> pDigit = parseDigit<char>();
        // auto p3Digits =  andThen(andThen(pDigit, pDigit), pDigit);
        auto p3Digits = pDigit >> pDigit >> pDigit;
        std::function<string(pair<pair<char,char>,char>)> transformTuple = [](auto t){
            string res;
            res += t.first.first;
            res += t.first.second;
            res += t.second;
            return res;
        };
        // auto res = p3Digits(input);
        // if(IsSuccess(res)){
        //     auto r = std::get<0>(res);
        //     std::cout << transformTuple(r.first) << endl;
        // } else {
        //      cout << GetInputOrMessage(res) << endl;
        // }
        // Parser<string> parser = mapP(transformTuple, p3Digits);
        // Parser<string> parser = p3Digits ^ transformTuple;

        std::function<int(string)> toInt = [](string s){ return std::stoi(s); };

        // Parser<int> intParser = mapP(toInt, parser);
        // Parser<int> intParser = toInt ^ parser;
        Parser<int> intParser = pDigit >> pDigit >> pDigit ^ transformTuple ^ toInt;

        auto res = intParser(input);
        if(IsSuccess(res)){
            auto [num, rest] = std::get<0>(res);
            cout << num << ", " << rest << endl;
        } else {
            cout << GetInputOrMessage(res) << endl;
        }
    }    
}

void TestApply(){
    {
        string input = GetInput();
        Parser<char> pDigit = parseDigit<char>();
        auto p3Digits = pDigit >> pDigit >> pDigit;
        std::function<string(pair<pair<char,char>,char>)> transformTuple = [](auto t){
            string res;
            res += t.first.first;
            res += t.first.second;
            res += t.second;
            return res;
        };
        auto transformParser = returnP(transformTuple);
        auto stringParser = applyP(transformParser, p3Digits);
        auto res = stringParser(input);
        if(IsSuccess(res)){
            cout << std::get<0>(res).first << endl;
        } else {
            cout << GetInputOrMessage(res) << endl;
        }

    }
}