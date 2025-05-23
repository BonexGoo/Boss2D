/* $Id: tif_packbits.c,v 1.13.2.2 2010-06-08 18:50:42 bfriesen Exp $ */

/*
 * Copyright (c) 1988-1997 Sam Leffler
 * Copyright (c) 1991-1997 Silicon Graphics, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and 
 * its documentation for any purpose is hereby granted without fee, provided
 * that (i) the above copyright notices and this permission notice appear in
 * all copies of the software and related documentation, and (ii) the names of
 * Sam Leffler and Silicon Graphics may not be used in any advertising or
 * publicity relating to the software without the specific, prior written
 * permission of Sam Leffler and Silicon Graphics.
 * 
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  
 * 
 * IN NO EVENT SHALL SAM LEFFLER OR SILICON GRAPHICS BE LIABLE FOR
 * ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF 
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE 
 * OF THIS SOFTWARE.
 */

#include BOSS_OPENALPR_U_tiffiop_h //original-code:"tiffiop.h"
#ifdef PACKBITS_SUPPORT
/*
 * TIFF Library.
 *
 * PackBits Compression Algorithm Support
 */
#include <stdio.h>

static int
PackBitsPreEncode(TIFF* tif, tsample_t s)
{
	(void) s;

        if (!(tif->tif_data = (tidata_t)_TIFFmalloc(sizeof(tsize_t))))
		return (0);
	/*
	 * Calculate the scanline/tile-width size in bytes.
	 */
	if (isTiled(tif))
		*(tsize_t*)tif->tif_data = TIFFTileRowSize(tif);
	else
		*(tsize_t*)tif->tif_data = TIFFScanlineSize(tif);
	return (1);
}

static int
PackBitsPostEncode(TIFF* tif)
{
        if (tif->tif_data)
            _TIFFfree(tif->tif_data);
	return (1);
}

/*
 * NB: tidata is the type representing *(tidata_t);
 *     if tidata_t is made signed then this type must
 *     be adjusted accordingly.
 */
typedef unsigned char tidata;

/*
 * Encode a run of pixels.
 */
static int
PackBitsEncode(TIFF* tif, tidata_t buf, tsize_t cc, tsample_t s)
{
	unsigned char* bp = (unsigned char*) buf;
	tidata_t op, ep, lastliteral;
	long n, slop;
	int b;
	enum { BASE, LITERAL, RUN, LITERAL_RUN } state;

	(void) s;
	op = tif->tif_rawcp;
	ep = tif->tif_rawdata + tif->tif_rawdatasize;
	state = BASE;
	lastliteral = 0;
	while (cc > 0) {
		/*
		 * Find the longest string of identical bytes.
		 */
		b = *bp++, cc--, n = 1;
		for (; cc > 0 && b == *bp; cc--, bp++)
			n++;
	again:
		if (op + 2 >= ep) {		/* insure space for new data */
			/*
			 * Be careful about writing the last
			 * literal.  Must write up to that point
			 * and then copy the remainder to the
			 * front of the buffer.
			 */
			if (state == LITERAL || state == LITERAL_RUN) {
				slop = op - lastliteral;
				tif->tif_rawcc += lastliteral - tif->tif_rawcp;
				if (!TIFFFlushData1(tif))
					return (-1);
				op = tif->tif_rawcp;
				while (slop-- > 0)
					*op++ = *lastliteral++;
				lastliteral = tif->tif_rawcp;
			} else {
				tif->tif_rawcc += op - tif->tif_rawcp;
				if (!TIFFFlushData1(tif))
					return (-1);
				op = tif->tif_rawcp;
			}
		}
		switch (state) {
		case BASE:		/* initial state, set run/literal */
			if (n > 1) {
				state = RUN;
				if (n > 128) {
					*op++ = (tidata) -127;
					*op++ = (tidataval_t) b;
					n -= 128;
					goto again;
				}
				*op++ = (tidataval_t)(-(n-1));
				*op++ = (tidataval_t) b;
			} else {
				lastliteral = op;
				*op++ = 0;
				*op++ = (tidataval_t) b;
				state = LITERAL;
			}
			break;
		case LITERAL:		/* last object was literal string */
			if (n > 1) {
				state = LITERAL_RUN;
				if (n > 128) {
					*op++ = (tidata) -127;
					*op++ = (tidataval_t) b;
					n -= 128;
					goto again;
				}
				*op++ = (tidataval_t)(-(n-1));	/* encode run */
				*op++ = (tidataval_t) b;
			} else {			/* extend literal */
				if (++(*lastliteral) == 127)
					state = BASE;
				*op++ = (tidataval_t) b;
			}
			break;
		case RUN:		/* last object was run */
			if (n > 1) {
				if (n > 128) {
					*op++ = (tidata) -127;
					*op++ = (tidataval_t) b;
					n -= 128;
					goto again;
				}
				*op++ = (tidataval_t)(-(n-1));
				*op++ = (tidataval_t) b;
			} else {
				lastliteral = op;
				*op++ = 0;
				*op++ = (tidataval_t) b;
				state = LITERAL;
			}
			break;
		case LITERAL_RUN:	/* literal followed by a run */
			/*
			 * Check to see if previous run should
			 * be converted to a literal, in which
			 * case we convert literal-run-literal
			 * to a single literal.
			 */
			if (n == 1 && op[-2] == (tidata) -1 &&
			    *lastliteral < 126) {
				state = (((*lastliteral) += 2) == 127 ?
				    BASE : LITERAL);
				op[-2] = op[-1];	/* replicate */
			} else
				state = RUN;
			goto again;
		}
	}
	tif->tif_rawcc += op - tif->tif_rawcp;
	tif->tif_rawcp = op;
	return (1);
}

/*
 * Encode a rectangular chunk of pixels.  We break it up
 * into row-sized pieces to insure that encoded runs do
 * not span rows.  Otherwise, there can be problems with
 * the decoder if data is read, for example, by scanlines
 * when it was encoded by strips.
 */
static int
PackBitsEncodeChunk(TIFF* tif, tidata_t bp, tsize_t cc, tsample_t s)
{
	tsize_t rowsize = *(tsize_t*)tif->tif_data;

	while ((long)cc > 0) {
		int	chunk = rowsize;
		
		if( cc < chunk )
		    chunk = cc;

		if (PackBitsEncode(tif, bp, chunk, s) < 0)
		    return (-1);
		bp += chunk;
		cc -= chunk;
	}
	return (1);
}

static int
PackBitsDecode(TIFF* tif, tidata_t op, tsize_t occ, tsample_t s)
{
	char *bp;
	tsize_t cc;
	long n;
	int b;

	(void) s;
	bp = (char*) tif->tif_rawcp;
	cc = tif->tif_rawcc;
	while (cc > 0 && (long)occ > 0) {
		n = (long) *bp++, cc--;
		/*
		 * Watch out for compilers that
		 * don't sign extend chars...
		 */
		if (n >= 128)
			n -= 256;
		if (n < 0) {		/* replicate next byte -n+1 times */
			if (n == -128)	/* nop */
				continue;
                        n = -n + 1;
                        if( occ < n )
                        {
							TIFFWarningExt(tif->tif_clientdata, tif->tif_name,
                                        "PackBitsDecode: discarding %ld bytes "
                                        "to avoid buffer overrun",
                                        n - occ);
                            n = occ;
                        }
			occ -= n;
			b = *bp++, cc--;
			while (n-- > 0)
				*op++ = (tidataval_t) b;
		} else {		/* copy next n+1 bytes literally */
			if (occ < n + 1)
                        {
                            TIFFWarningExt(tif->tif_clientdata, tif->tif_name,
                                        "PackBitsDecode: discarding %ld bytes "
                                        "to avoid buffer overrun",
                                        n - occ + 1);
                            n = occ - 1;
                        }
                        _TIFFmemcpy(op, bp, ++n);
			op += n; occ -= n;
			bp += n; cc -= n;
		}
	}
	tif->tif_rawcp = (tidata_t) bp;
	tif->tif_rawcc = cc;
	if (occ > 0) {
		TIFFErrorExt(tif->tif_clientdata, tif->tif_name,
		    "PackBitsDecode: Not enough data for scanline %ld",
		    (long) tif->tif_row);
		return (0);
	}
	return (1);
}

int
TIFFInitPackBits(TIFF* tif, int scheme)
{
	(void) scheme;
	tif->tif_decoderow = PackBitsDecode;
	tif->tif_decodestrip = PackBitsDecode;
	tif->tif_decodetile = PackBitsDecode;
	tif->tif_preencode = PackBitsPreEncode;
        tif->tif_postencode = PackBitsPostEncode;
	tif->tif_encoderow = PackBitsEncode;
	tif->tif_encodestrip = PackBitsEncodeChunk;
	tif->tif_encodetile = PackBitsEncodeChunk;
	return (1);
}
#endif /* PACKBITS_SUPPORT */

/* vim: set ts=8 sts=8 sw=8 noet: */
/*
 * Local Variables:
 * mode: c
 * c-basic-offset: 8
 * fill-column: 78
 * End:
 */
