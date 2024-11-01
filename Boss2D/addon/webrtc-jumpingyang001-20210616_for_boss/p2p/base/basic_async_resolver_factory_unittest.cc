/*
 *  Copyright 2018 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_p2p__base__basic_async_resolver_factory_h //original-code:"p2p/base/basic_async_resolver_factory.h"

#include BOSS_WEBRTC_U_api__test__mock_async_dns_resolver_h //original-code:"api/test/mock_async_dns_resolver.h"
#include BOSS_WEBRTC_U_p2p__base__mock_async_resolver_h //original-code:"p2p/base/mock_async_resolver.h"
#include BOSS_WEBRTC_U_rtc_base__async_resolver_h //original-code:"rtc_base/async_resolver.h"
#include BOSS_WEBRTC_U_rtc_base__gunit_h //original-code:"rtc_base/gunit.h"
#include BOSS_WEBRTC_U_rtc_base__socket_address_h //original-code:"rtc_base/socket_address.h"
#include BOSS_WEBRTC_U_rtc_base__third_party__sigslot__sigslot_h //original-code:"rtc_base/third_party/sigslot/sigslot.h"
#include BOSS_WEBRTC_U_test__gmock_h //original-code:"test/gmock.h"
#include BOSS_WEBRTC_U_test__gtest_h //original-code:"test/gtest.h"
#include "test/testsupport/rtc_expect_death.h"

namespace webrtc {

class BasicAsyncResolverFactoryTest : public ::testing::Test,
                                      public sigslot::has_slots<> {
 public:
  void TestCreate() {
    BasicAsyncResolverFactory factory;
    rtc::AsyncResolverInterface* resolver = factory.Create();
    ASSERT_TRUE(resolver);
    resolver->SignalDone.connect(
        this, &BasicAsyncResolverFactoryTest::SetAddressResolved);

    rtc::SocketAddress address("", 0);
    resolver->Start(address);
    ASSERT_TRUE_WAIT(address_resolved_, 10000 /*ms*/);
    resolver->Destroy(false);
  }

  void SetAddressResolved(rtc::AsyncResolverInterface* resolver) {
    address_resolved_ = true;
  }

 private:
  bool address_resolved_ = false;
};

// This test is primarily intended to let tools check that the created resolver
// doesn't leak.
TEST_F(BasicAsyncResolverFactoryTest, TestCreate) {
  TestCreate();
}

TEST(WrappingAsyncDnsResolverFactoryTest, TestCreateAndResolve) {
  WrappingAsyncDnsResolverFactory factory(
      std::make_unique<BasicAsyncResolverFactory>());

  std::unique_ptr<AsyncDnsResolverInterface> resolver(factory.Create());
  ASSERT_TRUE(resolver);

  bool address_resolved = false;
  rtc::SocketAddress address("", 0);
  resolver->Start(address, [&address_resolved]() { address_resolved = true; });
  ASSERT_TRUE_WAIT(address_resolved, 10000 /*ms*/);
  resolver.reset();
}

TEST(WrappingAsyncDnsResolverFactoryTest, WrapOtherResolver) {
  BasicAsyncResolverFactory non_owned_factory;
  WrappingAsyncDnsResolverFactory factory(&non_owned_factory);
  std::unique_ptr<AsyncDnsResolverInterface> resolver(factory.Create());
  ASSERT_TRUE(resolver);

  bool address_resolved = false;
  rtc::SocketAddress address("", 0);
  resolver->Start(address, [&address_resolved]() { address_resolved = true; });
  ASSERT_TRUE_WAIT(address_resolved, 10000 /*ms*/);
  resolver.reset();
}

#if GTEST_HAS_DEATH_TEST && defined(WEBRTC_LINUX)
// Tests that the prohibition against deleting the resolver from the callback
// is enforced. This is required by the use of sigslot in the wrapped resolver.
// Checking the error message fails on a number of platforms, so run this
// test only on the platforms where it works.
void CallResolver(WrappingAsyncDnsResolverFactory& factory) {
  rtc::SocketAddress address("", 0);
  std::unique_ptr<AsyncDnsResolverInterface> resolver(factory.Create());
  resolver->Start(address, [&resolver]() { resolver.reset(); });
  WAIT(!resolver.get(), 10000 /*ms*/);
}

TEST(WrappingAsyncDnsResolverFactoryDeathTest, DestroyResolverInCallback) {
  // This test requires the main thread to be wrapped. So we defeat the
  // workaround in test/test_main_lib.cc by explicitly wrapping the main
  // thread here.
  auto thread = rtc::Thread::CreateWithSocketServer();
  thread->WrapCurrent();
  // TODO(bugs.webrtc.org/12652): Rewrite as death test in loop style when it
  // works.
  WrappingAsyncDnsResolverFactory factory(
      std::make_unique<BasicAsyncResolverFactory>());

  // Since EXPECT_DEATH is thread sensitive, and the resolver creates a thread,
  // we wrap the whole creation section in EXPECT_DEATH.
  RTC_EXPECT_DEATH(CallResolver(factory),
                   "Check failed: !within_resolve_result_");
  // If we get here, we have to unwrap the thread.
  thread->Quit();
  thread->Run();
  thread->UnwrapCurrent();
  thread = nullptr;
}
#endif

}  // namespace webrtc
