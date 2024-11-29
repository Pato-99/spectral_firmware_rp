#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "mg90s.h"


MG90S::MG90S(uint pin)
    : pin(pin),
      pwmSlice(pwm_gpio_to_slice_num(pin)),
      pwmChannel(pwm_gpio_to_channel(pin))
{
    gpio_set_function(pin, GPIO_FUNC_PWM);
    
    // assuming that pico runs at 125 000 000 Hz
    pwm_set_clkdiv(this->pwmSlice, MG90S_PWM_CLK_DIV);
    pwm_set_wrap(this->pwmSlice, MG90S_PWM_WRAP);
}

void MG90S::enable() const
{
    pwm_set_enabled(this->pwmSlice, true);
}

void MG90S::disable() const
{
    pwm_set_enabled(this->pwmSlice, false);
}

void MG90S::setLevel(uint16_t level) const
{
    pwm_set_chan_level(this->pwmSlice, this->pwmChannel, level);
}
