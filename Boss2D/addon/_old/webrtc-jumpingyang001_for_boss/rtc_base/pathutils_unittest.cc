/*
 *  Copyright 2007 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_rtc_base__pathutils_h //original-code:"rtc_base/pathutils.h"
#include BOSS_WEBRTC_U_rtc_base__gunit_h //original-code:"rtc_base/gunit.h"

TEST(Pathname, ReturnsDotForEmptyPathname) {
  const std::string kCWD =
      std::string(".") + rtc::Pathname::DefaultFolderDelimiter();

  rtc::Pathname path("/", "");
  EXPECT_TRUE (path.filename().empty());
  EXPECT_FALSE(path.pathname().empty());
  EXPECT_EQ(std::string("/"), path.pathname());

  path.SetPathname("", "foo");
  EXPECT_FALSE(path.filename().empty());
  EXPECT_FALSE(path.pathname().empty());
  EXPECT_EQ(std::string("foo"), path.pathname());

  path.SetPathname("", "");
  EXPECT_TRUE (path.filename().empty());
  EXPECT_FALSE(path.pathname().empty());
  EXPECT_EQ(kCWD, path.pathname());

  path.SetPathname(kCWD, "");
  EXPECT_TRUE (path.filename().empty());
  EXPECT_FALSE(path.pathname().empty());
  EXPECT_EQ(kCWD, path.pathname());
}
