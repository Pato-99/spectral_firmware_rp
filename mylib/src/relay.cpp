
#include <pico/stdlib.h>

#include "relay.h"

Relay::Relay(uint relayPin)
    : relayPin(relayPin)
{
    gpio_init(this->relayPin);
    gpio_set_dir(this->relayPin, GPIO_OUT);
    this->off();
}

void Relay::on() const
{
    gpio_put(this->relayPin, false);
}

void Relay::off() const
{
    gpio_put(this->relayPin, true);
}

bool Relay::status() const
{
    return !gpio_get(this->relayPin);
}
