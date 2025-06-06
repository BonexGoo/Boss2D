// Copyright 2018 Google Inc.
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

#include "./fuzz_utils.h"
#include BOSS_WEBP_U_src__utils__rescaler_utils_h //original-code:"src/utils/rescaler_utils.h"
#include BOSS_WEBP_U_src__webp__decode_h //original-code:"src/webp/decode.h"

int LLVMFuzzerTestOneInput(const uint8_t* const data, size_t size) {
  WebPDecoderConfig config;
  if (!WebPInitDecoderConfig(&config)) return 0;
  if (WebPGetFeatures(data, size, &config.input) != VP8_STATUS_OK) return 0;
  if ((size_t)config.input.width * config.input.height > kFuzzPxLimit) return 0;

  // Using two independent criteria ensures that all combinations of options
  // can reach each path at the decoding stage, with meaningful differences.

  const uint8_t value = FuzzHash(data, size);
  const float factor = value / 255.f;  // 0-1

  config.options.flip = value & 1;
  config.options.bypass_filtering = value & 2;
  config.options.no_fancy_upsampling = value & 4;
  config.options.use_threads = value & 8;
  if (size & 1) {
    config.options.use_cropping = 1;
    config.options.crop_width = (int)(config.input.width * (1 - factor));
    config.options.crop_height = (int)(config.input.height * (1 - factor));
    config.options.crop_left = config.input.width - config.options.crop_width;
    config.options.crop_top = config.input.height - config.options.crop_height;
  }
  if (size & 2) {
    int strength = (int)(factor * 100);
    config.options.dithering_strength = strength;
    config.options.alpha_dithering_strength = 100 - strength;
  }
  if (size & 4) {
    config.options.use_scaling = 1;
    config.options.scaled_width = (int)(config.input.width * factor * 2);
    config.options.scaled_height = (int)(config.input.height * factor * 2);
  }

#if defined(WEBP_REDUCE_CSP)
  config.output.colorspace = (value & 1)
                                 ? ((value & 2) ? MODE_RGBA : MODE_BGRA)
                                 : ((value & 2) ? MODE_rgbA : MODE_bgrA);
#else
  config.output.colorspace = (WEBP_CSP_MODE)(value % MODE_LAST);
#endif  // WEBP_REDUCE_CSP

  for (int i = 0; i < 2; ++i) {
    if (i == 1) {
      // Use the bitstream data to generate extreme ranges for the options. An
      // alternative approach would be to use a custom corpus containing webp
      // files prepended with sizeof(config.options) zeroes to allow the fuzzer
      // to modify these independently.
      const int data_offset = 50;
      if (data_offset + sizeof(config.options) >= size) break;
      memcpy(&config.options, data + data_offset, sizeof(config.options));

      // Skip easily avoidable out-of-memory fuzzing errors.
      if (config.options.use_scaling) {
        int scaled_width = config.options.scaled_width;
        int scaled_height = config.options.scaled_height;
        if (WebPRescalerGetScaledDimensions(config.input.width,
                                            config.input.height, &scaled_width,
                                            &scaled_height)) {
          size_t fuzz_px_limit = kFuzzPxLimit;
          if (scaled_width != config.input.width ||
              scaled_height != config.input.height) {
            // Using the WebPRescalerImport internally can significantly slow
            // down the execution. Avoid timeouts due to that.
            fuzz_px_limit /= 2;
          }
          // A big output canvas can lead to out-of-memory and timeout issues,
          // but a big internal working buffer can too. Also, rescaling from a
          // very wide input image to a very tall canvas can be as slow as
          // decoding a huge number of pixels. Avoid timeouts due to these.
          const uint64_t max_num_operations =
              (uint64_t)Max(scaled_width, config.input.width) *
              Max(scaled_height, config.input.height);
          if (max_num_operations > fuzz_px_limit) {
            break;
          }
        }
      }
    }
    if (size % 3) {
      // Decodes incrementally in chunks of increasing size.
      WebPIDecoder* idec = WebPIDecode(NULL, 0, &config);
      if (!idec) return 0;
      VP8StatusCode status;
      if (size & 8) {
        size_t available_size = value + 1;
        while (1) {
          if (available_size > size) available_size = size;
          status = WebPIUpdate(idec, data, available_size);
          if (status != VP8_STATUS_SUSPENDED || available_size == size) break;
          available_size *= 2;
        }
      } else {
        // WebPIAppend expects new data and its size with each call.
        // Implemented here by simply advancing the pointer into data.
        const uint8_t* new_data = data;
        size_t new_size = value + 1;
        while (1) {
          if (new_data + new_size > data + size) {
            new_size = data + size - new_data;
          }
          status = WebPIAppend(idec, new_data, new_size);
          if (status != VP8_STATUS_SUSPENDED || new_size == 0) break;
          new_data += new_size;
          new_size *= 2;
        }
      }
      WebPIDelete(idec);
    } else {
      WebPDecode(data, size, &config);
    }

    WebPFreeDecBuffer(&config.output);
  }
  return 0;
}
