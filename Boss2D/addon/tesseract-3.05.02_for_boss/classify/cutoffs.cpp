// author BOSS

/******************************************************************************
 ** Filename:    cutoffs.c
 ** Purpose:     Routines to manipulate an array of class cutoffs.
 ** Author:      Dan Johnson
 ** History:     Wed Feb 20 09:28:51 1991, DSJ, Created.
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
#include "cutoffs.h"

#include <stdio.h>

#include BOSS_TESSERACT_U_classify_h //original-code:"classify.h"
#include BOSS_TESSERACT_U_efio_h //original-code:"efio.h"
#include BOSS_TESSERACT_U_globals_h //original-code:"globals.h"
#include BOSS_TESSERACT_U_helpers_h //original-code:"helpers.h"
#include BOSS_TESSERACT_U_scanutils_h //original-code:"scanutils.h"
#include BOSS_TESSERACT_U_serialis_h //original-code:"serialis.h"
#include BOSS_TESSERACT_U_unichar_h //original-code:"unichar.h"

#define REALLY_QUOTE_IT(x) QUOTE_IT(x)

#define MAX_CUTOFF      1000

namespace tesseract {
/**
 * Open Filename, read in all of the class-id/cutoff pairs
 * and insert them into the Cutoffs array.  Cutoffs are
 * indexed in the array by class id.  Unused entries in the
 * array are set to an arbitrarily high cutoff value.
 * @param CutoffFile name of file containing cutoff definitions
 * @param Cutoffs array to put cutoffs into
 * @param swap
 * @param end_offset
 * @return none
 * @note Globals: none
 * @note Exceptions: none
 * @note History: Wed Feb 20 09:38:26 1991, DSJ, Created.
 */
void Classify::ReadNewCutoffs(FILE *CutoffFile, bool swap, inT64 end_offset,
                              CLASS_CUTOFF_ARRAY Cutoffs) {
  BOSS_TRACE("@@@@@ ReadNewCutoffs-1", false);
  char Class[UNICHAR_LEN + 1];
  CLASS_ID ClassId;
  int Cutoff;
  int i;

  BOSS_TRACE("@@@@@ ReadNewCutoffs-2", false);
  if (shape_table_ != NULL) {
    if (!shapetable_cutoffs_.DeSerialize(swap, CutoffFile)) {
      tprintf("Error during read of shapetable pffmtable!\n");
    }
  }
  BOSS_TRACE("@@@@@ ReadNewCutoffs-3", false);
  for (i = 0; i < MAX_NUM_CLASSES; i++)
    Cutoffs[i] = MAX_CUTOFF;

  BOSS_TRACE("@@@@@ ReadNewCutoffs-4", false);
  while ((end_offset < 0 || BOSS_TESSERACT_ftell(CutoffFile) < end_offset) && //original-code:ftell(CutoffFile) < end_offset) &&
         tfscanf(CutoffFile, "%" REALLY_QUOTE_IT(UNICHAR_LEN) "s %d",
                Class, &Cutoff) == 2) {
	BOSS_TRACE("@@@@@ ReadNewCutoffs-5", false);
    if (strcmp(Class, "NULL") == 0) {
      ClassId = unicharset.unichar_to_id(" ");
    } else {
      ClassId = unicharset.unichar_to_id(Class);
    }
	BOSS_TRACE("@@@@@ ReadNewCutoffs-6", false);
    Cutoffs[ClassId] = Cutoff;
    SkipNewline(CutoffFile);
	BOSS_TRACE("@@@@@ ReadNewCutoffs-7", false);
  }
}

}  // namespace tesseract
