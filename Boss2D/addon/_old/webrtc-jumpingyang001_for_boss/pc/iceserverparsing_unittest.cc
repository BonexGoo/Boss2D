/*
 *  Copyright 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <string>
#include <vector>

#include BOSS_WEBRTC_U_pc__iceserverparsing_h //original-code:"pc/iceserverparsing.h"
#include BOSS_WEBRTC_U_rtc_base__gunit_h //original-code:"rtc_base/gunit.h"

namespace webrtc {

class IceServerParsingTest : public testing::Test {
 public:
  // Convenience functions for parsing a single URL. Result is stored in
  // |stun_servers_| and |turn_servers_|.
  bool ParseUrl(const std::string& url) {
    return ParseUrl(url, std::string(), std::string());
  }

  bool ParseTurnUrl(const std::string& url) {
    return ParseUrl(url, "username", "password");
  }

  bool ParseUrl(const std::string& url,
                const std::string& username,
                const std::string& password) {
    return ParseUrl(
        url, username, password,
        PeerConnectionInterface::TlsCertPolicy::kTlsCertPolicySecure);
  }

  bool ParseUrl(const std::string& url,
                const std::string& username,
                const std::string& password,
                PeerConnectionInterface::TlsCertPolicy tls_certificate_policy) {
    return ParseUrl(url, username, password, tls_certificate_policy, "");
  }

  bool ParseUrl(const std::string& url,
                const std::string& username,
                const std::string& password,
                PeerConnectionInterface::TlsCertPolicy tls_certificate_policy,
                const std::string& hostname) {
    stun_servers_.clear();
    turn_servers_.clear();
    PeerConnectionInterface::IceServers servers;
    PeerConnectionInterface::IceServer server;
    server.urls.push_back(url);
    server.username = username;
    server.password = password;
    server.tls_cert_policy = tls_certificate_policy;
    server.hostname = hostname;
    servers.push_back(server);
    return webrtc::ParseIceServers(servers, &stun_servers_, &turn_servers_) ==
           webrtc::RTCErrorType::NONE;
  }

 protected:
  cricket::ServerAddresses stun_servers_;
  std::vector<cricket::RelayServerConfig> turn_servers_;
};

// Make sure all STUN/TURN prefixes are parsed correctly.
TEST_F(IceServerParsingTest, ParseStunPrefixes) {
  EXPECT_TRUE(ParseUrl("stun:hostname"));
  EXPECT_EQ(1U, stun_servers_.size());
  EXPECT_EQ(0U, turn_servers_.size());

  EXPECT_TRUE(ParseUrl("stuns:hostname"));
  EXPECT_EQ(1U, stun_servers_.size());
  EXPECT_EQ(0U, turn_servers_.size());

  EXPECT_TRUE(ParseTurnUrl("turn:hostname"));
  EXPECT_EQ(0U, stun_servers_.size());
  EXPECT_EQ(1U, turn_servers_.size());
  EXPECT_EQ(cricket::PROTO_UDP, turn_servers_[0].ports[0].proto);

  EXPECT_TRUE(ParseTurnUrl("turns:hostname"));
  EXPECT_EQ(0U, stun_servers_.size());
  EXPECT_EQ(1U, turn_servers_.size());
  EXPECT_EQ(cricket::PROTO_TLS, turn_servers_[0].ports[0].proto);
  EXPECT_TRUE(turn_servers_[0].tls_cert_policy ==
              cricket::TlsCertPolicy::TLS_CERT_POLICY_SECURE);

  EXPECT_TRUE(ParseUrl(
      "turns:hostname", "username", "password",
      PeerConnectionInterface::TlsCertPolicy::kTlsCertPolicyInsecureNoCheck));
  EXPECT_EQ(0U, stun_servers_.size());
  EXPECT_EQ(1U, turn_servers_.size());
  EXPECT_TRUE(turn_servers_[0].tls_cert_policy ==
              cricket::TlsCertPolicy::TLS_CERT_POLICY_INSECURE_NO_CHECK);
  EXPECT_EQ(cricket::PROTO_TLS, turn_servers_[0].ports[0].proto);

  // invalid prefixes
  EXPECT_FALSE(ParseUrl("stunn:hostname"));
  EXPECT_FALSE(ParseUrl(":hostname"));
  EXPECT_FALSE(ParseUrl(":"));
  EXPECT_FALSE(ParseUrl(""));
}

TEST_F(IceServerParsingTest, VerifyDefaults) {
  // TURNS defaults
  EXPECT_TRUE(ParseTurnUrl("turns:hostname"));
  EXPECT_EQ(1U, turn_servers_.size());
  EXPECT_EQ(5349, turn_servers_[0].ports[0].address.port());
  EXPECT_EQ(cricket::PROTO_TLS, turn_servers_[0].ports[0].proto);

  // TURN defaults
  EXPECT_TRUE(ParseTurnUrl("turn:hostname"));
  EXPECT_EQ(1U, turn_servers_.size());
  EXPECT_EQ(3478, turn_servers_[0].ports[0].address.port());
  EXPECT_EQ(cricket::PROTO_UDP, turn_servers_[0].ports[0].proto);

  // STUN defaults
  EXPECT_TRUE(ParseUrl("stun:hostname"));
  EXPECT_EQ(1U, stun_servers_.size());
  EXPECT_EQ(3478, stun_servers_.begin()->port());
}

// Check that the 6 combinations of IPv4/IPv6/hostname and with/without port
// can be parsed correctly.
TEST_F(IceServerParsingTest, ParseHostnameAndPort) {
  EXPECT_TRUE(ParseUrl("stun:1.2.3.4:1234"));
  EXPECT_EQ(1U, stun_servers_.size());
  EXPECT_EQ("1.2.3.4", stun_servers_.begin()->hostname());
  EXPECT_EQ(1234, stun_servers_.begin()->port());

  EXPECT_TRUE(ParseUrl("stun:[1:2:3:4:5:6:7:8]:4321"));
  EXPECT_EQ(1U, stun_servers_.size());
  EXPECT_EQ("1:2:3:4:5:6:7:8", stun_servers_.begin()->hostname());
  EXPECT_EQ(4321, stun_servers_.begin()->port());

  EXPECT_TRUE(ParseUrl("stun:hostname:9999"));
  EXPECT_EQ(1U, stun_servers_.size());
  EXPECT_EQ("hostname", stun_servers_.begin()->hostname());
  EXPECT_EQ(9999, stun_servers_.begin()->port());

  EXPECT_TRUE(ParseUrl("stun:1.2.3.4"));
  EXPECT_EQ(1U, stun_servers_.size());
  EXPECT_EQ("1.2.3.4", stun_servers_.begin()->hostname());
  EXPECT_EQ(3478, stun_servers_.begin()->port());

  EXPECT_TRUE(ParseUrl("stun:[1:2:3:4:5:6:7:8]"));
  EXPECT_EQ(1U, stun_servers_.size());
  EXPECT_EQ("1:2:3:4:5:6:7:8", stun_servers_.begin()->hostname());
  EXPECT_EQ(3478, stun_servers_.begin()->port());

  EXPECT_TRUE(ParseUrl("stun:hostname"));
  EXPECT_EQ(1U, stun_servers_.size());
  EXPECT_EQ("hostname", stun_servers_.begin()->hostname());
  EXPECT_EQ(3478, stun_servers_.begin()->port());

  // Both TURN IP and host exist
  EXPECT_TRUE(
      ParseUrl("turn:1.2.3.4:1234", "username", "password",
               PeerConnectionInterface::TlsCertPolicy::kTlsCertPolicySecure,
               "hostname"));
  EXPECT_EQ(1U, turn_servers_.size());
  rtc::SocketAddress address = turn_servers_[0].ports[0].address;
  EXPECT_EQ("hostname", address.hostname());
  EXPECT_EQ(1234, address.port());
  EXPECT_FALSE(address.IsUnresolvedIP());
  EXPECT_EQ("1.2.3.4", address.ipaddr().ToString());

  // Try some invalid hostname:port strings.
  EXPECT_FALSE(ParseUrl("stun:hostname:99a99"));
  EXPECT_FALSE(ParseUrl("stun:hostname:-1"));
  EXPECT_FALSE(ParseUrl("stun:hostname:port:more"));
  EXPECT_FALSE(ParseUrl("stun:hostname:port more"));
  EXPECT_FALSE(ParseUrl("stun:hostname:"));
  EXPECT_FALSE(ParseUrl("stun:[1:2:3:4:5:6:7:8]junk:1000"));
  EXPECT_FALSE(ParseUrl("stun::5555"));
  EXPECT_FALSE(ParseUrl("stun:"));
}

// Test parsing the "?transport=xxx" part of the URL.
TEST_F(IceServerParsingTest, ParseTransport) {
  EXPECT_TRUE(ParseTurnUrl("turn:hostname:1234?transport=tcp"));
  EXPECT_EQ(1U, turn_servers_.size());
  EXPECT_EQ(cricket::PROTO_TCP, turn_servers_[0].ports[0].proto);

  EXPECT_TRUE(ParseTurnUrl("turn:hostname?transport=udp"));
  EXPECT_EQ(1U, turn_servers_.size());
  EXPECT_EQ(cricket::PROTO_UDP, turn_servers_[0].ports[0].proto);

  EXPECT_FALSE(ParseTurnUrl("turn:hostname?transport=invalid"));
  EXPECT_FALSE(ParseTurnUrl("turn:hostname?transport="));
  EXPECT_FALSE(ParseTurnUrl("turn:hostname?="));
  EXPECT_FALSE(ParseTurnUrl("turn:hostname?"));
  EXPECT_FALSE(ParseTurnUrl("?"));
}

// Test parsing ICE username contained in URL.
TEST_F(IceServerParsingTest, ParseUsername) {
  EXPECT_TRUE(ParseTurnUrl("turn:user@hostname"));
  EXPECT_EQ(1U, turn_servers_.size());
  EXPECT_EQ("user", turn_servers_[0].credentials.username);

  EXPECT_FALSE(ParseTurnUrl("turn:@hostname"));
  EXPECT_FALSE(ParseTurnUrl("turn:username@"));
  EXPECT_FALSE(ParseTurnUrl("turn:@"));
  EXPECT_FALSE(ParseTurnUrl("turn:user@name@hostname"));
}

// Test that username and password from IceServer is copied into the resulting
// RelayServerConfig.
TEST_F(IceServerParsingTest, CopyUsernameAndPasswordFromIceServer) {
  EXPECT_TRUE(ParseUrl("turn:hostname", "username", "password"));
  EXPECT_EQ(1U, turn_servers_.size());
  EXPECT_EQ("username", turn_servers_[0].credentials.username);
  EXPECT_EQ("password", turn_servers_[0].credentials.password);
}

// Ensure that if a server has multiple URLs, each one is parsed.
TEST_F(IceServerParsingTest, ParseMultipleUrls) {
  PeerConnectionInterface::IceServers servers;
  PeerConnectionInterface::IceServer server;
  server.urls.push_back("stun:hostname");
  server.urls.push_back("turn:hostname");
  server.username = "foo";
  server.password = "bar";
  servers.push_back(server);
  EXPECT_EQ(webrtc::RTCErrorType::NONE,
            webrtc::ParseIceServers(servers, &stun_servers_, &turn_servers_));
  EXPECT_EQ(1U, stun_servers_.size());
  EXPECT_EQ(1U, turn_servers_.size());
}

// Ensure that TURN servers are given unique priorities,
// so that their resulting candidates have unique priorities.
TEST_F(IceServerParsingTest, TurnServerPrioritiesUnique) {
  PeerConnectionInterface::IceServers servers;
  PeerConnectionInterface::IceServer server;
  server.urls.push_back("turn:hostname");
  server.urls.push_back("turn:hostname2");
  server.username = "foo";
  server.password = "bar";
  servers.push_back(server);
  EXPECT_EQ(webrtc::RTCErrorType::NONE,
            webrtc::ParseIceServers(servers, &stun_servers_, &turn_servers_));
  EXPECT_EQ(2U, turn_servers_.size());
  EXPECT_NE(turn_servers_[0].priority, turn_servers_[1].priority);
}

}  // namespace webrtc
