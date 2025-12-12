/*
 * Test 05: Comparison operations
 * Tests: eq, ne, lt, gt, le, ge
 */

int main() {
	int a, b, r;
	
	a = 10;
	b = 20;
	
	r = (a == b);   /* 0 */
	r = (a != b);   /* 1 */
	r = (a < b);    /* 1 */
	r = (a > b);    /* 0 */
	r = (a <= b);   /* 1 */
	r = (a >= b);   /* 0 */
	
	return r;
}
