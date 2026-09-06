/*
 *  Minimal C surface over the xcftools core.
 *
 *  xcftools ships as three command-line converters rather than a library, and
 *  its headers collide with GPAC's (both define a Bool-like set of names and
 *  xcftools.h pulls in its own config.h). The parts that actually decode -
 *  xcf-general, pixels, flatten, flatspec and friends - are compiled into
 *  libxcftools.a and reached only from xcf_decode.c, so the filter itself
 *  never includes an xcftools header.
 */

#ifndef _XCF_DECODE_H_
#define _XCF_DECODE_H_

#include <stddef.h>

/* Flattens a whole XCF document to a tightly packed 24-bit RGB buffer,
 * compositing the visible layers over the canvas.
 *
 * On success returns 0, stores a malloc()'d buffer in *out (released with
 * xcf_decode_free) and its dimensions in *width and *height. */
int xcf_decode_rgb(const unsigned char *data, size_t size,
                   unsigned char **out, unsigned int *width, unsigned int *height);

void xcf_decode_free(unsigned char *buffer);

#define XCF_DEC_OK              0
#define XCF_DEC_ERR_MEMORY     -1
#define XCF_DEC_ERR_BITSTREAM  -2   /* not an XCF file */

#endif
