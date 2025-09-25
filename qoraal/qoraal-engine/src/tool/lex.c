/* picoc lexer - converts source text into a tokenised form */ 

#include "lex.h"

#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#define NO_FP

#ifdef NO_CTYPE
#define isalpha(c) (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z'))
#define isdigit(c) ((c) >= '0' && (c) <= '9')
#define isalnum(c) (isalpha(c) || isdigit(c))
#define isspace(c) ((c) == ' ' || (c) == '\t' || (c) == '\r' || (c) == '\n')
#endif
#define isCidstart(c) (isalpha(c) || (c)=='_' || (c)=='#')
#define isCident(c) (isalnum(c) || (c)=='_')

#define IS_HEX_ALPHA_DIGIT(c) (((c) >= 'a' && (c) <= 'f') || ((c) >= 'A' && (c) <= 'F'))
#define IS_BASE_DIGIT(c,b) (((c) >= '0' && (c) < '0' + (((b)<10)?(b):10)) || (((b) > 10) ? IS_HEX_ALPHA_DIGIT(c) : FALSE))
#define GET_BASE_DIGIT(c) (((c) <= '9') ? ((c) - '0') : (((c) <= 'F') ? ((c) - 'A' + 10) : ((c) - 'a' + 10)))

#define NEXTIS(c,x,y) { if (NextChar == (c)) { LEXER_INC(Lexer); GotToken = (x); } else GotToken = (y); }
#define NEXTIS3(c,x,d,y,z) { if (NextChar == (c)) { LEXER_INC(Lexer); GotToken = (x); } else NEXTIS(d,y,z) }
#define NEXTIS4(c,x,d,y,e,z,a) { if (NextChar == (c)) { LEXER_INC(Lexer); GotToken = (x); } else NEXTIS3(d,y,e,z,a) }
#define NEXTIS3PLUS(c,x,d,y,e,z,a) { if (NextChar == (c)) { LEXER_INC(Lexer); GotToken = (x); } else if (NextChar == (d)) { if (Lexer->Pos[1] == (e)) { LEXER_INCN(Lexer, 2); GotToken = (z); } else { LEXER_INC(Lexer); GotToken = (y); } } else GotToken = (a); }
#define NEXTISEXACTLY3(c,d,y,z) { if (NextChar == (c) && Lexer->Pos[1] == (d)) { LEXER_INCN(Lexer, 2); GotToken = (y); } else GotToken = (z); }

#define LEXER_INC(l) ( (l)->Pos++, (l)->CharacterPos++ )
#define LEXER_INCN(l, n) ( (l)->Pos+=(n), (l)->CharacterPos+=(n) )
#define TOKEN_DATA_OFFSET 2

#define MAX_CHAR_VALUE 255      /* maximum value which can be represented by a "char" data type */


#if 0
static struct ReservedWord ReservedWords[] =
{
#if 0
    { "#define", TokenHashDefine },
    { "#else", TokenHashElse },
    { "#endif", TokenHashEndif },
    { "#if", TokenHashIf },
    { "#ifdef", TokenHashIfdef },
    { "#ifndef", TokenHashIfndef },
    { "#include", TokenHashInclude },

    { "auto", TokenAutoType },
    { "break", TokenBreak },
    { "case", TokenCase },
    { "char", TokenCharType },
    { "continue", TokenContinue },
    { "default", TokenDefault },
    { "delete", TokenDelete },
    { "do", TokenDo },
#ifndef NO_FP
    { "double", TokenDoubleType },
#endif
    { "else", TokenElse },
    { "enum", TokenEnumType },
    { "extern", TokenExternType },
#ifndef NO_FP
    { "float", TokenFloatType },
#endif
    { "for", TokenFor },
    { "goto", TokenGoto },
    { "if", TokenIf },
    { "int", TokenIntType },
    { "long", TokenLongType },
    { "new", TokenNew },
    { "register", TokenRegisterType },
    { "return", TokenReturn },
    { "short", TokenShortType },
    { "signed", TokenSignedType },
    { "sizeof", TokenSizeof },
    { "static", TokenStaticType },
    { "struct", TokenStructType },
    { "switch", TokenSwitch },
    { "typedef", TokenTypedef },
    { "union", TokenUnionType },
    { "unsigned", TokenUnsignedType },
    { "void", TokenVoidType },
    { "while", TokenWhile },
#endif

};
#endif

//static void LexFail(struct LexState *Lexer, const char *Message, ...) ;
//static const char* LexTokenName(enum LexToken Token) ;
//static const char* LexGetValue(struct Value * value) ;

/* initialise the lexer */

void LexInit(struct LexState * Lexer, LEX_CB_IF* cb, void* ctx)
{
    memset( Lexer, 0, sizeof(struct LexState)) ;
    Lexer->cb = cb ;
    Lexer->ctx = ctx ;
}

void* LexGetCtx(struct LexState * Lexer)
{
    return Lexer->ctx ;
}

/* get a numeric literal - used while scanning */
enum LexToken LexGetNumber(struct LexState *Lexer, struct Value *Value)
{
    long Result = 0;
    long Base = 10;
    enum LexToken ResultToken;
#ifndef NO_FP
    double FPResult;
    double FPDiv;
#endif
    /* long/unsigned flags */
#if 1 /* unused for now */
    char IsLong = 0;
    char IsUnsigned = 0;
#endif
    
    if (*Lexer->Pos == '0')
    { 
        /* a binary, octal or hex literal */
        LEXER_INC(Lexer);
        if (Lexer->Pos != Lexer->End)
        {
            if (*Lexer->Pos == 'x' || *Lexer->Pos == 'X')
                { Base = 16; LEXER_INC(Lexer); }
            else if (*Lexer->Pos == 'b' || *Lexer->Pos == 'B')
                { Base = 2; LEXER_INC(Lexer); }
            else if (*Lexer->Pos != '.')
                Base = 8;
        }
    }

    /* get the value */
    for (; Lexer->Pos != Lexer->End && IS_BASE_DIGIT(*Lexer->Pos, Base); LEXER_INC(Lexer))
        Result = Result * Base + GET_BASE_DIGIT(*Lexer->Pos);

    Value->Typ = TypeInt ;
    if (*Lexer->Pos == 'u' || *Lexer->Pos == 'U')
    {
        LEXER_INC(Lexer);
        
        IsUnsigned = 1; 
    }
    if (*Lexer->Pos == 'l' || *Lexer->Pos == 'L')
    {
        LEXER_INC(Lexer);
        Value->Typ = TypeLong ;

         IsLong = 1;
    }
    if (IsUnsigned && IsLong) Value->Typ = TypeUnsignedLong ;
    else if (IsUnsigned) Value->Typ = TypeUnsignedInt ;
    else if (IsLong) Value->Typ = TypeLong ;
    else Value->Typ = TypeInt ;
    
 //   Value->Typ = &pc->LongType; /* ignored? */
    Value->Val.LongInteger = Result;

    ResultToken = TokenIntegerConstant;
    
    if (Lexer->Pos == Lexer->End)
        return ResultToken;
        
#if 0 // ndef NO_FP
    if (Lexer->Pos == Lexer->End)
    {
        return ResultToken;
    }
    
    if (*Lexer->Pos != '.' && *Lexer->Pos != 'e' && *Lexer->Pos != 'E')
    {
        return ResultToken;
    }
    
    Value->Typ = &pc->FPType;
    FPResult = (double)Result;
    
    if (*Lexer->Pos == '.')
    {
        LEXER_INC(Lexer);
        for (FPDiv = 1.0/Base; Lexer->Pos != Lexer->End && IS_BASE_DIGIT(*Lexer->Pos, Base); LEXER_INC(Lexer), FPDiv /= (double)Base)
        {
            FPResult += GET_BASE_DIGIT(*Lexer->Pos) * FPDiv;
        }
    }

    if (Lexer->Pos != Lexer->End && (*Lexer->Pos == 'e' || *Lexer->Pos == 'E'))
    {
        int ExponentSign = 1;
        
        LEXER_INC(Lexer);
        if (Lexer->Pos != Lexer->End && *Lexer->Pos == '-')
        {
            ExponentSign = -1;
            LEXER_INC(Lexer);
        }
        
        Result = 0;
        while (Lexer->Pos != Lexer->End && IS_BASE_DIGIT(*Lexer->Pos, Base))
        {
            Result = Result * Base + GET_BASE_DIGIT(*Lexer->Pos);
            LEXER_INC(Lexer);
        }

        FPResult *= pow((double)Base, (double)Result * ExponentSign);
    }
    
    Value->Val->FP = FPResult;

    if (*Lexer->Pos == 'f' || *Lexer->Pos == 'F')
        LEXER_INC(Lexer);

    return TokenFPConstant;
#else
    return ResultToken;
#endif
}

/* get a reserved word or identifier - used while scanning */
enum LexToken LexGetWord(struct LexState *Lexer, struct Value *Value)
{
    const char *StartPos = Lexer->Pos;
    enum LexToken Token = TokenNone ;
    //struct dlist * np ;
    //unsigned int id ;
    int res = 1 ;
    char* err ;
    
    do {
        LEXER_INC(Lexer);
    } while (Lexer->Pos != Lexer->End && isCident((int)*Lexer->Pos));
 

    Value->Typ = 0;
    Value->Id = 0 ;
    Value->Val.Integer = 0 ;

    
    //if(!__LexGetReservedWord(Lexer, StartPos, Lexer->Pos - StartPos, &Token)) {  
    //    if (!__LexGetIdentifier(Lexer, StartPos, Lexer->Pos - StartPos, Value)) {
    //         __LexError(Lexer, ErrorMemory, "out of memory");
    //            return TokenError ;
    //    }
    //    Token = TokenIdentifier ;
    //}
    if(Lexer->cb->GetReservedWord &&
            !Lexer->cb->GetReservedWord (Lexer, StartPos, Lexer->Pos - StartPos, &Token)) {
        if (Lexer->cb->GetIdentifier) res = Lexer->cb->GetIdentifier(Lexer, StartPos, Lexer->Pos - StartPos, Value) ;
        if (res <= 0) {
            switch (res) {
            case ErrorSyntax:       err = "Syntax"; break ;
            case ErrorUndeclared:   err = "Undeclared"; break ;
            case ErrorParser:       err = "Parser"; break ;
            case ErrorMemory:       err = "Memory"; break ;
            case ErrorUnexpected:   err = "Unexpected"; break ;
            case ErrorRedeclared:   err = "Redeclared"; break ;
            default:                err = "Abort"; break ;

            }
            if (Lexer->cb->Error) Lexer->cb->Error(Lexer, res, err);
            return TokenError ;
        }
        Token = TokenIdentifier ;
    }

    
   
    return Token ;

}

/* unescape a character from an octal character constant */
unsigned char LexUnEscapeCharacterConstant(const char **From, const char *End, unsigned char FirstChar, int Base)
{
    unsigned char Total = GET_BASE_DIGIT(FirstChar);
    int CCount;
    for (CCount = 0; IS_BASE_DIGIT(**From, Base) && CCount < 2; CCount++, (*From)++)
        Total = Total * Base + GET_BASE_DIGIT(**From);
    
    return Total;
}

/* unescape a character from a string or character constant */
unsigned char LexUnEscapeCharacter(const char **From, const char *End)
{
    unsigned char ThisChar;
    
    while ( *From != End && **From == '\\' && 
            &(*From)[1] != End && (*From)[1] == '\n' )
        (*From) += 2;       /* skip escaped end of lines with LF line termination */
    
    while ( *From != End && **From == '\\' && 
            &(*From)[1] != End && &(*From)[2] != End && (*From)[1] == '\r' && (*From)[2] == '\n')
        (*From) += 3;       /* skip escaped end of lines with CR/LF line termination */
    
    if (*From == End)
        return '\\';
    
    if (**From == '\\')
    { 
        /* it's escaped */
        (*From)++;
        if (*From == End)
            return '\\';
        
        ThisChar = *(*From)++;
        switch (ThisChar)
        {
            case '\\': return '\\'; 
            case '\'': return '\'';
            case '"':  return '"';
            case 'a':  return '\a';
            case 'b':  return '\b';
            case 'f':  return '\f';
            case 'n':  return '\n';
            case 'r':  return '\r';
            case 't':  return '\t';
            case 'v':  return '\v';
            case '0': case '1': case '2': case '3': return LexUnEscapeCharacterConstant(From, End, ThisChar, 8);
            case 'x': return LexUnEscapeCharacterConstant(From, End, '0', 16);
            default:   return ThisChar;
        }
    }
    else
        return *(*From)++;
}

#if 0
/* get a string constant - used while scanning */
enum LexToken LexGetStringConstant(struct LexState *Lexer, struct Value *Value, char EndChar)
{
    int Escape = FALSE;
    const char *StartPos = Lexer->Pos;
    const char *EndPos;
    char *EscBuf;
    char *EscBufPos;
    //char *RegString;
    //struct Value *ArrayValue;
    //struct dlist * np ;
    
    while (Lexer->Pos != Lexer->End && (*Lexer->Pos != EndChar || Escape))
    { 
        /* find the end */
        if (Escape)
        {
            if (*Lexer->Pos == '\r' && Lexer->Pos+1 != Lexer->End)
                Lexer->Pos++;
            
            if (*Lexer->Pos == '\n' && Lexer->Pos+1 != Lexer->End)
            {
                Lexer->Line++;
                Lexer->Pos++;
                Lexer->CharacterPos = 0;
                //Lexer->EmitExtraNewlines++;
            }
            
            Escape = FALSE;
        }
        else if (*Lexer->Pos == '\\')
            Escape = TRUE;
            
        LEXER_INC(Lexer);
    }
    EndPos = Lexer->Pos;
    
    EscBuf = LEX_MALLOC(EndPos - StartPos);
    if (EscBuf == NULL) {
        if (Lexer->cb->Error) Lexer->cb->Error(Lexer, ErrorMemory,  "out of memory");
        return TokenError ;
    }
    
    for (EscBufPos = EscBuf, Lexer->Pos = StartPos; Lexer->Pos != EndPos;)
        *EscBufPos++ = LexUnEscapeCharacter(&Lexer->Pos, EndPos);
    
    /* try to find an existing copy of this string literal */
    //if (!__LexGetString (Lexer, EscBuf, EscBufPos - EscBuf, Value) ) {
    if (Lexer->cb->GetString &&
            !Lexer->cb->GetString (Lexer, EscBuf, EscBufPos - EscBuf, Value) ) {
        LEX_FREE (EscBuf) ;
        if (Lexer->cb->Error) Lexer->cb->Error(Lexer, ErrorMemory, "out of memory");
        return TokenError ;

    }


    
    LEX_FREE (EscBuf) ;

    if (*Lexer->Pos == EndChar)
        LEXER_INC(Lexer);
    
    return TokenStringConstant;
}
#else
/* get a string constant - used while scanning */
enum LexToken LexGetStringConstant(struct LexState *Lexer, struct Value *Value, char EndChar)
{
    char *AccBuf = NULL;          /* Accumulated buffer for the final merged string */
    size_t AccAllocated = 0;      /* How many bytes we've allocated for AccBuf so far */
    size_t AccUsed = 0;           /* How many bytes of AccBuf are actually used */

    for (;;)
    {
        /* --------- Parse a single quoted chunk --------- */
        int Escape = FALSE;
        const char *ChunkStart = Lexer->Pos;
        const char *ChunkEnd;

        while (Lexer->Pos != Lexer->End && (*Lexer->Pos != EndChar || Escape))
        {
            if (Escape)
            {
                if (*Lexer->Pos == '\r' && (Lexer->Pos+1) != Lexer->End)
                    Lexer->Pos++;
                
                if (*Lexer->Pos == '\n' && (Lexer->Pos+1) != Lexer->End)
                {
                    Lexer->Line++;
                    Lexer->Pos++;
                    Lexer->CharacterPos = 0;
                }
                Escape = FALSE;
            }
            else if (*Lexer->Pos == '\\')
            {
                Escape = TRUE;
            }
            LEXER_INC(Lexer);
        }

        /* If we never found a closing quote, bail out */
        if (Lexer->Pos == Lexer->End)
        {
            if (Lexer->cb->Error)
                Lexer->cb->Error(Lexer, ErrorSyntax, "unterminated string literal");
            if (AccBuf) LEX_FREE(AccBuf);
            return TokenError;
        }

        ChunkEnd = Lexer->Pos;  /* points to the quote character (or end) */

        /* Allocate a buffer to hold the unescaped chunk */
        /* The chunk length is (ChunkEnd - ChunkStart), but we allocate exactly that 
           because we'll do actual growth on AccBuf, not on this temp. */
        size_t rawLen = (size_t)(ChunkEnd - ChunkStart);
        char *EscBuf = (char*)LEX_MALLOC(rawLen);
        if (!EscBuf)
        {
            if (Lexer->cb->Error)
                Lexer->cb->Error(Lexer, ErrorMemory, "out of memory in string parse");
            if (AccBuf) LEX_FREE(AccBuf);
            return TokenError;
        }

        /* Unescape into EscBuf */
        {
            const char *p = ChunkStart;
            char *outp = EscBuf;
            while (p != ChunkEnd)
            {
                *outp++ = LexUnEscapeCharacter(&p, ChunkEnd);
            }
            rawLen = (size_t)(outp - EscBuf);  /* actual length after unescaping */
        }

        /* Now we append EscBuf into AccBuf. Let's do it the manual "expand if needed" way. */
        {
            size_t needed = AccUsed + rawLen + 1; /* +1 for the final null terminator */
            if (needed > AccAllocated)
            {
                /* We need to grow AccBuf. Let's pick a new size. Could just do `needed`,
                   or some doubling strategy, e.g. max(needed, AccAllocated*2) for fewer 
                   allocations over time. */
                size_t newAlloc = (AccAllocated == 0) ? needed : (AccAllocated * 2);
                if (newAlloc < needed)
                    newAlloc = needed;

                /* Malloc a bigger chunk, copy from old if any, then free old. */
                char *newBuf = (char*)LEX_MALLOC(newAlloc);
                if (!newBuf)
                {
                    if (Lexer->cb->Error)
                        Lexer->cb->Error(Lexer, ErrorMemory, "out of memory growing string buffer");
                    LEX_FREE(EscBuf);
                    if (AccBuf) LEX_FREE(AccBuf);
                    return TokenError;
                }

                /* copy existing data */
                if (AccBuf)
                {
                    memcpy(newBuf, AccBuf, AccUsed);
                    LEX_FREE(AccBuf);
                }

                AccBuf = newBuf;
                AccAllocated = newAlloc;
            }

            /* Now copy this chunk in */
            memcpy(AccBuf + AccUsed, EscBuf, rawLen);
            AccUsed += rawLen;
            AccBuf[AccUsed] = '\0';
        }

        /* free the temporary buffer for the chunk */
        LEX_FREE(EscBuf);

        /* Consume the closing quote if present */
        if (*Lexer->Pos == EndChar)
            LEXER_INC(Lexer);

        /* --------- Check if there's another adjacent string --------- */
        /* skip whitespace/newlines to see if there's another quote right after */
        while (Lexer->Pos < Lexer->End && isspace((unsigned char)*Lexer->Pos))
        {
            if (*Lexer->Pos == '\n')
            {
                Lexer->Line++;
                Lexer->CharacterPos = 0;
            }
            LEXER_INC(Lexer);
        }

        /* If next char != EndChar, we stop. */
        if (Lexer->Pos >= Lexer->End || *Lexer->Pos != EndChar)
            break;

        /* Otherwise, there's another adjacent string chunk. 
           Skip the opening quote and parse the next chunk in the loop. */
        LEXER_INC(Lexer);
    }

    /* We have the final, merged string in AccBuf. Time to call GetString(). */
    if (Lexer->cb->GetString && 
        !Lexer->cb->GetString(Lexer, AccBuf, AccUsed, Value))
    {
        LEX_FREE(AccBuf);
        if (Lexer->cb->Error)
            Lexer->cb->Error(Lexer, ErrorMemory, "out of memory storing final string");
        return TokenError;
    }

    /* done! free our local buffer if the callback didn't store it internally */
    LEX_FREE(AccBuf);

    return TokenStringConstant;
}
#endif

/* get a string constant - used while scanning */
enum LexToken LexGetIndexConstant(struct LexState *Lexer, struct Value *Value)
{
    const char *StartPos ;
    const char *EndPos;
    enum LexToken tok = TokenIndexConstant ;

    StartPos = Lexer->Pos;

    while (Lexer->Pos != Lexer->End && (*Lexer->Pos != ']'))
    {
        /* find the end */

        LEXER_INC(Lexer);
    }
    EndPos = Lexer->Pos;


    if (Lexer->cb->GetIndex &&
            !Lexer->cb->GetIndex (Lexer, tok, StartPos, EndPos - StartPos, Value) ) {
        if (Lexer->cb->Error) Lexer->cb->Error(Lexer, ErrorSyntax, "identifier or index expected");
        return TokenError ;

    }

    if (*Lexer->Pos == ']')
        LEXER_INC(Lexer);

    return tok ;
}

#if 0
/* get a string constant - used while scanning */
enum LexToken LexGetIndexString(struct LexState *Lexer, struct Value *Value)
{
    const char *StartPos ;
    const char *EndPos;
    enum LexToken tok = TokenIndexString ;

    StartPos = Lexer->Pos;

    while (Lexer->Pos != Lexer->End && (*Lexer->Pos != '>'))
    {
        /* find the end */

        LEXER_INC(Lexer);
    }
    EndPos = Lexer->Pos;

    if (Lexer->cb->GetIndex &&
            !Lexer->cb->GetIndex (Lexer, tok, StartPos, EndPos - StartPos, Value) ) {
        if (Lexer->cb->Error) Lexer->cb->Error(Lexer, ErrorMemory, "out of memory");
        return TokenError ;

    }

    if (*Lexer->Pos == '>')
        LEXER_INC(Lexer);

    return tok ;
}
#endif

/* get a character constant - used while scanning */
enum LexToken LexGetCharacterConstant(struct LexState *Lexer, struct Value *Value)
{
    Value->Typ = TypeChar ;// &pc->CharType;
    Value->Val.Character = LexUnEscapeCharacter(&Lexer->Pos, Lexer->End);
    if (Lexer->Pos != Lexer->End && *Lexer->Pos != '\'') {
        if (Lexer->cb->Error) Lexer->cb->Error(Lexer, ErrorUnexpected, "expected \"'\"");
        return TokenError; 
    }
        
    LEXER_INC(Lexer);
    return TokenCharacterConstant;
}

/* skip a comment - used while scanning */
void LexSkipComment(struct LexState *Lexer, char NextChar, enum LexToken *ReturnToken)
{
    if (NextChar == '*')
    {   
        /* conventional C comment */
        while (Lexer->Pos != Lexer->End && (*(Lexer->Pos-1) != '*' || *Lexer->Pos != '/'))
        {
        //    if (*Lexer->Pos == '\n')
        //        Lexer->EmitExtraNewlines++;
            if (*Lexer->Pos == '\n')
            {
                Lexer->Line++;
            }

            LEXER_INC(Lexer);
        }
        
        if (Lexer->Pos != Lexer->End)
            LEXER_INC(Lexer);
        
        //Lexer->Mode = LexModeNormal;
    }
    else
    {   
        /* C++ style comment */
        while (Lexer->Pos != Lexer->End && *Lexer->Pos != '\n')
            LEXER_INC(Lexer);
    }
}

/* get a single token from the source - used while scanning */
enum LexToken LexScanGetToken(struct LexState *Lexer, struct Value *Value)
{
    char ThisChar;
    char NextChar;
    enum LexToken GotToken = TokenNone;
    
    /* handle cases line multi-line comments or string constants which mess up the line count */
    /* scan for a token */
    do
    {
        //*Value = &pc->LexValue;
        Value->Typ = 0;
        Value->Val.UnsignedLongInteger = 0;
        Value->Id = 0 ;
         while (Lexer->Pos != Lexer->End && isspace((int)*Lexer->Pos))
        {
            if (*Lexer->Pos == '\n')
            {
                Lexer->Line++;
                Lexer->Pos++;
                //Lexer->Mode = LexModeNormal;
                Lexer->CharacterPos = 0;
                return TokenEndOfLine;
            }
#if 0
            else if (Lexer->Mode == LexModeHashDefine || Lexer->Mode == LexModeHashDefineSpace)
                Lexer->Mode = LexModeHashDefineSpace;
            
            else if (Lexer->Mode == LexModeHashDefineSpaceIdent)
                Lexer->Mode = LexModeNormal;
#endif    
            LEXER_INC(Lexer);
        }
        
        if (Lexer->Pos == Lexer->End || *Lexer->Pos == '\0')
            return TokenEOF;
        
        ThisChar = *Lexer->Pos;
        if (isCidstart((int)ThisChar))
            return LexGetWord(Lexer, Value);
        
        if (isdigit((int)ThisChar))
            return LexGetNumber(Lexer, Value);
        
        NextChar = (Lexer->Pos+1 != Lexer->End) ? *(Lexer->Pos+1) : 0;
        LEXER_INC(Lexer);
        switch (ThisChar)
        {
            case '"': GotToken = LexGetStringConstant(Lexer, Value, '"'); break;
            case '\'': GotToken = LexGetCharacterConstant(Lexer, Value); break;
            case '(': GotToken = TokenOpenBracket; break ; // if (Lexer->Mode == LexModeHashDefineSpaceIdent) GotToken = TokenOpenMacroBracket; else GotToken = TokenOpenBracket; Lexer->Mode = LexModeNormal; break;
            case ')': GotToken = TokenCloseBracket; break;
            case '=': NEXTIS('=', TokenEqual, TokenAssign); break;
            case '+': NEXTIS3('=', TokenAddAssign, '+', TokenIncrement, TokenPlus); break;
            case '-': NEXTIS4('=', TokenSubtractAssign, '>', TokenArrow, '-', TokenDecrement, TokenMinus); break;
            case '*': NEXTIS('=', TokenMultiplyAssign, TokenAsterisk); break;
            case '/': if (NextChar == '/' || NextChar == '*') { LEXER_INC(Lexer); LexSkipComment(Lexer, NextChar, &GotToken); } else NEXTIS('=', TokenDivideAssign, TokenSlash); break;
            case '%': NEXTIS('=', TokenModulusAssign, TokenModulus); break;
#if 1
            case '<': NEXTIS3PLUS('=', TokenLessEqual, '<', TokenShiftLeft, '=', TokenShiftLeftAssign, TokenLessThan); break ; //if (Lexer->Mode == LexModeHashInclude) GotToken = LexGetStringConstant(Lexer, Value, '>'); else { NEXTIS3PLUS('=', TokenLessEqual, '<', TokenShiftLeft, '=', TokenShiftLeftAssign, TokenLessThan); } break; 
            case '>': NEXTIS3PLUS('=', TokenGreaterEqual, '>', TokenShiftRight, '=', TokenShiftRightAssign, TokenGreaterThan); break;
#else
            case '<': GotToken = LexGetIndexString(Lexer, Value); break;
#endif
            case ';': GotToken = TokenSemicolon; break;
            case '&': NEXTIS3('=', TokenArithmeticAndAssign, '&', TokenLogicalAnd, TokenAmpersand); break;
            case '|': NEXTIS3('=', TokenArithmeticOrAssign, '|', TokenLogicalOr, TokenArithmeticOr); break;
            case '{': GotToken = TokenLeftBrace; break;
            case '}': GotToken = TokenRightBrace; break;
#if 0
            case '[': GotToken = TokenLeftSquareBracket; break;
            case ']': GotToken = TokenRightSquareBracket; break;
#else
            case '[': GotToken = LexGetIndexConstant(Lexer, Value); break;
#endif
            case '!': NEXTIS('=', TokenNotEqual, TokenUnaryNot); break;
            case '^': NEXTIS('=', TokenArithmeticExorAssign, TokenArithmeticExor); break;
            case '~': GotToken = TokenUnaryExor; break;
            case ',': GotToken = TokenComma; break;
            case '.': NEXTISEXACTLY3('.', '.', TokenEllipsis, TokenDot); break;
            case '?': GotToken = TokenQuestionMark; break;
            case ':': GotToken = TokenColon; break;
            //default:  __LexError(Lexer, ErrorUnexpected, , Lexer->Line, "illegal character '%c'", ThisChar); break;
            default:
                if (Lexer->cb->Error) Lexer->cb->Error(Lexer, ErrorUnexpected, "illegal character");
                GotToken = TokenError ;
                break;
        }
    } while (GotToken == TokenNone);
    
    return GotToken;
}


/* what size value goes with each token
int LexTokenSize(enum LexToken Token)
{
    switch (Token)
    {
        case TokenIdentifier: case TokenStringConstant: return sizeof(char *);
        case TokenIntegerConstant: return sizeof(long);
        case TokenCharacterConstant: return sizeof(unsigned char);
        case TokenFPConstant: return sizeof(double);
        default: return 0;
    }
}
 */

/* produce tokens from the lexer and return a heap buffer with the result - used for scanning */
enum LexToken LexTokenise(struct LexState *Lexer)
{
    enum LexToken Token = TokenNone ;
    struct Value GotValue;
    int status ;
     
    do
    { 
        /* store the token at the end of the stack area */
        Token = LexScanGetToken(Lexer, &GotValue);
        // printf ("{%s}\t%s\t0x%.4x\r\n", LexTokenName(Token), LexGetValue(&GotValue), GotValue.Id) ;
        //if ((status = __LexParseToken (Lexer, Token, &GotValue)) <= 0) {
        if (Lexer->cb->ParseToken &&
                (status = Lexer->cb->ParseToken (Lexer, Token, &GotValue)) <= 0) {
           
            Token = TokenError ;
            if (Lexer->cb->Error) {
                switch (status) {
                case ErrorAbort:
                    Lexer->cb->Error(Lexer, ErrorAbort, "abort");
                    break ;
                case ErrorSyntax:
                    Lexer->cb->Error(Lexer, ErrorSyntax, "syntax error");
                    break ;
                case ErrorUndeclared:
                   Lexer->cb->Error(Lexer, ErrorUndeclared, "undeclared");
                   break ;
                case ErrorRedeclared:
                   Lexer->cb->Error(Lexer, ErrorUndeclared, "redeclared");
                   break ;
                case ErrorParser:
                   Lexer->cb->Error(Lexer, ErrorParser, "parser");
                   break ;
                case ErrorMemory:
                   Lexer->cb->Error(Lexer, ErrorParser, "memory");
                   break ;
                case ErrorUnexpected:
                   Lexer->cb->Error(Lexer, ErrorParser, "unexpected");
                   break ;
                default:
                    Lexer->cb->Error(Lexer, ErrorParser, "undefined");
                    break ;

                }
            }


        }
#ifdef DEBUG_LEXER
        printf("Token: %02x\n", Token);
#endif
                
    } while ((Token != TokenEOF) && (Token != TokenError));
    
   
    return Token;
}

/* lexically analyse some source text */
enum LexToken  LexAnalyse(struct LexState * Lexer, const char *Source, int SourceLen, int Line)
{
    //struct LexState Lexer;
    
    Lexer->Pos = Source;
    Lexer->End = Source + SourceLen;
    Lexer->Line = Line;
    // Lexer.Mode = LexModeNormal;
    //Lexer.EmitExtraNewlines = 0;
    Lexer->CharacterPos = 1;
    //Lexer.SourceText = Source;
    
    return LexTokenise(Lexer);
}

const char* LexGetValue(struct Value * value, char* val, int len)
{
    //static char val[64] ;
       if ((value->Typ == TypeIdentifier) ||
            (value->Typ == TypeCharPointer)) {
            return value->Val.Identifier ;
       }


       switch (value->Typ) {
    case TypeInt:
    case TypeShort:
    case TypeLong:
        snprintf(val, len, "%d", value->Val.Integer) ;
        break ;
    case TypeUnsignedInt:
    case TypeUnsignedShort:
    case TypeUnsignedLong:
        snprintf(val, len, "%u", value->Val.UnsignedInteger) ;
        break ;

    case TypeUnsignedChar:
            case TypeChar:
        snprintf(val, len, "%c", value->Val.Character) ;
        break ;
    default:
        val[0] = '\0' ;
        break ;


       }

       return val ;
}



#if 0 /* useful for debug */
const char* LexTokenName(enum LexToken Token)
{
    const char* TokenNames[] = {
        /* 0x00 */ "None", 
        /* 0x01 */ "Comma",
        /* 0x02 */ "Assign", "AddAssign", "SubtractAssign", "MultiplyAssign", "DivideAssign", "ModulusAssign",
        /* 0x08 */ "ShiftLeftAssign", "ShiftRightAssign", "ArithmeticAndAssign", "ArithmeticOrAssign", "ArithmeticExorAssign",
        /* 0x0d */ "QuestionMark", "Colon", 
        /* 0x0f */ "LogicalOr", 
        /* 0x10 */ "LogicalAnd", 
        /* 0x11 */ "ArithmeticOr", 
        /* 0x12 */ "ArithmeticExor", 
        /* 0x13 */ "Ampersand", 
        /* 0x14 */ "Equal", "NotEqual", 
        /* 0x16 */ "LessThan", "GreaterThan", "LessEqual", "GreaterEqual",
        /* 0x1a */ "ShiftLeft", "ShiftRight", 
        /* 0x1c */ "Plus", "Minus", 
        /* 0x1e */ "Asterisk", "Slash", "Modulus",
        /* 0x21 */ "Increment", "Decrement", "UnaryNot", "UnaryExor", "Sizeof", "Cast",
        /* 0x27 */ "LeftSquareBracket", "RightSquareBracket", "Dot", "Arrow", 
        /* 0x2b */ "OpenBracket", "CloseBracket",
        /* 0x2d */ "Identifier", "IntegerConstant", "FPConstant", "StringConstant", "CharacterConstant",
        /* 0x32 */ "Semicolon", "Ellipsis",
        /* 0x34 */ "LeftBrace", "RightBrace",
#if 0
        /* 0x36 */ "IntType", "CharType", "FloatType", "DoubleType", "VoidType", "EnumType",
        /* 0x3c */ "LongType", "SignedType", "ShortType", "StaticType", "AutoType", "RegisterType", "ExternType", "StructType", "UnionType", "UnsignedType", "Typedef",
        /* 0x46 */ "Continue", "Do", "Else", "For", "Goto", "If", "While", "Break", "Switch", "Case", "Default", "Return",
        /* 0x52 */ "HashDefine", "HashInclude", "HashIf", "HashIfdef", "HashIfndef", "HashElse", "HashEndif",
        /* 0x59 */ "New", "Delete",
        /* 0x5b */ "OpenMacroBracket",
#endif
        /* 0x5c */ "EOF", "EndOfLine", "EndOfFunction",
#if 0
        /* 0x5f */ "Events", "Statemachine",  "State",   "Defautlt", "Enter",   "Event",   "Exit",   "Deferred",   "Previous", 
#endif

         "Error!"

    };
    //printf("{%s}", TokenNames[Token]);
    return TokenNames[Token] ;
}
#endif
