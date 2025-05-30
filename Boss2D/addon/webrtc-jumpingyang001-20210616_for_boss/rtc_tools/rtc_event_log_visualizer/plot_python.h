/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef RTC_TOOLS_RTC_EVENT_LOG_VISUALIZER_PLOT_PYTHON_H_
#define RTC_TOOLS_RTC_EVENT_LOG_VISUALIZER_PLOT_PYTHON_H_

#include BOSS_ABSEILCPP_U_absl__base__attributes_h //original-code:"absl/base/attributes.h"
#include "rtc_tools/rtc_event_log_visualizer/plot_base.h"

namespace webrtc {

class PythonPlot final : public Plot {
 public:
  PythonPlot();
  ~PythonPlot() override;
  void Draw() override;
};

class ABSL_DEPRECATED("Use PlotCollection and PrintPythonCode() instead.")
    PythonPlotCollection final : public PlotCollection {
 public:
  explicit PythonPlotCollection(bool shared_xaxis = false);
  ~PythonPlotCollection() override;
  void Draw() override;
  Plot* AppendNewPlot() override;

 private:
  bool shared_xaxis_;
};

}  // namespace webrtc

#endif  // RTC_TOOLS_RTC_EVENT_LOG_VISUALIZER_PLOT_PYTHON_H_
