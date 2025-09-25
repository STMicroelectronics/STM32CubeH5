#ifndef __LEX_H__
#define __LEX_H__

#include "../port/port.h"

/* handy definitions */
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef min
#define min(x,y) (((x)<(y))?(x):(y))
#endif

#define MEM_ALIGN(x) (((x) + sizeof(ALIGN_TYPE) - 1) & ~(sizeof(ALIGN_TYPE)-1))

#define GETS_BUF_MAX 256


#define LEX_MALLOC(size)                        engine_port_malloc (heapParser, size) ;
#define LEX_FREE(mem)                           engine_port_free (heapParser, mem) ;

enum LexError
{
    ErrorAbort = 0,
    ErrorSyntax = -1,
    ErrorUndeclared = -2,
    ErrorParser = -3,
    ErrorMemory = -4,
    ErrorUnexpected = -5,
    ErrorRedeclared = -6

} ;

/* lexical tokens */
enum LexToken
{
    /* 0x00 */ TokenNone, 
    /* 0x01 */ TokenComma,
    /* 0x02 */ TokenAssign, TokenAddAssign, TokenSubtractAssign, TokenMultiplyAssign, TokenDivideAssign, TokenModulusAssign,
    /* 0x08 */ TokenShiftLeftAssign, TokenShiftRightAssign, TokenArithmeticAndAssign, TokenArithmeticOrAssign, TokenArithmeticExorAssign,
    /* 0x0d */ TokenQuestionMark, TokenColon, 
    /* 0x0f */ TokenLogicalOr, 
    /* 0x10 */ TokenLogicalAnd, 
    /* 0x11 */ TokenArithmeticOr, 
    /* 0x12 */ TokenArithmeticExor, 
    /* 0x13 */ TokenAmpersand, 
    /* 0x14 */ TokenEqual, TokenNotEqual, 
    /* 0x16 */ TokenLessThan, TokenGreaterThan, TokenLessEqual, TokenGreaterEqual,
    /* 0x1a */ TokenShiftLeft, TokenShiftRight, 
    /* 0x1c */ TokenPlus, TokenMinus, 
    /* 0x1e */ TokenAsterisk, TokenSlash, TokenModulus,
    /* 0x21 */ TokenIncrement, TokenDecrement, TokenUnaryNot, TokenUnaryExor, TokenSizeof, TokenCast,
    /* 0x27 */ TokenLeftSquareBracket, TokenRightSquareBracket, TokenDot, TokenArrow, 
    /* 0x2b */ TokenOpenBracket, TokenCloseBracket,
    /* 0x2d */ TokenIdentifier, TokenIntegerConstant, TokenFPConstant, TokenStringConstant, TokenCharacterConstant,
    /* 0x32 */ TokenSemicolon, TokenEllipsis,
    /* 0x34 */ TokenLeftBrace, TokenRightBrace,
#if 0
    /* 0x36 */ TokenIntType, TokenCharType, TokenFloatType, TokenDoubleType, TokenVoidType, TokenEnumType,
    /* 0x3c */ TokenLongType, TokenSignedType, TokenShortType, TokenStaticType, TokenAutoType, TokenRegisterType, TokenExternType, TokenStructType, TokenUnionType, TokenUnsignedType, TokenTypedef,
    /* 0x46 */ TokenContinue, TokenDo, TokenElse, TokenFor, TokenGoto, TokenIf, TokenWhile, TokenBreak, TokenSwitch, TokenCase, TokenDefault, TokenReturn,
    /* 0x52 */ TokenHashDefine, TokenHashInclude, TokenHashIf, TokenHashIfdef, TokenHashIfndef, TokenHashElse, TokenHashEndif,
    /* 0x59 */ TokenNew, TokenDelete,
    /* 0x5b */ TokenOpenMacroBracket,
#endif
    /* 0x00 */ TokenIndexConstant, /* TokenIndexString, */
    /* 0x00 */ TokenEOF, TokenEndOfLine, TokenEndOfFunction,
#if 0
    /* 0x00 */ TokenEvents, TokenStatemachine, TokenState, TokenDefaultState, TokenEnter, TokenEvent, TokenExit,  TokenDeferred, TokenPrevious,
#endif
    /* 0x00 */ TokenError,
    //
    TokenEvents,        \
    TokenVariables,     \
    TokenVersion,       \
    TokenName,          \
    TokenStartup,       \
    TokenStatemachine,  \
    TokenState,         \
    TokenSuperState,    \
    TokenDefaultState,  \
    TokenEnter,         \
    TokenEvent,         \
    TokenEventIf,       \
    TokenEventNot,      \
    TokenEventIfR,      \
    TokenEventNotR,     \
    TokenExit,          \
    TokenAction,        \
    TokenActionEventEq, \
    TokenActionLt,      \
    TokenActionGt,      \
    TokenActionEq,      \
    TokenActionNe,      \
    TokenActionLoad,    \
    TokenDeferred,      \
    TokenStartState,
    /* 0x00 */ TokenLast
};


/* lexer state */
#if 0
enum LexMode
{
    LexModeNormal /*,
    LexModeHashInclude,
    LexModeHashDefine,
    LexModeHashDefineSpace,
    LexModeHashDefineSpaceIdent */
};
#endif

#if 1
/* values */
enum BaseType
{
    TypeVoid,                   /* no type */
    TypeInt,                    /* integer */
    TypeShort,                  /* short integer */
    TypeChar,                   /* a single character (signed) */
    TypeLong,                   /* long integer */
    TypeUnsignedInt,            /* unsigned integer */
    TypeUnsignedShort,          /* unsigned short integer */
    TypeUnsignedChar,           /* unsigned 8-bit number */ /* must be before unsigned long */
    TypeUnsignedLong,           /* unsigned long integer */
#ifndef NO_FP
    TypeFP,                     /* floating point */
#endif
    TypeIdentifier, 
    TypeFunction,               /* a function */
    TypeMacro,                  /* a macro */
    TypePointer,                /* a pointer */
    TypeCharPointer,                /* a pointer */
    TypeArray,                  /* an array of a sub-type */
    TypeStruct,                 /* aggregate type */
    TypeUnion,                  /* merged type */
    TypeEnum,                   /* enumerated integer type */
    TypeGotoLabel,              /* a label we can "goto" */
    Type_Type                   /* a type for storing types */
};
#endif 



/* values */
union AnyValue
{
    char Character;
    short ShortInteger;
    int Integer;
    long LongInteger;
    unsigned short UnsignedShortInteger;
    unsigned int UnsignedInteger;
    unsigned long UnsignedLongInteger;
    unsigned char UnsignedCharacter;
    char *Identifier;
#if 0 
    char ArrayMem[2];               /* placeholder for where the data starts, doesn't point to it */
    struct ValueType *Typ;
    struct FuncDef FuncDef;
    struct MacroDef MacroDef;
#endif
    void *Pointer;                  /* unsafe native pointers */
};


struct Value
{
    enum BaseType Typ;          /* the type of this value */
    union AnyValue Val;            /* pointer to the AnyValue which holds the actual content */
    unsigned int Id ;
 };

typedef struct LEX_CB_IF_S* PLEX_CB_IF ;
struct LexState
{
    const char *Pos;
    const char *End;
    int Line;
    int CharacterPos;
 
    struct LEX_CB_IF_S* cb ;
    void* ctx ;
};


#ifdef __cplusplus
extern "C" {
#endif


typedef struct LEX_CB_IF_S {

    int (*GetReservedWord) (struct LexState * /*Lexer*/, const char* /*name*/, int /*len*/, enum LexToken * /*Token*/) ;
    int (*GetIdentifier) (struct LexState * /*Lexer*/, const char* /*name*/, int /*len*/, struct Value* /*Value*/) ;
    int (*GetString) (struct LexState * /*Lexer*/, const char* /*name*/, int /*len*/, struct Value* /*Value*/) ;
    int (*GetIndex) (struct LexState * /*Lexer*/, enum LexToken /*idxtok*/, const char* /*name*/, int /*len*/, struct Value* /*Value*/) ;
    int (*ParseToken) (struct LexState * /*Lexer*/, enum LexToken /*Token*/, struct Value* /*Value*/) ;

    void (*Error) (struct LexState * /*Lexer*/, enum LexError /*Error*/, char* /*Message*/) ;

} LEX_CB_IF ;

extern void             LexInit(struct LexState * Lexer, LEX_CB_IF* cb, void* ctx) ;
extern void*            LexGetCtx(struct LexState * Lexer) ;
extern enum LexToken    LexAnalyse(struct LexState * Lexer, const char *Source, int SourceLen, int Line) ;
extern enum LexToken    LexScanGetToken(struct LexState *Lexer, struct Value *Value) ;



extern const char*      LexGetValue(struct Value * value, char* val, int len) ;

#ifdef __cplusplus
}
#endif

#endif /* __LEX_H__ */
