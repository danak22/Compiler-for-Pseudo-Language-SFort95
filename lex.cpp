

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include "lex.h"
#include <cctype>
#include <map>

using namespace std;

static map<string, Token> keywords = {
         {"PROGRAM", PROGRAM},
         {"program", PROGRAM},
        {"PRINT", PRINT},
        {"print", PRINT},
        {"IF", IF},
        {"if",IF},
        {"ELSE", ELSE},
        {"else", ELSE},
        {"END", END},
        {"end", END},
        {"INTEGER", INTEGER},
        {"integer", INTEGER},
        {"REAL", REAL},
        {"real", REAL},
        {"CHARACTER", CHARACTER},
        {"character", CHARACTER},
        {"THEN", THEN},
        {"then", THEN}
    
        
    };

LexItem id_or_kw(const string& lexeme, int linenum){
    
    if(keywords.find(lexeme) != keywords.end()){
        return LexItem(keywords[lexeme], lexeme, linenum);
    }
    else{
        return LexItem(IDENT, lexeme, linenum);
    }
}

static map<Token,string> tokenPrint = {
    {PROGRAM, "PROGRAM"},
    {END, "END"},
    {ELSE, "ELSE"},
    {IF, "IF"},
    {THEN, "THEN"},
    {INTEGER, "INTEGER"},
    {REAL, "REAL"},
    {CHARACTER, "CHARACTER"},
    {PRINT, "PRINT"},
    {LEN, "LEN"},
    {PLUS, "PLUS"},
    {MINUS, "MINUS"},
    {MULT, "MULT"},
    {DIV, "DIV"},
    {POW, "POW"},
    {ASSOP, "ASSOP"},
    {EQ, "EQ"},
    {LTHAN, "LTHAN"},
    {GTHAN, "GTHAN"},
    {CAT, "CAT"},
    {COMMA, "COMMA"},
    {LPAREN, "LPAREN"},
    {RPAREN, "RPAREN"},
    {DCOLON, "DCOLON"},
    {DOT, "DOT"},
    {DEF, "DEF"},
    {ICONST, "ICONST"},
    {RCONST, "RCONST"},
    {SCONST, "SCONST"},
    {IDENT, "IDENT"},
    {ERR, "ERR"},
    {DONE, "DONE"}
};


ostream& operator<<(ostream& out, const LexItem& tok) {
    Token tt = tok.GetToken();
    out << tokenPrint[tt];

    if (tok.GetToken() == IDENT) {
        out << ": \'" << tok.GetLexeme() << "\'";
    } else if (tok.GetToken() == ICONST || tok.GetToken() == RCONST) {
        out << ": (" << tok.GetLexeme() << ")";
    } else if (tok.GetToken() == SCONST) {
        out << ": \"" << tok.GetLexeme() << "\"";
    }

    return out;
}




 
LexItem getNextToken (istream& in, int& linenumber){
    enum TokState {START, INID, INSTRING, ININT, INCOMMENT, INREAL};
    LexItem lex;
    string lexeme;
    TokState state = START;
    char ch;
    bool isDoubleQuote = false;

    while(in.get(ch)){
        switch(state){
            case START:
                if(ch == '\n'){
                    linenumber++;
                }       
                if(isspace(ch)){
                    continue;
                }
                lexeme = ch;

                if(isalpha(ch)){
                    lexeme = ch;;
                    state = INID;
                }
                
                else if(isdigit(ch)){
                    lexeme = ch;
                    state = ININT;

                }

                else if (ch == '\"') {
                    state = INSTRING;
                    isDoubleQuote = true;
                } else if (ch == '\'') {
                    state = INSTRING;
                }
                else if(ch == '!'){
                    state = INCOMMENT;
                }
                
                else if(ch == '+'){
                    lexeme += ch;
                    lex = LexItem(PLUS, "PLUS", linenumber);
                    return lex;
                }
                else if(ch == '-'){
                    lexeme += ch;
                    lex = LexItem(MINUS, "MINUS", linenumber);
                    return lex;
                }
                else if(ch == '*'){
                    lexeme += ch;
                    in.get(ch);
                    if(ch == ','){
                        //lexeme += ch;
                        in.putback(ch);
                        lex = LexItem(DEF, "DEF", linenumber);
                        return lex;
                    }
                    if(ch == '*'){
                        lexeme += ch;
                        lex = LexItem(POW, "POW", linenumber);
                        return lex;
                    }
                    else{
                        in.putback(ch);
                        lex = LexItem(MULT, "MULT", linenumber);
                        return lex;
                    }
                }
                else if(ch == '/'){
                    lexeme += ch;
                    in.get(ch);
                    if (ch == '/') {
                        lexeme += ch;
                        lex = LexItem(CAT, "CAT", linenumber);
                        return lex;
                    } else {
                        in.putback(ch);
                        lex = LexItem(DIV, "DIV", linenumber);
                        return lex;
                    }
                }
                else if(ch =='>'){
                    lexeme +=ch;
                    return LexItem(GTHAN, "GTHAN", linenumber);
                }
                else if(ch =='<'){
                    lexeme +=ch;
                    return LexItem(LTHAN, "LTHAN", linenumber);
                }
                
                else if(ch == '='){
                    lexeme += ch;
                    in.get(ch);
                    if (ch == '=') {
                        lexeme += ch;
                        lex = LexItem(EQ, "EQ", linenumber);
                        return lex;
                    } else {
                        in.putback(ch);
                        lex = LexItem(ASSOP, "ASSOP", linenumber);
                        return lex;
                    }
                }
                else if(ch == '('){
                    lexeme += ch;
    
                    lex = LexItem(LPAREN, "LPAREN", linenumber);
                    return lex;
                }
                else if(ch == ')'){
                    lexeme += ch;
                    lex = LexItem(RPAREN, "RPAREN", linenumber);
                    return lex;
                }
                else if(ch == ','){
                    lexeme += ch;
                    lex = LexItem(COMMA, "COMMA", linenumber);
                    return lex;
                }
                else if(ch == ':'){
                    lexeme += ch;
                    in.get(ch);
                    if(ch == ':'){
                        lexeme += ch;
                        lex = LexItem(DCOLON, "DCOLON", linenumber);
                        return lex;
                    }
                    else{
                        in.putback(ch);
                        lex = LexItem(ERR, lexeme, linenumber);
                        return lex;
                    }
                }
                // ...

                else if (ch == '.') {
                    if (isdigit(in.peek())) {
                        state = INREAL;
                    } else {
                        return LexItem(DOT, "DOT", linenumber);
                    }
                }

                // ...

                else{
                    //lexeme += ch;
                    lex = LexItem(ERR, lexeme, linenumber);
                    return lex;
                }
                break;
            case INID:
                if(isalpha(ch) || isdigit(ch) || ch == '_'){
                    lexeme += ch;
                    continue;
                }
                else{
                    in.putback(ch);
                    lex = id_or_kw(lexeme, linenumber);
                    return lex;
                }
                break;
            case INSTRING:
                if (ch == '\n') {
                    //linenumber++;
                    return LexItem(ERR, lexeme, linenumber);
                }
                if ((isDoubleQuote && ch == '\"') || (!isDoubleQuote && ch == '\'')) {
                    return LexItem(SCONST, lexeme.substr(1, lexeme.size() - 1), linenumber);
                    lexeme = "";
                    state = START;
                } else {
                    lexeme += ch;
                    continue;
                }
                break;

            
            case ININT:
                if(isdigit(ch)){                   
                    lexeme += ch;
                }
                else if(ch == '.'){
                    state = INREAL;
                    lexeme += ch;
                }
                else{
                    in.putback(ch);
                    lex = LexItem(ICONST, lexeme, linenumber);
                    return lex;
                }
                break;
            // ...

            // ...

            // ...

            case INREAL:
                if (isdigit(ch)) {
                    lexeme += ch;
                } else if (ch == '.') {
                    lexeme += ch;
                    return LexItem(ERR, lexeme, linenumber);
                } else {
                    in.putback(ch);
                    return LexItem(RCONST, lexeme, linenumber);
                }

                break;

            


            case INCOMMENT:
                if(ch == '\n'){
                    linenumber++;
                    state = START;
                    continue;
                }
                break;

        }


    }


    
	return LexItem(DONE, "", linenumber);
    
}
