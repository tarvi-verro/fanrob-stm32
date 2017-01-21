
/**
 * sleep_busy() - busyloop for given time
 * ns:		time to spend
 *
 * There is also additional time spent for function entrance and exit and also
 * on additional 3 or less cycles when %ns is not a multiple of
 * sleep_granuality.
 */
extern void sleep_busy(unsigned ns);

/* At the default 8 MHz this is 500 nanoseconds. */
extern const unsigned sleep_granuality_ns;

