// Copyright 2023 Google Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <string.h>

#include BOSS_WEBP_U_src__dec__vp8li_dec_h //original-code:"src/dec/vp8li_dec.h"
#include BOSS_WEBP_U_src__utils__bit_reader_utils_h //original-code:"src/utils/bit_reader_utils.h"
#include BOSS_WEBP_U_src__utils__huffman_utils_h //original-code:"src/utils/huffman_utils.h"
#include BOSS_WEBP_U_src__utils__utils_h //original-code:"src/utils/utils.h"
#include BOSS_WEBP_U_src__webp__format_constants_h //original-code:"src/webp/format_constants.h"

int LLVMFuzzerTestOneInput(const uint8_t* const data, size_t size) {
  // Number of bits to initialize data.
  static const int kColorCacheBitsBits = 4;
  // 'num_htree_groups' is contained in the RG channel, hence 16 bits.
  static const int kNumHtreeGroupsBits = 16;
  if (size * sizeof(*data) < kColorCacheBitsBits + kNumHtreeGroupsBits) {
    return 0;
  }

  // A non-NULL mapping brings minor changes that are tested by the normal
  // fuzzer.
  int* const mapping = NULL;
  HuffmanTables huffman_tables;
  memset(&huffman_tables, 0, sizeof(huffman_tables));
  HTreeGroup* htree_groups = NULL;

  VP8LDecoder* dec = VP8LNew();
  if (dec == NULL) goto Error;
  VP8LBitReader* const br = &dec->br_;
  VP8LInitBitReader(br, data, size);

  const int color_cache_bits = VP8LReadBits(br, kColorCacheBitsBits);
  if (color_cache_bits < 1 || color_cache_bits > MAX_CACHE_BITS) goto Error;

  const int num_htree_groups = VP8LReadBits(br, kNumHtreeGroupsBits);
  // 'num_htree_groups' cannot be 0 as it is built from a non-empty image.
  if (num_htree_groups == 0) goto Error;
  // This variable is only useful when mapping is not NULL.
  const int num_htree_groups_max = num_htree_groups;
  ReadHuffmanCodesHelper(color_cache_bits, num_htree_groups,
                         num_htree_groups_max, mapping, dec, &huffman_tables,
                         &htree_groups);

 Error:
  WebPSafeFree(mapping);
  VP8LHtreeGroupsFree(htree_groups);
  VP8LHuffmanTablesDeallocate(&huffman_tables);
  VP8LDelete(dec);
  return 0;
}
