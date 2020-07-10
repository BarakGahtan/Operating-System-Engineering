/* See COPYRIGHT for copyright information. */

#include <inc/mmu.h>
#include <inc/memlayout.h>
#include <inc/trap.h>



###################################################################
# exceptions/interrupts
###################################################################

/* TRAPHANDLER defines a globally-visible function for handling a trap.
 * It pushes a trap number onto the stack, then jumps to _alltraps.
 * Use TRAPHANDLER for traps where the CPU automatically pushes an error code.
 *
 * You shouldnt call a TRAPHANDLER function from C, but you may
 * need to _declare_ one in C (for instance, to get a function pointer
 * during IDT setup).  You can declare the function with
 *   void NAME();
 * where NAME is the argument passed to TRAPHANDLER.
 */
 
 
 /*
#define TRAPHANDLER(name, num)						\
	.globl name;		\
	.type name, @function;			\
	.align 2;				\
	name:					\
	pushl $(num);							\
	jmp _alltraps
*/

/* Use TRAPHANDLER_NOEC for traps where the CPU doesnt push an error code.
 * It pushes a 0 in place of the error code, so the trap frame has the same
 * format in either case.
 */

/*
#define TRAPHANDLER_NOEC(name, num)					\
	.globl name;							\
	.type name, @function;						\
	.align 2;							\
	name:								\
	pushl $0;							\
	pushl $(num);							\
	jmp _alltraps

.text
*/

// CHALLENGE : 

#define TRAPHANDLER(name, num)						\
    .data;  \
        .long name; \
    .text;  \
        .globl name;			\
    	.type name, @function;			\
    	.align 2;				\
	name:					\
        .if !(num == 8 || num == 17 || (num >= 10 && num <= 14));   \
        pushl $0;   \
        .endif;     \
    	pushl $(num);							\
    	jmp _alltraps


.data
    .globl  handlers

handlers:

/*
 * Lab 3: Your code here for generating entry points for the different traps.
 */

/*
TRAPHANDLER_NOEC(handler_divide, T_DIVIDE) 
TRAPHANDLER_NOEC(handler_debug, T_DEBUG)  
TRAPHANDLER_NOEC(handler_nmi, T_NMI) 
TRAPHANDLER_NOEC(handler_brkpt, T_BRKPT) 
TRAPHANDLER_NOEC(handler_oflow, T_OFLOW) 
TRAPHANDLER_NOEC(handler_bound, T_BOUND) 
TRAPHANDLER_NOEC(handler_illop, T_ILLOP) 
TRAPHANDLER_NOEC(handler_device, T_DEVICE)
TRAPHANDLER(handler_dblflt, T_DBLFLT) 
TRAPHANDLER(handler_tss, T_TSS) 
TRAPHANDLER(handler_segnp, T_SEGNP) 
TRAPHANDLER(handler_stack, T_STACK) 
TRAPHANDLER(handler_gpflt, T_GPFLT) 
TRAPHANDLER(handler_pgflt, T_PGFLT) 

TRAPHANDLER_NOEC(handler_fperr, T_FPERR) 
TRAPHANDLER(handler_align, T_ALIGN)
TRAPHANDLER_NOEC(handler_mchk, T_MCHK)
TRAPHANDLER_NOEC(handler_simderr, T_SIMDERR)
TRAPHANDLER_NOEC(handler_syscall, T_SYSCALL)
*/

    TRAPHANDLER(handler0, 0)
    TRAPHANDLER(handler1, 1)
    TRAPHANDLER(handler2, 2)
    TRAPHANDLER(handler3, 3)
    TRAPHANDLER(handler4, 4)
    TRAPHANDLER(handler5, 5)
    TRAPHANDLER(handler6, 6)
    TRAPHANDLER(handler7, 7)
    TRAPHANDLER(handler8, 8)
    TRAPHANDLER(handler9, 9)
    TRAPHANDLER(handler10, 10)
    TRAPHANDLER(handler11, 11)
    TRAPHANDLER(handler12, 12)
    TRAPHANDLER(handler13, 13)
    TRAPHANDLER(handler14, 14)
    TRAPHANDLER(handler15, 15)
    TRAPHANDLER(handler16, 16)
    TRAPHANDLER(handler17, 17)
    TRAPHANDLER(handler18, 18)
    TRAPHANDLER(handler19, 19)
    TRAPHANDLER(handler20, 20)
    TRAPHANDLER(handler21, 21)
    TRAPHANDLER(handler22, 22)
    TRAPHANDLER(handler23, 23)
    TRAPHANDLER(handler24, 24)
    TRAPHANDLER(handler25, 25)
    TRAPHANDLER(handler26, 26)
    TRAPHANDLER(handler27, 27)
    TRAPHANDLER(handler28, 28)
    TRAPHANDLER(handler29, 29)
    TRAPHANDLER(handler30, 30)
    TRAPHANDLER(handler31, 31)
    TRAPHANDLER(handler32, 32)
    TRAPHANDLER(handler33, 33)
    TRAPHANDLER(handler34, 34)
    TRAPHANDLER(handler35, 35)
    TRAPHANDLER(handler36, 36)
    TRAPHANDLER(handler37, 37)
    TRAPHANDLER(handler38, 38)
    TRAPHANDLER(handler39, 39)
    TRAPHANDLER(handler40, 40)
    TRAPHANDLER(handler41, 41)
    TRAPHANDLER(handler42, 42)
    TRAPHANDLER(handler43, 43)
    TRAPHANDLER(handler44, 44)
    TRAPHANDLER(handler45, 45)
    TRAPHANDLER(handler46, 46)
    TRAPHANDLER(handler47, 47)
    TRAPHANDLER(handler48, 48)

/*
 * Lab 3: Your code here for _alltraps
 */
 
/*
 _alltraps:
    pushl %ds;
    pushl %es;
    pushal;
    #movw $0x10, %ax;
    pushl $GD_KD;
    popl %ds;
    pushl $GD_KD;
    popl %es;
    pushl %esp;
    call trap;
*/


_alltraps:
    pushl %ds
    pushl %es
    pushal
    movw $GD_KD, %ax
    movw %ax, %ds
    movw %ax, %es
    pushl %esp
    call trap

