
#include<iostream>
#include<map>
#include<cctype>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <cmath>
#include "lex.h"
#include <vector>
#include <algorithm>
#include <set>
#include <unordered_map>
using namespace std;


int main(int argc, char *argv[])
{

    // flags
    bool printEveryToken = false;
    bool printint = false;
    bool printreal = false;
    bool printstr = false;
    bool printid = false;
    bool printkw = false;
    bool fileGiven = false;
    string filename;
    int i = 1;


    /* GET THE FLAGS */
    while(i<argc){

        if (strcmp(argv[i], "-all") == 0){
            printEveryToken = true;
        }
        else if (strcmp(argv[i], "-int") == 0){
            printint = true;
        }
        else if (strcmp(argv[i], "-real") == 0){
            printreal = true;
        }
        else if (strcmp(argv[i], "-str") == 0){
            printstr = true;
        }
        else if (strcmp(argv[i], "-id") == 0){
            printid = true;
        }
        else if (strcmp(argv[i], "-kw") == 0){
            printkw = true;
        }
        else if (argv[i][0] == '-'){
            cout<<"UNRECOGNIZED FLAG {"<<argv[i]<<"}"<<endl;
            return 0;
        }
        else if(fileGiven){
            cout<<"ONLY ONE FILE NAME IS ALLOWED."<<endl;
            return 0;
        }
        else{
            fileGiven = true;
            filename = argv[i];
        }
        i++;
    }
    /* OPEN THE FILE */
    if(!fileGiven){ // no file inputted
        cout<<"NO SPECIFIED INPUT FILE."<<endl;
		return 0;
    }
    ifstream fin(filename); // open it fr this time
    if(fin.fail()){ // file didnt open
		cout<<"CANNOT OPEN THE FILE " <<filename<<endl;
		return 0;
	}

    LexItem lex;
    int linenumber = 1;
    int totalTokens = -1;
    int totalIdentifiers = 0;
    int totalIntegers = 0;
    int totalReals = 0;
    int totalStrings = 0;
    map<string, int> identifiers;
    map<string, int> keywords;
    map<string, int> integers;
    map<string, int> reals;
    map<string, int> strings;


    /*    LEXITEM LOOP     */
    while(true){
        lex = getNextToken(fin, linenumber);
        totalTokens++;

        if(lex.GetToken() == DONE){
            break;
        }

        else if (lex.GetToken() == IDENT) {
            // check if ident has already been accounted for
            auto it = identifiers.find(lex.GetLexeme());
            if (it == identifiers.end()) {
                // put it in if not
                identifiers[lex.GetLexeme()] = 1;
                totalIdentifiers++;  // Increment only for a new identifier
            } else {
                // increment the count for each occurrence
                identifiers[lex.GetLexeme()]++;
            }
        }




        else if (lex.GetToken() == ICONST) {
            // Check if the number has already been encountered
            auto it = integers.find(lex.GetLexeme());

            if (it == integers.end()) {
                integers[lex.GetLexeme()] = 1;  // Put it in if not
                totalIntegers++;
            }
        }

        else if(lex.GetToken()==RCONST){
               // Check if the number has already been encountered
            auto it = reals.find(lex.GetLexeme());

            if (it == reals.end()) {
                reals[lex.GetLexeme()] = 1;  // Put it in if not
                totalReals++;
            }
        }
        else if(lex.GetToken()==SCONST){
            totalStrings++;
            if(strings.find(lex.GetLexeme()) == strings.end()){
                strings[lex.GetLexeme()] = 1;
            }
            else{
                strings[lex.GetLexeme()]++;
            }
        }
        else if(lex.GetToken() == IF || lex.GetToken() == ELSE || lex.GetToken() == PRINT || lex.GetToken() == INTEGER || lex.GetToken() == REAL || lex.GetToken() == CHARACTER || lex.GetToken() == END || lex.GetToken() == PROGRAM || lex.GetToken() == THEN || lex.GetToken() == LEN){
            if(keywords.find(lex.GetLexeme()) == keywords.end()){
                keywords[lex.GetLexeme()] = 1;
            }
            else{
                keywords[lex.GetLexeme()]++;
            }
        }

        if(lex.GetToken() == ERR){
            cout<<"Error in line "<<lex.GetLinenum()<<": Unrecognized Lexeme {" << lex.GetLexeme()<<"}"<<endl;
            return 0;
        }

        if(printEveryToken){
            cout<<lex<<endl;
        }
    }

    if(totalTokens == 0){ // empty file
        cout<<"Empty File."<<endl;
        return 0;
    }
    cout << "\nLines: " << linenumber-1 << endl;
    cout << "Total Tokens: " << totalTokens << endl;
    cout << "Identifiers: " << totalIdentifiers << endl;
    cout << "Integers: " << totalIntegers << endl;
    cout << "Reals: " << totalReals << endl;
    cout << "Strings: " << totalStrings << endl;

    
    if(printid){
        cout<<"IDENTIFIERS:" <<endl;
        for(auto it = identifiers.begin(); it != identifiers.end(); it++){
            cout<<it->first<<" ("<<it->second<<")";
            if(next(it) != identifiers.end()){
                cout<<", ";
            }
        }
        cout<<endl;
    }

    if(printkw){
        cout<<"KEYWORDS:" <<endl;
        for(auto it = keywords.begin(); it != keywords.end(); it++){
            cout<<it->first<<" ("<<it->second<<")";
            if(next(it) != keywords.end()){
                cout<<", ";
            }
        }
        cout<<endl;
    }
    if (printint) {
        cout << "INTEGERS:\n";
        vector<string> integerKeys;
        for (const auto& entry : integers) {
            integerKeys.push_back(entry.first);
        }

        // Custom comparison function to sort by numeric values
        sort(integerKeys.begin(), integerKeys.end(), [](const string& a, const string& b) {
            return stoi(a) < stoi(b);
        });

        for (const auto& key : integerKeys) {
            cout << key;
            if (next(find(integerKeys.begin(), integerKeys.end(), key)) != integerKeys.end()) {
                cout << ", ";
            }
        }
        cout << '\n';
    }



    if (printreal) {
    cout << "REALS:" << endl;
    for (auto it = reals.begin(); it != reals.end(); it++) {
        if (it->first[0] == '.' && it->first.size() > 1) {
            cout << "0";
        }
        cout << it->first;
        if (next(it) != reals.end()) {
            cout << ", ";
        }
    }
    cout << endl;
}


    if (printstr) {
    cout << "STRINGS:" << endl;
    bool firstString = true;  // Flag to check if it's the first string

    for (const auto& pair : strings) {
        if (!firstString) {
            cout << ", ";
        } else {
            firstString = false;
        }

        cout << "\"" << pair.first << "\"";
    }

    cout << endl;
}



    return 0;       
}
