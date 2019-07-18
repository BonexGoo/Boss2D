///////////////////////////////////////////////////////////////////////
// File:        dawg_cache.h
// Description: A class that knows about loading and caching dawgs.
// Author:      David Eger
// Created:     Fri Jan 27 12:08:00 PST 2012
//
// (C) Copyright 2012, Google Inc.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
///////////////////////////////////////////////////////////////////////

#ifndef TESSERACT_DICT_DAWG_CACHE_H_
#define TESSERACT_DICT_DAWG_CACHE_H_

#include BOSS_TESSERACT_U_dawg_h //original-code:"dawg.h"
#include BOSS_TESSERACT_U_object_cache_h //original-code:"object_cache.h"
#include BOSS_TESSERACT_U_strngs_h //original-code:"strngs.h"
#include BOSS_TESSERACT_U_tessdatamanager_h //original-code:"tessdatamanager.h"

namespace tesseract {

class DawgCache {
 public:
  Dawg *GetSquishedDawg(
      const STRING &lang,
      const char *data_file_name,
      TessdataType tessdata_dawg_type,
      int debug_level);

  // If we manage the given dawg, decrement its count,
  // and possibly delete it if the count reaches zero.
  // If dawg is unknown to us, return false.
  bool FreeDawg(Dawg *dawg) {
    return dawgs_.Free(dawg);
  }

  // Free up any currently unused dawgs.
  void DeleteUnusedDawgs() {
    dawgs_.DeleteUnusedObjects();
  }

 private:
  ObjectCache<Dawg> dawgs_;
};

}  // namespace tesseract

#endif  // TESSERACT_DICT_DAWG_CACHE_H_
