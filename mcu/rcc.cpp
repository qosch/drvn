#include "mcu/rcc.hpp"

#include <array>
#include <cstddef>
#include <cstdint>

#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_pwr.h"
#include "stm32g4xx_ll_rcc.h"
#include "stm32g4xx_ll_system.h"

namespace drvn::mcu::rcc {
namespace {

constexpr std::uint32_t kTimeoutIterations         = 1'600'000;
constexpr std::uint32_t kTransitionDelayIterations = 100;

constexpr std::array<std::uint32_t, 16> kPllM{
    LL_RCC_PLLM_DIV_1,  LL_RCC_PLLM_DIV_2,  LL_RCC_PLLM_DIV_3,
    LL_RCC_PLLM_DIV_4,  LL_RCC_PLLM_DIV_5,  LL_RCC_PLLM_DIV_6,
    LL_RCC_PLLM_DIV_7,  LL_RCC_PLLM_DIV_8,  LL_RCC_PLLM_DIV_9,
    LL_RCC_PLLM_DIV_10, LL_RCC_PLLM_DIV_11, LL_RCC_PLLM_DIV_12,
    LL_RCC_PLLM_DIV_13, LL_RCC_PLLM_DIV_14, LL_RCC_PLLM_DIV_15,
    LL_RCC_PLLM_DIV_16};

constexpr std::array<std::uint32_t, 4> kPllR{
    LL_RCC_PLLR_DIV_2, LL_RCC_PLLR_DIV_4, LL_RCC_PLLR_DIV_6, LL_RCC_PLLR_DIV_8};

constexpr std::array<std::uint32_t, 5> kLatency{
    LL_FLASH_LATENCY_0, LL_FLASH_LATENCY_1, LL_FLASH_LATENCY_2,
    LL_FLASH_LATENCY_3, LL_FLASH_LATENCY_4};

template <typename Predicate>
bool WaitFor(Predicate predicate) {
  for (std::uint32_t i = 0; i < kTimeoutIterations; ++i) {
    if (predicate()) {
      return true;
    }
  }
  return false;
}

void Delay() {
  for (volatile std::uint32_t i = 0; i < kTransitionDelayIterations; i += 1) {
  }
}

}  // namespace

std::optional<ClockError> ApplyClock(const ClockConfig& config) {
  if (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI ||
      LL_RCC_PLL_IsReady() != 0U) {
    return ClockError::kNotResetState;
  }

  if (config.transition_state_) {
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_2);
    while (LL_RCC_GetAHBPrescaler() != LL_RCC_SYSCLK_DIV_2) {
    }
  }

  if (config.boost_) {
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
    LL_PWR_EnableRange1BoostMode();
  }

  const std::uint32_t latency =
      kLatency[static_cast<std::size_t>(config.latency_)];
  LL_FLASH_SetLatency(latency);
  while (LL_FLASH_GetLatency() != latency) {
  }

  LL_RCC_PLL_ConfigDomain_SYS(
      LL_RCC_PLLSOURCE_HSE, kPllM[static_cast<std::size_t>(config.m_ - 1)],
      config.n_, kPllR[static_cast<std::size_t>(config.r_ / 2 - 1)]);

  LL_RCC_HSE_Enable();
  if (!WaitFor([] { return LL_RCC_HSE_IsReady() != 0U; })) {
    LL_RCC_HSE_Disable();
    return ClockError::kHseNotReady;
  }

  LL_RCC_PLL_Enable();
  if (!WaitFor([] { return LL_RCC_PLL_IsReady() != 0U; })) {
    LL_RCC_PLL_Disable();
    LL_RCC_HSE_Disable();
    return ClockError::kPllNotReady;
  }

  LL_RCC_PLL_EnableDomain_SYS();
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
  if (!WaitFor([] {
        return LL_RCC_GetSysClkSource() == LL_RCC_SYS_CLKSOURCE_STATUS_PLL;
      })) {
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);
    return ClockError::kSwitchFailed;
  }

  if (config.transition_state_) {
    Delay();
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  }

  return std::nullopt;
}

}  // namespace drvn::mcu::rcc
