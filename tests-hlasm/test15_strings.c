/*
 * Test 15: String literals
 * Tests: string constants, char arrays
 */

char *msg;

int strlen_simple(char *s) {
	int len;
	len = 0;
	while (*s) {
		len = len + 1;
		s = s + 1;
	}
	return len;
}

int main() {
	msg = "Hello";
	return strlen_simple(msg);  /* 5 */
}
