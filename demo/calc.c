
#include <stdio.h>
#include <stdlib.h>

int evaluate(int a, int b, char op) {
    if (op == '+') {
        return a + b;
    } else if (op == '-') {
        return a - b;
    } else if (op == '*') {
        return a * b;
    } else if (op == '/') {
        if (b == 0) {
            printf("Error: Division by zero\n");
            return 0;
        } else {
            return a / b;
        }
    } else if (op == '%') {
        if (b == 0) {
            printf("Error: Modulo by zero\n");
            return 0;
        } else {
            return a % b;
        }
    } else {
        printf("Error: Unknown operator\n");
        return -1;
    }
}

void classifyResult(int result) {
    if (result < 0) {
        if (result < -1000) {
            printf("Category: Very Large Negative\n");
        } else if (result < -100) {
            printf("Category: Large Negative\n");
        } else {
            printf("Category: Small Negative\n");
        }
    } else if (result == 0) {
        printf("Category: Zero\n");
    } else {
        if (result > 1000) {
            printf("Category: Very Large Positive\n");
        } else if (result > 100) {
            printf("Category: Large Positive\n");
        } else {
            printf("Category: Small Positive\n");
        }
    }
}

int main() {
    int t;
    scanf("%d", &t);

    while (t--) {
        int a, b;
        char op;
        scanf("%d %c %d", &a, &op, &b);

        int result = evaluate(a, b, op);
        printf("Result: %d\n", result);
        classifyResult(result);
    }

    return 0;
}
