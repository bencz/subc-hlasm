/*
 * Test: Complex structures
 * Tests: nested structs, struct arrays, struct pointers, linked lists
 */
#include <stdio.h>
#include <string.h>

struct date {
    int day;
    int month;
    int year;
};

struct person {
    char name[32];
    int age;
    struct date birth;
};

struct node {
    int value;
    struct node *next;
};

struct company {
    char name[32];
    struct person employees[3];
    int num_employees;
};

int main(void) {
    struct person p1;
    struct person *pp;
    struct person people[3];
    struct node n1, n2, n3;
    struct node *head;
    struct company comp;
    int i, sum;
    int passed;

    passed = 1;

    /* Nested struct initialization */
    strcpy(p1.name, "John");
    p1.age = 30;
    p1.birth.day = 15;
    p1.birth.month = 6;
    p1.birth.year = 1993;

    if (p1.birth.year != 1993 || p1.birth.month != 6) {
        printf("FAIL: nested struct: year=%d, month=%d\n", 
               p1.birth.year, p1.birth.month);
        passed = 0;
    }

    /* Pointer to struct with nested access */
    pp = &p1;
    if (pp->birth.day != 15) {
        printf("FAIL: pp->birth.day = %d (expected 15)\n", pp->birth.day);
        passed = 0;
    }

    /* Modify nested through pointer */
    pp->birth.year = 1990;
    if (p1.birth.year != 1990) {
        printf("FAIL: p1.birth.year = %d after pp-> (expected 1990)\n", 
               p1.birth.year);
        passed = 0;
    }

    /* Array of structs */
    strcpy(people[0].name, "Alice");
    people[0].age = 25;
    strcpy(people[1].name, "Bob");
    people[1].age = 30;
    strcpy(people[2].name, "Carol");
    people[2].age = 35;

    sum = 0;
    for (i = 0; i < 3; i++) {
        sum += people[i].age;
    }
    if (sum != 90) {
        printf("FAIL: sum of ages = %d (expected 90)\n", sum);
        passed = 0;
    }

    /* Linked list simulation */
    n1.value = 10;
    n2.value = 20;
    n3.value = 30;
    n1.next = &n2;
    n2.next = &n3;
    n3.next = 0;

    head = &n1;
    sum = 0;
    while (head != 0) {
        sum += head->value;
        head = head->next;
    }
    if (sum != 60) {
        printf("FAIL: linked list sum = %d (expected 60)\n", sum);
        passed = 0;
    }

    /* Struct containing array of structs */
    strcpy(comp.name, "TechCorp");
    comp.num_employees = 3;
    strcpy(comp.employees[0].name, "Dev1");
    comp.employees[0].age = 28;
    strcpy(comp.employees[1].name, "Dev2");
    comp.employees[1].age = 32;
    strcpy(comp.employees[2].name, "Dev3");
    comp.employees[2].age = 26;

    sum = 0;
    for (i = 0; i < comp.num_employees; i++) {
        sum += comp.employees[i].age;
    }
    if (sum != 86) {
        printf("FAIL: company employees age sum = %d (expected 86)\n", sum);
        passed = 0;
    }

    /* Access nested struct in array through pointer */
    comp.employees[0].birth.year = 1995;
    if (comp.employees[0].birth.year != 1995) {
        printf("FAIL: comp.employees[0].birth.year = %d\n", 
               comp.employees[0].birth.year);
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_struct_complex\n");
    }
    return passed ? 0 : 1;
}
