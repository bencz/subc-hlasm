/*
 * Test 10: Switch statement
 * Tests: switch, case, default, break
 */

int main() {
	int x, result;
	
	x = 2;
	
	switch (x) {
	case 1:
		result = 10;
		break;
	case 2:
		result = 20;
		break;
	case 3:
		result = 30;
		break;
	default:
		result = 0;
		break;
	}
	
	return result;  /* 20 */
}
