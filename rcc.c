#include "rcc-c.h"

// These values are based on default startup system clock state.
static const unsigned t_clock_ns = 125; // 8 MHz = 1/(8 * 10⁶) s = 0.000000125 s = 125 ns
const unsigned sleep_granuality_ns = 125 * 4; // 500 ns; substract-loop is 4 cycles

void sleep_busy(unsigned ns)
{
	/*
	 * With -O3 on a specific gcc and sleep_granuality=125*4 the boilerplate becomes:
	 *
	 * instruction	cycles taken
	 * movs		1
	 * lsls		1
	 * subs		1 (useful waste of time)
	 * bhi.n	3 (useful waste of time) or 1
	 * bx		3
	 *
	 * Total of 6 cycles extra time wasted. (0.75 µs at 8 Mhz)
	 *
	 * 14+ cycles with -O0. (about 2 µs at 8 MHz)
	 */
	/*
	 * How many cycles instructions take are specified at:
	 * http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0337h/CHDDIGAC.html
	 *
	 * instr.	cycles
	 * sub		1
	 * b{--}	1 or 3 when branching
	 *
	 * This means that a single time-waste run that substracts and jumps
	 * back will take 4 cycles.
	 */
	/*
	 * The bunch of asm does something like:
	 *
	 *	while (iterations > 0)
	 *		iterations -= granuality;
	 *
	 * But the amount of cycles it will take is transparent.
	 */
	asm volatile (
		".syntax unified\n"
		"start:\n\t"
		"subs	%[i], %[i], %[g]\n\t"
		"bhi	start\n" // ($[i] > 0) then jump,
				 // unsigned comparison, also jumps on overflow
		: //[i] "=r" (ns) // This line has no effect if following
					// C code doesn't use the variable.
					// In the debugger try instead 'i r'.
		: [i] "r" (ns), [g] "r" (sleep_granuality_ns)
		: "cc"); // Modifies n,z,c,v flags in the APSR register.
	/*
	 * TODO: This function could possibly be made more accurate with some
	 * nop instructions and jumping to the correct nop.
	 *
	 * Calculating the remainder is an issue.
	 */
	return;
}

