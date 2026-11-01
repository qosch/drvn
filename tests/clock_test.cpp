#include "mcu/clock.hpp"

#include <cstdint>
#include <optional>

#include "doctest/doctest.h"

namespace {

using drvn::mcu::rcc::ClockConfig;
using drvn::mcu::rcc::ComputeClockConfig;
using drvn::mcu::rcc::Frequency;

constexpr Frequency Hertz(std::int64_t hz) { return au::hertz(hz); }

constexpr Frequency MegaHertz(std::uint32_t mhz) {
  return au::mega(au::hertz)(mhz);
}

std::uint64_t OutputHz(const ClockConfig& config, std::uint64_t hse_hz) {
  return hse_hz * config.n_ / (std::uint64_t(config.m_) * config.r_);
}

}  // namespace

TEST_CASE("24 MHz HSE to 170 MHz") {
  const std::optional<ClockConfig> config =
      ComputeClockConfig(MegaHertz(24), MegaHertz(170));
  REQUIRE(config.has_value());
  CHECK(config->m_ == 6);
  CHECK(config->n_ == 85);
  CHECK(config->r_ == 2);
  CHECK(config->latency_ == 4);
  CHECK(config->boost_);
  CHECK(config->transition_state_);
  CHECK(OutputHz(*config, 24'000'000) == 170'000'000);
}

TEST_CASE("24 MHz HSE to 144 MHz") {
  const std::optional<ClockConfig> config =
      ComputeClockConfig(MegaHertz(24), MegaHertz(144));
  REQUIRE(config.has_value());
  CHECK(config->m_ == 2);
  CHECK(config->n_ == 24);
  CHECK(config->r_ == 2);
  CHECK(config->latency_ == 4);
  CHECK(!config->boost_);
  CHECK(config->transition_state_);
  CHECK(OutputHz(*config, 24'000'000) == 144'000'000);
}

TEST_CASE("24 MHz HSE to 48 MHz, VCO at the lower bound") {
  const std::optional<ClockConfig> config =
      ComputeClockConfig(MegaHertz(24), MegaHertz(48));
  REQUIRE(config.has_value());
  CHECK(config->m_ == 2);
  CHECK(config->n_ == 8);
  CHECK(config->r_ == 2);
  CHECK(config->latency_ == 1);
  CHECK(!config->boost_);
  CHECK(!config->transition_state_);
  CHECK(OutputHz(*config, 24'000'000) == 48'000'000);
}

TEST_CASE("25 MHz HSE to 100 MHz, fractional PLL input") {
  const std::optional<ClockConfig> config =
      ComputeClockConfig(MegaHertz(25), MegaHertz(100));
  REQUIRE(config.has_value());
  CHECK(config->m_ == 2);
  CHECK(config->n_ == 16);
  CHECK(config->r_ == 2);
  CHECK(config->latency_ == 3);
  CHECK(!config->boost_);
  CHECK(config->transition_state_);
  CHECK(OutputHz(*config, 25'000'000) == 100'000'000);
}

TEST_CASE("24 MHz HSE to 150 MHz, boost boundary") {
  const std::optional<ClockConfig> config =
      ComputeClockConfig(MegaHertz(24), MegaHertz(150));
  REQUIRE(config.has_value());
  CHECK(config->m_ == 2);
  CHECK(config->n_ == 25);
  CHECK(config->r_ == 2);
  CHECK(config->latency_ == 4);
  CHECK(!config->boost_);
  CHECK(config->transition_state_);
  CHECK(OutputHz(*config, 24'000'000) == 150'000'000);
}

TEST_CASE("24 MHz HSE to 80 MHz, transition state boundary") {
  const std::optional<ClockConfig> config =
      ComputeClockConfig(MegaHertz(24), MegaHertz(80));
  REQUIRE(config.has_value());
  CHECK(config->m_ == 3);
  CHECK(config->n_ == 20);
  CHECK(config->r_ == 2);
  CHECK(config->latency_ == 2);
  CHECK(!config->boost_);
  CHECK(!config->transition_state_);
  CHECK(OutputHz(*config, 24'000'000) == 80'000'000);
}

TEST_CASE("24 MHz HSE to 32 MHz") {
  const std::optional<ClockConfig> config =
      ComputeClockConfig(MegaHertz(24), MegaHertz(32));
  REQUIRE(config.has_value());
  CHECK(config->m_ == 2);
  CHECK(config->n_ == 16);
  CHECK(config->r_ == 6);
  CHECK(config->latency_ == 1);
  CHECK(!config->boost_);
  CHECK(!config->transition_state_);
  CHECK(OutputHz(*config, 24'000'000) == 32'000'000);
}

TEST_CASE("24 MHz HSE to 16 MHz, zero wait states") {
  const std::optional<ClockConfig> config =
      ComputeClockConfig(MegaHertz(24), MegaHertz(16));
  REQUIRE(config.has_value());
  CHECK(config->m_ == 2);
  CHECK(config->n_ == 8);
  CHECK(config->r_ == 6);
  CHECK(config->latency_ == 0);
  CHECK(!config->boost_);
  CHECK(!config->transition_state_);
  CHECK(OutputHz(*config, 24'000'000) == 16'000'000);
}

TEST_CASE("24 MHz HSE to 86 MHz, VCO at the upper bound") {
  const std::optional<ClockConfig> config =
      ComputeClockConfig(MegaHertz(24), MegaHertz(86));
  REQUIRE(config.has_value());
  CHECK(config->m_ == 3);
  CHECK(config->n_ == 43);
  CHECK(config->r_ == 4);
  CHECK(config->latency_ == 2);
  CHECK(!config->boost_);
  CHECK(config->transition_state_);
  CHECK(OutputHz(*config, 24'000'000) == 86'000'000);
}

TEST_CASE("24 MHz HSE to 30 MHz, wait state boundary") {
  const std::optional<ClockConfig> config =
      ComputeClockConfig(MegaHertz(24), MegaHertz(30));
  REQUIRE(config.has_value());
  CHECK(config->m_ == 2);
  CHECK(config->n_ == 10);
  CHECK(config->r_ == 4);
  CHECK(config->latency_ == 0);
  CHECK(!config->boost_);
  CHECK(!config->transition_state_);
  CHECK(OutputHz(*config, 24'000'000) == 30'000'000);
}

TEST_CASE("24 MHz HSE to 120 MHz, wait state boundary") {
  const std::optional<ClockConfig> config =
      ComputeClockConfig(MegaHertz(24), MegaHertz(120));
  REQUIRE(config.has_value());
  CHECK(config->m_ == 2);
  CHECK(config->n_ == 20);
  CHECK(config->r_ == 2);
  CHECK(config->latency_ == 3);
  CHECK(!config->boost_);
  CHECK(config->transition_state_);
  CHECK(OutputHz(*config, 24'000'000) == 120'000'000);
}

TEST_CASE("8 MHz HSE to 160 MHz, m = 1") {
  const std::optional<ClockConfig> config =
      ComputeClockConfig(MegaHertz(8), MegaHertz(160));
  REQUIRE(config.has_value());
  CHECK(config->m_ == 1);
  CHECK(config->n_ == 40);
  CHECK(config->r_ == 2);
  CHECK(config->latency_ == 4);
  CHECK(config->boost_);
  CHECK(config->transition_state_);
  CHECK(OutputHz(*config, 8'000'000) == 160'000'000);
}

TEST_CASE("48 MHz HSE to 170 MHz, HSE upper bound") {
  const std::optional<ClockConfig> config =
      ComputeClockConfig(MegaHertz(48), MegaHertz(170));
  REQUIRE(config.has_value());
  CHECK(config->m_ == 12);
  CHECK(config->n_ == 85);
  CHECK(config->r_ == 2);
  CHECK(config->latency_ == 4);
  CHECK(config->boost_);
  CHECK(config->transition_state_);
  CHECK(OutputHz(*config, 48'000'000) == 170'000'000);
}

TEST_CASE("24 MHz HSE to 100100000 Hz, no exact solution") {
  CHECK_FALSE(
      ComputeClockConfig(MegaHertz(24), Hertz(100'100'000)).has_value());
}

TEST_CASE("24 MHz HSE to 200 MHz, sysclk above the limit") {
  CHECK_FALSE(ComputeClockConfig(MegaHertz(24), MegaHertz(200)).has_value());
}

TEST_CASE("50 MHz HSE, above the limit") {
  CHECK_FALSE(ComputeClockConfig(MegaHertz(50), MegaHertz(100)).has_value());
}

TEST_CASE("3999999 Hz HSE, below the limit") {
  CHECK_FALSE(ComputeClockConfig(Hertz(3'999'999), MegaHertz(100)).has_value());
}

TEST_CASE("32 MHz HSE to 12 MHz, fractional PLL input") {
  const std::optional<ClockConfig> config =
      ComputeClockConfig(MegaHertz(32), MegaHertz(12));
  REQUIRE(config.has_value());
  CHECK(config->m_ == 3);
  CHECK(config->n_ == 9);
  CHECK(config->r_ == 8);
  CHECK(config->latency_ == 0);
  CHECK(!config->boost_);
  CHECK(!config->transition_state_);
  CHECK(OutputHz(*config, 32'000'000) == 12'000'000);
}

TEST_CASE("Frequency above the uint32 range, no silent truncation") {
  CHECK_FALSE(
      ComputeClockConfig(Hertz(4'294'967'296 + 24'000'000), MegaHertz(170))
          .has_value());
}

TEST_CASE("Boost wait states at the table boundaries") {
  CHECK(drvn::mcu::rcc::detail::Latency(34'000'000, true) == 0);
  CHECK(drvn::mcu::rcc::detail::Latency(68'000'000, true) == 1);
  CHECK(drvn::mcu::rcc::detail::Latency(102'000'000, true) == 2);
  CHECK(drvn::mcu::rcc::detail::Latency(136'000'000, true) == 3);
  CHECK(drvn::mcu::rcc::detail::Latency(170'000'000, true) == 4);
  CHECK(drvn::mcu::rcc::detail::Latency(50'000'000, true) == 1);
}
