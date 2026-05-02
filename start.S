.intel_syntax noprefix

.text

/* Important stuffs
 * 
 * functions: rdi, rsi, rdx, rcx, r8, r9
 *  syscalls: rdi, rsi, rdx, r10, r8, r9
 *                           ^^^
 * By moving the value of rcx to r10, we can
 * convert a function call into a syscall
 */
.globl syscall
_syscall:
    mov r10, rcx
    syscall
    ret

// actually, we can write a macro to register all the syscalls for us
#define make_call(call_name, call_number) \
.globl call_name; \
.type call_name @function; \
call_name:; \
    mov rax, call_number; \
    jmp _syscall

make_call(write, 1)
make_call(close, 3)
make_call(exit, 60)
make_call(getdents64, 217)
make_call(openat, 257)

// prepare to call main()
.global _start
_start:
    xor rbp, rbp
    pop rdi      // pop argv from the initial stack
    mov rsi, rsp // argv ptr
    // we don't need any environment variables

    // magic that aligns the stack
    and rsp, -16
    sub rsp, 8

    call main    // just as it says

    mov rdi, rax // the return value of main() is the argument for exit()

    call exit

    jmp .done    // just in case

.done:           // will never be reached
    ret
