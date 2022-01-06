/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <memory>

#include BOSS_ABSEILCPP_U_absl__debugging__failure_signal_handler_h //original-code:"absl/debugging/failure_signal_handler.h"
#include BOSS_ABSEILCPP_U_absl__debugging__symbolize_h //original-code:"absl/debugging/symbolize.h"
#include BOSS_ABSEILCPP_U_absl__flags__parse_h //original-code:"absl/flags/parse.h"
#include BOSS_WEBRTC_U_test__gmock_h //original-code:"test/gmock.h"
#include BOSS_WEBRTC_U_test__test_main_lib_h //original-code:"test/test_main_lib.h"

int main(int argc, char* argv[]) {
  // Initialize the symbolizer to get a human-readable stack trace
  absl::InitializeSymbolizer(argv[0]);
  testing::InitGoogleMock(&argc, argv);
  absl::ParseCommandLine(argc, argv);

  absl::FailureSignalHandlerOptions options;
  absl::InstallFailureSignalHandler(options);

  std::unique_ptr<webrtc::TestMain> main = webrtc::TestMain::Create();
  int err_code = main->Init();
  if (err_code != 0) {
    return err_code;
  }
  return main->Run(argc, argv);
}
