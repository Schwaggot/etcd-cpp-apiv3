#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <chrono>
#include <iostream>
#include <thread>

#include "etcd/Client.hpp"
#include "etcd/KeepAlive.hpp"

static const std::string etcd_url = etcdv3::detail::resolve_etcd_endpoints("http://127.0.0.1:2379");

TEST_CASE("setup")
{
  etcd::Client etcd(etcd_url);
  etcd.rmdir("/test", true).wait();
}

TEST_CASE("campaign and resign")
{
  etcd::Client etcd(etcd_url);

  auto keepalive = etcd.leasekeepalive(60).get();
  auto lease_id = keepalive->Lease();

  // campaign
  auto resp1 = etcd.campaign("test", lease_id, "xxxx").get();
  REQUIRE(0 == resp1.error_code());

  // leader
  {
    auto resp2 = etcd.leader("test").get();
    REQUIRE(0 == resp2.error_code());
    REQUIRE(resp1.value().key() == resp2.value().key());
    REQUIRE("xxxx" == resp2.value().as_string());
  }

  // proclaim
  auto resp3 = etcd.proclaim("test", lease_id,
                             resp1.value().key(), resp1.value().created_index(),
                             "tttt").get();
  REQUIRE(0 == resp3.error_code());

  // leader
  {
    auto resp4 = etcd.leader("test").get();
    REQUIRE(0 == resp4.error_code());
    REQUIRE(resp1.value().key() == resp4.value().key());
    REQUIRE("tttt" == resp4.value().as_string());
  }

  // resign
  auto resp5 = etcd.resign("test", lease_id,
                           resp1.value().key(), resp1.value().created_index()).get();
  REQUIRE(0 == resp5.error_code());
}

TEST_CASE("cleanup")
{
  etcd::Client etcd(etcd_url);
  etcd.rmdir("/test", true).get();
}
