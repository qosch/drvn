#pragma once

#include <array>
#include <cstdint>
#include <optional>

#include "au/au.hh"
#include "au/units/hertz.hh"

namespace drvn::mcu::rcc {

using Frequency = au::QuantityI64<au::Hertz>;

struct ClockConfig {
  std::uint8_t m_;
  std::uint8_t n_;
  std::uint8_t r_;
  std::uint8_t latency_;
  bool boost_;
  bool transition_state_;
};

consteval std::optional<ClockConfig> ComputeClockConfig(Frequency hse,
                                                        Frequency sysclk);

namespace detail {

// RM0440 §7.2.1, §7.2.4, §7.4.4; DS12288 §5.3.9 Table 46; RM0440 §7.2.7
constexpr std::int64_t kHseMinHz        = 4'000'000;
constexpr std::int64_t kHseMaxHz        = 48'000'000;
constexpr std::int64_t kSysclkMaxHz     = 170'000'000;
constexpr std::int64_t kFInMinHz        = 2'660'000;
constexpr std::int64_t kFInMaxHz        = 16'000'000;
constexpr std::int64_t kVcoMinHz        = 96'000'000;
constexpr std::int64_t kVcoMaxHz        = 344'000'000;
constexpr std::int64_t kBoostMinHz      = 150'000'000;
constexpr std::int64_t kTransitionMinHz = 80'000'000;

// RM0440 FLASH Table 19, category 4
constexpr std::uint8_t Latency(std::int64_t sysclk_hz, bool boost) {
  constexpr std::array<std::int64_t, 5> kBoostMaxHz{
      34'000'000, 68'000'000, 102'000'000, 136'000'000, 170'000'000};
  constexpr std::array<std::int64_t, 5> kNormalMaxHz{
      30'000'000, 60'000'000, 90'000'000, 120'000'000, 150'000'000};
  const std::array<std::int64_t, 5>& max_hz =
      boost ? kBoostMaxHz : kNormalMaxHz;
  for (std::size_t i = 0; i < max_hz.size(); ++i) {
    if (sysclk_hz <= max_hz[i]) {
      return static_cast<std::uint8_t>(i);
    }
  }
  return 4;
}

constexpr std::optional<ClockConfig> Solve(std::int64_t hse_hz,
                                           std::int64_t sysclk_hz) {
  if (hse_hz < kHseMinHz || hse_hz > kHseMaxHz || sysclk_hz <= 0 ||
      sysclk_hz > kSysclkMaxHz) {
    return std::nullopt;
  }
  constexpr std::array<std::int64_t, 4> kR{2, 4, 6, 8};
  for (std::int64_t m = 1; m <= 16; ++m) {
    if (hse_hz < kFInMinHz * m || hse_hz > kFInMaxHz * m) {
      continue;
    }
    for (const std::int64_t r : kR) {
      const std::int64_t num = sysclk_hz * m * r;
      if (num % hse_hz != 0) {
        continue;
      }
      const std::int64_t n = num / hse_hz;
      if (n < 8 || n > 127) {
        continue;
      }
      const std::int64_t vco = hse_hz * n;
      if (vco < kVcoMinHz * m || vco > kVcoMaxHz * m) {
        continue;
      }
      const bool boost = sysclk_hz > kBoostMinHz;
      return ClockConfig{static_cast<std::uint8_t>(m),
                         static_cast<std::uint8_t>(n),
                         static_cast<std::uint8_t>(r),
                         Latency(sysclk_hz, boost),
                         boost,
                         sysclk_hz > kTransitionMinHz};
    }
  }
  return std::nullopt;
}

}  // namespace detail

consteval std::optional<ClockConfig> ComputeClockConfig(Frequency hse,
                                                        Frequency sysclk) {
  return detail::Solve(hse.data_in(au::hertz), sysclk.data_in(au::hertz));
}

}  // namespace drvn::mcu::rcc
