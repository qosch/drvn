#pragma once

#include "au/au.hh"
#include "au/units/hertz.hh"
#include "boards/common/board_status.hpp"

namespace drvn::board {

constexpr auto kHse    = au::mega(au::hertz)(24);
constexpr auto kSysclk = au::mega(au::hertz)(144);

Status Init();

void IndicateFailure();

}  // namespace drvn::board
