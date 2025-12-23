/*
 * Test: Bit manipulation (simulating bitfields)
 * Tests: bit set/clear/toggle, masks, flags
 */
#include <stdio.h>

/* Flag definitions */
#define FLAG_READ    0x01
#define FLAG_WRITE   0x02
#define FLAG_EXEC    0x04
#define FLAG_HIDDEN  0x08
#define FLAG_SYSTEM  0x10
#define FLAG_ARCHIVE 0x20

/* Bit manipulation macros (simulated as functions for SubC) */
int bit_set(int value, int bit) {
    return value | (1 << bit);
}

int bit_clear(int value, int bit) {
    return value & ~(1 << bit);
}

int bit_toggle(int value, int bit) {
    return value ^ (1 << bit);
}

int bit_test(int value, int bit) {
    return (value >> bit) & 1;
}

int bits_set(int value, int mask) {
    return value | mask;
}

int bits_clear(int value, int mask) {
    return value & ~mask;
}

int bits_test_all(int value, int mask) {
    return (value & mask) == mask;
}

int bits_test_any(int value, int mask) {
    return (value & mask) != 0;
}

/* Count set bits (population count) */
int popcount(int value) {
    int count;
    count = 0;
    while (value) {
        count += value & 1;
        value >>= 1;
    }
    return count;
}

/* Find lowest set bit position (0-indexed, -1 if none) */
int find_lowest_bit(int value) {
    int pos;
    if (value == 0) return -1;
    pos = 0;
    while ((value & 1) == 0) {
        value >>= 1;
        pos++;
    }
    return pos;
}

/* Find highest set bit position */
int find_highest_bit(int value) {
    int pos;
    if (value == 0) return -1;
    pos = 0;
    while (value > 1) {
        value >>= 1;
        pos++;
    }
    return pos;
}

/* Rotate left */
int rotate_left(int value, int bits, int width) {
    return ((value << bits) | (value >> (width - bits))) & ((1 << width) - 1);
}

/* Rotate right */
int rotate_right(int value, int bits, int width) {
    return ((value >> bits) | (value << (width - bits))) & ((1 << width) - 1);
}

/* Extract bits [start, start+len) */
int extract_bits(int value, int start, int len) {
    return (value >> start) & ((1 << len) - 1);
}

/* Insert bits at position */
int insert_bits(int value, int bits, int start, int len) {
    int mask;
    mask = ((1 << len) - 1) << start;
    return (value & ~mask) | ((bits << start) & mask);
}

/* Reverse bits in a byte */
int reverse_byte(int b) {
    int result;
    result = 0;
    result |= (b & 0x01) << 7;
    result |= (b & 0x02) << 5;
    result |= (b & 0x04) << 3;
    result |= (b & 0x08) << 1;
    result |= (b & 0x10) >> 1;
    result |= (b & 0x20) >> 3;
    result |= (b & 0x40) >> 5;
    result |= (b & 0x80) >> 7;
    return result;
}

int main(void) {
    int flags, result;
    int passed;

    passed = 1;

    /* Test individual bit operations */
    result = bit_set(0, 3);
    if (result != 8) {
        printf("FAIL: bit_set(0, 3) = %d (expected 8)\n", result);
        passed = 0;
    }

    result = bit_clear(0xFF, 4);
    if (result != 0xEF) {
        printf("FAIL: bit_clear(0xFF, 4) = 0x%x (expected 0xEF)\n", result);
        passed = 0;
    }

    result = bit_toggle(0x0F, 2);
    if (result != 0x0B) {
        printf("FAIL: bit_toggle(0x0F, 2) = 0x%x (expected 0x0B)\n", result);
        passed = 0;
    }

    result = bit_test(0x08, 3);
    if (result != 1) {
        printf("FAIL: bit_test(0x08, 3) = %d (expected 1)\n", result);
        passed = 0;
    }

    result = bit_test(0x08, 2);
    if (result != 0) {
        printf("FAIL: bit_test(0x08, 2) = %d (expected 0)\n", result);
        passed = 0;
    }

    /* Test flag operations */
    flags = 0;
    flags = bits_set(flags, FLAG_READ | FLAG_WRITE);
    if (flags != 0x03) {
        printf("FAIL: set READ|WRITE = 0x%x\n", flags);
        passed = 0;
    }

    flags = bits_set(flags, FLAG_EXEC);
    if (!bits_test_all(flags, FLAG_READ | FLAG_WRITE | FLAG_EXEC)) {
        printf("FAIL: test all RWX\n");
        passed = 0;
    }

    flags = bits_clear(flags, FLAG_WRITE);
    if (bits_test_any(flags, FLAG_WRITE)) {
        printf("FAIL: WRITE should be cleared\n");
        passed = 0;
    }

    /* Test popcount */
    result = popcount(0);
    if (result != 0) {
        printf("FAIL: popcount(0) = %d\n", result);
        passed = 0;
    }

    result = popcount(0xFF);
    if (result != 8) {
        printf("FAIL: popcount(0xFF) = %d (expected 8)\n", result);
        passed = 0;
    }

    result = popcount(0x55);  /* 01010101 */
    if (result != 4) {
        printf("FAIL: popcount(0x55) = %d (expected 4)\n", result);
        passed = 0;
    }

    /* Test find_lowest_bit */
    result = find_lowest_bit(0x18);  /* 00011000 */
    if (result != 3) {
        printf("FAIL: find_lowest_bit(0x18) = %d (expected 3)\n", result);
        passed = 0;
    }

    result = find_lowest_bit(0);
    if (result != -1) {
        printf("FAIL: find_lowest_bit(0) = %d (expected -1)\n", result);
        passed = 0;
    }

    /* Test find_highest_bit */
    result = find_highest_bit(0x18);
    if (result != 4) {
        printf("FAIL: find_highest_bit(0x18) = %d (expected 4)\n", result);
        passed = 0;
    }

    result = find_highest_bit(1);
    if (result != 0) {
        printf("FAIL: find_highest_bit(1) = %d (expected 0)\n", result);
        passed = 0;
    }

    /* Test rotate */
    result = rotate_left(0x0F, 2, 8);  /* 00001111 -> 00111100 */
    if (result != 0x3C) {
        printf("FAIL: rotate_left(0x0F, 2, 8) = 0x%x (expected 0x3C)\n", result);
        passed = 0;
    }

    result = rotate_right(0xF0, 2, 8);  /* 11110000 -> 00111100 */
    if (result != 0x3C) {
        printf("FAIL: rotate_right(0xF0, 2, 8) = 0x%x (expected 0x3C)\n", result);
        passed = 0;
    }

    /* Test extract_bits */
    result = extract_bits(0xABCD, 4, 8);  /* extract middle byte */
    if (result != 0xBC) {
        printf("FAIL: extract_bits(0xABCD, 4, 8) = 0x%x (expected 0xBC)\n", result);
        passed = 0;
    }

    /* Test insert_bits */
    result = insert_bits(0xFF00, 0x5A, 4, 8);
    if (result != 0xF5A0) {
        printf("FAIL: insert_bits = 0x%x (expected 0xF5A0)\n", result);
        passed = 0;
    }

    /* Test reverse_byte */
    result = reverse_byte(0x01);  /* 00000001 -> 10000000 */
    if (result != 0x80) {
        printf("FAIL: reverse_byte(0x01) = 0x%x (expected 0x80)\n", result);
        passed = 0;
    }

    result = reverse_byte(0xA5);  /* 10100101 -> 10100101 */
    if (result != 0xA5) {
        printf("FAIL: reverse_byte(0xA5) = 0x%x (expected 0xA5)\n", result);
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_bitfield_sim\n");
    }
    return passed ? 0 : 1;
}
