/*
 * Test 04: Logical and bitwise operations
 * Tests: and, or, xor, not, shifts
 */

int main() {
	int a, b, c;
	
	a = 0x0F;
	b = 0xF0;
	
	c = a & b;    /* 0x00 */
	c = a | b;    /* 0xFF */
	c = a ^ b;    /* 0xFF */
	c = ~a;       /* 0xFFFFFFF0 */
	
	c = a << 4;   /* 0xF0 */
	c = b >> 4;   /* 0x0F */
	
	return c;
}
