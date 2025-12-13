/*
 * Test 06: If-else statements
 * Tests: conditional branching
 */

int main() {
	int x, result;
	
	x = 5;
	
	if (x > 0) {
		result = 1;
	}
	else {
		result = -1;
	}
	
	if (x == 5) {
		result = result + 10;
	}
	
	return result;
}
