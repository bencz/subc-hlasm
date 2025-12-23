/*
 * Test file for C89 type support in SubC
 */

/* Test type declarations */
char c;
signed char sc;
unsigned char uc;

short s;
short int si;
signed short ss;
signed short int ssi;
unsigned short us;
unsigned short int usi;

int i;
signed x;
signed int sx;
unsigned u;
unsigned int ui;

long l;
long int li;
signed long sl;
signed long int sli;
unsigned long ul;
unsigned long int uli;

/* Pointers to new types */
short *sp;
unsigned int *uip;
long *lp;
unsigned long *ulp;

/* Test const (should be ignored as no-op) */
const int ci;
const char *ccp;

/* Test function with new types */
short add_shorts(short a, short b) {
    return a + b;
}

unsigned long mul_ulong(unsigned long a, unsigned long b) {
    return a * b;
}

int main(void) {
    short local_short;
    unsigned int local_uint;
    long local_long;
    
    local_short = 100;
    local_uint = 50000;
    local_long = 100000;
    
    s = add_shorts(10, 20);
    ul = mul_ulong(1000, 2000);
    
    return 0;
}
