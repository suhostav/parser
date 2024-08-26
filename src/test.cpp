#include <iostream>
#include <fstream>

#include "parser.h"

using namespace std;

string GetInput(){
    string line;
    ifstream f{"input.txt", };
    getline(f, line);
    return line;
}

pair<bool, string&> parseA(string& input){
    static string null_string{""};
    if (input.empty()){
        return {false, null_string};
    } else if(input[0] == 'A'){
        input.erase(0, 1);
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
        auto parser = andThen(pcharA, pcharB);
        auto res = parser(input);
        if(IsSuccess(res)){
            auto [p, rest] = std::get<0>(res);
            cout << p.first << ", " << p.second << ", rest = " << rest << endl;
        }
    }
    {
        string input {"ABC"};
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
        string input {"AQZ"};
        Parser<char> parseA = pchar('A');
        Parser<char> parseB = pchar('B');
        Parser<char> parseC = pchar('C');
        auto bOrElseC = orElse(parseB, parseC);
        auto aAndThenBorC = andThen(parseA, bOrElseC);
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
        string input {"AQZ"};
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
        string input{"ABC"};
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
        auto p3Digits =  andThen(andThen(pDigit, pDigit), pDigit);
        std::function<string(pair<pair<char,char>,char>)> transformTuple = [](auto t){
            string res;
            res += t.first.first;
            res += t.first.second;
            res += t.second;
            return res;
        };

        Parser<string> parser = mapP(transformTuple, p3Digits);

        std::function<int(string)> toInt = [](string s){ return std::stoi(s); };

        Parser<int> intParser = mapP(toInt, parser);

        auto res = intParser(input);
        if(IsSuccess(res)){
            auto [c, rest] = std::get<0>(res);
            int r = c + 1000;
            cout << r << ", " << rest << endl;
        } else {
            cout << GetInputOrMessage(res) << endl;
        }
    }    
}
