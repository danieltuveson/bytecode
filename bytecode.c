#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>

// Enable if you need to debug your bytecode
#ifndef ENABLE_LOGS
    #define ENABLE_LOGS false
#endif

// Bytecode for our virtual machine.
// PUSH, ADD, SUBTRACT, and DUPLICATE are ways to manipulate values on the stack.
// JUMP_IF is our only operation for doing control flow.
enum Bytecode {
    PUSH = 1,      // Push a value onto the stack (takes one argument)
    ADD = 2,       // Pop two values off of the stack and add them together
    SUBTRACT = 3,  // Pop two values off of the stack, and subtract the second value from the first
    DUPLICATE = 4, // Make a copy of the value on the top of the stack and push it onto the stack
    JUMP_IF = 5,   // Pop a value off of the stack. If it is greater than 0, jump to the given 
                   // location (takes one argument)
    RET = 6        // Terminate the progarm and return the value at the top of the stack
};

// Logging functions and macros, for deubgging purposes 
#if ENABLE_LOGS
    static void bar(void)
    {
        log("================\n");
    }

    static void print_stack(int *stack, int sp)
    {
        bar();
        log("stack: [ ");
        for (int i = 1; i < 10; i++) {
            if (i < sp) {
                log("%d, ", stack[i]);
            }
        }
        log("]\n");
        log("Stack pointer - index: %d, value: %d\n", sp-1, stack[sp-1]);
    }
    #define log(...) printf(__VA_ARGS__)
#else
    // Make the logging functions no-ops when logging is disabled
    static void print_stack(int *stack, int sp) { (void)stack; (void)sp; }
    static inline void do_nothing(char *string, ...) { (void)string; }
    #define log(...) do_nothing(__VA_ARGS__)
#endif


// Utility macros for manipulating the stack
#define pop() stack[--sp]
#define push(value) stack[sp++] = (value)
#define next() ip++

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
                next();
                log("%d\n", bytecode[ip]);
                push(bytecode[next()]);
                break;
            case ADD:
            log("adding ");
                next();
                temp1 = pop();
                temp2 = pop();
                push(temp1 + temp2);
                log("%d + %d = %d\n", temp1, temp2, temp1 + temp2);
                break;
            case SUBTRACT:
            log("subtracting ");
                next();
                temp1 = pop();
                temp2 = pop();
                push(temp1 - temp2);
                log("%d - %d = %d\n", temp1, temp2, temp1 - temp2);
                break;
            case DUPLICATE:
            log("duplicating ");
                next();
                temp1 = pop();
                log("%d\n", temp1);
                push(temp1);
                push(temp1);
                break;
            case JUMP_IF:
            log("jump condition: ");
                next();
                if (pop() > 0) {
                    ip = bytecode[ip];
                    log("true - jumping to instruction %d\n", ip);
                } else {
                    log("false - continuing to instruction %d\n", ip + 1);
                    next();
                }
                break;
            case RET:
            log("returning\n");
                goto end;
        }
    }
    end:
    return stack[sp - 1];
}

#define cont() \
    opcode = bytecode[ip];\
    log("next opcode: %d\n", opcode);\
    target = targets[opcode];\
    print_stack(stack, sp);\
    goto *target

int threaded(const int *bytecode)
{
    // Order of the items in this array must match the values of bytecode
    void *targets[] = { NULL, &&PUSH, &&ADD, &&SUBTRACT, &&DUPLICATE, &&JUMP_IF, &&RET };
    int stack[10] = { 0 };
    int sp = 1;
    int ip = 0;
    int temp1, temp2;
    void *target;
    int opcode;
    cont();
    PUSH:
    log("pushing ");
        next();
        log("%d\n", bytecode[ip]);
        push(bytecode[next()]);
        cont();
    ADD:
    log("adding ");
        next();
        temp1 = pop();
        temp2 = pop();
        push(temp1 + temp2);
        log("%d + %d = %d\n", temp1, temp2, temp1 + temp2);
        cont();
    SUBTRACT:
    log("subtracting ");
        next();
        temp1 = pop();
        temp2 = pop();
        push(temp1 - temp2);
        log("%d - %d = %d\n", temp1, temp2, temp1 - temp2);
        cont();
    DUPLICATE:
    log("duplicating ");
        next();
        temp1 = pop();
        log("%d\n", temp1);
        push(temp1);
        push(temp1);
        cont();
    JUMP_IF:
    log("jump condition: ");
        next();
        if (pop() > 0) {
            ip = bytecode[ip];
            log("true - jumping to instruction %d\n", ip);
        } else {
            log("false - continuing to instruction %d\n", ip + 1);
            next();
        }
        cont();
    RET:
    log("returning\n");
        goto end;
    end:
    return stack[sp - 1];
}

// Example bytecode program. Loops from 1 to INT_MAX.
const int bytecode[] = {
    PUSH, 1,
    PUSH, 1,
    ADD,
    DUPLICATE,
    PUSH, INT_MAX,
    SUBTRACT,
    JUMP_IF, 2, // jump to the second "PUSH 1"
    RET
};

int main(void)
{
    // Loop and switch version of bytecode interpreter
    #ifdef LOOPED
        printf("Running loop bytecode\n");
        int ret = looped(bytecode);
        printf("returned: %d\n", ret);
    // Threaded version of bytecode interpreter
    #else
    #ifdef THREADED
        printf("Running threaded bytecode\n");
        int ret = threaded(bytecode);
        printf("returned: %d\n", ret);
    #endif
    #endif
    return 0;
}
