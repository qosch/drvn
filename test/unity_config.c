#include "unity_config.h"
#include "stm32g4xx_ll_rcc.h"
#include "stm32g4xx_ll_utils.h"
#include "stm32g4xx_ll_system.h"
#include "stm32g4xx_ll_gpio.h"
#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_lpuart.h"
#include "stm32g4xx_ll_pwr.h"

#define USARTx LPUART1

#define UART_SPEED 115200
#define HSE_OSC_VALUE 24000000
#define WAIT_MS_BEFORE_TEST_START 250

#define USARTx_TX_PIN GPIO_PIN_2
#define USARTx_TX_GPIO_PORT GPIOA
#define USARTx_TX_AF GPIO_AF12_LPUART1
#define USARTx_RX_PIN GPIO_PIN_3
#define USARTx_RX_GPIO_PORT GPIOA
#define USARTx_RX_AF GPIO_AF12_LPUART1

void unityOutputStart(void)
{
    uint32_t hseFreq = HSE_OSC_VALUE;
    uint32_t pllM = LL_RCC_PLLM_DIV_2;
    uint32_t pllN = 28;
    uint32_t pllP = LL_RCC_PLLP_DIV_2;
    uint32_t pllQ = LL_RCC_PLLQ_DIV_2;
    uint32_t pllR = LL_RCC_PLLR_DIV_2;
    uint32_t ahb = LL_RCC_SYSCLK_DIV_1;
    uint32_t apb1 = LL_RCC_APB1_DIV_1;
    uint32_t apb2 = LL_RCC_APB2_DIV_1;
    /* Flash Latency configuration */
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);

    /* Enable boost mode to be able to reach 170MHz */
    LL_PWR_EnableRange1BoostMode();

    /* HSE configuration and activation */
    LL_RCC_HSE_Enable();
    while (LL_RCC_HSE_IsReady() != 1);

    /* Main PLL configuration and activation */
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, pllM, pllN, pllR);
    LL_RCC_PLL_ConfigDomain_48M(LL_RCC_PLLSOURCE_HSE, pllM, pllN, pllQ);
    LL_RCC_PLL_ConfigDomain_ADC(LL_RCC_PLLSOURCE_HSE, pllM, pllN, pllP);

    LL_RCC_PLL_Enable();
    LL_RCC_PLL_EnableDomain_SYS();
    LL_RCC_PLL_EnableDomain_48M();
    while (LL_RCC_PLL_IsReady() != 1);

    /* Sysclk activation on the main PLL */
    LL_RCC_SetAHBPrescaler(ahb);
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL);

    /* Insure 1µs transition state at intermediate medium speed clock based on DWT */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    while (DWT->CYCCNT < 100);

    LL_RCC_SetAPB1Prescaler(apb1);
    LL_RCC_SetAPB2Prescaler(apb2);

    LL_Init1msTick(__LL_RCC_CALC_PLLCLK_FREQ(hseFreq, pllM, pllN, pllR));

    LL_SetSystemCoreClock(__LL_RCC_CALC_PLLCLK_FREQ(hseFreq, pllM, pllN, pllR));

    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);

    LL_GPIO_SetPinMode(USARTx_TX_GPIO_PORT, USARTx_TX_PIN, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinOutputType(USARTx_TX_GPIO_PORT, USARTx_TX_PIN, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinSpeed(USARTx_TX_GPIO_PORT, USARTx_TX_PIN, LL_GPIO_SPEED_FREQ_HIGH);
    LL_GPIO_SetPinPull(USARTx_TX_GPIO_PORT, USARTx_TX_PIN, LL_GPIO_PULL_NO);
    if (USARTx_TX_PIN <= LL_GPIO_PIN_7)
        LL_GPIO_SetAFPin_0_7(USARTx_TX_GPIO_PORT, USARTx_TX_PIN, USARTx_TX_AF);
    else
        LL_GPIO_SetAFPin_8_15(USARTx_TX_GPIO_PORT, USARTx_TX_PIN, USARTx_TX_AF);

    LL_GPIO_SetPinMode(USARTx_RX_GPIO_PORT, USARTx_RX_PIN, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinOutputType(USARTx_RX_GPIO_PORT, USARTx_RX_PIN, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinSpeed(USARTx_RX_GPIO_PORT, USARTx_RX_PIN, LL_GPIO_SPEED_FREQ_HIGH);
    LL_GPIO_SetPinPull(USARTx_RX_GPIO_PORT, USARTx_RX_PIN, LL_GPIO_PULL_NO);
    if (USARTx_RX_PIN <= LL_GPIO_PIN_7)
        LL_GPIO_SetAFPin_0_7(USARTx_RX_GPIO_PORT, USARTx_RX_PIN, USARTx_RX_AF);
    else
        LL_GPIO_SetAFPin_8_15(USARTx_RX_GPIO_PORT, USARTx_RX_PIN, USARTx_RX_AF);

    /* (2) Configure LPUART1 functional parameters */
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_LPUART1);
    LL_RCC_SetLPUARTClockSource(LL_RCC_LPUART1_CLKSOURCE_PCLK1);

    LL_LPUART_Disable(USARTx);
    LL_LPUART_SetTransferDirection(USARTx, LL_LPUART_DIRECTION_TX_RX);
    LL_LPUART_ConfigCharacter(USARTx, LL_LPUART_DATAWIDTH_8B, LL_LPUART_PARITY_NONE, LL_LPUART_STOPBITS_1);
    LL_LPUART_SetPrescaler(USARTx, LL_LPUART_PRESCALER_DIV1);
    LL_LPUART_SetBaudRate(USARTx, __LL_RCC_CALC_PCLK1_FREQ(__LL_RCC_CALC_HCLK_FREQ(__LL_RCC_CALC_PLLCLK_FREQ(hseFreq, pllM, pllN, pllR), ahb), apb1), LL_LPUART_PRESCALER_DIV1, UART_SPEED);
    LL_LPUART_Enable(USARTx);

    LL_mDelay(WAIT_MS_BEFORE_TEST_START);
}

void unityOutputChar(char c)
{
    while (!LL_LPUART_IsActiveFlag_TXE(USARTx));
    LL_LPUART_TransmitData8(USARTx, c);
}

void unityOutputFlush() {}

void unityOutputComplete() {}
