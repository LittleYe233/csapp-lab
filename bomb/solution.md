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

## Phase 3

- Address of `phase_3()`: 0000000000400f43

This phase is quite easy! A lot of `jmp` to the same address and the denotion of `*0x402470(,%rax,8)` may be confusing, but the potential difficulty is far easier than that of the prior one I think.

It calls `sscanf()` to read two 32-bit signed integers (whose format string is `%d %d`) from the input string, and check whether the first number is greater than 0x07 or not - if so, boom! Then the instruction at 0x400f75 calculates the value of `*0x402470(,%rax,8)`, which is identical to `0x402470 + *(%rax) * 8`, an offset of %rax scaled to 8 to base address 0x402470, and jumps to the resulting address. For %rax is not greater than 8 (and not negative as we think), we only need to dig out the first 8 gigawords (64-bit) starting at it:

```text
(gdb) x/16wx 0x402470
0x402470:       0x00400f7c      0x00000000      0x00400fb9      0x00000000
0x402480:       0x00400f83      0x00000000      0x00400f8a      0x00000000
0x402490:       0x00400f91      0x00000000      0x00400f98      0x00000000
0x4024a0:       0x00400f9f      0x00000000      0x00400fa6      0x00000000
```

It is clear to know the exact target address we should go to according to our given first number. Next, it always jumps to a `mov` instruction to set a certain value to %eax, jumping again to 0x400fbe, which leads to a final `cmp` with the second number (0xc(%rsp)) from the input string. The certain values are shown in the dumped assembly, so we can quickly get 8 possible input strings.

- Solution:
  - `0 207`
  - `1 311`
  - `2 707`
  - `3 256`
  - `4 389`
  - `5 206`
  - `6 682`
  - `7 327`

## Phase 4

- Address of `phase_4()`: 000000000040100c

This time we will encounter another function called `func4` (at 0000000000400fce) to be tackled with. Now let's look into the structure of `phase_4()` first:

1. Read two integers by `sscanf()` from the input string and store them in the stack;
2. Check if the first input is not greater than 0xe:
   - If so: go next;
   - If not: boom!
3. Call `func4(<first input>, 0, 0xe)`;
4. Check if the return value is 0 **AND** the second input is 0:
   - If so: you have defused this phase of binary bomb!
   - If not: boom certainly!

So we only need to manage the `func4()` to return a zero, unless it modifies the value of 0xc(%rsp) (of course it is bound to the second input).

And here is the structure of `func4()` (the three original arguments are denoted as "1st", "2nd", "3rd"):

1. Set %eax to `(3rd - 2nd + ((3rd - 2nd) >> 31)) >> 1` (the former shift right is **logical**, while the latter is **arithmetic**);
2. Set %ecx to `%rax + 2nd`;
3. Check if %ecx <= 1st:
   - If so, go to subroutine 1:
     1. Set %eax to 0;
     2. Check if %ecx >= 1st:
        - If so, jump to the end;
        - If not, go to subroutine 3;
     3. Set 2nd to `%rcx + 1`;
     4. Call itself;
     5. Set %eax to `%eax * 2 + 1`;
     6. Jump to the end;
   - If not, go to step 4;
4. Set 3rd to `%rcx - 1`;
5. Call itself (recursion!);
6. Multiply %eax with 2;
7. Jump to the end.

Emm, a bit hard! In fact I don't know if I find all the solutions of this phase, but if we delve into it carefully we will find a shortest path to reach the goal: **to make both of the comparisons TRUE!** If so, we will get a return value with zero between the two checks (Step 3 and Step 3.2) and get to the end happily and safely. To do so, we need to make sure that %ecx == 1st, that is `((((3rd - 2nd) >> 31) + 3rd - 2nd) >> 1) + 2nd == 1st`. The left hand statement equals to 7 due to the fixed values of 2nd (0) and 3rd (0xe), so just set 1st to 7 before calling `func4()` in `phase_4()`.

Also, we can find that the second number from the original input string isn't modified in `func4()`, hence we get the final value of it.

- Solution: `7 0` **(may be not unique)**

