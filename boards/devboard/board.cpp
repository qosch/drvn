#include "boards/devboard/board.hpp"

#include "mcu/clock.hpp"
#include "mcu/rcc.hpp"
#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_gpio.h"

namespace drvn::board {
namespace {

constexpr auto kClockConfig =
    drvn::mcu::rcc::ComputeClockConfig(kHse, kSysclk).value();

}  // namespace

Status Init() {
  if (drvn::mcu::rcc::ApplyClock(kClockConfig).has_value()) {
    return Status::kClockFailure;
  }
  return Status::kOk;
}

void IndicateFailure() {
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOD);
  LL_GPIO_SetPinMode(GPIOD, LL_GPIO_PIN_10, LL_GPIO_MODE_OUTPUT);
  LL_GPIO_SetOutputPin(GPIOD, LL_GPIO_PIN_10);
}

}  // namespace drvn::board
