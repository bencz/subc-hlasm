/*
 * Test 17: Break and continue
 * Tests: break, continue in loops
 */

int main() {
	int i, sum;
	
	sum = 0;
	
	for (i = 0; i < 20; i = i + 1) {
		if (i == 10)
			break;
		if (i % 2 == 0)
			continue;
		sum = sum + i;
	}
	
	return sum;  /* 1+3+5+7+9 = 25 */
}
