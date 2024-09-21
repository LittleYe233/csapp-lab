# Bomb Lab Solution

## Preparation & First Glance

I used `objdump -d bomb | less` to show the dumped assembly code of the executable, and `gdb -tui bomb` to run the debugger.

The source code of bomb (bomb.c) provides the basic structure of it, but I think it is not very useful in most time of debugging. Just glance at it quickly and memorize it.

Ignoring `initialize_bomb()` temporarily, all the six phase operations comprises a `read_line()` from STDIN or file, `phase_X(input)` to validate phase and `phase_defused()` to do something. Sometimes we only need to analyze `phase_X()`.

The basic procedure of debugging is running the program in gdb, setting up a breakpoint just before the execution of `explode_bomb()` and checking the values of registers and variables.

Some useful commands in gdb:

- `info registers [REG]` or `i r [REG]`: check all registers or one specific REG;
- `p <ADDR/NAME>`: print the value of variable by NAME or its ADDR;
- `printf <FMT>, <ADDR/NAME>`: print with fmt (e.g. `printf "%s", 0x12345678`);
- `stepi` or `si`: step to next machine instruction;
- `b *<ADDR>`: add breakpoint at address ADDR.

## Phase 1

- Address of `phase_1()`: 0000000000400ee0

This is just a simple string check: it loads the key string somewhere (0x402400) and compares my input string with it. We only need to print the actual value of the string pointer.

- Solution: `Border relations with Canada have never been better.` **(You should regard the whole character sequence in monospace font as well as the last period "." as the final solution.)**

## Phase 2

- Address of `phase_2()`: 0000000000400efc

Oh, this phase seems a little complicated! We can try to distill the procedure detail of `phase_2()` (the denotion of data address below is only the least 16 bits of itself, while the most 32 bits are 0x7fffffff):

1. Read six 32-bit signed numbers by `read_six_numbers()` and store them at %rsp (0xcfb0);
2. Compare the first number (at 0xcfb0) with 0x01:
   - Equal: go to 0x400f30;
   - Not equal: go to `explode_bomb()`;
3. Set %rbx to %rsp+0x4 (0xcfb4, address of the second number), set Frame Pointer %rbp to %rsp+0x18 (address just after the six numbers in stack), then go to 0x400f17;
4. In the code segment of 0x400f17, set **the pointed value** of %eax (I denoted it as `*(%eax)`) to the pointed value of %rbx-0x4, a.k.a. the first number at present, then multiply itself with 2;
5. Compare the new value of %eax with the pointed value of %rbx:
   - Equal: go to 0x400f25;
   - Not equal: go to `explode_bomb()` too;
6. In the code segment of 0x400f25, increase %rbx with 4 (pointing to the next number, the third one now);
7. Compare %rbp (it actually stores an **address** now) with %rbx:
   - Equal: go to 0x400f3c;
   - Not equal: **go back** to 0x400f17 (go back to step 4; now it forms a loop to compare one number with **double of the previous number**);
7. In the code segment of 0x400f3c, clean up and return.

For the first number should be 0x01, it is obvious that the solution of this phase is:

- Solution: `1 2 4 8 16 32`

