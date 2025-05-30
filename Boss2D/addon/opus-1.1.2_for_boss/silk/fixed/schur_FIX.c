/***********************************************************************
Copyright (c) 2006-2011, Skype Limited. All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
- Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
- Neither the name of Internet Society, IETF or IETF Trust, nor the
names of specific contributors, may be used to endorse or promote
products derived from this software without specific prior written
permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
***********************************************************************/

#ifdef HAVE_CONFIG_H
#include BOSS_OPUS_U_config_h //original-code:"config.h"
#endif

#include BOSS_OPUS_U_SigProc_FIX_h //original-code:"SigProc_FIX.h"

/* Faster than schur64(), but much less accurate.                       */
/* uses SMLAWB(), requiring armv5E and higher.                          */
opus_int32 silk_schur(                              /* O    Returns residual energy                                     */
    opus_int16                  *rc_Q15,            /* O    reflection coefficients [order] Q15                         */
    const opus_int32            *c,                 /* I    correlations [order+1]                                      */
    const opus_int32            order               /* I    prediction order                                            */
)
{
    opus_int        k, n, lz;
    opus_int32    C[ SILK_MAX_ORDER_LPC + 1 ][ 2 ];
    opus_int32    Ctmp1, Ctmp2, rc_tmp_Q15;

    silk_assert( order==6||order==8||order==10||order==12||order==14||order==16 );

    /* Get number of leading zeros */
    lz = silk_CLZ32( c[ 0 ] );

    /* Copy correlations and adjust level to Q30 */
    if( lz < 2 ) {
        /* lz must be 1, so shift one to the right */
        for( k = 0; k < order + 1; k++ ) {
            C[ k ][ 0 ] = C[ k ][ 1 ] = silk_RSHIFT( c[ k ], 1 );
        }
    } else if( lz > 2 ) {
        /* Shift to the left */
        lz -= 2;
        for( k = 0; k < order + 1; k++ ) {
            C[ k ][ 0 ] = C[ k ][ 1 ] = silk_LSHIFT( c[ k ], lz );
        }
    } else {
        /* No need to shift */
        for( k = 0; k < order + 1; k++ ) {
            C[ k ][ 0 ] = C[ k ][ 1 ] = c[ k ];
        }
    }

    for( k = 0; k < order; k++ ) {
        /* Check that we won't be getting an unstable rc, otherwise stop here. */
        if (silk_abs_int32(C[ k + 1 ][ 0 ]) >= C[ 0 ][ 1 ]) {
           if ( C[ k + 1 ][ 0 ] > 0 ) {
              rc_Q15[ k ] = -SILK_FIX_CONST( .99f, 15 );
           } else {
              rc_Q15[ k ] = SILK_FIX_CONST( .99f, 15 );
           }
           k++;
           break;
        }

        /* Get reflection coefficient */
        rc_tmp_Q15 = -silk_DIV32_16( C[ k + 1 ][ 0 ], silk_max_32( silk_RSHIFT( C[ 0 ][ 1 ], 15 ), 1 ) );

        /* Clip (shouldn't happen for properly conditioned inputs) */
        rc_tmp_Q15 = silk_SAT16( rc_tmp_Q15 );

        /* Store */
        rc_Q15[ k ] = (opus_int16)rc_tmp_Q15;

        /* Update correlations */
        for( n = 0; n < order - k; n++ ) {
            Ctmp1 = C[ n + k + 1 ][ 0 ];
            Ctmp2 = C[ n ][ 1 ];
            C[ n + k + 1 ][ 0 ] = silk_SMLAWB( Ctmp1, silk_LSHIFT( Ctmp2, 1 ), rc_tmp_Q15 );
            C[ n ][ 1 ]         = silk_SMLAWB( Ctmp2, silk_LSHIFT( Ctmp1, 1 ), rc_tmp_Q15 );
        }
    }

    for(; k < order; k++ ) {
       rc_Q15[ k ] = 0;
    }

    /* return residual energy */
    return silk_max_32( 1, C[ 0 ][ 1 ] );
}
