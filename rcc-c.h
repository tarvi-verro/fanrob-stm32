
/**
 * sleep_busy() - busyloop for given time
 * ns:		time to spend
 *
 * There is also additional time spent for function entrance and exit and also
 * on additional 3 or less cycles when %ns is not a multiple of
 * sleep_granuality.
 */
extern void sleep_busy(unsigned ns);

extern const unsigned sleep_substract_loop_cycles;

/**
 * sleep_granuality_ns - the minimum step for sleep_busy()
 *
 * This value should be set accordingly:
 *
 * 	floor(10^9 * sslc / sysclk_frequency)
 *
 * where sslc is short for sleep_substract_loop_cycles.
 *
 * Ideally this value should be an exact integer, otherwise round it down so
 * that sleep_busy waits _at least_ the specified amount of time.
 *
 * At the default 8 MHz this is 500 nanoseconds.
 */
extern unsigned sleep_granuality_ns;
