/* $Id: tif_color.c,v 1.12.2.1 2010-06-08 18:50:41 bfriesen Exp $ */

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

/*
 * CIE L*a*b* to CIE XYZ and CIE XYZ to RGB conversion routines are taken
 * from the VIPS library (http://www.vips.ecs.soton.ac.uk) with
 * the permission of John Cupitt, the VIPS author.
 */

/*
 * TIFF Library.
 *
 * Color space conversion routines.
 */

#include BOSS_OPENALPR_U_tiffiop_h //original-code:"tiffiop.h"
#include <math.h>

/*
 * Convert color value from the CIE L*a*b* 1976 space to CIE XYZ.
 */
void
TIFFCIELabToXYZ(TIFFCIELabToRGB *cielab, uint32 l, int32 a, int32 b,
		float *X, float *Y, float *Z)
{
	float L = (float)l * 100.0F / 255.0F;
	float cby, tmp;

	if( L < 8.856F ) {
		*Y = (L * cielab->Y0) / 903.292F;
		cby = 7.787F * (*Y / cielab->Y0) + 16.0F / 116.0F;
	} else {
		cby = (L + 16.0F) / 116.0F;
		*Y = cielab->Y0 * cby * cby * cby;
	}

	tmp = (float)a / 500.0F + cby;
	if( tmp < 0.2069F )
		*X = cielab->X0 * (tmp - 0.13793F) / 7.787F;
	else    
		*X = cielab->X0 * tmp * tmp * tmp;

	tmp = cby - (float)b / 200.0F;
	if( tmp < 0.2069F )
		*Z = cielab->Z0 * (tmp - 0.13793F) / 7.787F;
	else    
		*Z = cielab->Z0 * tmp * tmp * tmp;
}

#define RINT(R) ((uint32)((R)>0?((R)+0.5):((R)-0.5)))
/*
 * Convert color value from the XYZ space to RGB.
 */
void
TIFFXYZToRGB(TIFFCIELabToRGB *cielab, float X, float Y, float Z,
	     uint32 *r, uint32 *g, uint32 *b)
{
	int i;
	float Yr, Yg, Yb;
	float *matrix = &cielab->display.d_mat[0][0];

	/* Multiply through the matrix to get luminosity values. */
	Yr =  matrix[0] * X + matrix[1] * Y + matrix[2] * Z;
	Yg =  matrix[3] * X + matrix[4] * Y + matrix[5] * Z;
	Yb =  matrix[6] * X + matrix[7] * Y + matrix[8] * Z;

	/* Clip input */
	Yr = TIFFmax(Yr, cielab->display.d_Y0R);
	Yg = TIFFmax(Yg, cielab->display.d_Y0G);
	Yb = TIFFmax(Yb, cielab->display.d_Y0B);

	/* Avoid overflow in case of wrong input values */
	Yr = TIFFmin(Yr, cielab->display.d_YCR);
	Yg = TIFFmin(Yg, cielab->display.d_YCG);
	Yb = TIFFmin(Yb, cielab->display.d_YCB);

	/* Turn luminosity to colour value. */
	i = (int)((Yr - cielab->display.d_Y0R) / cielab->rstep);
	i = TIFFmin(cielab->range, i);
	*r = RINT(cielab->Yr2r[i]);

	i = (int)((Yg - cielab->display.d_Y0G) / cielab->gstep);
	i = TIFFmin(cielab->range, i);
	*g = RINT(cielab->Yg2g[i]);

	i = (int)((Yb - cielab->display.d_Y0B) / cielab->bstep);
	i = TIFFmin(cielab->range, i);
	*b = RINT(cielab->Yb2b[i]);

	/* Clip output. */
	*r = TIFFmin(*r, cielab->display.d_Vrwr);
	*g = TIFFmin(*g, cielab->display.d_Vrwg);
	*b = TIFFmin(*b, cielab->display.d_Vrwb);
}
#undef RINT

/* 
 * Allocate conversion state structures and make look_up tables for
 * the Yr,Yb,Yg <=> r,g,b conversions.
 */
int
TIFFCIELabToRGBInit(TIFFCIELabToRGB* cielab,
		    TIFFDisplay *display, float *refWhite)
{
	int i;
	double gamma;

	cielab->range = CIELABTORGB_TABLE_RANGE;

	_TIFFmemcpy(&cielab->display, display, sizeof(TIFFDisplay));

	/* Red */
	gamma = 1.0 / cielab->display.d_gammaR ;
	cielab->rstep =
		(cielab->display.d_YCR - cielab->display.d_Y0R)	/ cielab->range;
	for(i = 0; i <= cielab->range; i++) {
		cielab->Yr2r[i] = cielab->display.d_Vrwr
		    * ((float)pow((double)i / cielab->range, gamma));
	}

	/* Green */
	gamma = 1.0 / cielab->display.d_gammaG ;
	cielab->gstep =
	    (cielab->display.d_YCR - cielab->display.d_Y0R) / cielab->range;
	for(i = 0; i <= cielab->range; i++) {
		cielab->Yg2g[i] = cielab->display.d_Vrwg
		    * ((float)pow((double)i / cielab->range, gamma));
	}

	/* Blue */
	gamma = 1.0 / cielab->display.d_gammaB ;
	cielab->bstep =
	    (cielab->display.d_YCR - cielab->display.d_Y0R) / cielab->range;
	for(i = 0; i <= cielab->range; i++) {
		cielab->Yb2b[i] = cielab->display.d_Vrwb
		    * ((float)pow((double)i / cielab->range, gamma));
	}

	/* Init reference white point */
	cielab->X0 = refWhite[0];
	cielab->Y0 = refWhite[1];
	cielab->Z0 = refWhite[2];

	return 0;
}

/* 
 * Convert color value from the YCbCr space to CIE XYZ.
 * The colorspace conversion algorithm comes from the IJG v5a code;
 * see below for more information on how it works.
 */
#define	SHIFT			16
#define	FIX(x)			((int32)((x) * (1L<<SHIFT) + 0.5))
#define	ONE_HALF		((int32)(1<<(SHIFT-1)))
#define	Code2V(c, RB, RW, CR)	((((c)-(int32)(RB))*(float)(CR))/(float)(((RW)-(RB)) ? ((RW)-(RB)) : 1))
#define	CLAMP(f,min,max)	((f)<(min)?(min):(f)>(max)?(max):(f))
#define HICLAMP(f,max)		((f)>(max)?(max):(f))

void
TIFFYCbCrtoRGB(TIFFYCbCrToRGB *ycbcr, uint32 Y, int32 Cb, int32 Cr,
	       uint32 *r, uint32 *g, uint32 *b)
{
	/* XXX: Only 8-bit YCbCr input supported for now */
	Y = HICLAMP(Y, 255), Cb = CLAMP(Cb, 0, 255), Cr = CLAMP(Cr, 0, 255);

	*r = ycbcr->clamptab[ycbcr->Y_tab[Y] + ycbcr->Cr_r_tab[Cr]];
	*g = ycbcr->clamptab[ycbcr->Y_tab[Y]
	    + (int)((ycbcr->Cb_g_tab[Cb] + ycbcr->Cr_g_tab[Cr]) >> SHIFT)];
	*b = ycbcr->clamptab[ycbcr->Y_tab[Y] + ycbcr->Cb_b_tab[Cb]];
}

/*
 * Initialize the YCbCr->RGB conversion tables.  The conversion
 * is done according to the 6.0 spec:
 *
 *    R = Y + Cr*(2 - 2*LumaRed)
 *    B = Y + Cb*(2 - 2*LumaBlue)
 *    G =   Y
 *        - LumaBlue*Cb*(2-2*LumaBlue)/LumaGreen
 *        - LumaRed*Cr*(2-2*LumaRed)/LumaGreen
 *
 * To avoid floating point arithmetic the fractional constants that
 * come out of the equations are represented as fixed point values
 * in the range 0...2^16.  We also eliminate multiplications by
 * pre-calculating possible values indexed by Cb and Cr (this code
 * assumes conversion is being done for 8-bit samples).
 */
int
TIFFYCbCrToRGBInit(TIFFYCbCrToRGB* ycbcr, float *luma, float *refBlackWhite)
{
    TIFFRGBValue* clamptab;
    int i;
    
#define LumaRed	    luma[0]
#define LumaGreen   luma[1]
#define LumaBlue    luma[2]

    clamptab = (TIFFRGBValue*)(
	(tidata_t) ycbcr+TIFFroundup(sizeof (TIFFYCbCrToRGB), sizeof (long)));
    _TIFFmemset(clamptab, 0, 256);		/* v < 0 => 0 */
    ycbcr->clamptab = (clamptab += 256);
    for (i = 0; i < 256; i++)
	clamptab[i] = (TIFFRGBValue) i;
    _TIFFmemset(clamptab+256, 255, 2*256);	/* v > 255 => 255 */
    ycbcr->Cr_r_tab = (int*) (clamptab + 3*256);
    ycbcr->Cb_b_tab = ycbcr->Cr_r_tab + 256;
    ycbcr->Cr_g_tab = (int32*) (ycbcr->Cb_b_tab + 256);
    ycbcr->Cb_g_tab = ycbcr->Cr_g_tab + 256;
    ycbcr->Y_tab = ycbcr->Cb_g_tab + 256;

    { float f1 = 2-2*LumaRed;		int32 D1 = FIX(f1);
      float f2 = LumaRed*f1/LumaGreen;	int32 D2 = -FIX(f2);
      float f3 = 2-2*LumaBlue;		int32 D3 = FIX(f3);
      float f4 = LumaBlue*f3/LumaGreen;	int32 D4 = -FIX(f4);
      int x;

#undef LumaBlue
#undef LumaGreen
#undef LumaRed
      
      /*
       * i is the actual input pixel value in the range 0..255
       * Cb and Cr values are in the range -128..127 (actually
       * they are in a range defined by the ReferenceBlackWhite
       * tag) so there is some range shifting to do here when
       * constructing tables indexed by the raw pixel data.
       */
      for (i = 0, x = -128; i < 256; i++, x++) {
	    int32 Cr = (int32)Code2V(x, refBlackWhite[4] - 128.0F,
			    refBlackWhite[5] - 128.0F, 127);
	    int32 Cb = (int32)Code2V(x, refBlackWhite[2] - 128.0F,
			    refBlackWhite[3] - 128.0F, 127);

	    ycbcr->Cr_r_tab[i] = (int32)((D1*Cr + ONE_HALF)>>SHIFT);
	    ycbcr->Cb_b_tab[i] = (int32)((D3*Cb + ONE_HALF)>>SHIFT);
	    ycbcr->Cr_g_tab[i] = D2*Cr;
	    ycbcr->Cb_g_tab[i] = D4*Cb + ONE_HALF;
	    ycbcr->Y_tab[i] =
		    (int32)Code2V(x + 128, refBlackWhite[0], refBlackWhite[1], 255);
      }
    }

    return 0;
}
#undef	HICLAMP
#undef	CLAMP
#undef	Code2V
#undef	SHIFT
#undef	ONE_HALF
#undef	FIX

/* vim: set ts=8 sts=8 sw=8 noet: */
/*
 * Local Variables:
 * mode: c
 * c-basic-offset: 8
 * fill-column: 78
 * End:
 */
