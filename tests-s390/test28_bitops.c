/*
 * Test 28: Bit manipulation
 * Tests: bit counting, bit setting/clearing
 */

int count_bits(int n) {
	int count;
	count = 0;
	while (n) {
		count = count + (n & 1);
		n = n >> 1;
	}
	return count;
}

int set_bit(int n, int pos) {
	return n | (1 << pos);
}

int clear_bit(int n, int pos) {
	return n & ~(1 << pos);
}

int toggle_bit(int n, int pos) {
	return n ^ (1 << pos);
}

int get_bit(int n, int pos) {
	return (n >> pos) & 1;
}

int main() {
	int x, bits;
	
	x = 0;
	x = set_bit(x, 0);    /* 1 */
	x = set_bit(x, 2);    /* 5 */
	x = set_bit(x, 4);    /* 21 */
	
	bits = count_bits(x);  /* 3 */
	
	x = toggle_bit(x, 2);  /* 17 */
	x = clear_bit(x, 0);   /* 16 */
	
	return x + bits;  /* 19 */
}
