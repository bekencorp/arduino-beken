// Copyright 2025-2026 Beken
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "Arduino.h"

#include <driver/gpio.h>
extern "C" bk_err_t gpio_dev_unmap(gpio_id_t gpio_id);

namespace {
struct InterruptCallback {
    voidFuncPtr callback;
    voidFuncPtrParam callbackParam;
    void* param;
    bool hasParam;
    PinStatus mode;
    gpio_int_type_t currentIntType;
};

InterruptCallback s_interruptCallbacks[SOC_GPIO_NUM] = {0};
PinMode s_pinModes[SOC_GPIO_NUM] = {INPUT};

gpio_id_t to_gpio(uint8_t pin) {
    return static_cast<gpio_id_t>(pin);
}

gpio_int_type_t arduinoPinStatusToGpioIntType(PinStatus mode) {
    switch (mode) {
        case LOW:
            return GPIO_INT_TYPE_LOW_LEVEL;
        case HIGH:
            return GPIO_INT_TYPE_HIGH_LEVEL;
        case RISING:
            return GPIO_INT_TYPE_RISING_EDGE;
        case FALLING:
            return GPIO_INT_TYPE_FALLING_EDGE;
        case CHANGE:
            return GPIO_INT_TYPE_FALLING_EDGE;
        default:
            return GPIO_INT_TYPE_FALLING_EDGE;
    }
}

// Keep pull configuration from pinMode(); attachInterrupt used to drop it and leave the pad floating.
void configureInterruptPinInput(gpio_id_t gpio, pin_size_t pin) {
    gpio_dev_unmap(gpio);
    bk_gpio_disable_output(gpio);
    bk_gpio_enable_input(gpio);
    bk_gpio_disable_pull(gpio);

    switch (s_pinModes[pin]) {
        case INPUT_PULLUP:
            bk_gpio_enable_pull(gpio);
            bk_gpio_pull_up(gpio);
            break;
        case INPUT_PULLDOWN:
            bk_gpio_enable_pull(gpio);
            bk_gpio_pull_down(gpio);
            break;
        default:
            break;
    }
}

void gpio_isr_handler(gpio_id_t gpio_id) {
    InterruptCallback& cb = s_interruptCallbacks[gpio_id];
    
    // 先清除中断标志
    bk_gpio_clear_interrupt(gpio_id);
    
    if (cb.hasParam && cb.callbackParam) {
        cb.callbackParam(cb.param);
    } else if (cb.callback) {
        cb.callback();
    }
    
    if (cb.mode == CHANGE) {
        cb.currentIntType = (cb.currentIntType == GPIO_INT_TYPE_RISING_EDGE) 
            ? GPIO_INT_TYPE_FALLING_EDGE 
            : GPIO_INT_TYPE_RISING_EDGE;
        
        bk_gpio_disable_interrupt(gpio_id);
        bk_gpio_set_interrupt_type(gpio_id, cb.currentIntType);
        bk_gpio_enable_interrupt(gpio_id);
    }
}

void enableGpioInterrupt(pin_size_t pin, PinStatus mode) {
    gpio_id_t gpio = to_gpio(pin);
    configureInterruptPinInput(gpio, pin);

    gpio_int_type_t intType = arduinoPinStatusToGpioIntType(mode);
    if (mode == CHANGE) {
        bool currentLevel = bk_gpio_get_input(gpio);
        intType = currentLevel ? GPIO_INT_TYPE_FALLING_EDGE : GPIO_INT_TYPE_RISING_EDGE;
    }

    s_interruptCallbacks[pin].mode = mode;
    s_interruptCallbacks[pin].currentIntType = intType;

    bk_gpio_disable_interrupt(gpio);
    bk_gpio_clear_interrupt(gpio);
    bk_gpio_set_interrupt_type(gpio, intType);
    bk_gpio_register_isr(gpio, gpio_isr_handler);
    bk_gpio_enable_interrupt(gpio);
}
}

void pinMode(uint8_t pin, PinMode mode) {
    const gpio_id_t gpio = to_gpio(pin);
    if (pin < SOC_GPIO_NUM) {
        s_pinModes[pin] = mode;
    }

    // BK7236 GPIOs can stay mapped to a peripheral function by default.
    // Switch the pad back to plain GPIO before changing direction.
    gpio_dev_unmap(gpio);
    bk_gpio_disable_pull(gpio);

    switch (mode) {
        case OUTPUT:
            bk_gpio_disable_input(gpio);
            bk_gpio_enable_output(gpio);
            break;
        case INPUT_PULLUP:
            bk_gpio_disable_output(gpio);
            bk_gpio_enable_input(gpio);
            bk_gpio_enable_pull(gpio);
            bk_gpio_pull_up(gpio);
            break;
        case INPUT_PULLDOWN:
            bk_gpio_disable_output(gpio);
            bk_gpio_enable_input(gpio);
            bk_gpio_enable_pull(gpio);
            bk_gpio_pull_down(gpio);
            break;
        case INPUT:
        default:
            bk_gpio_disable_output(gpio);
            bk_gpio_enable_input(gpio);
            break;
    }
}

void digitalWrite(uint8_t pin, PinStatus value) {
    if (value == LOW) {
        bk_gpio_set_output_low(to_gpio(pin));
    } else {
        bk_gpio_set_output_high(to_gpio(pin));
    }
}

PinStatus digitalRead(uint8_t pin) {
    return bk_gpio_get_input(to_gpio(pin)) ? HIGH : LOW;
}

void attachInterrupt(pin_size_t interruptNumber, voidFuncPtr callback, PinStatus mode) {
    if (interruptNumber >= SOC_GPIO_NUM) {
        return;
    }

    s_interruptCallbacks[interruptNumber].callback = callback;
    s_interruptCallbacks[interruptNumber].callbackParam = nullptr;
    s_interruptCallbacks[interruptNumber].param = nullptr;
    s_interruptCallbacks[interruptNumber].hasParam = false;
    enableGpioInterrupt(interruptNumber, mode);
}

void attachInterruptParam(pin_size_t interruptNumber, voidFuncPtrParam callback, PinStatus mode, void* param) {
    if (interruptNumber >= SOC_GPIO_NUM) {
        return;
    }

    s_interruptCallbacks[interruptNumber].callback = nullptr;
    s_interruptCallbacks[interruptNumber].callbackParam = callback;
    s_interruptCallbacks[interruptNumber].param = param;
    s_interruptCallbacks[interruptNumber].hasParam = true;
    enableGpioInterrupt(interruptNumber, mode);
}

void detachInterrupt(pin_size_t interruptNumber) {
    if (interruptNumber >= SOC_GPIO_NUM) {
        return;
    }
    
    gpio_id_t gpio = to_gpio(interruptNumber);
    
    bk_gpio_disable_interrupt(gpio);
    bk_gpio_unregister_isr(gpio);
    
    s_interruptCallbacks[interruptNumber].callback = nullptr;
    s_interruptCallbacks[interruptNumber].callbackParam = nullptr;
    s_interruptCallbacks[interruptNumber].param = nullptr;
    s_interruptCallbacks[interruptNumber].hasParam = false;
}