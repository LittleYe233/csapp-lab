/* 
 * CS:APP Data Lab 
 * 
 * <Please put your name and userid here>
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting if the shift amount
     is less than 0 or greater than 31.


EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implement floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants. You can use any arithmetic,
logical, or comparison operations on int or unsigned data.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operations (integer, logical,
     or comparison) that you are allowed to use for your implementation
     of the function.  The max operator count is checked by dlc.
     Note that assignment ('=') is not counted; you may use as many of
     these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
//1
/* 
 * bitXor - x^y using only ~ and & 
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y) {
  return ~(~(~x & y) & ~(~y & x));
}
/* 
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
  return ~(1 << 31) + 1;
}
//2
/*
 * isTmax - returns 1 if x is the maximum, two's complement number,
 *     and 0 otherwise 
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */
int isTmax(int x) {
  /*
   * (1) Tmax + 1 == Tmin;
   * (2) The least (n-1) bits of Tmax is 1, while only the highest bit of Tmin
   * is 1;
   * (3) Use XOR instead of OR to pass the test of -2 (0xFFFFFFFE);
   * (4) Add !!t check to pass the test of -1 (0xFFFFFFFF).
   */
  int t = x + 1;
  return !!t & !(~(x ^ t));
}
/* 
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   where bits are numbered from 0 (least significant) to 31 (most significant)
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allOddBits(int x) {
  int a = x & (x >> 16);
  int b = a & (a >> 8);
  return !((~b) & 0xAA);
}
/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
  /* The basic of two's complement! */
  return (~x) + 1;
}
//3
/* 
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x) {
  /*
   * (1) x is an ASCII digit iff.
   *     (a) Bit[7,6,3] = 0, Bit[5,4] = 1; or
   *     (b) Bit[7,6,2,1] = 0, Bit[5,4,3] = 1.
   * (2) (A == 0 && B == 0) == !(A | B).
   * (3) Specified bits are zeros: !(x & mask).
   * (4) Specified bits are ones: !((~x) & mask).
   * (5) You should consider the numbers out of range [0, 256)!
   */
  int t = ~x;
  int A = x & 0xC8;
  int B = t & 0x30;
  int C = x & 0xC6;
  int D = t & 0x38;
  return (!(x >> 8)) & ((!(A | B)) | (!(C | D)));
}
/* 
 * conditional - same as x ? y : z 
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z) {
  /*
   * The expression of t is genereated by ChatGPT o1-preview.
   * t is a bit mask that becomes 0xFFFFFFFF if x is non-zero while 0x0 if x is
   * zero.
   */
  int t = (!x) + (~0);
  return (t & y) | ((~t) & z);
}
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
  /*
   * (1) t == x - y;
   * (2) x <= y iff.
   *     (a) signs of x and y differ (sx ^ sy) and sx == 1, or
   *     (b) t <= 0;
   * (3) Should AND 1 when check sign bits.
   */
  // Extract sign bits
  int sx = (x >> 31) & 1;
  int sy = (y >> 31) & 1;
  // Now assume that there won't be any overflow
  int t = x + ((~y) + 1);
  int st = (t >> 31) & 1;
  // Check whether t is zero (1) or not (0)
  int z = !t;
  int d = sx ^ sy;
  return (d & sx) | ((~d) & (z | st));
}
//4
/* 
 * logicalNeg - implement the ! operator, using all of 
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int logicalNeg(int x) {
  return (~(x | (~x + 1)) >> 31) & 1;
}
/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 */
int howManyBits(int x) {
  /*
   * This solution is generated by ChatGPT o1-preview. Explanation is attached
   * to important code pieces below.
   */

  // Get sign bit, but it becomes -1 when x is negative
  int s = x >> 31;

  // Get absolute value of x
  unsigned int a = (x ^ s) + (~s + 1);

  // Check whether x is negative (1) or not (0)
  int e = s & 1;

  // Check whether a is a power of two (1) or not (0)
  int w = !(a & (a - 1)) & !!a;

  // Check whether x is Tmin (1) or not (0)
  //int i = e & !(x & ~(1 << 31));
  // Refer to conditional()
  //int k = (!i) + (~0);

  // Binary search of MSB
  // n denotes whether the searching segment is non-zero (1) or not (0)
  // m is 2^n if n == 1, otherwise 0
  int t, b = 0, n, m;

  /// Search in most/least 16 bits
  t = (a >> 16) & (~(0xFF << 16)) & (~(0xFF << 24));
  n = !!t;
  m = n << 4;
  b += m;
  //// Shift the absolute value if m>0 (MSB is in the higher segment), otherwise
  //// do nothing (MSB is in the lower segment)
  a >>= m;

  /// Search in most/least 8 bits in the previous segment
  t = a >> 8;
  n = !!t;
  m = n << 3;
  b += m;
  a >>= m;

  /// Search in most/least 4 bits in the previous segment
  t = a >> 4;
  n = !!t;
  m = n << 2;
  b += m;
  a >>= m;

  /// Search in most/least 2 bits in the previous segment
  t = a >> 2;
  n = !!t;
  m = n << 1;
  b += m;
  a >>= m;

  /// Search in most/least 1 bit in the previous segment
  t = a >> 1;
  n = !!t;
  m = n;
  b += m;
  a >>= m;

  //// An edge case of x == 0
  /// Now a is either 1 (x != 0) or 0 (x == 0)
  b += a;

  // Add a sign bit
  b += 1;

  // b should substract 1 if absolute value of x is a power of two and x is
  // negative
  b += (~(e & w) + 1);

  // Final special case for Tmin
  //b = (k & 32) | (~k & b);

  return b;
}
//float
/* 
 * floatScale2 - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatScale2(unsigned uf) {
  // Mantissa part (23 bits in total)
  int k = ~(0xFF << 23);
  unsigned m = uf & k & (~(1 << 31));
  // Exponent part (8 bits in total)
  unsigned e = (uf >> 23) & 0xFF;
  // Extract sign bit
  int s = (uf >> 31) & 1;
  // Check whether uf is NaN/infinty or not
  // You should AND 0xFF mask here because e is a 32-bit integer
  if ((~e) & 0xFF) {
    // Check whether uf is normalized or not
    if (e) {
      e += 1;
      uf = (uf & k) | (e << 23);
    } else {
      m <<= 1;
      uf = (s << 31) | m;
    }
  }
  return uf;
}
/* 
 * floatFloat2Int - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int floatFloat2Int(unsigned uf) {
  unsigned m = uf & (~(0xFF << 23)) & (~(1 << 31));
  unsigned e = (uf >> 23) & 0xFF;
  unsigned s = uf >> 31;
  int a;
  // If either all bits of e are 1 (NaN or infinity) or e is too large
  // (overflow), returns
  if (!(~e) || e >= 158) {
    a = 0x80000000u;
  }
  // If e is too small, returns 0
  else if (e < 127) {
    a = 0;
  }
  // Normal manipulation
  else {
    // Add implicit 1
    m |= (1 << 23);
    // Shift m to its actual integer value
    if (e <= 150) {
      m >>= (150 - e);
    } else {
      m <<= (e - 150);
    }
    // Operate on sign bit
    if (s) {
      a = -m;
    } else {
      a = m;
    }
  }
  return a;
}
/* 
 * floatPower2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 * 
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while 
 *   Max ops: 30 
 *   Rating: 4
 */
unsigned floatPower2(int x) {
  unsigned uf;
  unsigned e;
  if (x < -127) {
    uf = 0u;
  } else if (x > 127) {
    uf = 0x7F800000;
  } else {
    e = x + 127;
    uf = e << 23;
  }
  return uf;
}

// vim: ts=2 sw=2 et:
