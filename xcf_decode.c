/*
 *  xcftools-facing half of the XCF filter - see xcf_decode.h for why it is a
 *  translation unit of its own.
 */

#include <stdlib.h>
#include <string.h>

#include <xcftools.h>
#include <flatten.h>
#include <pixels.h>

#include "xcf_decode.h"

void xcf_decode_free(unsigned char *buffer)
{
	free(buffer);
}

/* xcf2png hands complete_flatspec a guesser so it can pick a colour mode from
 * the image contents. We always want RGB out, so the guesser is never
 * consulted for anything but that. */
static enum out_color_mode xcfdec_guess_rgb(struct FlattenSpec *spec, rgba **allPixels)
{
	(void)spec;
	(void)allPixels;
	return COLOR_RGB;
}

int xcf_decode_rgb(const unsigned char *data, size_t size,
                   unsigned char **out, unsigned int *width, unsigned int *height)
{
	struct FlattenSpec spec;
	rgba **rows;
	unsigned char *rgb;
	unsigned int w, h, x, y;

	/* xcftools reports a malformed file by calling exit(), which in a side
	 * module tears down the whole session, so the signature is checked here
	 * first: that is the failure that actually happens in practice (a file of
	 * the wrong type), and it is turned into a return code instead. */
	if (size < 14 || memcmp(data, "gimp xcf ", 9) != 0)
		return XCF_DEC_ERR_BITSTREAM;

	/* xcftools reads the document through these two globals rather than a
	 * handle, which is exactly what lets the packet be used in place: no copy,
	 * and io-unix.c (mmap, open, the gz/bz2 unzipper pipes) stays out of the
	 * build entirely. The cast drops const because the globals are typed
	 * mutable; nothing on the decode path writes through them. */
	xcf_file = (uint8_t *)data;
	xcf_length = (xcfptr_t)size;

	/* Take layer names as the UTF-8 they already are. Otherwise xcftools tries
	 * to transcode them to the local charset through iconv, which does not
	 * exist here - and the names are never used anyway. */
	use_utf8 = 1;

	getBasicXcfInfo();
	initColormap();

	init_flatspec(&spec);
	spec.out_color_mode = COLOR_RGB;
	/* Flatten onto an opaque canvas: the output pid is RGB, so there is no
	 * alpha to carry and partial transparency has to be resolved here. */
	spec.partial_transparency_mode = FORBID_PARTIAL_TRANSPARENCY;
	spec.default_pixel = FORCE_ALPHA_CHANNEL;
	complete_flatspec(&spec, xcfdec_guess_rgb);

	w = (unsigned int)spec.dim.width;
	h = (unsigned int)spec.dim.height;
	if (!w || !h)
		return XCF_DEC_ERR_BITSTREAM;

	rows = flattenAll(&spec);
	if (!rows)
		return XCF_DEC_ERR_MEMORY;

	rgb = (unsigned char *)malloc((size_t)w * h * 3);
	if (!rgb)
		return XCF_DEC_ERR_MEMORY;

	for (y = 0; y < h; y++)
	{
		const rgba *src = rows[y];
		unsigned char *dst = rgb + (size_t)y * w * 3;
		for (x = 0; x < w; x++)
		{
			/* pixels.h only publishes the shifts, not per-component accessors:
			 * an rgba is alpha in the low byte, then R, G and B. Alpha is
			 * dropped - the flatten step above was told to composite it away. */
			dst[x * 3] = (unsigned char)(src[x] >> RED_SHIFT);
			dst[x * 3 + 1] = (unsigned char)(src[x] >> GREEN_SHIFT);
			dst[x * 3 + 2] = (unsigned char)(src[x] >> BLUE_SHIFT);
		}
	}

	*out = rgb;
	*width = w;
	*height = h;
	return XCF_DEC_OK;
}
