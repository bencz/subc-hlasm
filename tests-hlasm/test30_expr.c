/*
 * Test 30: Complex expressions
 * Tests: operator precedence, nested expressions
 */

int main() {
	int a, b, c, d, r;
	
	a = 10;
	b = 20;
	c = 5;
	d = 2;
	
	/* Test operator precedence */
	r = a + b * c;           /* 10 + 100 = 110 */
	r = (a + b) * c;         /* 30 * 5 = 150 */
	r = a * b + c * d;       /* 200 + 10 = 210 */
	r = a + b - c + d;       /* 27 */
	r = a * b / c;           /* 40 */
	r = a % c + b % c;       /* 0 + 0 = 0 */
	
	/* Complex nested */
	r = ((a + b) * (c - d)) / d;  /* (30 * 3) / 2 = 45 */
	
	/* Logical expressions */
	r = (a > b) | (c < d);   /* 0 | 0 = 0 */
	r = (a < b) & (c > d);   /* 1 & 1 = 1 */
	r = (a == 10) + (b == 20) + (c == 5);  /* 3 */
	
	return r;  /* 3 */
}
