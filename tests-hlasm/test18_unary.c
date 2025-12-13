/*
 * Test 18: Unary operators
 * Tests: negation, logical not, increment, decrement
 */

int main() {
	int a, b, c;
	
	a = 5;
	b = -a;       /* -5 */
	c = !b;       /* 0 (b is non-zero) */
	
	a = 10;
	a = a + 1;    /* 11 (increment) */
	a = a - 1;    /* 10 (decrement) */
	
	return a + b + c;  /* 10 + (-5) + 0 = 5 */
}
