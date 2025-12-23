/*
 *	NMH's Simple C Compiler, 2011--2025
 *	AST operator definitions
 */

#ifndef OPS_H
#define OPS_H

/* AST operators */
enum {
	OP_GLUE, OP_ADD, OP_ADDR, OP_ASSIGN, OP_BINAND, OP_BINIOR,
	OP_BINXOR, OP_BOOL, OP_BRFALSE, OP_BRTRUE, OP_CALL, OP_CALR,
	OP_COMMA, OP_DEC, OP_DIV, OP_EQUAL, OP_GREATER, OP_GTEQ,
	OP_IDENT, OP_IFELSE, OP_LAB, OP_LDLAB, OP_LESS, OP_LIT,
	OP_LOGNOT, OP_LSHIFT, OP_LTEQ, OP_MOD, OP_MUL, OP_NEG,
	OP_NOT, OP_NOTEQ, OP_PLUS, OP_PREDEC, OP_PREINC, OP_POSTDEC,
	OP_POSTINC, OP_RSHIFT, OP_RVAL, OP_SCALE, OP_SCALEBY, OP_SUB,

	/* Floating-point operators */
	OP_FLIT,        /* float literal */
	OP_DLIT,        /* double literal */
	OP_FADD,        /* float add */
	OP_FSUB,        /* float subtract */
	OP_FMUL,        /* float multiply */
	OP_FDIV,        /* float divide */
	OP_FNEG,        /* float negate */
	OP_DADD,        /* double add */
	OP_DSUB,        /* double subtract */
	OP_DMUL,        /* double multiply */
	OP_DDIV,        /* double divide */
	OP_DNEG,        /* double negate */
	OP_ITOF,        /* int to float */
	OP_ITOD,        /* int to double */
	OP_FTOI,        /* float to int */
	OP_DTOI,        /* double to int */
	OP_FTOD,        /* float to double */
	OP_DTOF,        /* double to float */
	OP_FEQ,         /* float equal */
	OP_FNE,         /* float not equal */
	OP_FLT,         /* float less than */
	OP_FGT,         /* float greater than */
	OP_FLE,         /* float less or equal */
	OP_FGE,         /* float greater or equal */
	OP_DEQ,         /* double equal */
	OP_DNE,         /* double not equal */
	OP_DLT,         /* double less than */
	OP_DGT,         /* double greater than */
	OP_DLE,         /* double less or equal */
	OP_DGE          /* double greater or equal */
};

#endif /* OPS_H */
