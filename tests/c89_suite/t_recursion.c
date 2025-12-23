/*
 * Test: Complex recursion
 * Tests: factorial, fibonacci, mutual recursion, tree traversal
 */
#include <stdio.h>

/* Simple recursion: factorial */
int factorial(int n) {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

/* Recursion with multiple calls: fibonacci */
int fibonacci(int n) {
    if (n <= 0) {
        return 0;
    }
    if (n == 1) {
        return 1;
    }
    return fibonacci(n - 1) + fibonacci(n - 2);
}

/* Mutual recursion */
int is_even(int n);
int is_odd(int n);

int is_even(int n) {
    if (n == 0) {
        return 1;
    }
    return is_odd(n - 1);
}

int is_odd(int n) {
    if (n == 0) {
        return 0;
    }
    return is_even(n - 1);
}

/* Recursive sum */
int recursive_sum(int n) {
    if (n <= 0) {
        return 0;
    }
    return n + recursive_sum(n - 1);
}

/* Recursive power */
int power(int base, int exp) {
    if (exp == 0) {
        return 1;
    }
    return base * power(base, exp - 1);
}

/* GCD using Euclidean algorithm */
int gcd(int a, int b) {
    if (b == 0) {
        return a;
    }
    return gcd(b, a % b);
}

/* Binary tree node */
struct tree_node {
    int value;
    struct tree_node *left;
    struct tree_node *right;
};

/* Recursive tree sum */
int tree_sum(struct tree_node *node) {
    if (node == 0) {
        return 0;
    }
    return node->value + tree_sum(node->left) + tree_sum(node->right);
}

/* Recursive tree depth */
int tree_depth(struct tree_node *node) {
    int left_depth, right_depth;
    if (node == 0) {
        return 0;
    }
    left_depth = tree_depth(node->left);
    right_depth = tree_depth(node->right);
    if (left_depth > right_depth) {
        return left_depth + 1;
    }
    return right_depth + 1;
}

/* Count nodes in tree */
int tree_count(struct tree_node *node) {
    if (node == 0) {
        return 0;
    }
    return 1 + tree_count(node->left) + tree_count(node->right);
}

int main(void) {
    int result;
    int passed;
    struct tree_node n1, n2, n3, n4, n5, n6, n7;

    passed = 1;

    /* Factorial */
    result = factorial(5);
    if (result != 120) {
        printf("FAIL: factorial(5) = %d (expected 120)\n", result);
        passed = 0;
    }

    result = factorial(0);
    if (result != 1) {
        printf("FAIL: factorial(0) = %d (expected 1)\n", result);
        passed = 0;
    }

    result = factorial(10);
    if (result != 3628800) {
        printf("FAIL: factorial(10) = %d (expected 3628800)\n", result);
        passed = 0;
    }

    /* Fibonacci */
    result = fibonacci(0);
    if (result != 0) {
        printf("FAIL: fibonacci(0) = %d (expected 0)\n", result);
        passed = 0;
    }

    result = fibonacci(1);
    if (result != 1) {
        printf("FAIL: fibonacci(1) = %d (expected 1)\n", result);
        passed = 0;
    }

    result = fibonacci(10);
    if (result != 55) {
        printf("FAIL: fibonacci(10) = %d (expected 55)\n", result);
        passed = 0;
    }

    /* Mutual recursion */
    if (is_even(0) != 1) {
        printf("FAIL: is_even(0) = %d\n", is_even(0));
        passed = 0;
    }
    if (is_even(4) != 1) {
        printf("FAIL: is_even(4) = %d\n", is_even(4));
        passed = 0;
    }
    if (is_odd(5) != 1) {
        printf("FAIL: is_odd(5) = %d\n", is_odd(5));
        passed = 0;
    }
    if (is_even(7) != 0) {
        printf("FAIL: is_even(7) = %d\n", is_even(7));
        passed = 0;
    }

    /* Recursive sum */
    result = recursive_sum(10);
    /* 1+2+...+10 = 55 */
    if (result != 55) {
        printf("FAIL: recursive_sum(10) = %d (expected 55)\n", result);
        passed = 0;
    }

    /* Power */
    result = power(2, 10);
    if (result != 1024) {
        printf("FAIL: power(2,10) = %d (expected 1024)\n", result);
        passed = 0;
    }

    result = power(3, 4);
    if (result != 81) {
        printf("FAIL: power(3,4) = %d (expected 81)\n", result);
        passed = 0;
    }

    /* GCD */
    result = gcd(48, 18);
    if (result != 6) {
        printf("FAIL: gcd(48,18) = %d (expected 6)\n", result);
        passed = 0;
    }

    result = gcd(100, 35);
    if (result != 5) {
        printf("FAIL: gcd(100,35) = %d (expected 5)\n", result);
        passed = 0;
    }

    /* Build a tree:
     *        1
     *       / \
     *      2   3
     *     / \   \
     *    4   5   6
     *   /
     *  7
     */
    n1.value = 1;
    n2.value = 2;
    n3.value = 3;
    n4.value = 4;
    n5.value = 5;
    n6.value = 6;
    n7.value = 7;

    n1.left = &n2;
    n1.right = &n3;
    n2.left = &n4;
    n2.right = &n5;
    n3.left = 0;
    n3.right = &n6;
    n4.left = &n7;
    n4.right = 0;
    n5.left = 0;
    n5.right = 0;
    n6.left = 0;
    n6.right = 0;
    n7.left = 0;
    n7.right = 0;

    /* Tree sum */
    result = tree_sum(&n1);
    /* 1+2+3+4+5+6+7 = 28 */
    if (result != 28) {
        printf("FAIL: tree_sum = %d (expected 28)\n", result);
        passed = 0;
    }

    /* Tree depth */
    result = tree_depth(&n1);
    if (result != 4) {
        printf("FAIL: tree_depth = %d (expected 4)\n", result);
        passed = 0;
    }

    /* Tree count */
    result = tree_count(&n1);
    if (result != 7) {
        printf("FAIL: tree_count = %d (expected 7)\n", result);
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_recursion\n");
    }
    return passed ? 0 : 1;
}
