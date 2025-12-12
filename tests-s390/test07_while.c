/*
 * Test 07: While loop
 * Tests: loop, increment
 */

int main() {
	int i, sum;
	
	i = 0;
	sum = 0;
	
	while (i < 10) {
		sum = sum + i;
		i = i + 1;
	}
	
	return sum;  /* 45 */
}
