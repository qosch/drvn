#pragma once

#include <cstdint>

namespace drvn::board {

enum class Status : std::uint8_t {
  kOk           = 0,
  kClockFailure = 1,
};

}  // namespace drvn::board
