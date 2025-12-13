/*
 * Test 27: Prime number check
 * Tests: modulo, early return
 */

int is_prime(int n) {
	int i;
	
	if (n <= 1) return 0;
	if (n <= 3) return 1;
	if (n % 2 == 0) return 0;
	if (n % 3 == 0) return 0;
	
	i = 5;
	while (i * i <= n) {
		if (n % i == 0) return 0;
		if (n % (i + 2) == 0) return 0;
		i = i + 6;
	}
	return 1;
}

int count_primes(int max) {
	int i, count;
	count = 0;
	for (i = 2; i <= max; i = i + 1) {
		if (is_prime(i))
			count = count + 1;
	}
	return count;
}

int main() {
	return count_primes(30);  /* 10 primes: 2,3,5,7,11,13,17,19,23,29 */
}
