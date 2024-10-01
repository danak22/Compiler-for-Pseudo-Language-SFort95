#include "parser.h"
#include <vector>
#include <stack>



map<string, bool> defVar;
map<string, Token> SymTable;
bool startedRelExpr = false;

using namespace std;

namespace Parser {
	bool pushed_back = false;
	LexItem	pushed_token;

	static LexItem GetNextToken(istream& in, int& line) {
		if( pushed_back ) {
			pushed_back = false;
			return pushed_token;
		}
		return getNextToken(in, line);
	}

	static void PushBackToken(LexItem & t) {
		if( pushed_back ) {
			abort();
		}
		pushed_back = true;
		pushed_token = t;	
	}

}

static int error_count = 0;

int ErrCount()
{
    return error_count;
}

void ParseError(int line, string msg)
{
	++error_count;
	cout << line << ": " << msg << endl;
}

bool IdentList(istream& in, int& line);

//PrintStmt:= PRINT *, ExpreList 
bool PrintStmt(istream& in, int& line) {
	LexItem t;
	
	t = Parser::GetNextToken(in, line);
	
 	if( t != DEF ) {
		
		ParseError(line, "Print statement syntax error.");
		return false;
	}
	t = Parser::GetNextToken(in, line);
	
	if( t != COMMA ) {
		
		ParseError(line, "Missing Comma.");
		return false;
	}
	bool ex = ExprList(in, line);
	
	if( !ex ) {
		ParseError(line, "Missing expression after Print Statement");
		return false;
	}
	return ex;
}//End of PrintStmt


//ExprList:= Expr {,Expr}
bool ExprList(istream& in, int& line) {
	bool status = false;
	
	status = Expr(in, line);
	if(!status){
		ParseError(line, "Missing Expression");
		return false;
	}
	
	LexItem tok = Parser::GetNextToken(in, line);
	
	if (tok == COMMA) {
		status = ExprList(in, line);
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else{
		Parser::PushBackToken(tok);
		return true;
	}
	return status;
}//End of ExprList



//RelExpr:= Expr [ ( == | < | > ) Expr ] should be good? 
bool RelExpr(istream& in, int& line) {
    bool status = false;
    status = Expr(in,line);
    if(!status){
        ParseError(line, "Missing Expr");
        return false;
    }

    LexItem lex = Parser::GetNextToken(in, line);

    if (lex == GTHAN || lex == LTHAN || lex==EQ) {
        if(!startedRelExpr){
            startedRelExpr = true;
            status = RelExpr(in, line);
            startedRelExpr = false; // Reset the flag after completing the parsing
        }
        else{
            ParseError(line,"error is user doing two comparisons at once");
            return false;
        }
    }
    else if(lex.GetToken() == ERR){
        ParseError(line, "Unrecognized Expr Pattern");
        return false;
    }
    else{
        Parser::PushBackToken(lex);
        return true;
    }
    return status;
}

//End of RelExpr

//Expr:= MultExpr { ( + | - | // ) MultExpr }  good to go???
bool Expr(istream& in, int& line) {
    bool status = false;
    status = MultExpr(in, line);
    
    if(!status){
        ParseError(line, "Missing MultExpr");
        return false;
    }
    
    LexItem tok = Parser::GetNextToken(in, line);
    
    if (tok == PLUS || tok == MINUS || tok == CAT) {
        status = Expr(in, line);
        if(!status){
            ParseError(line, "Missing MultExpr");
            return false;
        }
    }
    else if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized MultExpr Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    else{
        Parser::PushBackToken(tok);
        return true;
    }
    return status;
}//End of Expr

//MultExpr:= TermExpr { ( * | / ) TermExpr } good to go??
bool MultExpr(istream& in, int& line) {
    bool status = TermExpr(in, line);
    
    if(!status){
        ParseError(line, "Missing TermExpr");
        return false;
    }
    
    LexItem tok = Parser::GetNextToken(in, line);
    
    if (tok == MULT || tok == DIV) {
        status = MultExpr(in, line);
    }
    else if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized TermExpr Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    else{
        Parser::PushBackToken(tok);
        return true;
    }
    return status;
}//End of MultExpr


//Factor:= IDENT | ICONST | RCONST | SCONST | (Expr) good to go??
bool Factor(istream& in, int& line, int sign){
    bool status = false;

	LexItem lex = Parser::GetNextToken(in,line);


	if(lex.GetToken()==ICONST || lex.GetToken()==RCONST || lex.GetToken()==SCONST){ 
		
        
        return true;

	}

    if (lex.GetToken()==IDENT){
        string varName = lex.GetLexeme();
        // Check if the variable has been declared by checking the defVar map and its boolean value to be true
        if (defVar.find(varName) == defVar.end()) {
            ParseError(line, "Variable '" + varName + "' never declared ");
            return false;
        }
        return true;
    }
    
    

    if (lex == END){
        Parser::PushBackToken(lex);
        return true;
    }
    

	if(lex!=LPAREN){
		ParseError(line,"who knows: " + lex.GetLexeme());
		return false;
	}

	status = Expr(in,line);
	lex = Parser::GetNextToken(in,line);
    if (!status) {
        ParseError(line, "Missing Expression in Factor");
        return false;
    }
    
	if(lex!=RPAREN){
		ParseError(line,"Missing RPAREN ");
		return false;
	}
	if(!status){
		ParseError(line,"Something wrong maybe");
	}
	return status;
}//End of Factor


//TermExpr:= SFactor { ** SFactor } good to go??
bool TermExpr(istream& in, int& line) {
    bool status = SFactor(in, line);
    
    if(!status){
        ParseError(line, "Missing Sfactor Expression");
        return false;
    }
    
    LexItem tok = Parser::GetNextToken(in, line);
    
    if (tok == POW) {
        status = TermExpr(in, line);
    }
    else if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    else{
        Parser::PushBackToken(tok);
        return true;
    }
    return status;
}//End of TermExpr

//SFactor:= [+ | -] Factor good to go??
bool SFactor(istream& in, int& line) {
    LexItem tok = Parser::GetNextToken(in, line);
    if (tok == PLUS || tok == MINUS) {
        tok = Parser::GetNextToken(in, line);
    }
    else{
        Parser::PushBackToken(tok);
    }
    bool status = Factor(in, line, 0);
    if(!status){
        ParseError(line, "Missing Factor");
        return false;
    }
    return true;
}//End of SFactor



//Var:= IDENT good to go??
bool Var(istream& in, int& line) {
    LexItem tok = Parser::GetNextToken(in,line);

    if (tok.GetToken() == ERR) {
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }

    if (tok.GetToken() == IDENT) {
        Parser::PushBackToken(tok);
        return true;
    }

    ParseError(line, "This isn't a Var");
    return false;
}//End of Var




//AssignStmt:= Var = Expr good to go??
bool AssignStmt(istream& in, int& line) {
    bool status = Var(in,line);
    if(!status){
        ParseError(line,"Missing Variable (in AssignStmt)");
        return false;
    }

    LexItem tok = Parser::GetNextToken(in, line);
    string varName = tok.GetLexeme();
        // Check if the variable has been declared by checking the defVar map and its boolean value to be false
        if (defVar.find(varName) == defVar.end()) {
            ParseError(line, "Variable '" + varName + "' does not exist in the declaration list.");
            return false;
            
        }


    using namespace std; // Add this line to specify the namespace for "cout"

    LexItem lex = Parser::GetNextToken(in,line);
    if(lex.GetToken() == ERR){
        ParseError(line, "Unrecognized Input Pattern");
        std::cout << "(" << lex.GetLexeme() << ")" << std::endl; // Specify the namespace for "cout"
        return false;
    }
    if(lex != ASSOP){
        ParseError(line, "Missing ASSOP");
        return false;
    }


    status = Expr(in,line);
        if(!status){
            ParseError(line,"Missing expression ");
            return false;
        }

        return true;
    }//End of AssignStmt

//SimpleStmt:= AssignStmt | PrintStmt good to go??
bool SimpleStmt(istream& in, int& line) {
    LexItem lookahead = Parser::GetNextToken(in, line);

    // Check if the lookahead token indicates an assignment statement
    if (lookahead == IDENT) {
        Parser::PushBackToken(lookahead);
        bool status = AssignStmt(in, line);
        if (!status) {
            ParseError(line, "Assignment didn't work in SimpleStmt");
            return false;
            }
    } 
    // Check if the lookahead token indicates a print statement
    else if (lookahead == PRINT) {
        //Parser::PushBackToken(lookahead);
        //lookahead = Parser::GetNextToken(in, line);
        bool status = PrintStmt(in, line);
        if (!status) {
            ParseError(line, "Print didn't work in SimpleStmt");
            return false;
        }
    }
    else {
        // If neither assignment nor print statement failed
        ParseError(line, "Expected an assignment or print statement");
        return false;
    }

    // If none of the above conditions were met, raise a parse error
    //ParseError(line, "Unexpected token");
    return true;
}




//End of SimpleStmt

//SimpleIfStmt:= IF (RelExpr) SimpleStmt good to go??
bool SimpleIfStmt(istream& in, int& line) {
    // LexItem tok = Parser::GetNextToken(in, line);
    
    // if (tok != IF) {
    //     ParseError(line, "Missing IF");
    //     return false;
    // }

    bool status = false;
    status = SimpleStmt(in, line);
    if(!status){
        ParseError(line, "Missing Simple Statement");
        return false;
    }

    cout << "Print statement in a Simple If statement." << endl;

    return true;
}//End of SimpleIfStmt

static int nestingLevel = 0;


//BlockIfStmt:= IF (RelExpr) THEN {Stmt} [ELSE {Stmt}] END IF      good to go?
bool BlockIfStmt(istream& in, int& line) {


     // Static variable to track nesting level, initialized to 1
    //static stack<int> nestingStack; // Stack to keep track of nesting levels

    // Check for "THEN" token
    LexItem tok = Parser::GetNextToken(in, line);
    if (tok != THEN) {
        ParseError(line, "Missing THEN");
        return false;
    }

    nestingLevel++; // Increment nesting level

    bool parsingIfBlock = true; // Flag to track whether parsing the "IF" block or "ELSE" block

    // Parse statements inside the "IF" block or "ELSE" block
    while (true) {
        // Try to parse a statement
        bool status = Stmt(in, line);
        if (!status) {
            ParseError(line, parsingIfBlock ? "Missing Statement in IF block" : "Missing Statement in ELSE block");
            return false;
        }

        // Check for "ELSE" token
        tok = Parser::GetNextToken(in, line);
        if (tok == ELSE) {
            if (parsingIfBlock) {
                parsingIfBlock = false; // Now parsing the "ELSE" block
            } else {
                ParseError(line, "Multiple ELSE statements in IF block");
                return false;
            }
        } else if (tok == END) {
            // Check for "IF" token after "END"
            tok = Parser::GetNextToken(in, line);
            
            if (tok == IF) {
                //if (!nestingStack.empty()) {
                    //nestingStack.pop(); // Remove top nesting level from stack
                  //  nestingLevel = nestingStack.empty() ? 1 : nestingStack.top() + 1; // Update nesting level
                //}
                cout << "End of Block If statement at nesting level " << nestingLevel << endl;
                nestingLevel--; // Decrement nesting level
                return true;
            } else {
                ParseError(line, "Missing IF after END");
                return false;
            }
            
        } else if (tok == IF) { // Nested IF
            //nestingStack.push(nestingLevel); // Push current nesting level to stack
            //nestingLevel++; // Increment nesting level
        } else if (tok.GetToken() == ERR) {
            ParseError(line, "Unrecognized Input Pattern: " + tok.GetLexeme());
            return false;
        } else {
            // Push back the token if it's not "ELSE" or "END"
            Parser::PushBackToken(tok);
        }
    }
}





//End of Block


//Stmt:= AssignStmt | PrintStmt | BlockIfStmt | SimpleIfStmt NEEDS A LOT OF WORK
bool Stmt(istream& in, int& line) {
    bool status = false;
    LexItem lex = Parser::GetNextToken(in, line);
    
    if (lex == IDENT) {
        Parser::PushBackToken(lex);
        status = AssignStmt(in, line); 
        if(!status){
            ParseError(line, "Assignment in Stmt didn't work out");
            return false;
        }

    }
    else if (lex == PRINT) {
        //Parser::PushBackToken(lex);
        //LexItem tok = Parser::GetNextToken(in, line);
        status = PrintStmt(in, line);
        if(!status){
            ParseError(line, "Print in Stmt didn't work out");
            return false;	
        }
    }
    else if (lex == IF) {
        //Parser::PushBackToken(lex);

        LexItem tok = Parser::GetNextToken(in, line);
        if (tok != LPAREN) {
            ParseError(line, "Missing Left Parenthesis");
            return false;
        }
        
        bool status = RelExpr(in, line);
        if(!status){
            ParseError(line, "Missing Relational Expression");
            return false;
        }
        
        tok = Parser::GetNextToken(in, line);
        if (tok != RPAREN) {
            ParseError(line, "Missing Right Parenthesis");
            return false;
        }


        tok = Parser::GetNextToken(in, line); 

        if (tok == THEN) {

            Parser::PushBackToken(tok);


            status = BlockIfStmt(in, line); // Try BlockIfStmt if SimpleIfStmt fails
            if (!status) {
                ParseError(line, "Block didn't work out");
                return false;
            }
        }
        else{
            Parser::PushBackToken(tok);
            status = SimpleIfStmt(in, line); // Try SimpleIfStmt
            if (!status) {
                ParseError(line, "SimpleStmt didn't work out in Stmt");
                return false;
            }

        }
        return true;
    }
    
    else if (lex == END){
        Parser::PushBackToken(lex);
        return true;
    }
    
    else {
        ParseError(line, "Unexpected token encountered (in Stmt): " +  lex.GetLexeme());
        return false;
    }
    

    if (!status) {
        ParseError(line, "none of the Stmts in Stmt worked out");
        return false;
    }
    else {
        return true;
    }
}




//VarList:= Var [= Expr] {, Var [= Expr]}   good to go??
bool VarList(istream& in, int& line) {

    LexItem tok2 = Parser::GetNextToken(in, line);
    string varName = tok2.GetLexeme();
    Parser::PushBackToken(tok2);


    bool status = Var(in, line); // Parse the first variable
    if (!status) {
        ParseError(line, "Invalid Variable (Varlist)");
        return false;
    }

    LexItem tok = Parser::GetNextToken(in, line);
    string varName2 = tok.GetLexeme();
        // Check if the variable has been declared by checking the defVar map and its boolean value to be false
        if (defVar.find(varName2) == defVar.end()) {
            defVar[varName2] = false;
            //return true;
        }

        else if (defVar[varName2] == false) {
                ParseError(line, "Variable '" + varName2 + "' already declared but not initialized");
                return false;
            
        }
        else {
            ParseError(line, "Variable '" + varName2 + "' already declared AND initialized so cant redeclare it.");
            return false;

            //say which variable is being declared
            //cout << "Variable " << varName << " was declared." << endl;
            //print the map of variables in good format
            //for(auto& it : defVar){
              //  cout << it.first << " " << it.second << endl;
            //}
        }

    // Check if there's an assignment operator and an expression
    
    tok = Parser::GetNextToken(in, line);
    if (tok == ASSOP) {
        //if(defVar.find(varName) != defVar.end()){
        //    if(defVar[varName] == false){
        //       ParseError(line, "Variable already declared but not initialized so you can't redeclare it");
        //        return false;
        //    }
        //}
        
        // Parse the expression

        
        status = Expr(in, line);
        if (!status) {
            ParseError(line, "Invalid Expression (Varlist)");
            return false;
        }

        cout << "Initialization of the variable " << varName << " in the declaration statement." << endl;
        // Mark the variable as declared
        defVar[varName] = true;


    } else {
        // If no assignment operator, push back the token to continue parsing
        Parser::PushBackToken(tok);
    }

    // Check for comma to determine if there's another variable
    tok = Parser::GetNextToken(in, line);
        if (tok == COMMA) {
            // If comma, use recursion
            status = VarList(in, line);
            
        }
        else if (tok.GetToken() == ERR) {
            ParseError(line, "Unrecognized Input Pattern");
            cout << "(" << tok.GetLexeme() << ")" << endl;
            return false;
        }
        else {
            // If no comma, push back the token to continue parsing
            Parser::PushBackToken(tok);
            return true;
        }
    return status;

    }
//End of VarList





static bool CharHasLen = false;
//Type:= INTEGER | REAL | CHARARACTER [(LEN = ICONST)]
#include <cstdlib> // for atoi

// Global variable to store the length of character strings
static string charStringLength = "";

bool Type(istream& in, int& line) {
    LexItem tok = Parser::GetNextToken(in, line);

    if (tok == INTEGER || tok == REAL || tok == CHARACTER) {
        // Check for optional length specification
        tok = Parser::GetNextToken(in, line);
        if (tok == LPAREN) {
            // Parse length specification
            tok = Parser::GetNextToken(in, line);
            if (tok != LEN) {
                ParseError(line, "Missing LEN");
                return false;
            }
            tok = Parser::GetNextToken(in, line);
            if (tok != ASSOP) {
                ParseError(line, "Missing Assignment Operator");
                return false;
            }
            tok = Parser::GetNextToken(in, line);
            if (tok != ICONST) {
                ParseError(line, "Missing Integer Constant");
                return false;
            }

            charStringLength = tok.GetLexeme(); // Store lexeme directly as a string
            tok = Parser::GetNextToken(in, line);

            //store the number or ICONST that is the length of the character
            if (tok != RPAREN) {
                ParseError(line, "Missing Right Parenthesis");
                return false;
            }
            CharHasLen = true;

            
        } else {
            // Push back the token if there's no length specification
            Parser::PushBackToken(tok);
        }
        return true;
    } else if (tok.GetToken() == ERR) {
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    } else {
        ParseError(line, "Invalid Type");
        return false;
    }
}
//End of Type


//Decl:= Type :: VarList
bool Decl(istream& in, int& line) {
    bool status = Type(in, line);

    if (!status) {
        ParseError(line, "Invalid Type");
        return false;
    }

    LexItem tok = Parser::GetNextToken(in, line);
    if (tok != DCOLON) {
        ParseError(line, "Missing Double Colon");
        return false;
    }

    // Marking variables declared within this declaration
    status = VarList(in, line);
    if (!status) {
        ParseError(line, "Missing Variable List (DECL)");
        return false;
    }

    // Set the declared variables to true in the defVar map
    for (auto& it : defVar) {
        it.second = true;
    }

    // Print the length of the character if specified
    if (CharHasLen) {
        cout << "Definition of Strings with length of " << charStringLength << " in declaration statement." << endl;
        CharHasLen = false;
        charStringLength = "";
    }

    return status;
}

//Prog:= PROGRAM IDENT {Decl} {Stmt} END PROGRAM IDENT
bool Prog(istream& in, int& line) {
    LexItem tok = Parser::GetNextToken(in,line);
    if(tok!=PROGRAM){
        ParseError(line,"Missing PROGRAM token");
        return false;
    }

    tok = Parser::GetNextToken(in,line);
    if(tok!=IDENT){
        ParseError(line,"Missing IDENT after PROGRAM");
        return false;
    }
    
    tok = Parser::GetNextToken(in, line);
    bool status;

    while (tok == INTEGER || tok == REAL || tok == CHARACTER) {
        Parser::PushBackToken(tok);
        status = Decl(in, line);
        if(!status){
            ParseError(line, "Error in declaration");
            return false;
        }
        tok = Parser::GetNextToken(in,line);
    }
    //Parser::PushBackToken(tok);

    //tok = Parser::GetNextToken(in, line);

    while (tok != END ) {
        Parser::PushBackToken(tok);

        if(tok.GetToken() == ERR){
            ParseError(line, "Unrecognized Input Pattern");
            cout << "(" << tok.GetLexeme() << ")" << endl;
            return false;
        }
        
        
        status = Stmt(in,line);
        if(!status){
            ParseError(line,"Error in statement (prog)");
            return false;
        }

        tok = Parser::GetNextToken(in, line);
    }
    //Parser::PushBackToken(tok);

    tok = Parser::GetNextToken(in, line);
    if (tok != PROGRAM) {
        ParseError(line, "Missing PROGRAM");
        return false;
    }

    tok = Parser::GetNextToken(in, line);
    if (tok != IDENT) {
        ParseError(line, "Missing IDENT after END PROGRAM");
        return false;
    }

    cout << "(DONE)" << endl;

    return true;
}
//End of Prog
