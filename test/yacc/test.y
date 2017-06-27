%{
#include <stdio.h>
#include <string>
#define EXPR_IDENT 1
#define EXPR_OP 2
#deifne EXPR_DIGITAL 3
struct Expr {
	~Expr() {
		if (lexpr) {
			delete lexpr;
			lexpr = NULL;
		}
		if (rexpr) {
			delete rexpr;
			rexpr = NULL;
		}
	}
	int kind;
	std::string name;
	int ival;
	struct Expr* lexpr;
	struct Expr* rexpr;
};

%}
%pure-parser
%expect 0
%locations
%define api.prefix {base_yy}

%union {
	int ival;
	std::string str;
	const char* keyword;
	struct Expr* expr;
}

%type <expr> expr
%token <ival> NUM
%token <str> IDENT
//%token <keyword> IF ELSE THEN PRINT
%left '+' '-' '='
%%

expr :
	IDENT				
	{ 
		struct Expr* n = new struct Expr();
		n->kind = EXPR_IDENT;
		n->name = $1;
		n->lexpr = NULL;
		n->rexpr = NULL;
		$$ = n; 
	}
	| NUM
	{
		struct Expr* n = new struct Expr();
		n->kind = EXPR_DIGITAL;
		n->ival = $1;
		n->lexpr = NULL;
		n->rexpr = NULL;
		$$ = n; 
	}
	| expr '+' expr     
	{
		struct Expr* n = new struct Expr();
		n->kind = EXPR_OP;
		n->name = "+";
		n->lexpr = $1;
		n->rexpr = $3;
		$$ = n;
	}
	| expr '-' expr
	{
		struct Expr* n = new struct Expr();
		n->kind = EXPR_OP;
		n->name = "-";
		n->lexpr = $1;
		n->rexpr = $3;
		$$ = n;
	}
	| IDENT '=' expr
	{
		struct Expr* lexpr = new struct Expr();
		lexpr->kind = EXPR_IDENT;
		lexpr->name = $1;
		lexpr->lexpr = NULL;
		lexpr->rexpr = $3;
		
		struct Expr* n = new struct Expr();
		n->kind = EXPR_OP;
		n->name = "=";
		n->lexpr = lexpr;
		n->rexpr = $3;
		$$ = n;
	}
	;


%%