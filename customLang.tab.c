/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "customLang.y"

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


#line 241 "customLang.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "customLang.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_INT = 3,                        /* INT  */
  YYSYMBOL_FLOAT = 4,                      /* FLOAT  */
  YYSYMBOL_CHAR = 5,                       /* CHAR  */
  YYSYMBOL_STRING = 6,                     /* STRING  */
  YYSYMBOL_BOOL = 7,                       /* BOOL  */
  YYSYMBOL_VOID = 8,                       /* VOID  */
  YYSYMBOL_CONSTANT = 9,                   /* CONSTANT  */
  YYSYMBOL_INTVALUE = 10,                  /* INTVALUE  */
  YYSYMBOL_FLOATVALUE = 11,                /* FLOATVALUE  */
  YYSYMBOL_CHARVALUE = 12,                 /* CHARVALUE  */
  YYSYMBOL_STRINGVALUE = 13,               /* STRINGVALUE  */
  YYSYMBOL_BOOLVALUE = 14,                 /* BOOLVALUE  */
  YYSYMBOL_IF = 15,                        /* IF  */
  YYSYMBOL_FOR = 16,                       /* FOR  */
  YYSYMBOL_WHILE = 17,                     /* WHILE  */
  YYSYMBOL_OPEN_PAREN = 18,                /* OPEN_PAREN  */
  YYSYMBOL_CLOSE_PAREN = 19,               /* CLOSE_PAREN  */
  YYSYMBOL_OPEN_BRACE = 20,                /* OPEN_BRACE  */
  YYSYMBOL_CLOSE_BRACE = 21,               /* CLOSE_BRACE  */
  YYSYMBOL_OPEN_BRACKET = 22,              /* OPEN_BRACKET  */
  YYSYMBOL_CLOSE_BRACKET = 23,             /* CLOSE_BRACKET  */
  YYSYMBOL_SEMICOLON = 24,                 /* SEMICOLON  */
  YYSYMBOL_COMMA = 25,                     /* COMMA  */
  YYSYMBOL_ASSIGN = 26,                    /* ASSIGN  */
  YYSYMBOL_PLUS = 27,                      /* PLUS  */
  YYSYMBOL_MINUS = 28,                     /* MINUS  */
  YYSYMBOL_MULT = 29,                      /* MULT  */
  YYSYMBOL_DIV = 30,                       /* DIV  */
  YYSYMBOL_AND = 31,                       /* AND  */
  YYSYMBOL_OR = 32,                        /* OR  */
  YYSYMBOL_NOT = 33,                       /* NOT  */
  YYSYMBOL_EQUAL = 34,                     /* EQUAL  */
  YYSYMBOL_NOT_EQUAL = 35,                 /* NOT_EQUAL  */
  YYSYMBOL_LESS_THAN = 36,                 /* LESS_THAN  */
  YYSYMBOL_GREATER_THAN = 37,              /* GREATER_THAN  */
  YYSYMBOL_CLASS = 38,                     /* CLASS  */
  YYSYMBOL_RETURN = 39,                    /* RETURN  */
  YYSYMBOL_IDENTIFIER = 40,                /* IDENTIFIER  */
  YYSYMBOL_YYACCEPT = 41,                  /* $accept  */
  YYSYMBOL_program = 42,                   /* program  */
  YYSYMBOL_declarations = 43,              /* declarations  */
  YYSYMBOL_declaration = 44,               /* declaration  */
  YYSYMBOL_type_variables = 45,            /* type_variables  */
  YYSYMBOL_type_value = 46,                /* type_value  */
  YYSYMBOL_var_declaration = 47,           /* var_declaration  */
  YYSYMBOL_array_declaration = 48,         /* array_declaration  */
  YYSYMBOL_constant_declaration = 49,      /* constant_declaration  */
  YYSYMBOL_func_declaration = 50,          /* func_declaration  */
  YYSYMBOL_func_header = 51,               /* func_header  */
  YYSYMBOL_class_declaration = 52,         /* class_declaration  */
  YYSYMBOL_class_header = 53,              /* class_header  */
  YYSYMBOL_class_body = 54,                /* class_body  */
  YYSYMBOL_class_member = 55,              /* class_member  */
  YYSYMBOL_parameters = 56,                /* parameters  */
  YYSYMBOL_parameter = 57,                 /* parameter  */
  YYSYMBOL_compound_statement = 58,        /* compound_statement  */
  YYSYMBOL_statements = 59,                /* statements  */
  YYSYMBOL_statement = 60,                 /* statement  */
  YYSYMBOL_if_statement = 61,              /* if_statement  */
  YYSYMBOL_while_statement = 62,           /* while_statement  */
  YYSYMBOL_for_statement = 63,             /* for_statement  */
  YYSYMBOL_64_1 = 64,                      /* $@1  */
  YYSYMBOL_return_statement = 65,          /* return_statement  */
  YYSYMBOL_assignment_statement = 66,      /* assignment_statement  */
  YYSYMBOL_left_value = 67,                /* left_value  */
  YYSYMBOL_expression = 68,                /* expression  */
  YYSYMBOL_function_call = 69,             /* function_call  */
  YYSYMBOL_arguments = 70,                 /* arguments  */
  YYSYMBOL_argument = 71                   /* argument  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   251

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  41
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  31
/* YYNRULES -- Number of rules.  */
#define YYNRULES  76
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  140

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   295


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   199,   199,   206,   207,   211,   212,   213,   214,   215,
     216,   219,   220,   221,   222,   223,   226,   227,   228,   229,
     230,   233,   236,   251,   255,   261,   267,   272,   280,   287,
     294,   295,   299,   300,   304,   305,   306,   310,   316,   320,
     321,   325,   326,   327,   328,   329,   330,   331,   335,   340,
     345,   345,   353,   354,   358,   372,   377,   383,   386,   389,
     392,   395,   398,   401,   404,   407,   410,   413,   416,   419,
     422,   425,   429,   435,   439,   440,   444
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "INT", "FLOAT", "CHAR",
  "STRING", "BOOL", "VOID", "CONSTANT", "INTVALUE", "FLOATVALUE",
  "CHARVALUE", "STRINGVALUE", "BOOLVALUE", "IF", "FOR", "WHILE",
  "OPEN_PAREN", "CLOSE_PAREN", "OPEN_BRACE", "CLOSE_BRACE", "OPEN_BRACKET",
  "CLOSE_BRACKET", "SEMICOLON", "COMMA", "ASSIGN", "PLUS", "MINUS", "MULT",
  "DIV", "AND", "OR", "NOT", "EQUAL", "NOT_EQUAL", "LESS_THAN",
  "GREATER_THAN", "CLASS", "RETURN", "IDENTIFIER", "$accept", "program",
  "declarations", "declaration", "type_variables", "type_value",
  "var_declaration", "array_declaration", "constant_declaration",
  "func_declaration", "func_header", "class_declaration", "class_header",
  "class_body", "class_member", "parameters", "parameter",
  "compound_statement", "statements", "statement", "if_statement",
  "while_statement", "for_statement", "$@1", "return_statement",
  "assignment_statement", "left_value", "expression", "function_call",
  "arguments", "argument", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-110)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-3)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -110,     9,     7,  -110,  -110,  -110,  -110,  -110,  -110,   -23,
     232,   -21,  -110,   -18,  -110,  -110,  -110,  -110,    10,  -110,
      20,    61,    25,    13,  -110,     2,  -110,  -110,  -110,    32,
      56,    63,    73,    70,    51,  -110,  -110,  -110,  -110,  -110,
    -110,  -110,    46,    66,   232,    72,   232,    92,  -110,    75,
      31,    89,    75,  -110,    75,  -110,  -110,  -110,    75,    81,
     118,    75,   -20,    75,  -110,    67,     6,  -110,   233,    50,
      88,  -110,   132,  -110,  -110,    76,  -110,  -110,  -110,    90,
     232,   104,  -110,  -110,  -110,    75,    75,    75,    75,    75,
      75,    75,    75,    75,    75,   171,    54,  -110,   146,  -110,
    -110,   232,  -110,  -110,  -110,  -110,  -110,   105,  -110,   106,
    -110,    15,    10,    75,    10,    74,    74,  -110,  -110,   193,
     182,   204,   204,   221,   221,  -110,    75,  -110,  -110,  -110,
    -110,  -110,   160,  -110,  -110,    97,  -110,    93,    10,  -110
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       4,     0,    40,     1,    11,    12,    13,    14,    15,     0,
       0,     0,     3,     0,     5,     6,     7,     8,     0,     9,
       0,    10,     0,     0,    29,     0,    40,    25,    31,     0,
       0,     0,    72,    56,     0,    41,    39,    43,    44,    45,
      46,    42,     0,     0,    36,     0,    36,     0,    21,    72,
       0,     0,    72,    50,    72,    68,    69,    70,    72,    71,
       0,    72,     0,    72,    47,     0,     0,    35,     0,     0,
       0,    71,     0,    38,    28,     0,    32,    33,    30,     0,
       0,     0,    67,    52,    53,    72,    72,    72,    72,    72,
      72,    72,    72,    72,    72,    76,     0,    75,     0,    37,
      27,     0,    16,    17,    18,    20,    19,     0,    26,     0,
      22,     0,     0,    72,     0,    57,    58,    59,    60,    65,
      66,    61,    62,    63,    64,    73,    72,    54,    34,    24,
      23,    48,     0,    49,    74,    55,    56,     0,     0,    51
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
    -110,  -110,  -110,  -110,    -2,  -110,     0,  -110,  -110,    77,
    -110,  -110,  -110,  -110,  -110,    68,    42,  -109,   125,  -110,
    -110,  -110,  -110,  -110,  -110,    22,  -110,   -31,  -110,  -110,
      18
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     1,     2,    12,    34,   107,    35,    15,    16,    17,
      18,    19,    20,    51,    78,    66,    67,    27,    21,    36,
      37,    38,    39,    80,    40,    41,    42,    95,    43,    96,
      97
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      13,    60,    14,   131,    48,   133,    49,    -2,    23,     3,
       4,     5,     6,     7,     8,     9,    10,    22,    72,    24,
      46,    79,    25,    81,    47,   100,    48,    82,    49,   139,
      26,   101,    98,    46,     4,     5,     6,     7,     8,    48,
      28,    49,    65,    44,    65,    11,    29,    30,    31,    75,
      52,    76,    73,    45,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,     4,     5,     6,     7,     8,   108,
      32,    33,    63,   125,    53,   101,    29,    30,    31,   126,
     113,    54,   132,    55,    56,    55,    56,    57,    61,    57,
      64,    62,     4,     5,     6,     7,     8,     9,    68,    65,
      32,    33,    70,    87,    88,    83,    58,    99,    58,   112,
      74,   109,   138,    59,    69,    71,   111,    85,    86,    87,
      88,    89,    90,   114,    91,    92,    93,    94,    77,   129,
     130,    85,    86,    87,    88,    89,    90,   136,    91,    92,
      93,    94,    84,   128,   134,    85,    86,    87,    88,    89,
      90,    50,    91,    92,    93,    94,   110,   137,     0,    85,
      86,    87,    88,    89,    90,     0,    91,    92,    93,    94,
     127,     0,     0,    85,    86,    87,    88,    89,    90,     0,
      91,    92,    93,    94,   135,     0,     0,    85,    86,    87,
      88,    89,    90,     0,    91,    92,    93,    94,    85,    86,
      87,    88,    89,    90,     0,    91,    92,    93,    94,    85,
      86,    87,    88,    89,     0,     0,    91,    92,    93,    94,
      85,    86,    87,    88,     0,     0,     0,    91,    92,    93,
      94,    85,    86,    87,    88,     4,     5,     6,     7,     8,
      93,    94,     0,   102,   103,   104,   105,   106,    85,    86,
      87,    88
};

static const yytype_int16 yycheck[] =
{
       2,    32,     2,   112,    24,   114,    26,     0,    10,     0,
       3,     4,     5,     6,     7,     8,     9,    40,    49,    40,
      18,    52,    40,    54,    22,    19,    24,    58,    26,   138,
      20,    25,    63,    18,     3,     4,     5,     6,     7,    24,
      20,    26,    44,    18,    46,    38,    15,    16,    17,    51,
      18,    51,    21,    40,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,     3,     4,     5,     6,     7,    19,
      39,    40,    26,    19,    18,    25,    15,    16,    17,    25,
      80,    18,   113,    10,    11,    10,    11,    14,    18,    14,
      24,    40,     3,     4,     5,     6,     7,     8,    26,   101,
      39,    40,    10,    29,    30,    24,    33,    40,    33,    19,
      21,    23,    19,    40,    46,    40,    40,    27,    28,    29,
      30,    31,    32,    19,    34,    35,    36,    37,    51,    24,
      24,    27,    28,    29,    30,    31,    32,    40,    34,    35,
      36,    37,    24,   101,   126,    27,    28,    29,    30,    31,
      32,    26,    34,    35,    36,    37,    24,   135,    -1,    27,
      28,    29,    30,    31,    32,    -1,    34,    35,    36,    37,
      24,    -1,    -1,    27,    28,    29,    30,    31,    32,    -1,
      34,    35,    36,    37,    24,    -1,    -1,    27,    28,    29,
      30,    31,    32,    -1,    34,    35,    36,    37,    27,    28,
      29,    30,    31,    32,    -1,    34,    35,    36,    37,    27,
      28,    29,    30,    31,    -1,    -1,    34,    35,    36,    37,
      27,    28,    29,    30,    -1,    -1,    -1,    34,    35,    36,
      37,    27,    28,    29,    30,     3,     4,     5,     6,     7,
      36,    37,    -1,    10,    11,    12,    13,    14,    27,    28,
      29,    30
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    42,    43,     0,     3,     4,     5,     6,     7,     8,
       9,    38,    44,    45,    47,    48,    49,    50,    51,    52,
      53,    59,    40,    45,    40,    40,    20,    58,    20,    15,
      16,    17,    39,    40,    45,    47,    60,    61,    62,    63,
      65,    66,    67,    69,    18,    40,    18,    22,    24,    26,
      59,    54,    18,    18,    18,    10,    11,    14,    33,    40,
      68,    18,    40,    26,    24,    45,    56,    57,    26,    56,
      10,    40,    68,    21,    21,    45,    47,    50,    55,    68,
      64,    68,    68,    24,    24,    27,    28,    29,    30,    31,
      32,    34,    35,    36,    37,    68,    70,    71,    68,    40,
      19,    25,    10,    11,    12,    13,    14,    46,    19,    23,
      24,    40,    19,    47,    19,    68,    68,    68,    68,    68,
      68,    68,    68,    68,    68,    19,    25,    24,    57,    24,
      24,    58,    68,    58,    71,    24,    40,    66,    19,    58
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    41,    42,    43,    43,    44,    44,    44,    44,    44,
      44,    45,    45,    45,    45,    45,    46,    46,    46,    46,
      46,    47,    47,    48,    49,    50,    51,    51,    52,    53,
      54,    54,    55,    55,    56,    56,    56,    57,    58,    59,
      59,    60,    60,    60,    60,    60,    60,    60,    61,    62,
      64,    63,    65,    65,    66,    66,    67,    68,    68,    68,
      68,    68,    68,    68,    68,    68,    68,    68,    68,    68,
      68,    68,    68,    69,    70,    70,    71
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,     0,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     5,     6,     6,     2,     5,     5,     4,     2,
       2,     0,     1,     1,     3,     1,     0,     2,     3,     2,
       0,     1,     1,     1,     1,     1,     1,     2,     5,     5,
       0,     9,     3,     3,     4,     0,     1,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     2,     1,     1,
       1,     1,     0,     4,     3,     1,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* program: declarations  */
#line 199 "customLang.y"
                {
        printSymbolTable("symbol_table.txt");
        std::cout << "Successful compilation!" << std::endl;
    }
#line 1421 "customLang.tab.c"
    break;

  case 21: /* var_declaration: type_variables IDENTIFIER SEMICOLON  */
#line 233 "customLang.y"
                                       {
        addVariable((yyvsp[-2].stringValue), (yyvsp[-1].stringValue));
    }
#line 1429 "customLang.tab.c"
    break;

  case 22: /* var_declaration: type_variables IDENTIFIER ASSIGN expression SEMICOLON  */
#line 236 "customLang.y"
                                                           {
        class ASTNode* exprAST = (yyvsp[-1].node);
        std::variant<int, double> result = exprAST->evaluate();

        // Using std::visit to handle the result
        std::string resultStr = std::visit([](auto&& value) -> std::string {
            return std::to_string(value);
        }, result);

        addVariable((yyvsp[-4].stringValue), (yyvsp[-3].stringValue), resultStr);
        delete exprAST;
    }
#line 1446 "customLang.tab.c"
    break;

  case 24: /* constant_declaration: CONSTANT type_variables IDENTIFIER ASSIGN type_value SEMICOLON  */
#line 255 "customLang.y"
                                                                  {
        addVariable((yyvsp[-4].stringValue), (yyvsp[-3].stringValue), (yyvsp[-1].stringValue));
    }
#line 1454 "customLang.tab.c"
    break;

  case 25: /* func_declaration: func_header compound_statement  */
#line 261 "customLang.y"
                                   {
        currentScope = currentClassName.empty() ? "global" : currentClassName;
    }
#line 1462 "customLang.tab.c"
    break;

  case 26: /* func_header: type_variables IDENTIFIER OPEN_PAREN parameters CLOSE_PAREN  */
#line 267 "customLang.y"
                                                                {
        currentScope = (yyvsp[-3].stringValue);
        addFunction((yyvsp[-3].stringValue), (yyvsp[-4].stringValue), param);
        param.clear();
    }
#line 1472 "customLang.tab.c"
    break;

  case 27: /* func_header: VOID IDENTIFIER OPEN_PAREN parameters CLOSE_PAREN  */
#line 272 "customLang.y"
                                                        {
        currentScope = (yyvsp[-3].stringValue);
        addFunction((yyvsp[-3].stringValue), "void", param);
        param.clear();
    }
#line 1482 "customLang.tab.c"
    break;

  case 28: /* class_declaration: class_header OPEN_BRACE class_body CLOSE_BRACE  */
#line 280 "customLang.y"
                                                   {
        currentScope = "global";
        currentClassName = "";
    }
#line 1491 "customLang.tab.c"
    break;

  case 29: /* class_header: CLASS IDENTIFIER  */
#line 287 "customLang.y"
                     {
        currentClassName = (yyvsp[0].stringValue);
        currentScope = (yyvsp[0].stringValue);
    }
#line 1500 "customLang.tab.c"
    break;

  case 37: /* parameter: type_variables IDENTIFIER  */
#line 310 "customLang.y"
                              {
        param.push_back((yyvsp[0].stringValue));
    }
#line 1508 "customLang.tab.c"
    break;

  case 50: /* $@1: %empty  */
#line 345 "customLang.y"
                   {
        currentScope = "for local";
    }
#line 1516 "customLang.tab.c"
    break;

  case 51: /* for_statement: FOR OPEN_PAREN $@1 var_declaration expression SEMICOLON assignment_statement CLOSE_PAREN compound_statement  */
#line 347 "customLang.y"
                                                                                               {
        currentScope = "global";
    }
#line 1524 "customLang.tab.c"
    break;

  case 54: /* assignment_statement: left_value ASSIGN expression SEMICOLON  */
#line 358 "customLang.y"
                                           {
        char* varName = (yyvsp[-3].stringValue);
        class ASTNode* exprAST = (yyvsp[-1].node);
        std::variant<int, double> result = exprAST->evaluate();

        // Using std::visit to handle the result
        std::string resultStr = std::visit([](auto&& value) -> std::string {
            return std::to_string(value);
        }, result);

        updateVariableValue(varName, resultStr);
        delete exprAST;
        delete[] varName;
    }
#line 1543 "customLang.tab.c"
    break;

  case 56: /* left_value: IDENTIFIER  */
#line 377 "customLang.y"
               {
        (yyval.stringValue) = (yyvsp[0].stringValue);
    }
#line 1551 "customLang.tab.c"
    break;

  case 57: /* expression: expression PLUS expression  */
#line 383 "customLang.y"
                               {
        (yyval.node) = new ASTNode('+', (yyvsp[-2].node), (yyvsp[0].node));
    }
#line 1559 "customLang.tab.c"
    break;

  case 58: /* expression: expression MINUS expression  */
#line 386 "customLang.y"
                                  {
        (yyval.node) = new ASTNode('-', (yyvsp[-2].node), (yyvsp[0].node));
    }
#line 1567 "customLang.tab.c"
    break;

  case 59: /* expression: expression MULT expression  */
#line 389 "customLang.y"
                                 {
        (yyval.node) = new ASTNode('*', (yyvsp[-2].node), (yyvsp[0].node));
    }
#line 1575 "customLang.tab.c"
    break;

  case 60: /* expression: expression DIV expression  */
#line 392 "customLang.y"
                                {
        (yyval.node) = new ASTNode('/', (yyvsp[-2].node), (yyvsp[0].node));
    }
#line 1583 "customLang.tab.c"
    break;

  case 61: /* expression: expression EQUAL expression  */
#line 395 "customLang.y"
                                  {
        (yyval.node) = new ASTNode('=', (yyvsp[-2].node), (yyvsp[0].node));
    }
#line 1591 "customLang.tab.c"
    break;

  case 62: /* expression: expression NOT_EQUAL expression  */
#line 398 "customLang.y"
                                      {
        (yyval.node) = new ASTNode('!', (yyvsp[-2].node), (yyvsp[0].node));
    }
#line 1599 "customLang.tab.c"
    break;

  case 63: /* expression: expression LESS_THAN expression  */
#line 401 "customLang.y"
                                      {
        (yyval.node) = new ASTNode('<', (yyvsp[-2].node), (yyvsp[0].node));
    }
#line 1607 "customLang.tab.c"
    break;

  case 64: /* expression: expression GREATER_THAN expression  */
#line 404 "customLang.y"
                                         {
        (yyval.node) = new ASTNode('>', (yyvsp[-2].node), (yyvsp[0].node));
    }
#line 1615 "customLang.tab.c"
    break;

  case 65: /* expression: expression AND expression  */
#line 407 "customLang.y"
                                {
        (yyval.node) = new ASTNode('&', (yyvsp[-2].node), (yyvsp[0].node));
    }
#line 1623 "customLang.tab.c"
    break;

  case 66: /* expression: expression OR expression  */
#line 410 "customLang.y"
                               {
        (yyval.node) = new ASTNode('|', (yyvsp[-2].node), (yyvsp[0].node));
    }
#line 1631 "customLang.tab.c"
    break;

  case 67: /* expression: NOT expression  */
#line 413 "customLang.y"
                     {
        (yyval.node) = new ASTNode('!', nullptr, (yyvsp[0].node));
    }
#line 1639 "customLang.tab.c"
    break;

  case 68: /* expression: INTVALUE  */
#line 416 "customLang.y"
              {
        (yyval.node) = new ASTNode((yyvsp[0].stringValue));
    }
#line 1647 "customLang.tab.c"
    break;

  case 69: /* expression: FLOATVALUE  */
#line 419 "customLang.y"
                {
        (yyval.node) = new ASTNode((yyvsp[0].stringValue), true);
    }
#line 1655 "customLang.tab.c"
    break;

  case 70: /* expression: BOOLVALUE  */
#line 422 "customLang.y"
                {
        (yyval.node) = new ASTNode(strcmp((yyvsp[0].stringValue), "true") == 0 ? "1" : "0");
    }
#line 1663 "customLang.tab.c"
    break;

  case 71: /* expression: IDENTIFIER  */
#line 425 "customLang.y"
                 {
        ASTNode* node = getVariableValueNode((yyvsp[0].stringValue));
        (yyval.node) = node;
    }
#line 1672 "customLang.tab.c"
    break;

  case 72: /* expression: %empty  */
#line 429 "customLang.y"
                 {
        (yyval.node) = nullptr;
    }
#line 1680 "customLang.tab.c"
    break;


#line 1684 "customLang.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 446 "customLang.y"

void yyerror(const char* s) {
    printf("error:%s\n",s);
}

int main(){
    yyparse();
}
