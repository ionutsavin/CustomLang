%{
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <stdexcept>
#include <variant>
#include <cstring>

extern int yylex();
void yyerror(const char *s);

class ASTNode {
private:
    char op;
    std::variant<int, double> value;
    ASTNode* left;
    ASTNode* right;
public:
    ASTNode(char op, ASTNode* left = nullptr, ASTNode* right = nullptr)
        : op(op), left(left), right(right) {}

    ASTNode(const char* valueStr, bool isFloat = false) 
        : op('\0'), left(nullptr), right(nullptr) {
        if (isFloat) {
            value = std::atof(valueStr);
        } else {
            value = std::atoi(valueStr);
        }
    }

    ~ASTNode() {
        delete left;
        delete right;
    }

    std::variant<int, double> evaluate() {
        if (op == '\0') {
            return value;
        }

        auto leftVal = left ? left->evaluate() : std::variant<int, double>(0);
        auto rightVal = right ? right->evaluate() : std::variant<int, double>(0);

        return std::visit([op = this->op](auto&& left, auto&& right) -> std::variant<int, double> {
            using T = std::decay_t<decltype(left)>;
            if constexpr (!std::is_same_v<T, std::decay_t<decltype(right)>>) {
                throw std::runtime_error("Operands must be of the same type");
            }
            switch (op) {
                case '+': return left + right;
                case '-': return left - right;
                case '*': return left * right;
                case '/':
                    if (right == 0) throw std::runtime_error("Division by zero");
                    return left / right;
                case '=': return left == right;
                case '!': 
                    if constexpr(std::is_same_v<decltype(right), double>) {
                        return left != right;
                    } else {
                        return !left;
                    }
                case '<': return left < right;
                case '>': return left > right;
                case '&': return left && right;
                case '|': return left || right;
                default:
                    throw std::runtime_error("Invalid operator");
            }
        }, leftVal, rightVal);
    }
};


struct VariableInfo
{
  std::string type;
  std::string name;
  std::string value;
  std::string definedIn;
};

struct FunctionInfo
{
  std::string name;
  std::string returnType;
  std::vector<std::string> parametersType;
  std::string definedIn;
};

std::vector<VariableInfo> variableTable;
std::vector<FunctionInfo> functionTable;
std::vector<std::string> param;
std::string currentScope = "global";
std::string currentClassName;

void addVariable(const std::string& type, const std::string& name, const std::string& value = "") {
    // Check for duplicate variable declaration in the current scope
    for (const auto& var : variableTable) {
        if (var.name == name && var.definedIn == currentScope) {
            throw std::runtime_error("Variable " + name + " already declared in this scope.");
        }
    }
    VariableInfo var = {type, name, value, currentScope};
    variableTable.push_back(var);
}

void addFunction(const std::string& name, const std::string& returnType, const std::vector<std::string>& params) {
    std::string scope = currentClassName.empty() ? "global" : currentClassName;
    FunctionInfo func = {name, returnType, params, scope};
    functionTable.push_back(func);
}


bool updateVariableInScope(const std::string& name, const std::string& value, const std::string& scope) {
    for (auto& var : variableTable) {
        if (var.name == name && var.definedIn == scope) {
            var.value = value;
            return true;
        }
    }
    return false;
}

void updateVariableValue(const std::string& name, const std::string& value) {
    if (!updateVariableInScope(name, value, currentScope)) {
        if (!currentClassName.empty() && !updateVariableInScope(name, value, currentClassName)) {
            if (!updateVariableInScope(name, value, "global")) {
                throw std::runtime_error("Variable " + name + " not found in any scope.");
            }
        }
    }
}

ASTNode* getVariableValueNode(const char* varName) {
    for (const auto& var : variableTable) {
        if (var.name == varName && (var.definedIn == currentScope || var.definedIn == "global")) {
            return new ASTNode(var.value.c_str());
        }
    }
    throw std::runtime_error(std::string("Variable ") + varName + " not found in current scope.");
    return nullptr;
}

void printSymbolTable(const std::string& filename) {
    std::ofstream outFile(filename);

    outFile << "Variables:\n";
    for (const auto& var : variableTable) {
        outFile << "Type: " << var.type << ", Name: " << var.name
                << ", Value: " << var.value << ", Defined in: " << var.definedIn << "\n";
    }

    outFile << "\nFunctions:\n";
    for (const auto& func : functionTable) {
        outFile << "Name: " << func.name << ", Return Type: " << func.returnType
                << ", Defined in: " << func.definedIn << ", Parameters: ";
        for (const auto& param : func.parametersType) {
            outFile << param << " ";
        }
        outFile << "\n";
    }

    outFile.close();
}

%}
%union {
    char* stringValue;
    struct ASTNode* node;

}
%token<stringValue> INT FLOAT CHAR STRING BOOL VOID CONSTANT
%token<stringValue> INTVALUE FLOATVALUE CHARVALUE  STRINGVALUE  BOOLVALUE
%token IF FOR WHILE
%token OPEN_PAREN CLOSE_PAREN OPEN_BRACE CLOSE_BRACE OPEN_BRACKET CLOSE_BRACKET
%token SEMICOLON COMMA
%token ASSIGN PLUS MINUS MULT DIV AND OR NOT EQUAL NOT_EQUAL LESS_THAN GREATER_THAN
%token CLASS
%token RETURN
%token<stringValue> IDENTIFIER
%type<stringValue> type_variables
%type<stringValue> type_value
%type <stringValue> parameter
%type <stringValue> left_value
%type <node> expression
%left OR
%left AND
%left EQUAL NOT_EQUAL
%left LESS_THAN GREATER_THAN
%left PLUS MINUS
%left MULT DIV
%right NOT
%start program
%%
program:
    declarations{
        printSymbolTable("symbol_table.txt");
        std::cout << "Successful compilation!" << std::endl;
    }
;

declarations:
    declarations declaration
    |  /*empty*/
;

declaration:
    var_declaration
    | array_declaration
    | constant_declaration
    | func_declaration
    | class_declaration
    | statements
;
type_variables:
    INT
    | FLOAT
    | CHAR
    | STRING
    | BOOL
;
type_value:
    INTVALUE
    | FLOATVALUE
    | CHARVALUE
    | BOOLVALUE
    | STRINGVALUE
;
var_declaration:
    type_variables IDENTIFIER SEMICOLON{
        addVariable($1, $2);
    }
    | type_variables IDENTIFIER ASSIGN expression SEMICOLON{
        class ASTNode* exprAST = $4;
        std::variant<int, double> result = exprAST->evaluate();

        // Using std::visit to handle the result
        std::string resultStr = std::visit([](auto&& value) -> std::string {
            return std::to_string(value);
        }, result);

        addVariable($1, $2, resultStr);
        delete exprAST;
    }
;

array_declaration:
    type_variables IDENTIFIER OPEN_BRACKET INTVALUE CLOSE_BRACKET SEMICOLON
;

constant_declaration:
    CONSTANT type_variables IDENTIFIER ASSIGN type_value SEMICOLON{
        addVariable($2, $3, $5);
    }
;

func_declaration:
    func_header compound_statement {
        currentScope = currentClassName.empty() ? "global" : currentClassName;
    }
;

func_header:
    type_variables IDENTIFIER OPEN_PAREN parameters CLOSE_PAREN {
        currentScope = $2;
        addFunction($2, $1, param);
        param.clear();
    }
    | VOID IDENTIFIER OPEN_PAREN parameters CLOSE_PAREN {
        currentScope = $2;
        addFunction($2, "void", param);
        param.clear();
    }
;

class_declaration:
    class_header OPEN_BRACE class_body CLOSE_BRACE {
        currentScope = "global";
        currentClassName = "";
    }
;

class_header:
    CLASS IDENTIFIER {
        currentClassName = $2;
        currentScope = $2;
    }
;

class_body:
    class_body class_member
    | /* empty */
;

class_member:
    var_declaration
    | func_declaration
;

parameters:
    parameters COMMA parameter
    | parameter
    | /* empty */
;

parameter:
    type_variables IDENTIFIER {
        param.push_back($2);
    }
;

compound_statement:
    OPEN_BRACE statements CLOSE_BRACE
;

statements:
    statements statement
    | /* empty */
;

statement:
    var_declaration
    | assignment_statement
    | if_statement
    | while_statement
    | for_statement
    | return_statement
    | function_call SEMICOLON
;

if_statement:
    IF OPEN_PAREN expression CLOSE_PAREN compound_statement
;


while_statement:
    WHILE OPEN_PAREN expression CLOSE_PAREN compound_statement
;


for_statement:
    FOR OPEN_PAREN {
        currentScope = "for local";
    } var_declaration expression SEMICOLON assignment_statement CLOSE_PAREN compound_statement {
        currentScope = "global";
    }
;

return_statement:
    RETURN IDENTIFIER SEMICOLON
    | RETURN expression SEMICOLON
;

assignment_statement:
    left_value ASSIGN expression SEMICOLON {
        char* varName = $1;
        class ASTNode* exprAST = $3;
        std::variant<int, double> result = exprAST->evaluate();

        // Using std::visit to handle the result
        std::string resultStr = std::visit([](auto&& value) -> std::string {
            return std::to_string(value);
        }, result);

        updateVariableValue(varName, resultStr);
        delete exprAST;
        delete[] varName;
    }
    | /* empty */
;


left_value:
    IDENTIFIER {
        $$ = $1;
    }
;

expression:
    expression PLUS expression {
        $$ = new ASTNode('+', $1, $3);
    }
    | expression MINUS expression {
        $$ = new ASTNode('-', $1, $3);
    }
    | expression MULT expression {
        $$ = new ASTNode('*', $1, $3);
    }
    | expression DIV expression {
        $$ = new ASTNode('/', $1, $3);
    }
    | expression EQUAL expression {
        $$ = new ASTNode('=', $1, $3);
    }
    | expression NOT_EQUAL expression {
        $$ = new ASTNode('!', $1, $3);
    }
    | expression LESS_THAN expression {
        $$ = new ASTNode('<', $1, $3);
    }
    | expression GREATER_THAN expression {
        $$ = new ASTNode('>', $1, $3);
    }
    | expression AND expression {
        $$ = new ASTNode('&', $1, $3);
    }
    | expression OR expression {
        $$ = new ASTNode('|', $1, $3);
    }
    | NOT expression {
        $$ = new ASTNode('!', nullptr, $2);
    }
    | INTVALUE{
        $$ = new ASTNode($1);
    }
    | FLOATVALUE{
        $$ = new ASTNode($1, true);
    }
    | BOOLVALUE {
        $$ = new ASTNode(strcmp($1, "true") == 0 ? "1" : "0");
    }
    | IDENTIFIER {
        ASTNode* node = getVariableValueNode($1);
        $$ = node;
    }
    | /* empty */{
        $$ = nullptr;
    }
;

function_call:
    IDENTIFIER OPEN_PAREN arguments CLOSE_PAREN
;

arguments:
    arguments COMMA argument
    | argument
;

argument:
    expression
;
%%
void yyerror(const char* s) {
    printf("error:%s\n",s);
}

int main(){
    yyparse();
}