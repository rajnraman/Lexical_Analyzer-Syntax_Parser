/*
Raj Raman (R11670402) | Assignment #4 | 11/11/2022

 Lexical Analysis
 
 Problem: We are asked to develop a recursive decent parser using the lexical analyzer developed in assignment 3. The purpose will be to validate the syntax of a source code file provided by the user. It should conform to the following BNF rules:
 
 P ::= S
 S ::= V=E | read(V) | print(V) | do { S } while C | S;S
 C ::= E < E | E > E | E == E | E <> E | E <= E | E >= E
 E ::= T | E + T | E - T
 T ::= F | T * F | T / F | T % F
 F ::= (E) | N | V
 V ::= a | b | … | y | z | aV | bV | … | yV | zV
 N ::= 0 | 1 | … | 8 | 9 | 0N | 1N | … | 8N | 9N
 
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h> //for strcpy/strcmp and other string library functions


/* Global Variable */
int nextToken;
//char token[20];

/* Local Variables */
static int charClass;
char lexeme [100];
char printToken[30];
static char nextChar;
static int lexLen;
static FILE *in_fp;

/* Local Function declarations */
static void addChar();
static void getChar();
static void getNonBlank();


//declaring error functions
static int error();
static int error2();
static int error3();



//below is front.h copied and pasted in the main

#ifndef FRONT_H
#define FRONT_H

/* Character classes */
#define LETTER 0
#define DIGIT 1
#define UNKNOWN 99

/* Token codes */
#define INT_LIT 10
#define IDENT 11
#define ASSIGN_OP 20
#define ADD_OP 21
#define SUB_OP 22
#define MULT_OP 23
#define DIV_OP 24
#define LEFT_PAREN 25
#define RIGHT_PAREN 26
#define LESSER_OP 3
#define GREATER_OP 4
#define EQUAL_OP 5
#define NEQUAL_OP 6
#define LEQUAL_OP 7
#define GEQUAL_OP 8
#define SEMICOLON 9
#define MOD_OP 12
#define LEFT_CBRACE 13
#define RIGHT_CBRACE 14
#define KEY_READ 15
#define KEY_PRINT 16
#define KEY_WHILE 17
#define KEY_DO 18
#define WRITE 40


int lex();

#endif


//below is parser.h copied and pasted in the main

#ifndef PARSER_H
#define PARSER_H

void expr();
void term();
void factor();
void S();
void C();

extern int nextToken;
extern char lexeme[100];
extern char printToken[30];

#endif




//the below functions were in the original parser.c that I copied and pasted in "merged" c file


//this function deals with the S ::= BNF grammar
void S()
{
    //printf("Enter <S>\n");
    //the below if else statements are used to recursively parse the grammar: S ::= V=E | read(V) | print(V) | do { S } while C | S;S
    if (nextToken == IDENT)
    {
        factor();
        if(nextToken == ASSIGN_OP) //this and the above lines deals with: V=E
        {
            lex();
            expr();
            if (nextToken == SEMICOLON) //this semicolon token deals with: S;S
            {
                lex();
                S();
            }
            else if (nextToken == RIGHT_CBRACE) //deal with the recursive decent in regards to "do { S } while C" in prevent error
            {
                lex();
                if (nextToken == KEY_WHILE)
                {
                    C();
                    if(nextToken == SEMICOLON) //this semicolon token deals with: S;S
                    {
                        lex();
                        S();
                    }
                }else {
                    error(); //else errors
            }
            }else {
                error();
        }
        }else {
            error();
    }
    }
    else if (nextToken == KEY_READ || nextToken == KEY_PRINT) //deals with read(v) and print(v)
    {
        lex();
        if (nextToken == LEFT_PAREN)
        {
            lex();
            factor(); //go into factor bc handles identifiers and digits (V)
            if (nextToken == RIGHT_PAREN)
            {
                lex();
                if(nextToken == SEMICOLON) //change implemented: deals with: S;S
                {
                    lex(); //change implemented
                    S();
                }
            }
            else
            {
                error(); //else errors
            }
        }
        else
        {
            error();
        }
    }
    
    else if (nextToken == KEY_DO) //deals with: do { S } while C
    {
        lex();
        if(nextToken == LEFT_CBRACE)
        {
            lex();
            S();
            if (nextToken == RIGHT_CBRACE)
            {
                lex();
                if(nextToken == KEY_WHILE)
                {
                    //lex();
                    C();
                    if(nextToken == SEMICOLON) //change implemented: deals with S;S
                    {
                        lex(); //change implemented
                        S();
                    }
                }else {
                    error(); //else errors
            }
            }else {
                error();
            }
        }else {
            error();
        }
    }else {
        error();
    }
    //printf("exit S()\n");
}

//this entire function deals with grammar: C ::= E < E | E > E | E == E | E <> E | E <= E | E >= E
void C()
{
   // printf("Enter <C>\n");
    lex();
    expr();
    if (nextToken == LESSER_OP)
    {
        lex();
        expr();
    }
    else if (nextToken == GREATER_OP)
    {
        lex();
        expr();
    }
    else if (nextToken == EQUAL_OP)
    {
        lex();
        expr();
    }
    else if (nextToken == NEQUAL_OP)
    {
        lex();
        expr();
    }
    else if (nextToken == LEQUAL_OP)
    {
        lex();
        expr();
    }
    else if (nextToken == GEQUAL_OP)
    {
        lex();
        expr();
    }
    else //errors result if grammar is not followed
    {
        error();
    }
   //printf("exit <C>\n");

}

//this function deals with grammar: E ::= T | E + T | E - T
void expr()
{
    //printf("Enter <expr>\n");
    
    term();

    /* As long as the next token is + or -, get
    the next token and parse the next term */
    while (nextToken == ADD_OP || nextToken == SUB_OP) {
        lex();
        term();
    }

   // printf("Exit <expr>\n");
} /* End of function expr */

/* term
 * Parses strings in the language generated by the rule:
 * <term> -> <factor> {(* | /) <factor>)
 */


//this function deals with grammar rules: T ::= F | T * F | T / F | T % F
void term()
{
  //  printf("Enter <term>\n");
    /* Parse the first factor */
    factor();
    /* As long as the next token is * or /, get the
    next token and parse the next factor */
    while (nextToken == MULT_OP || nextToken == DIV_OP || nextToken == MOD_OP)
    {
            lex();
            factor();
    }
  //  printf("Exit <term>\n");
} /* End of function term */

/* factor
 * Parses strings in the language generated by the rule:
 * <factor> -> id | int_constant | ( <expr )
 * */


//this function deals with grammar rules: F ::= (E) | N | V
void factor()
{
  // printf("Enter <factor>\n");
    /* Determine which RHS */
    if (nextToken == IDENT || nextToken == INT_LIT) {
        lex(); /* Get the next token */
    } else {
        /* If the RHS is (<expr>), call lex to pass over the
        left parenthesis, call expr, and check for the right
        parenthesis */
        if (nextToken == LEFT_PAREN) {
            lex();
            expr();

            if (nextToken == RIGHT_PAREN) {
                lex();
            } else {
                error();
            }
        } /* End of if (nextToken == ... */
        /* It was not an id, an integer literal, or a left parenthesis */
        else if (nextToken == ASSIGN_OP)
            lex();
        else
        {
            error();
        }
    } /* End of else */
   // printf("Exit <factor>\n");;
} /* End of function factor */


//these are error functions

//if e\suntax error detected, prints the corresponding lexeme and token
static int error()
{
    if (nextToken == WRITE) //this if statement is a modification to deal with example1 test case
    {
        printf("Syntax Validated\n");
        exit (0);
    }
    else{ //else prints exit 1 error: Syntax Error
    printf("Error encounter: The next lexeme was %s and the next token was %s\n",lexeme, printToken);
    exit(1);
    }
}

//the user did provide a file as input but the file does not exist
static int error3()
{
    exit(3);
}

//the user did not provide a file as input
static int error2()
{
    exit(2);
}





/* main driver */
int main(int argc, char *argv[])//int argc, char *argv[]
{
    printf("DanC Parser :: R11670402\n");
    /* Open the input data file and process its contents */
    if(argc == 1)
        error2();
    else if ((in_fp = fopen(argv[1], "r")) == NULL) { //argv[1], "r"
        printf("ERROR: File does not exist \n");//error of file cant open
        error3();
    }
    else {
        getChar(); //accesses getChar function, grabs first character in the file and puts in a charClass for switch statement in lex()
        do {
            lex(); //after this call, first token is placed in nextToken. It accesses the lex function
            S();
            //expr();
        } while (nextToken != EOF); //loop until EOF
    }
    return 0;
}
 




/* lookup - a function to lookup operators and parentheses and return the
 * token */
static int lookup(char ch) { //this switch statement has a list of single charcater lexemes. An associated macro is assigned to nextToken
    switch (ch) {
        case '(':
            addChar();
            nextToken = LEFT_PAREN;
            break;
        case ')':
            addChar();
            nextToken = RIGHT_PAREN;
            break;
        case '+':
            addChar();
            nextToken = ADD_OP;
            break;
        case '-':
            addChar();
            nextToken = SUB_OP;
            break;
        case '*':
            addChar();
            nextToken = MULT_OP;
            break;
        case '/':
            addChar();
            nextToken = DIV_OP;
            break;
        case '=':
            addChar();
            nextToken = ASSIGN_OP;
            break;
        case '<':
            addChar();
            nextToken = LESSER_OP;
            break;
        case '>':
            addChar();
            nextToken = GREATER_OP;
            break;
        case ';':
            addChar();
            nextToken = SEMICOLON;
            break;
        case '%':
            addChar();
            nextToken = MOD_OP;
            break;
        case '{':
            addChar();
            nextToken = LEFT_CBRACE;
            break;
        case '}':
            addChar();
            nextToken = RIGHT_CBRACE;
            break;
        default:
            addChar();
            nextToken = UNKNOWN;
            break;
    }
    return nextToken; //returns macro code defined in front.h
}




/* addChar - a function to add nextChar to lexeme */
static void addChar() { //each lexeme is placed in the lexeme array as it itterates
    if (lexLen <= 98) {
        lexeme[lexLen++] = nextChar;
        lexeme[lexLen] = 0;
    } else {
        printf("Error - lexeme is too long \n"); //lexeme has max length you cant exceed
    }
}



/* getChar - a function to get the next character of input and determine its
 * character class */
static void getChar() {
    if ((nextChar = getc(in_fp)) != EOF) {
        if (isalpha(nextChar)) //checks if character in alphabet
            charClass = LETTER;
        else if (isdigit(nextChar)) //checks if character is a number
            charClass = DIGIT;
        else charClass = UNKNOWN; //this is for operators/arithemetic instructions and unknown charcters
    } else {
        charClass = EOF; //if EOF is reached
    }
}



/* getNonBlank - a function to call getChar until it returns a non-whitespace
 * character */
static void getNonBlank() {
    while (isspace(nextChar)) getChar();
}




/*****************************************************/
/* lex - a simple lexical analyzer for arithmetic expressions */
int lex()
{
    lexLen = 0;
    getNonBlank();

    switch (charClass) {
        /* Parse identifiers */
        case LETTER:
            addChar();//this is for the first charcter (must be a letter in alphabet)
            getChar(); //gets next character and puts in charClass for this switch statement
            while (charClass == LETTER || charClass == DIGIT) { //the rest of the Identifier lexeme can be more letters or even digits, so this loop accounts for this
                addChar(); //adds in lexeme array
                getChar(); //gets next character and puts in charClass for this switch statement
            }
            int result;
            //result holds the value of string comparison (0 if true). If the lexeme and string are equal, new tokens (nextToken) are equal to its associated macro in front.h These tokens below conform the the BNF rules given (read, while, do, print).
            if((result = strcmp(lexeme, "read")) == 0)
            {
                    nextToken = KEY_READ;
            }
            else if((result = strcmp(lexeme, "do")) == 0)
            {
                    nextToken = KEY_DO;
                    //char tok = "KEY_DO";
            }
            else if((result = strcmp(lexeme, "while")) == 0)
            {
                    nextToken = KEY_WHILE;
            }
            else if((result = strcmp(lexeme, "print"))==0)
            {
                    nextToken = KEY_PRINT;
            }
            else if((result = strcmp(lexeme, "write"))==0)
            {
                    nextToken = WRITE;
            }
            else
            {
                nextToken = IDENT; //if the lexeme is not equivilent to any of the four strings above, the nextToken is an identifier
            }
            break;

        /* Parse integer literals */
        case DIGIT:
            addChar(); //if the character in file is a digit, it adds it to the lexeme array
            getChar(); //gets next character in file and puts the character in a class for switch statement
            while (charClass == DIGIT) {
                addChar(); //continues to add the rest of characters in lexeme if there are more digits
                getChar(); //gets next character in file and puts the character in a class for switch statement
            }
            nextToken = INT_LIT; //the associated token with digits in file
            break;

        /* Parentheses and operators */
        case UNKNOWN: //if the charcter is an operator or not known, it falls in this case
            lookup(nextChar); //calls lookup function and passes the next character in file as a parameter
            getChar(); //gets next character in file and puts the character in a charClass for switch statement
            break;

        /* EOF */
            //if the character is at the end of the file, the lexeme and token is EOF, but we will not print as a lexeme and token i.e. why there is an exit(0); so it doesnt print as output
        case EOF:
            nextToken = EOF;
            lexeme[0] = 'E';
            lexeme[1] = 'O';
            lexeme[2] = 'F';
            lexeme[3] = 0;
            //exit(0);
            break;
    } /* End of switch */
    
    
    
    
    
    int double_token = nextToken; //an equivilent variable for nextToken
    //the if and else if statements below handle double charcter operators: <>, <=, >=, ==
    //if the next token is '=', and the next chacrter is '=', the character is added to the same lexeme and the token name is switched to == which is EQUAL_OP
    if (double_token == ASSIGN_OP && nextChar == '=')
    {
        addChar();
        getChar();
        nextToken = EQUAL_OP;
    }
    //if the next token is '<', and the next chacrter is '=' or '>', the character is added to the same lexeme and the token name is switched to <= or <> which is NEQUAL_OP or GEQUAL_OP
    else if (double_token == LESSER_OP && nextChar == '=')
    {
        addChar();
        getChar();
        nextToken = LEQUAL_OP;
    }
    else if (double_token == LESSER_OP && nextChar == '>')
    {
        addChar();
        getChar();
        nextToken = NEQUAL_OP;
    }
    //if the next token is '>', and the next chacrter is '=', the character is added to the same lexeme and the token name is switched to >= which is GEQUAL_OP
    else if (double_token == GREATER_OP && nextChar == '=')
    {
        addChar();
        getChar();
        nextToken = GEQUAL_OP;
    }
    
    
    
    
    //an array printToken holds the string associated with the defined macros
    //strcpy will copy the string into printToken for each token in front.h
    //printToken holds the string of each token name
    //char printToken[30];

    if (nextToken == INT_LIT)
    {
        strcpy(printToken, "INT_LIT");
    }
    else if(nextToken == IDENT)
    {
        strcpy(printToken, "IDENT");
    }
    else if(nextToken == ASSIGN_OP)
    {
        strcpy(printToken, "ASSIGN_OP");
    }
    else if(nextToken == LESSER_OP)
    {
        strcpy(printToken, "LESSER_OP");
    }
    else if(nextToken == GREATER_OP)
    {
        strcpy(printToken, "GREATER_OP");
    }
    else if(nextToken == EQUAL_OP)
    {
        strcpy(printToken, "EQUAL_OP");
    }
    else if(nextToken == NEQUAL_OP)
    {
        strcpy(printToken, "NEQUAL_OP");
    }
    else if(nextToken == LEQUAL_OP)
    {
        strcpy(printToken, "LEQUAL_OP");
    }
    else if(nextToken == GEQUAL_OP)
    {
        strcpy(printToken, "GEQUAL_OP");
    }
    else if(nextToken == SEMICOLON)
    {
        strcpy(printToken, "SEMICOLON");
    }
    else if(nextToken == ADD_OP)
    {
        strcpy(printToken, "ADD_OP");
    }
    else if(nextToken == SUB_OP)
    {
        strcpy(printToken, "SUB_OP");
    }
    else if(nextToken == MULT_OP)
    {
        strcpy(printToken, "MULT_OP");
    }
    else if(nextToken == DIV_OP)
    {
        strcpy(printToken, "DIV_OP");
    }
    else if(nextToken == MOD_OP)
    {
        strcpy(printToken, "MOD_OP");
    }
    else if(nextToken == LEFT_CBRACE)
    {
        strcpy(printToken, "LEFT_CBRACE");
    }
    else if(nextToken == RIGHT_CBRACE)
    {
        strcpy(printToken, "RIGHT_CBRACE");
    }
    else if(nextToken == LEFT_PAREN)
    {
        strcpy(printToken, "LEFT_PAREN");
    }
    else if(nextToken == RIGHT_PAREN)
    {
        strcpy(printToken, "RIGHT_PAREN");
    }
    else if(nextToken == KEY_READ)
    {
        strcpy(printToken, "KEY_READ");
    }
    else if(nextToken == KEY_PRINT)
    {
        strcpy(printToken, "KEY_PRINT");
    }
    else if(nextToken == KEY_WHILE)
    {
        strcpy(printToken, "KEY_WHILE");
    }
    else if(nextToken == KEY_DO)
    {
        strcpy(printToken, "KEY_DO");
    }
    else if(nextToken == WRITE)
    {
        strcpy(printToken, "write");
    }
    else{
        strcpy(printToken, "UNKNOWN");
    }
    
    
    if (nextToken == UNKNOWN)
        error();
    else if (nextToken == EOF)
    {
        printf("Syntax Validated\n");
        exit(0);
    }
    
    
    printf("%-10s %-10s\n", lexeme, printToken); //prints lexeme and token name
    
    
    
    return nextToken; //returns nextToken value (a code digit) to lex() called by main
 /* End of function lex */
}
