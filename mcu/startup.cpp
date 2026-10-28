#include <cstddef>
#include <cstdint>

#include "stm32g474xx.h"

extern "C" {

int main();

// mcu/linker/stm32g474ve.ld
extern "C" char __stack_top[];
extern std::uint8_t const __data_load[];
extern std::uint8_t __data_start[];
extern std::uint8_t __data_end[];
extern std::uint8_t __bss_start[];
extern std::uint8_t __bss_end[];
extern void (*__init_array_start[])();
extern void (*__init_array_end[])();

}  // extern "C"

#define DEFAULT_IRQ_HANDLER(name) \
  extern "C" void name() { for (;;) {} }

DEFAULT_IRQ_HANDLER(NMI_Handler)
DEFAULT_IRQ_HANDLER(HardFault_Handler)
DEFAULT_IRQ_HANDLER(MemManage_Handler)
DEFAULT_IRQ_HANDLER(BusFault_Handler)
DEFAULT_IRQ_HANDLER(UsageFault_Handler)
DEFAULT_IRQ_HANDLER(SVC_Handler)
DEFAULT_IRQ_HANDLER(DebugMon_Handler)
DEFAULT_IRQ_HANDLER(PendSV_Handler)
DEFAULT_IRQ_HANDLER(SysTick_Handler)
DEFAULT_IRQ_HANDLER(Default_IRQHandler)

DEFAULT_IRQ_HANDLER(WWDG_IRQHandler)
DEFAULT_IRQ_HANDLER(PVD_PVM_IRQHandler)
DEFAULT_IRQ_HANDLER(RTC_TAMP_LSECSS_IRQHandler)
DEFAULT_IRQ_HANDLER(RTC_WKUP_IRQHandler)
DEFAULT_IRQ_HANDLER(FLASH_IRQHandler)
DEFAULT_IRQ_HANDLER(RCC_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI0_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI1_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI2_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI3_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI4_IRQHandler)
DEFAULT_IRQ_HANDLER(DMA1_Channel1_IRQHandler)
DEFAULT_IRQ_HANDLER(DMA1_Channel2_IRQHandler)
DEFAULT_IRQ_HANDLER(DMA1_Channel3_IRQHandler)
DEFAULT_IRQ_HANDLER(DMA1_Channel4_IRQHandler)
DEFAULT_IRQ_HANDLER(DMA1_Channel5_IRQHandler)
DEFAULT_IRQ_HANDLER(DMA1_Channel6_IRQHandler)
DEFAULT_IRQ_HANDLER(DMA1_Channel7_IRQHandler)
DEFAULT_IRQ_HANDLER(ADC1_2_IRQHandler)
DEFAULT_IRQ_HANDLER(USB_HP_IRQHandler)
DEFAULT_IRQ_HANDLER(USB_LP_IRQHandler)
DEFAULT_IRQ_HANDLER(FDCAN1_IT0_IRQHandler)
DEFAULT_IRQ_HANDLER(FDCAN1_IT1_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI9_5_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM1_BRK_TIM15_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM1_UP_TIM16_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM1_TRG_COM_TIM17_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM1_CC_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM2_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM3_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM4_IRQHandler)
DEFAULT_IRQ_HANDLER(I2C1_EV_IRQHandler)
DEFAULT_IRQ_HANDLER(I2C1_ER_IRQHandler)
DEFAULT_IRQ_HANDLER(I2C2_EV_IRQHandler)
DEFAULT_IRQ_HANDLER(I2C2_ER_IRQHandler)
DEFAULT_IRQ_HANDLER(SPI1_IRQHandler)
DEFAULT_IRQ_HANDLER(SPI2_IRQHandler)
DEFAULT_IRQ_HANDLER(USART1_IRQHandler)
DEFAULT_IRQ_HANDLER(USART2_IRQHandler)
DEFAULT_IRQ_HANDLER(USART3_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI15_10_IRQHandler)
DEFAULT_IRQ_HANDLER(RTC_Alarm_IRQHandler)
DEFAULT_IRQ_HANDLER(USBWakeUp_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM8_BRK_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM8_UP_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM8_TRG_COM_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM8_CC_IRQHandler)
DEFAULT_IRQ_HANDLER(ADC3_IRQHandler)
DEFAULT_IRQ_HANDLER(FMC_IRQHandler)
DEFAULT_IRQ_HANDLER(LPTIM1_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM5_IRQHandler)
DEFAULT_IRQ_HANDLER(SPI3_IRQHandler)
DEFAULT_IRQ_HANDLER(UART4_IRQHandler)
DEFAULT_IRQ_HANDLER(UART5_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM6_DAC_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM7_DAC_IRQHandler)
DEFAULT_IRQ_HANDLER(DMA2_Channel1_IRQHandler)
DEFAULT_IRQ_HANDLER(DMA2_Channel2_IRQHandler)
DEFAULT_IRQ_HANDLER(DMA2_Channel3_IRQHandler)
DEFAULT_IRQ_HANDLER(DMA2_Channel4_IRQHandler)
DEFAULT_IRQ_HANDLER(DMA2_Channel5_IRQHandler)
DEFAULT_IRQ_HANDLER(ADC4_IRQHandler)
DEFAULT_IRQ_HANDLER(ADC5_IRQHandler)
DEFAULT_IRQ_HANDLER(UCPD1_IRQHandler)
DEFAULT_IRQ_HANDLER(COMP1_2_3_IRQHandler)
DEFAULT_IRQ_HANDLER(COMP4_5_6_IRQHandler)
DEFAULT_IRQ_HANDLER(COMP7_IRQHandler)
DEFAULT_IRQ_HANDLER(HRTIM1_Master_IRQHandler)
DEFAULT_IRQ_HANDLER(HRTIM1_TIMA_IRQHandler)
DEFAULT_IRQ_HANDLER(HRTIM1_TIMB_IRQHandler)
DEFAULT_IRQ_HANDLER(HRTIM1_TIMC_IRQHandler)
DEFAULT_IRQ_HANDLER(HRTIM1_TIMD_IRQHandler)
DEFAULT_IRQ_HANDLER(HRTIM1_TIME_IRQHandler)
DEFAULT_IRQ_HANDLER(HRTIM1_FLT_IRQHandler)
DEFAULT_IRQ_HANDLER(HRTIM1_TIMF_IRQHandler)
DEFAULT_IRQ_HANDLER(CRS_IRQHandler)
DEFAULT_IRQ_HANDLER(SAI1_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM20_BRK_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM20_UP_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM20_TRG_COM_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM20_CC_IRQHandler)
DEFAULT_IRQ_HANDLER(FPU_IRQHandler)
DEFAULT_IRQ_HANDLER(I2C4_EV_IRQHandler)
DEFAULT_IRQ_HANDLER(I2C4_ER_IRQHandler)
DEFAULT_IRQ_HANDLER(SPI4_IRQHandler)
DEFAULT_IRQ_HANDLER(FDCAN2_IT0_IRQHandler)
DEFAULT_IRQ_HANDLER(FDCAN2_IT1_IRQHandler)
DEFAULT_IRQ_HANDLER(FDCAN3_IT0_IRQHandler)
DEFAULT_IRQ_HANDLER(FDCAN3_IT1_IRQHandler)
DEFAULT_IRQ_HANDLER(RNG_IRQHandler)
DEFAULT_IRQ_HANDLER(LPUART1_IRQHandler)
DEFAULT_IRQ_HANDLER(I2C3_EV_IRQHandler)
DEFAULT_IRQ_HANDLER(I2C3_ER_IRQHandler)
DEFAULT_IRQ_HANDLER(DMAMUX_OVR_IRQHandler)
DEFAULT_IRQ_HANDLER(QUADSPI_IRQHandler)
DEFAULT_IRQ_HANDLER(DMA1_Channel8_IRQHandler)
DEFAULT_IRQ_HANDLER(DMA2_Channel6_IRQHandler)
DEFAULT_IRQ_HANDLER(DMA2_Channel7_IRQHandler)
DEFAULT_IRQ_HANDLER(DMA2_Channel8_IRQHandler)
DEFAULT_IRQ_HANDLER(CORDIC_IRQHandler)
DEFAULT_IRQ_HANDLER(FMAC_IRQHandler)

#undef DEFAULT_IRQ_HANDLER

extern "C" void Reset_Handler()
{
  // PM0214 §4.6.1
  SCB->CPACR = (3U << 20U) | (3U << 22U);
  __DSB();
  __ISB();

  std::uint8_t* dst = __data_start;
  std::uint8_t const* src = __data_load;
  while (dst < __data_end)
    *dst++ = *src++;

  for (std::uint8_t* p = __bss_start; p < __bss_end; ++p)
    *p = 0;

  for (auto f = __init_array_start; f < __init_array_end; ++f)
    (*f)();

  main();

  for (;;) {}
}

namespace {

struct IsrVector
{
  const void* initial_stack_pointer_;
  void (*handlers_[117])();
};

// 0: initial SP. 1-15: ARMv7-M core exceptions, reserved slots null. 16+: IRQn_Type, stm32g474xx.h
constinit const IsrVector isr_vector [[gnu::section(".isr_vector")]] [[gnu::used]] = {
  &__stack_top,
  {
    &Reset_Handler,
    &NMI_Handler,
    &HardFault_Handler,
    &MemManage_Handler,
    &BusFault_Handler,
    &UsageFault_Handler,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    &SVC_Handler,
    &DebugMon_Handler,
    nullptr,
    &PendSV_Handler,
    &SysTick_Handler,
    &WWDG_IRQHandler,
    &PVD_PVM_IRQHandler,
    &RTC_TAMP_LSECSS_IRQHandler,
    &RTC_WKUP_IRQHandler,
    &FLASH_IRQHandler,
    &RCC_IRQHandler,
    &EXTI0_IRQHandler,
    &EXTI1_IRQHandler,
    &EXTI2_IRQHandler,
    &EXTI3_IRQHandler,
    &EXTI4_IRQHandler,
    &DMA1_Channel1_IRQHandler,
    &DMA1_Channel2_IRQHandler,
    &DMA1_Channel3_IRQHandler,
    &DMA1_Channel4_IRQHandler,
    &DMA1_Channel5_IRQHandler,
    &DMA1_Channel6_IRQHandler,
    &DMA1_Channel7_IRQHandler,
    &ADC1_2_IRQHandler,
    &USB_HP_IRQHandler,
    &USB_LP_IRQHandler,
    &FDCAN1_IT0_IRQHandler,
    &FDCAN1_IT1_IRQHandler,
    &EXTI9_5_IRQHandler,
    &TIM1_BRK_TIM15_IRQHandler,
    &TIM1_UP_TIM16_IRQHandler,
    &TIM1_TRG_COM_TIM17_IRQHandler,
    &TIM1_CC_IRQHandler,
    &TIM2_IRQHandler,
    &TIM3_IRQHandler,
    &TIM4_IRQHandler,
    &I2C1_EV_IRQHandler,
    &I2C1_ER_IRQHandler,
    &I2C2_EV_IRQHandler,
    &I2C2_ER_IRQHandler,
    &SPI1_IRQHandler,
    &SPI2_IRQHandler,
    &USART1_IRQHandler,
    &USART2_IRQHandler,
    &USART3_IRQHandler,
    &EXTI15_10_IRQHandler,
    &RTC_Alarm_IRQHandler,
    &USBWakeUp_IRQHandler,
    &TIM8_BRK_IRQHandler,
    &TIM8_UP_IRQHandler,
    &TIM8_TRG_COM_IRQHandler,
    &TIM8_CC_IRQHandler,
    &ADC3_IRQHandler,
    &FMC_IRQHandler,
    &LPTIM1_IRQHandler,
    &TIM5_IRQHandler,
    &SPI3_IRQHandler,
    &UART4_IRQHandler,
    &UART5_IRQHandler,
    &TIM6_DAC_IRQHandler,
    &TIM7_DAC_IRQHandler,
    &DMA2_Channel1_IRQHandler,
    &DMA2_Channel2_IRQHandler,
    &DMA2_Channel3_IRQHandler,
    &DMA2_Channel4_IRQHandler,
    &DMA2_Channel5_IRQHandler,
    &ADC4_IRQHandler,
    &ADC5_IRQHandler,
    &UCPD1_IRQHandler,
    &COMP1_2_3_IRQHandler,
    &COMP4_5_6_IRQHandler,
    &COMP7_IRQHandler,
    &HRTIM1_Master_IRQHandler,
    &HRTIM1_TIMA_IRQHandler,
    &HRTIM1_TIMB_IRQHandler,
    &HRTIM1_TIMC_IRQHandler,
    &HRTIM1_TIMD_IRQHandler,
    &HRTIM1_TIME_IRQHandler,
    &HRTIM1_FLT_IRQHandler,
    &HRTIM1_TIMF_IRQHandler,
    &CRS_IRQHandler,
    &SAI1_IRQHandler,
    &TIM20_BRK_IRQHandler,
    &TIM20_UP_IRQHandler,
    &TIM20_TRG_COM_IRQHandler,
    &TIM20_CC_IRQHandler,
    &FPU_IRQHandler,
    &I2C4_EV_IRQHandler,
    &I2C4_ER_IRQHandler,
    &SPI4_IRQHandler,
    &Default_IRQHandler,  /* no IRQn 85 on STM32G474 */
    &FDCAN2_IT0_IRQHandler,
    &FDCAN2_IT1_IRQHandler,
    &FDCAN3_IT0_IRQHandler,
    &FDCAN3_IT1_IRQHandler,
    &RNG_IRQHandler,
    &LPUART1_IRQHandler,
    &I2C3_EV_IRQHandler,
    &I2C3_ER_IRQHandler,
    &DMAMUX_OVR_IRQHandler,
    &QUADSPI_IRQHandler,
    &DMA1_Channel8_IRQHandler,
    &DMA2_Channel6_IRQHandler,
    &DMA2_Channel7_IRQHandler,
    &DMA2_Channel8_IRQHandler,
    &CORDIC_IRQHandler,
    &FMAC_IRQHandler,
  }
};

static_assert(sizeof(isr_vector) == 118u * 4u);

// pins the table layout to the IRQn numbers in stm32g474xx.h
static_assert(static_cast<std::size_t>(WWDG_IRQn) == 0u);
static_assert(static_cast<std::size_t>(SPI4_IRQn) == 84u);
static_assert(static_cast<std::size_t>(FDCAN2_IT0_IRQn) == 86u);
static_assert(static_cast<std::size_t>(FMAC_IRQn) == 101u);

}  // namespace
