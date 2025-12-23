/*
 * Test: #if with logical and bitwise operators
 */

/* Logical AND */
#if 1 && 1
int and1 = 1;
#else
int and1 = 0;
#endif

#if 1 && 0
int and2 = 1;
#else
int and2 = 0;
#endif

/* Logical OR */
#if 0 || 1
int or1 = 1;
#else
int or1 = 0;
#endif

#if 0 || 0
int or2 = 1;
#else
int or2 = 0;
#endif

/* Logical NOT */
#if !0
int not1 = 1;
#else
int not1 = 0;
#endif

#if !1
int not2 = 1;
#else
int not2 = 0;
#endif

/* Bitwise AND */
#if (0xFF & 0x0F) == 0x0F
int band_ok = 1;
#else
int band_ok = 0;
#endif

/* Bitwise OR */
#if (0xF0 | 0x0F) == 0xFF
int bor_ok = 1;
#else
int bor_ok = 0;
#endif

/* Bitwise XOR */
#if (0xFF ^ 0x0F) == 0xF0
int bxor_ok = 1;
#else
int bxor_ok = 0;
#endif

/* Bitwise NOT */
#if (~0 & 0xFF) == 0xFF
int bnot_ok = 1;
#else
int bnot_ok = 0;
#endif

/* Shift operators */
#if (1 << 4) == 16
int lshift_ok = 1;
#else
int lshift_ok = 0;
#endif

#if (16 >> 2) == 4
int rshift_ok = 1;
#else
int rshift_ok = 0;
#endif

int main(void) {
    if (!and1) return 1;
    if (and2) return 2;
    if (!or1) return 3;
    if (or2) return 4;
    if (!not1) return 5;
    if (not2) return 6;
    if (!band_ok) return 7;
    if (!bor_ok) return 8;
    if (!bxor_ok) return 9;
    if (!bnot_ok) return 10;
    if (!lshift_ok) return 11;
    if (!rshift_ok) return 12;
    return 0;
}
