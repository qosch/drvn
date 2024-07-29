#pragma once

#include "stm32g4xx_ll_gpio.h"

class GpioPin
{
public:
    enum gpio_pin_t
    {
        Pin0  = LL_GPIO_PIN_0,
        Pin1  = LL_GPIO_PIN_1,
        Pin2  = LL_GPIO_PIN_2,
        Pin3  = LL_GPIO_PIN_3,
        Pin4  = LL_GPIO_PIN_4,
        Pin5  = LL_GPIO_PIN_5,
        Pin6  = LL_GPIO_PIN_6,
        Pin7  = LL_GPIO_PIN_7,
        Pin8  = LL_GPIO_PIN_8,
        Pin9  = LL_GPIO_PIN_9,
        Pin10 = LL_GPIO_PIN_10,
        Pin11 = LL_GPIO_PIN_11,
        Pin12 = LL_GPIO_PIN_12,
        Pin13 = LL_GPIO_PIN_13,
        Pin14 = LL_GPIO_PIN_14,
        Pin15 = LL_GPIO_PIN_15,
    };

    enum gpio_port_t
    {
        PortA = GPIOA_BASE,
        PortB = GPIOB_BASE,
        PortC = GPIOC_BASE,
        PortD = GPIOD_BASE,
        PortE = GPIOE_BASE,
        PortF = GPIOF_BASE,
        PortG = GPIOG_BASE
    };

    enum gpio_mode_t
    {
        Input             = LL_GPIO_MODE_INPUT,
        Output            = LL_GPIO_MODE_OUTPUT,
        AlternateFunction = LL_GPIO_MODE_ALTERNATE,
        Analog            = LL_GPIO_MODE_ANALOG,
    };

    enum gpio_pupd_t
    {
        None     = LL_GPIO_PULL_NO,
        Pullup   = LL_GPIO_PULL_UP,
        Pulldown = LL_GPIO_PULL_DOWN
    };

    enum gpio_otype_t
    {
        PushPull  = LL_GPIO_OUTPUT_PUSHPULL,
        OpenDrain = LL_GPIO_OUTPUT_OPENDRAIN
    };

    enum gpio_speed_t
    {
        LOW       = LL_GPIO_SPEED_FREQ_LOW,
        MEDIUM    = LL_GPIO_SPEED_FREQ_MEDIUM,
        HIGH      = LL_GPIO_SPEED_FREQ_HIGH,
        VERY_HIGH = LL_GPIO_SPEED_FREQ_VERY_HIGH
    };

    enum gpio_af_t
    {
        AF0  = LL_GPIO_AF_0,
        AF1  = LL_GPIO_AF_1,
        AF2  = LL_GPIO_AF_2,
        AF3  = LL_GPIO_AF_3,
        AF4  = LL_GPIO_AF_4,
        AF5  = LL_GPIO_AF_5,
        AF6  = LL_GPIO_AF_6,
        AF7  = LL_GPIO_AF_7,
        AF8  = LL_GPIO_AF_8,
        AF9  = LL_GPIO_AF_9,
        AF10 = LL_GPIO_AF_10,
        AF11 = LL_GPIO_AF_11,
        AF12 = LL_GPIO_AF_12,
        AF13 = LL_GPIO_AF_13,
        AF14 = LL_GPIO_AF_14,
        AF15 = LL_GPIO_AF_15,
    };

    struct gpio_config_t
    {
        gpio_pin_t pin;
        gpio_port_t port;
        gpio_mode_t mode;
        gpio_pupd_t pupd;
        gpio_otype_t otype;
        gpio_speed_t speed;
        gpio_af_t af;
    };

    GpioPin(gpio_pin_t pinArg, gpio_port_t portArg, gpio_mode_t modeArg, gpio_pupd_t pupdArg, gpio_otype_t otypeArg, gpio_speed_t speedArg, gpio_af_t afArg);
    GpioPin(gpio_config_t gpio_config);
    void set(void);
    void set(bool state);
    void reset(void);
    void toggle(void);
    bool get(void);

private:
    gpio_pin_t pin;
    gpio_port_t port;
    gpio_mode_t mode;
    gpio_pupd_t pupd;
    gpio_otype_t otype;
    gpio_speed_t speed;
    gpio_af_t af;
};