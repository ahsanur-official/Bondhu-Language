#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cctype>
using namespace std;

// ---------------- Lexer (Bondhu Bhasha) ----------------
enum TokenType {
    TOK_DOST, TOK_CHOLLAM, // "sup apu" -> "dost", "tata apu" -> "chollam"
    TOK_RAKH, TOK_X_KO_DAO, TOK_BOL, // "dhoro apu" -> "rakh", "show apu" -> "bol"
    TOK_JODI, TOK_NAHOLE,
    TOK_GHURO, TOK_JOKHON, // "jhamela apu" -> "ghuro", "jokhon apu" -> "jokhon"
    TOK_LIST, // "list apu" -> "list"
    TOK_NUMBER, TOK_IDENTIFIER,
    TOK_LPAREN, TOK_RPAREN, TOK_LBRACE, TOK_RBRACE,
    TOK_SEMI, TOK_COMMA,
    TOK_PLUS, TOK_MINUS, TOK_MUL, TOK_DIV, TOK_MOD,
    TOK_EQ, TOK_LT, TOK_GT, TOK_LE, TOK_GE, TOK_NE, TOK_EE,
    TOK_STRING,
    TOK_EOF, TOK_UNKNOWN
};

struct Token {
    TokenType type;
    string value;
    int line;
};

// Lexer helper
vector<Token> tokenize(const string& src) {
    vector<Token> tokens;
    int i=0, line=1;
    auto peek = [&](){ return i<src.size()?src[i]:'\0'; };
    auto next = [&](){ char c = peek(); i++; if(c=='\n') line++; return c; };
    
    while(i<src.size()) {
        char c = peek();
        if(isspace((unsigned char)c)) { next(); continue; }

        // comments
        if(c=='/' && i+1<src.size() && src[i+1]=='/') {
            while(i<src.size() && peek()!='\n') next();
            continue;
        }

        // string literal
        if(c=='"') {
            string val; next();
            while(peek()!='"') val += next();
            next();
            tokens.push_back({TOK_STRING, val, line});
            continue;
        }

        // numbers
        if(isdigit(c)) {
            string val;
            while(isdigit(peek()) || peek()=='.') val += next();
            tokens.push_back({TOK_NUMBER, val, line});
            continue;
        }

        // identifiers / keywords
        if(isalpha(c)) {
            string val;
            while(isalnum(peek()) || peek()=='_' ) val += next();
            
            // Notun "Bondhu" keywords
            if(val=="dost") {
                tokens.push_back({TOK_DOST,"dost",line}); continue;
            }
            if(val=="chollam") {
                tokens.push_back({TOK_CHOLLAM,"chollam",line}); continue;
            }
            if(val=="rakh") {
                tokens.push_back({TOK_RAKH,"rakh",line}); continue;
            }
            if(val=="bol") {
                tokens.push_back({TOK_BOL,"bol",line}); continue;
            }
            if(val=="jodi") {
                tokens.push_back({TOK_JODI,"jodi",line}); continue;
            }
            if(val=="nahole") {
                tokens.push_back({TOK_NAHOLE,"nahole",line}); continue;
            }
            if(val=="ghuro") {
                tokens.push_back({TOK_GHURO,"ghuro",line}); continue;
            }
            if(val=="jokhon") {
                tokens.push_back({TOK_JOKHON,"jokhon",line}); continue;
            }
            if(val=="list") {
                tokens.push_back({TOK_LIST,"list",line}); continue;
            }

            // "x ke dao" logic ta apnar original compiler theke
            if(val=="x" && i+6<src.size() && src.substr(i,6)==" ke dao") { i+=6; tokens.push_back({TOK_X_KO_DAO,"x ke dao",line}); continue; }
            
            tokens.push_back({TOK_IDENTIFIER,val,line});
            continue;
        }

        // symbols
        switch(c){
            case '(' : tokens.push_back({TOK_LPAREN,"(",line}); next(); break;
            case ')' : tokens.push_back({TOK_RPAREN,")",line}); next(); break;
            case '{' : tokens.push_back({TOK_LBRACE,"{",line}); next(); break;
            case '}' : tokens.push_back({TOK_RBRACE,"}",line}); next(); break;
            case ';' : tokens.push_back({TOK_SEMI,";",line}); next(); break;
            case ',' : tokens.push_back({TOK_COMMA,",",line}); next(); break;
            case '+' : tokens.push_back({TOK_PLUS,"+",line}); next(); break;
            case '-' : tokens.push_back({TOK_MINUS,"-",line}); next(); break;
            case '*' : tokens.push_back({TOK_MUL,"*",line}); next(); break;
            case '/' : tokens.push_back({TOK_DIV,"/",line}); next(); break;
            case '%' : tokens.push_back({TOK_MOD,"%",line}); next(); break;
            case '=' :
                next();
                if(peek()=='='){ next(); tokens.push_back({TOK_EE,"==",line}); }
                else tokens.push_back({TOK_EQ,"=",line});
                break;
            case '<' :
                next();
                if(peek()=='='){ next(); tokens.push_back({TOK_LE,"<=",line}); }
                else tokens.push_back({TOK_LT,"<",line});
                break;
            case '>' :
                next();
                if(peek()=='='){ next(); tokens.push_back({TOK_GE,">=",line}); }
                else tokens.push_back({TOK_GT,">",line});
                break;
            case '!' :
                next();
                if(peek()=='='){ next(); tokens.push_back({TOK_NE,"!=",line}); }
                break;
            default: next(); tokens.push_back({TOK_UNKNOWN,string(1,c),line}); break;
        }
    }
    tokens.push_back({TOK_EOF,"EOF",line});
    return tokens;
}

// ---------------- Code Generator (C++ e Rupantor) ----------------
string generate_cpp(const vector<Token>& tokens){
    string out;
    // Program shurute default C++ code
    out += "#include <iostream>\n#include <vector>\nusing namespace std;\n";
    
    for(size_t i=0;i<tokens.size();i++){
        Token t = tokens[i];
        switch(t.type){
            // Notun keywords
            case TOK_DOST: out += "int main(){\n"; break; // "dost" keyword main function banabe
            case TOK_CHOLLAM: out += "return 0;\n}"; break;
            case TOK_RAKH: out += "auto "; break;
            case TOK_LIST: out += "vector<int> "; break;
            case TOK_X_KO_DAO: out += " = "; break; // "x ke dao"
            case TOK_BOL: out += "cout << "; break;
            case TOK_JODI: out += "if"; break;
            case TOK_NAHOLE: out += "else"; break;
            case TOK_GHURO: out += "for"; break;
            case TOK_JOKHON: out += "while"; break;
            
            // Standard C++ jinishpotro
            case TOK_NUMBER: case TOK_IDENTIFIER: case TOK_STRING:
                if(t.type==TOK_STRING) out += "\""+t.value+"\"";
                else out += t.value;
                break;
            case TOK_PLUS: out += "+"; break;
            case TOK_MINUS: out += "-"; break;
            case TOK_MUL: out += "*"; break;
            case TOK_DIV: out += "/"; break;
            case TOK_MOD: out += "%"; break;
            case TOK_EQ: out += "="; break; // '=' sign support korbe
            case TOK_LT: out += "<"; break;
            case TOK_GT: out += ">"; break;
            case TOK_LE: out += "<="; break;
            case TOK_GE: out += ">="; break;
            case TOK_NE: out += "!="; break;
            case TOK_EE: out += "=="; break;
            case TOK_LPAREN: out += "("; break;
            case TOK_RPAREN: out += ")"; break;
            case TOK_LBRACE: out += "{"; break;
            case TOK_RBRACE: out += "}"; break;
            case TOK_SEMI: out += ";\n"; break;
            case TOK_COMMA: out += ","; break;
            case TOK_EOF: break; // EOF kichu korbe na
            default: break; // Unknown token bad jabe
        }
    }
    return out;
}

// ---------------- Main ----------------
int main() {
    string srcFile = "example.bondhu"; // <-- Source file poriborton kora hoyeche
    string outFile = "out.cpp";

    ifstream in(srcFile);
    if(!in){ cerr<<"Cannot open "<<srcFile<<endl; return 1; }
    stringstream buffer; buffer<<in.rdbuf(); in.close();
    string src = buffer.str();

    vector<Token> tokens = tokenize(src);
    string cpp_code = generate_cpp(tokens);

    ofstream out(outFile);
    if(!out){ cerr<<"Cannot write "<<outFile<<endl; return 1; }
    out << cpp_code;
    out.close();

    cout<<"Compiled "<<srcFile<<" -> "<<outFile<<endl;
    return 0;
}