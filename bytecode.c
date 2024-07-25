#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>

#ifndef ENABLE_LOGS
    #define ENABLE_LOGS false
#endif

#if ENABLE_LOGS
    #define log(...) printf(__VA_ARGS__)
#else
    static inline void do_nothing(char *string, ...) { (void)string; }
    #define log(...) do_nothing(__VA_ARGS__)
#endif

enum Bytecode {
    PUSH = 1,
    ADD = 2,
    LESS_THAN = 3,
    JUMP_IF = 4,
    RET = 5
};

void bar(void)
{
    log("================\n");
}

void print_stack(int *stack, int sp)
{
    bar();
    log("stack: [ ");
    for (int i = 0; i < 10; i++) {
        log("%d, ", stack[i]);
    }
    log("]\n");
    log("Stack pointer - index: %d, value: %d\n", sp-1, stack[sp-1]);
}

int looped(const int *bytecode)
{
    int stack[10] = { 0 };
    int sp = 1;
    int ip = 0;
    int temp1, temp2;
    while (true) {
        print_stack(stack, sp);
        switch(bytecode[ip]) {
            case PUSH:
            log("pushing ");
                ip++;
                log("%d\n", bytecode[ip]);
                stack[sp++] = bytecode[ip++];
                break;
            case ADD:
            log("adding\n");
                ip++;
                temp1 = stack[--sp];
                log("temp1 %d\n", temp1);
                temp2 = stack[--sp];
                log("temp2 %d\n", temp2);
                stack[sp++] = temp1 + temp2;
                break;
            case LESS_THAN:
            log("comparing\n");
                ip++;
                if (stack[sp-1] < bytecode[ip++]) {
                    stack[sp++] = 1;
                    log("less than\n");
                } else {
                    stack[sp++] = 0;
                    log("greater than or equal\n");
                }
                break;
            case JUMP_IF:
            log("jumping\n");
                ip++;
                if (stack[--sp]) {
                    ip = bytecode[ip];
                    log("actually jumping\n");
                } else {
                    ip++;
                }
                break;
            case RET:
            log("returning\n");
                ip++;
                goto end;
        }
    }
    end:
    return stack[sp - 1];
}

#define next() \
    opcode = bytecode[ip];\
    log("opcode: %d\n", opcode);\
    target = targets[opcode];\
    print_stack(stack, sp);\
    goto *target

int threaded(const int *bytecode)
{
    // Order of the items in this array must match the values of bytecode
    void *targets[] = { NULL, &&PUSH, &&ADD, &&LESS_THAN, &&JUMP_IF, &&RET };
    int stack[10] = { 0 };
    int sp = 1;
    int ip = 0;
    int temp1, temp2;
    void *target;
    int opcode;
    next();
    PUSH:
    log("pushing ");
        ip++;
        log("%d\n", bytecode[ip]);
        stack[sp++] = bytecode[ip++];
        next();
    ADD:
    log("adding\n");
        ip++;
        temp1 = stack[--sp];
        log("temp1 %d\n", temp1);
        temp2 = stack[--sp];
        log("temp2 %d\n", temp2);
        stack[sp++] = temp1 + temp2;
        next();
    LESS_THAN:
    log("comparing\n");
        ip++;
        if (stack[sp-1] < bytecode[ip++]) {
            stack[sp++] = 1;
            log("less than\n");
        } else {
            stack[sp++] = 0;
            log("greater than or equal\n");
        }
        next();
    JUMP_IF:
    log("jumping\n");
        ip++;
        if (stack[--sp]) {
            ip = bytecode[ip];
            log("actually jumping\n");
        } else {
            ip++;
        }
        next();
    RET:
    log("returning\n");
        ip++;
        goto end;
    end:
    return stack[sp - 1];
}

// Example bytecode program. Loops from 1 to INT_MAX.
const int bytecode[] = {
    PUSH, 1,
    PUSH, 1,
    ADD,
    LESS_THAN, INT_MAX,
    JUMP_IF, 2, // jump to the second "PUSH 1"
    RET
};

int main(void)
{
    // Loop and switch version of bytecode interpreter
    #ifdef LOOPED
        printf("Running loop bytecode\n");
        int ret = looped(bytecode);
        printf("ret: %d\n", ret);
    // Threaded version of bytecode interpreter
    #else
    #ifdef THREADED
        printf("Running threaded bytecode\n");
        int ret = threaded(bytecode);
        printf("ret: %d\n", ret);
    #endif
    #endif
    return 0;
}
