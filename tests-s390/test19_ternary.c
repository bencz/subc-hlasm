/*
 * Test 19: Ternary operator
 * Tests: conditional expression
 */

int max(int a, int b) {
	return (a > b) ? a : b;
}

int min(int a, int b) {
	return (a < b) ? a : b;
}

int main() {
	int x, y;
	
	x = max(10, 20);  /* 20 */
	y = min(10, 20);  /* 10 */
	
	return x - y;  /* 10 */
}
