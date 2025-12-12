%token IDENT INTLIT STRLIT __ARGC SIZEOF INT CHAR VOID
%token SIZEOF BREAK CONTINUE DO WHILE FOR IF ELSE SWITCH
%token RETURN CASE DEFAULT ENUM EXTERN STATIC

%%

start:
         top
       | top start

primary :
         IDENT
       | INTLIT
       | string 
       | __ARGC
       | '(' expr ')'

string :
         STRLIT
       | STRLIT string

fnargs : 
         asgmnt
       | asgmnt ',' fnargs

postfix :
         primary
       | postfix '[' expr ']'
       | postfix '(' ')'
       | postfix '(' fnargs ')'
       | postfix '++'
       | postfix '--'

prefix : 
         postfix
       | '++' prefix 
       | '--' prefix  
       | '&' cast
       | '*' cast
       | '+' cast
       | '-' cast
       | '~' cast
       | '!' cast
       | SIZEOF '(' primtype ')'
       | SIZEOF '(' primtype '*' ')'
       | SIZEOF '(' primtype '*' '*' ')'
       | SIZEOF '(' INT '(' '*' ')' '(' ')' ')'
       | SIZEOF '(' IDENT ')'

primtype :
        INT
      | CHAR
      | VOID

cast :
         prefix
       | '(' primtype ')' prefix
       | '(' primtype '*' ')' prefix
       | '(' primtype '*' '*' ')' prefix
       | '(' INT '(' '*' ')' '(' ')' ')' prefix

term :
         cast  
       | term '*' cast
       | term '/' cast
       | term '%' cast

sum : 
         term
       | sum '+' term
       | sum '-' term

shift : 
         sum
       | shift '<<' sum
       | shift '>>' sum

relation :
         shift
       | relation '<' shift
       | relation '>' shift
       | relation '<=' shift
       | relation '>=' shift

equation :    
         relation
       | equation '==' relation
       | equation '!=' relation

binand :
         equation
       | binand '&' equation

binxor :
         binand
       | binxor '^' binand

binor : 
         binxor
       | binor '|' binxor

binexpr :
         binor

logand :
         binexpr
       | logand '&&' binexpr

logor : 
         logand
       | logor '||' logand

condexpr :
         logor  
       | logor '?' expr ':' condexpr

asgmnt :  
         condexpr
       | condexpr '=' asgmnt
       | condexpr '*=' asgmnt
       | condexpr '/=' asgmnt
       | condexpr '%=' asgmnt
       | condexpr '+=' asgmnt
       | condexpr '-=' asgmnt 
       | condexpr '<<=' asgmnt
       | condexpr '>>=' asgmnt
       | condexpr '&=' asgmnt
       | condexpr '^=' asgmnt
       | condexpr '|=' asgmnt

expr :
        asgmnt
      | asgmnt ',' expr

cfactor :
         INTLIT
       | IDENT 
       | '-' cfactor
       | '~' cfactor
       | '(' constexpr ')'

cterm :
         cfactor
       | cterm '*' cfactor
       | cterm '/' cfactor
       | cterm '%' cfactor

csum :
         cterm
       | csum '+' cterm
       | csum '-' cterm

cshift :
         csum
       | cshift '<<' csum
       | cshift '>>' csum

crelation :
         cshift
       | crelation '<' cshift
       | crelation '>' cshift
       | crelation '<=' cshift
       | crelation '>=' cshift

cequation :
         crelation
       | cequation '==' crelation
       | cequation '!=' crelation

cbinand :
         cequation
       | cbinand '&' cequation

cbinxor :
         cbinand
       | cbinxor '^' cbinand

cbinor :
         cbinxor
       | cbinor '|' cbinxor

constexpr :
         cbinor

compound :
         '{' stmt_list '}'
       | '{' '}'

stmt_list:
         stmt
       | stmt stmt_list

break_stmt    : BREAK ';'

continue_stmt : CONTINUE ';'

do_stmt : DO stmt WHILE '(' expr ')' ';'

for_stmt : FOR '(' opt_expr ';'
                  opt_expr ';'
                  opt_expr ')' stmt

opt_expr :
       | expr

if_stmt :
           IF '(' expr ')' stmt
         | IF '(' expr ')' stmt ELSE stmt

return_stmt :
           RETURN ';'
         | RETURN expr ';'

switch_stmt : SWITCH '(' expr ')' '{' switch_block '}'

switch_block :
         switch_block_stmt
       | switch_block_stmt switch_block

switch_block_stmt :
         CASE constexpr ':'
       | DEFAULT ':'
       | stmt

while_stmt : WHILE '(' expr ')' stmt

stmt :
         break_stmt
       | continue_stmt
       | do_stmt
       | for_stmt
       | if_stmt
       | return_stmt
       | switch_stmt
       | while_stmt
       | compound
       | ';'
       | expr ';'

enumdecl : ENUM '{' enumlist '}'

enumlist :
          enumerator
        | enumerator ',' enumlist

enumerator :
          IDENT
        | IDENT '=' constexpr

initlist : 
          '{' const_list '}'
        | STRLIT

const_list :
          constexpr
        | constexpr ',' const_list

pmtrdecls :
          '(' ')'
        | '(' pmtrlist ')'
        | '(' pmtrlist ',' '...' ')'

pmtrlist :
          opt_primtype declarator
        | opt_primtype declarator ',' pmtrlist

opt_primtype :
        | primtype

declarator :
          IDENT
        | '*' IDENT
        | '*' '*' IDENT
        | '*' IDENT '[' constexpr ']'
        | IDENT '[' constexpr ']'
        | IDENT '=' constexpr
        | IDENT '[' ']' '=' initlist
        | IDENT pmtrdecls
        | IDENT '[' ']'
        | '*' IDENT '[' ']'
        | '(' '*' IDENT ')' '(' ')'

localdecls : 
          ldecl
        | ldecl localdecls

ldecl :
          primtype ldecl_list ';'
        | STATIC ldecl_list ';'
        | STATIC primtype ldecl_list ';'

ldecl_list :
          declarator
        | declarator ',' ldecl_list

decl : 
          declarator '{' localdecls stmt_list '}'
        | decl_list ';'

decl_list :
          declarator
        | declarator ',' decl_list

top :  
          enumdecl
        | decl
        | primtype decl
        | storclass decl
        | storclass primtype decl

storclass :
          EXTERN
        | STATIC

