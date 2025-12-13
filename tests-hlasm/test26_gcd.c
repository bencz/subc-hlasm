/*
 * Test 26: GCD (Greatest Common Divisor)
 * Tests: Euclidean algorithm, recursion
 */

int gcd(int a, int b) {
	if (b == 0)
		return a;
	return gcd(b, a % b);
}

int lcm(int a, int b) {
	return (a * b) / gcd(a, b);
}

int main() {
	int g, l;
	
	g = gcd(48, 18);   /* 6 */
	l = lcm(4, 6);     /* 12 */
	
	return g + l;  /* 18 */
}
