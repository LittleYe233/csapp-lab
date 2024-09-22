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

## Phase 5

- Address of `phase_5()`: 0000000000401062

This phase is much more challenging than I thought! I felt very stressed and confused when looking at the assembly, which contains many unfamiliar registers and abstract (?) instructions. However, I finally managed to succeed and formed a rough understanding of `phase_5()`.

Here is the rough sketch of it:

1. Do something confusing, but finally set %eax to 0 (because of instruction at 0x401078); check if the length of input string is 6:
   - If so, go to next step (in fact it jumps to the rear, reset %eax and jumps back; why did the compiler do so?);
   - If not, explosion!
2. Get the first character of the input string, mask its ASCII with 0xF (that is, modulo 16), regarded as the offset of a base string located at 0x4024b0;
3. Jump to step 2 and loop until all six final characters are extracted from the base;
4. Compare another reference string at 0x40245e with it:
   - If equal, do some cleaning and (mostly) defused!
   - If not equal, you know the outcome, right?

The base string is "maduiersnfotvbylSo you think you can stop the bomb with ctrl-c, do you?" *(you will quickly know why there is an unusual questioning sentence here...)*, while the reference is "flyers". The reference characters are in the base with indices 9, 15, 14, 5, 6, 7 (starting with 0), respectively, hence we only need to give an input string that ASCII of every character modulos 16 equals the indices. For example, `9?>567` (starting with '0' whose ASCII is 0x30).

To be honest, I can't figure out the instructions from 0x4010de to 0x4010e9. They seem to be a stack check, but I don't know why it should be here.

What's more, by a stroke of good luck, I got this answer with an extremely ridiculous misunderstanding: I had attempted "012345" as the input string and run the debugger, getting the final string "maduie", just the same as the first six characters of the base string. So I thought it calculated the offset by substracting the ASCII with '0' (0x30). Were the ASCII of '0' not end with 4 bits of zeros, probably I wouldn't have solved this phase.

- Solution: `9?>567` (**not unique**)

## Phase 6

- Address of `phase_6()`: 00000000004010f4

I can't say anything... on this phase. It is the longest, most complicated (too many nested loops!), even comprising with a linked list (you can find some clues on it, but I failed)!

It is my poor understanding on assembly and coding that I ascribe to the failure of this time. What's worse, I am not good at English, so I can't give a detailed explanation about the solution. I can only provide some rough steps:

1. Read six integers from the input string;
2. Check if **ALL** the six numbers are less than 7 **AND** do **NOT** equal with each other;
3. Substract 7 with these numbers in-place;
4. Some obscure instructions to handle with the addresses of linked-list nodes (you can try to search `<node1>` in the dumped assembly); [^1]
5. Make sure that the value of one node is greater than that of its next node.

So where are the nodes? Well, when you continuously watch the registers, you may find some addresses shown frequently: 0x6032d0~0x603320. Check them:

```text
(gdb) x/24wx 0x6032d0
0x6032d0 <node1>:       0x0000014c      0x00000001      0x006032e0      0x00000000
0x6032e0 <node2>:       0x000000a8      0x00000002      0x006032f0      0x00000000
0x6032f0 <node3>:       0x0000039c      0x00000003      0x00603300      0x00000000
0x603300 <node4>:       0x000002b3      0x00000004      0x00603310      0x00000000
0x603310 <node5>:       0x000001dd      0x00000005      0x00603320      0x00000000
0x603320 <node6>:       0x000001bb      0x00000006      0x00000000      0x00000000
```

They are pre-defined global variables. Such as 0x14c, 0xa8 are the values. Our goal is to re-order them descendingly. The order should be 3, 4, 5, 6, 1, 2. But there is an operation of substraction before, so the input string should be "4 3 2 1 6 5".

- Solution: `4 3 2 1 6 5`

## Secret Phase

What...? There is still something left? In fact I have ignored it either, until I found the explanation of this blog[^1]. At end of `main()` of bomb.c is a piece of comment:

```c
/* Wow, they got it! But isn’t something… missing? Perhaps
 * something they overlooked? Mua ha ha ha ha! */
```

It implies the existence of a "secret phase", which will be covered by this section. When we try to search for all symbols named with "phase", we will notice one `secret_phase` at 0000000000401242 that is called by `phase_defused()`. But it needs other requirements to be executed:

- There are six strings in the input buffer (the bomb stores the whole input of ours to a string array); and
- the fourth input (for Phase 4) has three parameters (there is an `sscanf()` in the middle of `phase_defused()` whose format string is "%d %d %s"); and
- the third parameter is "DrEvil" (located at 0x402622).

If we do so and all the six inputs are correct, the bomb will print another two lines of message, meaning that the final secret phase has begun!

Next, continue to check its code. It is quite simpler than your thought, isn't it? It calls `strtol()` to convert your input string into a decimal (base 10) number, compares it with 0x3e8, calls `fun7(0x6030f0)` and expects a return value of 0x2. Either an input greater than 0x3e9 (mention the instruction 0x40125d) or an unexpected return value fuses the bomb. Quite easy. Then take a look of `fun7()`!

`fun7()` is short as well, but there is a latent trap inside:

1. Check if %rdi (the argument of this function) is zero:
   - If so, return 0xFFFFFFFF (-1);
   - If not, go next;
2. Check if %esi (our input of `secret_phase()`) is not less than %edx (the **pointed** value of %rdi):
   - If so, go next;
   - If not, return `2 * fun7(%rdi + 0x8)`;
3. Set %eax (the return value) to 0, check if %esi equals to %edx:
   - If so, return with the current %eax (that is, zero);
   - If not, return `1 + 2 * fun7(%rdi + 0x10)`.

It is recursive, which is annoying for novice programmers. So the pointer is the address 0x6030f0. Have a look at it:

```text
(gdb) x/120dx 0x00000000006030f0
0x6030f0 <n1>:  0x00000024      0x00000000      0x00603110      0x00000000
0x603100 <n1+16>:       0x00603130      0x00000000      0x00000000      0x00000000
0x603110 <n21>: 0x00000008      0x00000000      0x00603190      0x00000000
0x603120 <n21+16>:      0x00603150      0x00000000      0x00000000      0x00000000
0x603130 <n22>: 0x00000032      0x00000000      0x00603170      0x00000000
0x603140 <n22+16>:      0x006031b0      0x00000000      0x00000000      0x00000000
0x603150 <n32>: 0x00000016      0x00000000      0x00603270      0x00000000
0x603160 <n32+16>:      0x00603230      0x00000000      0x00000000      0x00000000
0x603170 <n33>: 0x0000002d      0x00000000      0x006031d0      0x00000000
0x603180 <n33+16>:      0x00603290      0x00000000      0x00000000      0x00000000
0x603190 <n31>: 0x00000006      0x00000000      0x006031f0      0x00000000
0x6031a0 <n31+16>:      0x00603250      0x00000000      0x00000000      0x00000000
0x6031b0 <n34>: 0x0000006b      0x00000000      0x00603210      0x00000000
0x6031c0 <n34+16>:      0x006032b0      0x00000000      0x00000000      0x00000000
0x6031d0 <n45>: 0x00000028      0x00000000      0x00000000      0x00000000
0x6031e0 <n45+16>:      0x00000000      0x00000000      0x00000000      0x00000000
0x6031f0 <n41>: 0x00000001      0x00000000      0x00000000      0x00000000
0x603200 <n41+16>:      0x00000000      0x00000000      0x00000000      0x00000000
0x603210 <n47>: 0x00000063      0x00000000      0x00000000      0x00000000
0x603220 <n47+16>:      0x00000000      0x00000000      0x00000000      0x00000000
0x603230 <n44>: 0x00000023      0x00000000      0x00000000      0x00000000
0x603240 <n44+16>:      0x00000000      0x00000000      0x00000000      0x00000000
0x603250 <n42>: 0x00000007      0x00000000      0x00000000      0x00000000
0x603260 <n42+16>:      0x00000000      0x00000000      0x00000000      0x00000000
0x603270 <n43>: 0x00000014      0x00000000      0x00000000      0x00000000
0x603280 <n43+16>:      0x00000000      0x00000000      0x00000000      0x00000000
0x603290 <n46>: 0x0000002f      0x00000000      0x00000000      0x00000000
0x6032a0 <n46+16>:      0x00000000      0x00000000      0x00000000      0x00000000
0x6032b0 <n48>: 0x000003e9      0x00000000      0x00000000      0x00000000
0x6032c0 <n48+16>:      0x00000000      0x00000000      0x00000000      0x00000000
```

To be empirical (from the experience of the linked list!), we guess that these n1 to n48 are nodes of a binary tree - the first 32-bit integer is its value, the third is the addresss to the left child, and the fifth the right one. The branches of `fun7()` are just selection of which child is the argument of inner function. Here may be several approaches, but I choosed to brute force. Quickly I got the final (and unique?) answer: 0x16 (22). The route of node selection is: 0x6030f0 -> 0x603110 -> 0x603150.

- Solution: `22`

## Short Review of This Lab

I am almost new to assembly and processor architecture, even lacking knowledge of registers (like %eax, %edx, %esi) until several days before (at that time I tried some RISC-V assembly on microcontrollers for the first time). I found x86 assembly is quite harder than RISC-V, so I still put enormous effort into it.

This lab, from my perspective, comprises a lot of knowledge of programming, foundation of assembly language, function and data structures, which helps me realize my weaknesses of computer science. Also I haven't master GDB and other tools like reverse engineering (I mean decompilers), hence I feel difficult to adapt to debugging.

Certainly it is *very* challenging, no, **exceedingly**, **tremendously** lol. However, it is its high difficulty that makes me excited and arouses my enthusiasm and energy of continuously learning computer science. I think that my two-day high intensity of working pays off.

At last, I sincerely hope you can enjoy this lab, and I believe you can.

## An Easter Egg

If you press Ctrl+C to send SIGINT to the binary bomb when it awaits your input from STDIN, it will show a short message and exits:

```text
So you think you can stop the bomb with ctrl-c, do you?
Well...OK. :-)
```

[^1] I was inspired by Viseator from his/her [blog](https://www.viseator.com/2017/06/21/CS_APP_BombLab).

