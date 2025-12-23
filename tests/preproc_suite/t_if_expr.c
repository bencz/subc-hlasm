/*
 * Test: #if with arithmetic expressions
 */

/* Addition and subtraction */
#if 2 + 3 == 5
int add_ok = 1;
#else
int add_ok = 0;
#endif

#if 10 - 3 == 7
int sub_ok = 1;
#else
int sub_ok = 0;
#endif

/* Multiplication and division */
#if 4 * 5 == 20
int mul_ok = 1;
#else
int mul_ok = 0;
#endif

#if 20 / 4 == 5
int div_ok = 1;
#else
int div_ok = 0;
#endif

#if 17 % 5 == 2
int mod_ok = 1;
#else
int mod_ok = 0;
#endif

/* Comparison operators */
#if 5 > 3
int gt_ok = 1;
#else
int gt_ok = 0;
#endif

#if 3 < 5
int lt_ok = 1;
#else
int lt_ok = 0;
#endif

#if 5 >= 5
int ge_ok = 1;
#else
int ge_ok = 0;
#endif

#if 5 <= 5
int le_ok = 1;
#else
int le_ok = 0;
#endif

#if 5 == 5
int eq_ok = 1;
#else
int eq_ok = 0;
#endif

#if 5 != 3
int ne_ok = 1;
#else
int ne_ok = 0;
#endif

int main(void) {
    if (!add_ok) return 1;
    if (!sub_ok) return 2;
    if (!mul_ok) return 3;
    if (!div_ok) return 4;
    if (!mod_ok) return 5;
    if (!gt_ok) return 6;
    if (!lt_ok) return 7;
    if (!ge_ok) return 8;
    if (!le_ok) return 9;
    if (!eq_ok) return 10;
    if (!ne_ok) return 11;
    return 0;
}
