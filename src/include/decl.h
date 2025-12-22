/*
 *	NMH's Simple C Compiler, 2011--2025
 *	Function declarations (Modular Version)
 *
 *	This header includes all function declarations from all modules.
 */

#ifndef DECL_ALL_H
#define DECL_ALL_H

#include "ops.h"

/* common/error.c */
void error(char *s, char *a);
void fatal(char *s);
void scnerror(char *s, int c);
int synch(int syn);

/* common/misc.c */
void init(void);
int chrpos(char *s, int c);
void copyname(char *name, char *s);
void match(int t, char *what);
void lparen(void);
void rparen(void);
void lbrace(void);
void rbrace(void);
void rbrack(void);
void semi(void);
void colon(void);
void ident(void);
int eofcheck(void);
int inttype(int p);
int comptype(int p);
void notvoid(int p);

/* frontend/lexer/scan.c */
int next(void);
void putback(int c);
int skip(void);
int scan(void);
int scanraw(void);
void reject(void);

/* frontend/preprocessor/prep.c - Modular preprocessor */
void playmac(char *s);
int getln(char *buf, int max);
int frozen(int depth);
void preproc(void);

/* frontend/parser/decl.c */
int primtype(int t, char *s);
int usertype(char *s);
int pointerto(int prim);
void decl(int clss, int prim, int utype);
void structdecl(int clss, int uniondecl);
void typedecl(void);
void top(void);
void defarg(char *s);
void program(char *name, FILE *in, FILE *out, char *def);

/* frontend/parser/stmt.c */
void compound(int lbr);

/* frontend/parser/expr.c */
int typematch(int p1, int p2);
int deref(int p);
void expr(int *lv, int cvoid);
void rexpr(void);
int const_expr(void);
int binop(int tok);
node *mkop(int op, int p1, int p2, node *l, node *r);
int arithop(int tok);

/* ast/tree.c */
node *mkleaf(int op, int n);
node *mkunop(int op, node *left);
node *mkunop1(int op, int n, node *left);
node *mkunop2(int op, int n1, int n2, node *left);
node *mkbinop(int op, node *left, node *right);
node *mkbinop1(int op, int n, node *left, node *right);
node *mkbinop2(int op, int n1, int n2, node *left, node *right);
void dumptree(node *a);
void emittree(node *a);

/* optimizer/opt.c */
void opt_init(void);
node *fold_reduce(node *n);
node *reorder_ops(node *n);
node *optimize(node *n);

/* symbols/sym.c */
int findglob(char *s);
int findloc(char *s);
int findsym(char *s);
int findmac(char *s);
int findstruct(char *s);
int findmem(int y, char *s);
char *galloc(int k, int align);
char *globname(char *s);
char *locname(char *s);
int addglob(char *name, int prim, int type, int scls, int size, int val,
		char *mtext, int init);
int addloc(char *name, int prim, int type, int scls, int size, int val,
		int init);
void clrlocs(void);
int objsize(int prim, int type, int size);
void dumpsyms(char *title, char *sub, int from, int to);

/* codegen/gen.c */
void clear(int q);
void load(void);
int label(void);
void spill(void);
void genraw(char *s);
void gen(char *s);
void ngen(char *s, char *inst, int n);
void ngen2(char *s, char *inst, int n, int a);
void lgen(char *s, char *inst, int n);
void lgen2(char *s, int v1, int v2);
void sgen(char *s, char *inst, char *s2);
void sgen2(char *s, char *inst, int v, char *s2);
void genlab(int id);
char *labname(int id);
char *gsym(char *s);
void gendata(void);
void gentext(void);
void genprelude(void);
void genpostlude(void);
void genname(char *name);
void genfuncname(char *name);
void genpublic(char *name);
void commit(void);
void commit_cmp(void);
void commit_bool(void);
void queue_cmp(int op);
void genaddr(int y);
void genldlab(int id);
void genlit(int v);
void genand(void);
void genior(void);
void genxor(void);
void genshl(int swapped);
void genshr(int swapped);
int genadd(int p1, int p2, int swapped);
int gensub(int p1, int p2, int swapped);
void genmul(void);
void gendiv(int swapped);
void genmod(int swapped);
int binoptype(int op, int p1, int p2);
void genbool(void);
void genlognot(void);
void genind(int p);
void genneg(void);
void gennot(void);
void genscale(void);
void genscale2(void);
void genscaleby(int v);
void genjump(int dest);
void genbrfalse(int dest);
void genbrtrue(int dest);
void gencall(int y);
void gencalr(void);
void genentry(int lsize, int nparams);
void genexit(void);
void genpush(void);
void genpushlit(int n);
void genstack(int n);
void genlocinit(void);
void genbss(char *name, int len, int statc);
void genalign(int k);
void genaligntext(void);
void gendefb(int v);
void gendefp(int v);
void gendefs(char *s, int len);
void gendefw(int v);
void geninc(int *lv, int inc, int pre);
void genswitch(int *vals, int *labs, int nc, int dflt);
void genstore(int *lv);
void genrval(int *lv);

/* main.c */
void cleanup(void);
char *newfilename(char *file, int sfx);

#endif /* DECL_ALL_H */
