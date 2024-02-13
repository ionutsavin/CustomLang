%{
    #include "customLang.tab.h"
%}

%option noyywrap

%%

[0-9]+              { yylval.stringValue = strdup(yytext);return INTVALUE; }
[0-9]+\.[0-9]+      { yylval.stringValue = strdup(yytext);return FLOATVALUE; }
"true"              { yylval.stringValue = strdup(yytext);return BOOLVALUE; }
"false"             { yylval.stringValue = strdup(yytext);return BOOLVALUE; }
\"(\\.|[^\\"])*\"   { yylval.stringValue = strdup(yytext);return STRINGVALUE; }
\'[^']\'            { yylval.stringValue = strdup(yytext);return CHARVALUE; }
"int"               { yylval.stringValue = strdup(yytext); return INT; }
"float"             { yylval.stringValue = strdup(yytext); return FLOAT; }
"bool"              { yylval.stringValue = strdup(yytext); return BOOL; }
"string"            { yylval.stringValue = strdup(yytext); return STRING; }
"char"              { yylval.stringValue = strdup(yytext); return CHAR; }
"void"              { yylval.stringValue = strdup(yytext); return VOID; }
"if"                { return IF; }
"for"               { return FOR; }
"while"             { return WHILE; }
"class"             { return CLASS; }
"return"            { return RETURN; }
"const"             { return CONSTANT; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.stringValue = strdup(yytext); return IDENTIFIER; }
"&&"                { return AND; }
"||"                { return OR; }
"=="                { return EQUAL; }
"!="                { return NOT_EQUAL; }
"<"                 { return LESS_THAN; }
">"                 { return GREATER_THAN; }
"+"                 { return PLUS; }
"-"                 { return MINUS; }
"*"                 { return MULT; }
"/"                 { return DIV; }
"!"                 { return NOT; }
"="                 { return ASSIGN; }
";"                 { return SEMICOLON; }
","                 { return COMMA; }
"("                 { return OPEN_PAREN; }
")"                 { return CLOSE_PAREN; }
"{"                 { return OPEN_BRACE; }
"}"                 { return CLOSE_BRACE; }
"["                 { return OPEN_BRACKET; }
"]"                 { return CLOSE_BRACKET; }

[ \t\n]+            ; /* Ignore whitespace */
.                   { return yytext[0]; } /* Any other character */

%%
