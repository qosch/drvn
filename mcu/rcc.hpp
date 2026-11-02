#pragma once

#include <cstdint>
#include <optional>

#include "mcu/clock.hpp"

namespace drvn::mcu::rcc {

enum class ClockError : std::uint8_t {
  kNotResetState,
  kHseNotReady,
  kPllNotReady,
  kSwitchFailed,
};

std::optional<ClockError> ApplyClock(const ClockConfig& config);

}  // namespace drvn::mcu::rcc
