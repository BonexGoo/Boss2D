
/* -----------------------------------------------------------------------------------------------------------
Software License for The Fraunhofer FDK AAC Codec Library for Android

?Copyright  1995 - 2013 Fraunhofer-Gesellschaft zur F?derung der angewandten Forschung e.V.
  All rights reserved.

 1.    INTRODUCTION
The Fraunhofer FDK AAC Codec Library for Android ("FDK AAC Codec") is software that implements
the MPEG Advanced Audio Coding ("AAC") encoding and decoding scheme for digital audio.
This FDK AAC Codec software is intended to be used on a wide variety of Android devices.

AAC's HE-AAC and HE-AAC v2 versions are regarded as today's most efficient general perceptual
audio codecs. AAC-ELD is considered the best-performing full-bandwidth communications codec by
independent studies and is widely deployed. AAC has been standardized by ISO and IEC as part
of the MPEG specifications.

Patent licenses for necessary patent claims for the FDK AAC Codec (including those of Fraunhofer)
may be obtained through Via Licensing (www.vialicensing.com) or through the respective patent owners
individually for the purpose of encoding or decoding bit streams in products that are compliant with
the ISO/IEC MPEG audio standards. Please note that most manufacturers of Android devices already license
these patent claims through Via Licensing or directly from the patent owners, and therefore FDK AAC Codec
software may already be covered under those patent licenses when it is used for those licensed purposes only.

Commercially-licensed AAC software libraries, including floating-point versions with enhanced sound quality,
are also available from Fraunhofer. Users are encouraged to check the Fraunhofer website for additional
applications information and documentation.

2.    COPYRIGHT LICENSE

Redistribution and use in source and binary forms, with or without modification, are permitted without
payment of copyright license fees provided that you satisfy the following conditions:

You must retain the complete text of this software license in redistributions of the FDK AAC Codec or
your modifications thereto in source code form.

You must retain the complete text of this software license in the documentation and/or other materials
provided with redistributions of the FDK AAC Codec or your modifications thereto in binary form.
You must make available free of charge copies of the complete source code of the FDK AAC Codec and your
modifications thereto to recipients of copies in binary form.

The name of Fraunhofer may not be used to endorse or promote products derived from this library without
prior written permission.

You may not charge copyright license fees for anyone to use, copy or distribute the FDK AAC Codec
software or your modifications thereto.

Your modified versions of the FDK AAC Codec must carry prominent notices stating that you changed the software
and the date of any change. For modified versions of the FDK AAC Codec, the term
"Fraunhofer FDK AAC Codec Library for Android" must be replaced by the term
"Third-Party Modified Version of the Fraunhofer FDK AAC Codec Library for Android."

3.    NO PATENT LICENSE

NO EXPRESS OR IMPLIED LICENSES TO ANY PATENT CLAIMS, including without limitation the patents of Fraunhofer,
ARE GRANTED BY THIS SOFTWARE LICENSE. Fraunhofer provides no warranty of patent non-infringement with
respect to this software.

You may use this FDK AAC Codec software or modifications thereto only for purposes that are authorized
by appropriate patent licenses.

4.    DISCLAIMER

This FDK AAC Codec software is provided by Fraunhofer on behalf of the copyright holders and contributors
"AS IS" and WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES, including but not limited to the implied warranties
of merchantability and fitness for a particular purpose. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE for any direct, indirect, incidental, special, exemplary, or consequential damages,
including but not limited to procurement of substitute goods or services; loss of use, data, or profits,
or business interruption, however caused and on any theory of liability, whether in contract, strict
liability, or tort (including negligence), arising in any way out of the use of this software, even if
advised of the possibility of such damage.

5.    CONTACT INFORMATION

Fraunhofer Institute for Integrated Circuits IIS
Attention: Audio and Multimedia Departments - FDK AAC LL
Am Wolfsmantel 33
91058 Erlangen, Germany

www.iis.fraunhofer.de/amm
amm-info@iis.fraunhofer.de
----------------------------------------------------------------------------------------------------------- */

/*!
  \file
  \brief  Sbr decoder  
*/
#ifndef __PSDEC_H
#define __PSDEC_H

#include "sbrdecoder.h"



/* This PS decoder implements the baseline version. So it always uses the     */
/* hybrid filter structure for 20 stereo bands and does not implemet IPD/OPD  */
/* synthesis. The baseline version has to support the complete PS bitstream   */
/* syntax. But IPD/OPD data is ignored and set to 0. If 34 stereo band config */
/* is used in the bitstream for IIS/ICC the decoded parameters are mapped to  */
/* 20 stereo bands.                                                           */


#include BOSS_FDKAAC_U_FDK_bitstream_h //original-code:"FDK_bitstream.h"

#include "psdec_hybrid.h"

#define SCAL_HEADROOM                     ( 2 )

#define PS_EXTENSION_SIZE_BITS            ( 4 )
#define PS_EXTENSION_ESC_COUNT_BITS       ( 8 )

#define NO_QMF_CHANNELS                   ( 64 )
#define MAX_NUM_COL                       ( 32 )


  #define NO_QMF_BANDS_HYBRID20           ( 3  )
  #define NO_SUB_QMF_CHANNELS             ( 12 )

  #define NRG_INT_COEFF                   ( 0.75f )
  #define INT_FILTER_COEFF                (FL2FXCONST_DBL( 1.0f - NRG_INT_COEFF ))
  #define PEAK_DECAY_FACTOR               (FL2FXCONST_DBL( 0.765928338364649f ))
  #define TRANSIENT_IMPACT_FACTOR         (FL2FXCONST_DBL( 2.0 / 3.0 ))

  #define NO_SERIAL_ALLPASS_LINKS         ( 3 )
  #define MAX_NO_PS_ENV                   ( 4 + 1 )   /* +1 needed for VAR_BORDER */

  #define MAX_DELAY_BUFFER_SIZE           ( 14 )
  #define NO_DELAY_BUFFER_BANDS           ( 35 )

  #define NO_HI_RES_BINS                  ( 34 )
  #define NO_MID_RES_BINS                 ( 20 )
  #define NO_LOW_RES_BINS                 ( 10 )

  #define FIRST_DELAY_SB                  ( 23 )
  #define NO_SAMPLE_DELAY_ALLPASS         ( 2 )
  #define NO_DELAY_LENGTH_VECTORS         ( 12 )     /* d(m): d(0)=3 + d(1)=4 + d(2)=5 */

  #define NO_HI_RES_IID_BINS              ( NO_HI_RES_BINS )
  #define NO_HI_RES_ICC_BINS              ( NO_HI_RES_BINS )

  #define NO_MID_RES_IID_BINS             ( NO_MID_RES_BINS )
  #define NO_MID_RES_ICC_BINS             ( NO_MID_RES_BINS )

  #define NO_LOW_RES_IID_BINS             ( NO_LOW_RES_BINS )
  #define NO_LOW_RES_ICC_BINS             ( NO_LOW_RES_BINS )

  #define SUBQMF_GROUPS                   ( 10 )
  #define QMF_GROUPS                      ( 12 )

  #define SUBQMF_GROUPS_HI_RES            ( 32 )
  #define QMF_GROUPS_HI_RES               ( 18 )

  #define NO_IID_GROUPS                   ( SUBQMF_GROUPS + QMF_GROUPS )
  #define NO_IID_GROUPS_HI_RES            ( SUBQMF_GROUPS_HI_RES + QMF_GROUPS_HI_RES )

  #define NO_IID_STEPS                    ( 7 )  /* 1 .. + 7 */
  #define NO_IID_STEPS_FINE               ( 15 ) /* 1 .. +15 */
  #define NO_ICC_STEPS                    ( 8 )  /* 0 .. + 7 */

  #define NO_IID_LEVELS                   ( 2 * NO_IID_STEPS + 1 )      /* - 7 ..  + 7 */
  #define NO_IID_LEVELS_FINE              ( 2 * NO_IID_STEPS_FINE + 1 ) /* -15 ..  +15 */
  #define NO_ICC_LEVELS                   ( NO_ICC_STEPS )              /*   0 ..  + 7 */

  #define FIXP_SQRT05                     ((FIXP_DBL)0x5a827980) /* 1/SQRT2 */

  struct PS_DEC_COEFFICIENTS {

    FIXP_DBL H11r[NO_IID_GROUPS];                       /*!< coefficients of the sub-subband groups                               */
    FIXP_DBL H12r[NO_IID_GROUPS];                       /*!< coefficients of the sub-subband groups                               */
    FIXP_DBL H21r[NO_IID_GROUPS];                       /*!< coefficients of the sub-subband groups                               */
    FIXP_DBL H22r[NO_IID_GROUPS];                       /*!< coefficients of the sub-subband groups                               */

    FIXP_DBL DeltaH11r[NO_IID_GROUPS];                  /*!< coefficients of the sub-subband groups                               */
    FIXP_DBL DeltaH12r[NO_IID_GROUPS];                  /*!< coefficients of the sub-subband groups                               */
    FIXP_DBL DeltaH21r[NO_IID_GROUPS];                  /*!< coefficients of the sub-subband groups                               */
    FIXP_DBL DeltaH22r[NO_IID_GROUPS];                  /*!< coefficients of the sub-subband groups                               */

    SCHAR aaIidIndexMapped[MAX_NO_PS_ENV][NO_HI_RES_IID_BINS];       /*!< The mapped IID index for all envelopes and all IID bins */
    SCHAR aaIccIndexMapped[MAX_NO_PS_ENV][NO_HI_RES_ICC_BINS];       /*!< The mapped ICC index for all envelopes and all ICC bins */

  };




typedef enum {
  ppt_none = 0,
  ppt_mpeg = 1,
  ppt_drm  = 2
} PS_PAYLOAD_TYPE;


typedef struct {
  UCHAR bPsHeaderValid;                                      /*!< set if new header is available from bitstream                 */

  UCHAR bEnableIid;                                          /*!< One bit denoting the presence of IID parameters               */
  UCHAR bEnableIcc;                                          /*!< One bit denoting the presence of ICC parameters               */
  UCHAR bEnableExt;                                          /*!< The PS extension layer is enabled using the enable_ext bit.
                                                                  If it is set to %1 the IPD and OPD parameters are sent.
                                                                  If it is disabled, i.e. %0, the extension layer is skipped.   */

  UCHAR modeIid;                                             /*!< The configuration of IID parameters (number of bands and
                                                                  quantisation grid, iid_quant) is determined by iid_mode.      */
  UCHAR modeIcc;                                             /*!< The configuration of Inter-channel Coherence parameters
                                                                  (number of bands and quantisation grid) is determined by
                                                                  icc_mode.                                                     */

  UCHAR freqResIid;                                          /*!< 0=low, 1=mid or 2=high frequency resolution for iid           */
  UCHAR freqResIcc;                                          /*!< 0=low, 1=mid or 2=high frequency resolution for icc           */

  UCHAR bFineIidQ;                                           /*!< Use fine Iid quantisation.                                    */

  UCHAR bFrameClass;                                         /*!< The frame_class bit determines whether the parameter
                                                                  positions of the current frame are uniformly spaced
                                                                  accross the frame or they are defined using the positions
                                                                  described by border_position.                                 */

  UCHAR noEnv;                                               /*!< The number of envelopes per frame                             */
  UCHAR aEnvStartStop[MAX_NO_PS_ENV+1];                      /*!< In case of variable parameter spacing the parameter
                                                                  positions are determined by border_position                   */

  SCHAR abIidDtFlag[MAX_NO_PS_ENV];                                /*!< Deltacoding time/freq flag for IID, 0 => freq           */
  SCHAR abIccDtFlag[MAX_NO_PS_ENV];                                /*!< Deltacoding time/freq flag for ICC, 0 => freq           */

  SCHAR aaIidIndex[MAX_NO_PS_ENV][NO_HI_RES_IID_BINS];             /*!< The IID index for all envelopes and all IID bins        */
  SCHAR aaIccIndex[MAX_NO_PS_ENV][NO_HI_RES_ICC_BINS];             /*!< The ICC index for all envelopes and all ICC bins        */

} MPEG_PS_BS_DATA;



struct PS_DEC {

  SCHAR noSubSamples;
  SCHAR noChannels;

  SCHAR procFrameBased;                                      /*!< Helper to detected switching from frame based to slot based
                                                                  processing                                                    */

  PS_PAYLOAD_TYPE bPsDataAvail[(1)+1];   /*!< set if new data available from bitstream                      */
  UCHAR psDecodedPrv;                                        /*!< set if PS has been processed in the last frame                */

  /* helpers for frame delay line */
  UCHAR bsLastSlot;                                          /*!< Index of last read slot.                                      */
  UCHAR bsReadSlot;                                          /*!< Index of current read slot for additional delay.              */
  UCHAR processSlot;                                         /*!< Index of current slot for processing (need for add. delay).   */


  INT  rescal;
  INT  sf_IntBuffer;

  union {  /* Bitstream data */
    MPEG_PS_BS_DATA  mpeg;                                   /*!< Struct containing all MPEG specific PS data from bitstream.   */
  } bsData[(1)+1];

  shouldBeUnion {  /* Static data */
    struct {
      SCHAR aIidPrevFrameIndex[NO_HI_RES_IID_BINS];          /*!< The IID index for previous frame                              */
      SCHAR aIccPrevFrameIndex[NO_HI_RES_ICC_BINS];          /*!< The ICC index for previous frame                              */

      UCHAR delayBufIndex;                                         /*!< Pointer to where the latest sample is in buffer         */
      UCHAR noSampleDelay;                                         /*!< How many QMF samples delay is used.                     */
      UCHAR lastUsb;                                               /*!< uppermost WMF delay band of last frame                  */

      UCHAR aDelayRBufIndexSer[NO_SERIAL_ALLPASS_LINKS];             /*!< Delay buffer for reverb filter                        */
      UCHAR aDelayBufIndexDelayQmf[NO_QMF_CHANNELS-FIRST_DELAY_SB];  /*!< Delay buffer for ICC group 20 & 21                    */

      SCHAR scaleFactorPsDelayBuffer;                                /*!< Scale factor for ps delay buffer                      */

      /* hybrid filter bank delay lines */
      FIXP_DBL aaQmfDelayBufReal[(NO_QMF_CHANNELS-FIRST_DELAY_SB) + (MAX_DELAY_BUFFER_SIZE-1)*(NO_DELAY_BUFFER_BANDS-FIRST_DELAY_SB)];
      FIXP_DBL aaQmfDelayBufImag[(NO_QMF_CHANNELS-FIRST_DELAY_SB) + (MAX_DELAY_BUFFER_SIZE-1)*(NO_DELAY_BUFFER_BANDS-FIRST_DELAY_SB)];

      FIXP_DBL *pAaRealDelayBufferQmf[MAX_DELAY_BUFFER_SIZE];          /*!< Real part delay buffer                                  */
      FIXP_DBL *pAaImagDelayBufferQmf[MAX_DELAY_BUFFER_SIZE];          /*!< Imaginary part delay buffer                             */

      FIXP_DBL aaRealDelayBufferQmf[NO_SAMPLE_DELAY_ALLPASS][FIRST_DELAY_SB]; /*!< Real part delay buffer     */
      FIXP_DBL aaImagDelayBufferQmf[NO_SAMPLE_DELAY_ALLPASS][FIRST_DELAY_SB]; /*!< Imaginary part delay buffer*/

      FIXP_DBL aaRealDelayBufferSubQmf[NO_SAMPLE_DELAY_ALLPASS][NO_SUB_QMF_CHANNELS];          /*!< Real part delay buffer          */
      FIXP_DBL aaImagDelayBufferSubQmf[NO_SAMPLE_DELAY_ALLPASS][NO_SUB_QMF_CHANNELS];          /*!< Imaginary part delay buffer     */

      FIXP_DBL aaaRealDelayRBufferSerQmf[FIRST_DELAY_SB][NO_DELAY_LENGTH_VECTORS];             /*!< Real part delay buffer          */
      FIXP_DBL aaaImagDelayRBufferSerQmf[FIRST_DELAY_SB][NO_DELAY_LENGTH_VECTORS];             /*!< Imaginary part delay buffer     */

      FIXP_DBL aaaRealDelayRBufferSerSubQmf[NO_SUB_QMF_CHANNELS][NO_DELAY_LENGTH_VECTORS];     /*!< Real part delay buffer          */
      FIXP_DBL aaaImagDelayRBufferSerSubQmf[NO_SUB_QMF_CHANNELS][NO_DELAY_LENGTH_VECTORS];     /*!< Imaginary part delay buffer     */

      HYBRID hybrid;                                      /*!< hybrid filter bank struct 1 or 2.                          */

      FIXP_DBL aPrevNrgBin[NO_MID_RES_BINS];              /*!< energy of previous frame                                             */
      FIXP_DBL aPrevPeakDiffBin[NO_MID_RES_BINS];         /*!< peak difference of previous frame                                    */
      FIXP_DBL aPeakDecayFastBin[NO_MID_RES_BINS];        /*!< Saved max. peak decay value per bin                                  */
      SCHAR aPowerPrevScal[NO_MID_RES_BINS];              /*!< Last power value (each bin) of previous frame                        */

      FIXP_DBL h11rPrev[NO_IID_GROUPS];                   /*!< previous calculated h(xy) coefficients                               */
      FIXP_DBL h12rPrev[NO_IID_GROUPS];                   /*!< previous calculated h(xy) coefficients                               */
      FIXP_DBL h21rPrev[NO_IID_GROUPS];                   /*!< previous calculated h(xy) coefficients                               */
      FIXP_DBL h22rPrev[NO_IID_GROUPS];                   /*!< previous calculated h(xy) coefficients                               */

      PS_DEC_COEFFICIENTS  coef;                         /*!< temporal coefficients (reusable scratch memory)                 */

    } mpeg;

  } specificTo;


};

typedef struct PS_DEC *HANDLE_PS_DEC;


int CreatePsDec(HANDLE_PS_DEC *h_PS_DEC, int aacSamplesPerFrame);

int DeletePsDec(HANDLE_PS_DEC *h_PS_DEC);

void
scalFilterBankValues( HANDLE_PS_DEC   h_ps_d,                        /* parametric stereo decoder handle     */
                      FIXP_DBL      **fixpQmfReal,                   /* qmf filterbank values                */
                      FIXP_DBL      **fixpQmfImag,                   /* qmf filterbank values                */
                      int             lsb,                           /* sbr start subband                    */
                      int             scaleFactorLowBandSplitLow,
                      int             scaleFactorLowBandSplitHigh,
                      SCHAR          *scaleFactorLowBand_lb,
                      SCHAR          *scaleFactorLowBand_hb,
                      int             scaleFactorHighBands,
                      INT            *scaleFactorHighBand,
                      INT             noCols);

void
rescalFilterBankValues( HANDLE_PS_DEC   h_ps_d,                      /* parametric stereo decoder handle     */
                        FIXP_DBL      **QmfBufferReal,               /* qmf filterbank values                */
                        FIXP_DBL      **QmfBufferImag,               /* qmf filterbank values                */
                        int             lsb,                         /* sbr start subband                    */
                        INT             noCols);


void
initSlotBasedRotation( HANDLE_PS_DEC h_ps_d,
                       int env,
                       int usb);

void
ApplyPsSlot( HANDLE_PS_DEC h_ps_d,            /* parametric stereo decoder handle    */
             FIXP_DBL **rIntBufferLeft,       /* real values of left qmf timeslot    */
             FIXP_DBL **iIntBufferLeft,       /* imag values of left qmf timeslot    */
             FIXP_DBL *rIntBufferRight,       /* real values of right qmf timeslot   */
             FIXP_DBL *iIntBufferRight);      /* imag values of right qmf timeslot   */



#endif  /* __PSDEC_H */
