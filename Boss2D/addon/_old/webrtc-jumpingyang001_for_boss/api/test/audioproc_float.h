/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef API_TEST_AUDIOPROC_FLOAT_H_
#define API_TEST_AUDIOPROC_FLOAT_H_

#include <memory>

#include BOSS_WEBRTC_U_modules__audio_processing__include__audio_processing_h //original-code:"modules/audio_processing/include/audio_processing.h"

namespace webrtc {
namespace test {

// This is an interface for the audio processing simulation utility. This
// utility can be used to simulate the audioprocessing module using a recording
// (either an AEC dump or wav files), and generate the output as a wav file.
// The |ap_builder| object will be used to create the AudioProcessing instance
// that is used during the simulation. The |ap_builder| supports setting of
// injectable components, which will be passed on to the created AudioProcessing
// instance. It is needed to pass the command line flags as |argc| and |argv|,
// so these can be interpreted properly by the utility.
// To get a fully-working audioproc_f utility, all that is needed is to write a
// main function, create an AudioProcessingBuilder, optionally set custom
// processing components on it, and pass the builder together with the command
// line arguments into this function.
// To see a list of all supported command line flags, run the executable with
// the '--help' flag.
int AudioprocFloat(std::unique_ptr<AudioProcessingBuilder> ap_builder,
                   int argc,
                   char* argv[]);

}  // namespace test
}  // namespace webrtc

#endif  // API_TEST_AUDIOPROC_FLOAT_H_
