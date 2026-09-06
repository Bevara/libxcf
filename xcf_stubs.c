/*
 *  Symbols the xcftools core references that solver_minimal_1 does not export.
 *  A side module whose imports are not all resolved never instantiates at all -
 *  the filter simply never registers - so each has to be defined here, the
 *  approach libape, libjxr and libpsd already take.
 *
 *  iconv is the interesting one: xcftools uses it only to transcode layer
 *  names out of UTF-8, a path xcf_decode.c disables by setting use_utf8, so
 *  these three exist purely to satisfy the linker. They report failure the way
 *  a system without the requested conversion would, which is a case xcftools
 *  already handles by keeping the string as-is.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

typedef void *iconv_t;

iconv_t iconv_open(const char *tocode, const char *fromcode)
{
	(void)tocode;
	(void)fromcode;
	errno = EINVAL;
	return (iconv_t)-1;
}

size_t iconv(iconv_t cd, char **inbuf, size_t *inbytesleft, char **outbuf, size_t *outbytesleft)
{
	(void)cd; (void)inbuf; (void)inbytesleft; (void)outbuf; (void)outbytesleft;
	errno = EINVAL;
	return (size_t)-1;
}

int iconv_close(iconv_t cd)
{
	(void)cd;
	return 0;
}

/* zlib is linked into the solver, but it exports the inflate entry points
 * without zError, which xcftools only calls to phrase an error message. */
const char *zError(int err)
{
	(void)err;
	return "zlib error";
}

char *strerror(int errnum)
{
	(void)errnum;
	return (char *)"error";
}

/* xcftools uses rand() to dissolve partial transparency; any decent sequence
 * will do. This is the LCG from the C standard's example. */
static unsigned long xcf_rand_state = 1;

int rand(void)
{
	xcf_rand_state = xcf_rand_state * 1103515245 + 12345;
	return (int)((xcf_rand_state / 65536) % 32768);
}

void srand(unsigned int seed)
{
	xcf_rand_state = seed;
}

void __assert_fail(const char *expr, const char *file, unsigned int line, const char *func)
{
	fprintf(stderr, "[XCFDec] assertion failed: %s at %s:%u in %s\n",
	        expr ? expr : "?", file ? file : "?", line, func ? func : "?");
	abort();
}
