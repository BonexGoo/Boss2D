/******************************************************************************
 ** Filename:    picofeat.c
 ** Purpose:     Definition of pico-features.
 ** Author:      Dan Johnson
 ** History:     9/4/90, DSJ, Created.
 **
 ** (c) Copyright Hewlett-Packard Company, 1988.
 ** Licensed under the Apache License, Version 2.0 (the "License");
 ** you may not use this file except in compliance with the License.
 ** You may obtain a copy of the License at
 ** http://www.apache.org/licenses/LICENSE-2.0
 ** Unless required by applicable law or agreed to in writing, software
 ** distributed under the License is distributed on an "AS IS" BASIS,
 ** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 ** See the License for the specific language governing permissions and
 ** limitations under the License.
 ******************************************************************************/
/*----------------------------------------------------------------------------
          Include Files and Type Defines
----------------------------------------------------------------------------*/
#include BOSS_TESSERACT_U_picofeat_h //original-code:"picofeat.h"

#include BOSS_TESSERACT_U_classify_h //original-code:"classify.h"
#include BOSS_TESSERACT_U_efio_h //original-code:"efio.h"
#include BOSS_TESSERACT_U_featdefs_h //original-code:"featdefs.h"
#include "fpoint.h"
#include BOSS_TESSERACT_U_mfoutline_h //original-code:"mfoutline.h"
#include BOSS_TESSERACT_U_ocrfeatures_h //original-code:"ocrfeatures.h"
#include BOSS_TESSERACT_U_params_h //original-code:"params.h"
#include BOSS_TESSERACT_U_trainingsample_h //original-code:"trainingsample.h"

#include <math.h>
#include <stdio.h>

/*---------------------------------------------------------------------------
          Variables
----------------------------------------------------------------------------*/

double_VAR(classify_pico_feature_length, 0.05, "Pico Feature Length");

/*---------------------------------------------------------------------------
          Private Function Prototypes
----------------------------------------------------------------------------*/
void ConvertSegmentToPicoFeat(FPOINT *Start,
                              FPOINT *End,
                              FEATURE_SET FeatureSet);

void ConvertToPicoFeatures2(MFOUTLINE Outline, FEATURE_SET FeatureSet);

void NormalizePicoX(FEATURE_SET FeatureSet);

/*----------------------------------------------------------------------------
              Public Code
----------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
namespace tesseract {
/**
 * Operation: Dummy for now.
 *
 * Globals:
 * - classify_norm_method normalization method currently specified
 * @param Blob blob to extract pico-features from
 * @return Pico-features for Blob.
 * @note Exceptions: none
 * @note History: 9/4/90, DSJ, Created.
 */
FEATURE_SET Classify::ExtractPicoFeatures(TBLOB *Blob) {
  LIST Outlines;
  LIST RemainingOutlines;
  MFOUTLINE Outline;
  FEATURE_SET FeatureSet;
  FLOAT32 XScale, YScale;

  FeatureSet = NewFeatureSet(MAX_PICO_FEATURES);
  Outlines = ConvertBlob(Blob);
  NormalizeOutlines(Outlines, &XScale, &YScale);
  RemainingOutlines = Outlines;
  iterate(RemainingOutlines) {
    Outline = (MFOUTLINE) first_node (RemainingOutlines);
    ConvertToPicoFeatures2(Outline, FeatureSet);
  }
  if (classify_norm_method == baseline)
    NormalizePicoX(FeatureSet);
  FreeOutlines(Outlines);
  return (FeatureSet);

}                                /* ExtractPicoFeatures */
}  // namespace tesseract

/*----------------------------------------------------------------------------
              Private Code
----------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/**
 * This routine converts an entire segment of an outline
 * into a set of pico features which are added to
 * FeatureSet.  The length of the segment is rounded to the
 * nearest whole number of pico-features.  The pico-features
 * are spaced evenly over the entire segment.
 * Globals:
 * - classify_pico_feature_length length of a single pico-feature
 * @param Start starting point of pico-feature
 * @param End ending point of pico-feature
 * @param FeatureSet set to add pico-feature to
 * @return none (results are placed in FeatureSet)
 * @note Exceptions: none
 * @note History: Tue Apr 30 15:44:34 1991, DSJ, Created.
 */
void ConvertSegmentToPicoFeat(FPOINT *Start,
                              FPOINT *End,
                              FEATURE_SET FeatureSet) {
  FEATURE Feature;
  FLOAT32 Angle;
  FLOAT32 Length;
  int NumFeatures;
  FPOINT Center;
  FPOINT Delta;
  int i;

  Angle = NormalizedAngleFrom (Start, End, 1.0);
  Length = DistanceBetween (*Start, *End);
  NumFeatures = (int) floor (Length / classify_pico_feature_length + 0.5);
  if (NumFeatures < 1)
    NumFeatures = 1;

  /* compute vector for one pico feature */
  Delta.x = XDelta (*Start, *End) / NumFeatures;
  Delta.y = YDelta (*Start, *End) / NumFeatures;

  /* compute position of first pico feature */
  Center.x = Start->x + Delta.x / 2.0;
  Center.y = Start->y + Delta.y / 2.0;

  /* compute each pico feature in segment and add to feature set */
  for (i = 0; i < NumFeatures; i++) {
    Feature = NewFeature (&PicoFeatDesc);
    Feature->Params[PicoFeatDir] = Angle;
    Feature->Params[PicoFeatX] = Center.x;
    Feature->Params[PicoFeatY] = Center.y;
    AddFeature(FeatureSet, Feature);

    Center.x += Delta.x;
    Center.y += Delta.y;
  }
}                                /* ConvertSegmentToPicoFeat */


/*---------------------------------------------------------------------------*/
/**
 * This routine steps through the specified outline and cuts it
 * up into pieces of equal length.  These pieces become the
 * desired pico-features.  Each segment in the outline
 * is converted into an integral number of pico-features.
 *
 * Globals:
 * - classify_pico_feature_length length of features to be extracted
 * @param Outline outline to extract micro-features from
 * @param FeatureSet set of features to add pico-features to
 * @return none (results are returned in FeatureSet)
 * @note Exceptions: none
 * @note History: 4/30/91, DSJ, Adapted from ConvertToPicoFeatures().
 */
void ConvertToPicoFeatures2(MFOUTLINE Outline, FEATURE_SET FeatureSet) {
  MFOUTLINE Next;
  MFOUTLINE First;
  MFOUTLINE Current;

  if (DegenerateOutline(Outline))
    return;

  First = Outline;
  Current = First;
  Next = NextPointAfter(Current);
  do {
    /* note that an edge is hidden if the ending point of the edge is
       marked as hidden.  This situation happens because the order of
       the outlines is reversed when they are converted from the old
       format.  In the old format, a hidden edge is marked by the
       starting point for that edge. */
    if (!(PointAt(Next)->Hidden))
      ConvertSegmentToPicoFeat (&(PointAt(Current)->Point),
        &(PointAt(Next)->Point), FeatureSet);

    Current = Next;
    Next = NextPointAfter(Current);
  }
  while (Current != First);

}                                /* ConvertToPicoFeatures2 */


/*---------------------------------------------------------------------------*/
/**
 * This routine computes the average x position over all
 * of the pico-features in FeatureSet and then renormalizes
 * the pico-features to force this average to be the x origin
 * (i.e. x=0).
 * @param FeatureSet pico-features to be normalized
 * @return none (FeatureSet is changed)
 * @note Globals: none
 * @note Exceptions: none
 * @note History: Tue Sep  4 16:50:08 1990, DSJ, Created.
 */
void NormalizePicoX(FEATURE_SET FeatureSet) {
  int i;
  FEATURE Feature;
  FLOAT32 Origin = 0.0;

  for (i = 0; i < FeatureSet->NumFeatures; i++) {
    Feature = FeatureSet->Features[i];
    Origin += Feature->Params[PicoFeatX];
  }
  Origin /= FeatureSet->NumFeatures;

  for (i = 0; i < FeatureSet->NumFeatures; i++) {
    Feature = FeatureSet->Features[i];
    Feature->Params[PicoFeatX] -= Origin;
  }
}                                /* NormalizePicoX */

namespace tesseract {
/*---------------------------------------------------------------------------*/
/**
 * @param blob blob to extract features from
 * @param fx_info
 * @return Integer character-normalized features for blob.
 * @note Exceptions: none
 * @note History: 8/8/2011, rays, Created.
 */
FEATURE_SET Classify::ExtractIntCNFeatures(
    const TBLOB& blob, const INT_FX_RESULT_STRUCT& fx_info) {
  INT_FX_RESULT_STRUCT local_fx_info(fx_info);
  GenericVector<INT_FEATURE_STRUCT> bl_features;
  tesseract::TrainingSample* sample = tesseract::BlobToTrainingSample(
      blob, false, &local_fx_info, &bl_features);
  if (sample == NULL) return NULL;

  int num_features = sample->num_features();
  const INT_FEATURE_STRUCT* features = sample->features();
  FEATURE_SET feature_set = NewFeatureSet(num_features);
  for (int f = 0; f < num_features; ++f) {
    FEATURE feature = NewFeature(&IntFeatDesc);

    feature->Params[IntX] = features[f].X;
    feature->Params[IntY] = features[f].Y;
    feature->Params[IntDir] = features[f].Theta;
    AddFeature(feature_set, feature);
  }
  delete sample;

  return feature_set;
}                                /* ExtractIntCNFeatures */

/*---------------------------------------------------------------------------*/
/**
 * @param blob blob to extract features from
 * @param fx_info
 * @return Geometric (top/bottom/width) features for blob.
 * @note Exceptions: none
 * @note History: 8/8/2011, rays, Created.
 */
FEATURE_SET Classify::ExtractIntGeoFeatures(
    const TBLOB& blob, const INT_FX_RESULT_STRUCT& fx_info) {
  INT_FX_RESULT_STRUCT local_fx_info(fx_info);
  GenericVector<INT_FEATURE_STRUCT> bl_features;
  tesseract::TrainingSample* sample = tesseract::BlobToTrainingSample(
      blob, false, &local_fx_info, &bl_features);
  if (sample == NULL) return NULL;

  FEATURE_SET feature_set = NewFeatureSet(1);
  FEATURE feature = NewFeature(&IntFeatDesc);

  feature->Params[GeoBottom] = sample->geo_feature(GeoBottom);
  feature->Params[GeoTop] = sample->geo_feature(GeoTop);
  feature->Params[GeoWidth] = sample->geo_feature(GeoWidth);
  AddFeature(feature_set, feature);
  delete sample;

  return feature_set;
}                                /* ExtractIntGeoFeatures */

}  // namespace tesseract.
