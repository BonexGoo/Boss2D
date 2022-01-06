/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include <stdio.h>
#include <stdlib.h>

#include <string>

#include BOSS_ABSEILCPP_U_absl__flags__flag_h //original-code:"absl/flags/flag.h"
#include BOSS_ABSEILCPP_U_absl__flags__parse_h //original-code:"absl/flags/parse.h"
#include BOSS_ABSEILCPP_U_absl__flags__usage_h //original-code:"absl/flags/usage.h"
#include "rtc_tools/frame_analyzer/reference_less_video_analysis_lib.h"

ABSL_FLAG(std::string,
          video_file,
          "",
          "Path of the video file to be analyzed, only y4m file format is "
          "supported");

int main(int argc, char* argv[]) {
  absl::SetProgramUsageMessage(
      "Outputs the freezing score by comparing "
      "current frame with the previous frame.\n"
      "Example usage:\n"
      "./reference_less_video_analysis "
      "--video_file=video_file.y4m\n");
  absl::ParseCommandLine(argc, argv);

  std::string video_file = absl::GetFlag(FLAGS_video_file);
  if (video_file.empty()) {
    exit(EXIT_FAILURE);
  }

  return run_analysis(video_file);
}
