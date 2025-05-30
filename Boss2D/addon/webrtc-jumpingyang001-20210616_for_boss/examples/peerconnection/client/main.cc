/*
 *  Copyright 2012 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

// clang-format off
// clang formating would change include order.
#include <windows.h>
#include <shellapi.h>  // must come after windows.h
// clang-format on

#include <string>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__flags__parse_h //original-code:"absl/flags/parse.h"
#include "examples/peerconnection/client/conductor.h"
#include "examples/peerconnection/client/flag_defs.h"
#include "examples/peerconnection/client/main_wnd.h"
#include "examples/peerconnection/client/peer_connection_client.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__constructor_magic_h //original-code:"rtc_base/constructor_magic.h"
#include BOSS_WEBRTC_U_rtc_base__ssl_adapter_h //original-code:"rtc_base/ssl_adapter.h"
#include BOSS_WEBRTC_U_rtc_base__string_utils_h //original-code:"rtc_base/string_utils.h"  // For ToUtf8
#include BOSS_WEBRTC_U_rtc_base__win32_socket_init_h //original-code:"rtc_base/win32_socket_init.h"
#include BOSS_WEBRTC_U_rtc_base__win32_socket_server_h //original-code:"rtc_base/win32_socket_server.h"
#include BOSS_WEBRTC_U_system_wrappers__include__field_trial_h //original-code:"system_wrappers/include/field_trial.h"
#include BOSS_WEBRTC_U_test__field_trial_h //original-code:"test/field_trial.h"

namespace {
// A helper class to translate Windows command line arguments into UTF8,
// which then allows us to just pass them to the flags system.
// This encapsulates all the work of getting the command line and translating
// it to an array of 8-bit strings; all you have to do is create one of these,
// and then call argc() and argv().
class WindowsCommandLineArguments {
 public:
  WindowsCommandLineArguments();

  int argc() { return argv_.size(); }
  char** argv() { return argv_.data(); }

 private:
  // Owned argument strings.
  std::vector<std::string> args_;
  // Pointers, to get layout compatible with char** argv.
  std::vector<char*> argv_;

 private:
  RTC_DISALLOW_COPY_AND_ASSIGN(WindowsCommandLineArguments);
};

WindowsCommandLineArguments::WindowsCommandLineArguments() {
  // start by getting the command line.
  LPCWSTR command_line = ::GetCommandLineW();
  // now, convert it to a list of wide char strings.
  int argc;
  LPWSTR* wide_argv = ::CommandLineToArgvW(command_line, &argc);

  // iterate over the returned wide strings;
  for (int i = 0; i < argc; ++i) {
    args_.push_back(rtc::ToUtf8(wide_argv[i], wcslen(wide_argv[i])));
    // make sure the argv array points to the string data.
    argv_.push_back(const_cast<char*>(args_.back().c_str()));
  }
  LocalFree(wide_argv);
}

}  // namespace
int PASCAL wWinMain(HINSTANCE instance,
                    HINSTANCE prev_instance,
                    wchar_t* cmd_line,
                    int cmd_show) {
  rtc::WinsockInitializer winsock_init;
  rtc::Win32SocketServer w32_ss;
  rtc::Win32Thread w32_thread(&w32_ss);
  rtc::ThreadManager::Instance()->SetCurrentThread(&w32_thread);

  WindowsCommandLineArguments win_args;
  int argc = win_args.argc();
  char** argv = win_args.argv();

  absl::ParseCommandLine(argc, argv);

  // InitFieldTrialsFromString stores the char*, so the char array must outlive
  // the application.
  const std::string forced_field_trials =
      absl::GetFlag(FLAGS_force_fieldtrials);
  webrtc::field_trial::InitFieldTrialsFromString(forced_field_trials.c_str());

  // Abort if the user specifies a port that is outside the allowed
  // range [1, 65535].
  if ((absl::GetFlag(FLAGS_port) < 1) || (absl::GetFlag(FLAGS_port) > 65535)) {
    printf("Error: %i is not a valid port.\n", absl::GetFlag(FLAGS_port));
    return -1;
  }

  const std::string server = absl::GetFlag(FLAGS_server);
  MainWnd wnd(server.c_str(), absl::GetFlag(FLAGS_port),
              absl::GetFlag(FLAGS_autoconnect), absl::GetFlag(FLAGS_autocall));
  if (!wnd.Create()) {
    RTC_NOTREACHED();
    return -1;
  }

  rtc::InitializeSSL();
  PeerConnectionClient client;
  rtc::scoped_refptr<Conductor> conductor(
      new rtc::RefCountedObject<Conductor>(&client, &wnd));

  // Main loop.
  MSG msg;
  BOOL gm;
  while ((gm = ::GetMessage(&msg, NULL, 0, 0)) != 0 && gm != -1) {
    if (!wnd.PreTranslateMessage(&msg)) {
      ::TranslateMessage(&msg);
      ::DispatchMessage(&msg);
    }
  }

  if (conductor->connection_active() || client.is_connected()) {
    while ((conductor->connection_active() || client.is_connected()) &&
           (gm = ::GetMessage(&msg, NULL, 0, 0)) != 0 && gm != -1) {
      if (!wnd.PreTranslateMessage(&msg)) {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
      }
    }
  }

  rtc::CleanupSSL();
  return 0;
}
