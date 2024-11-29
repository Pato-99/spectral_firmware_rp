#ifndef AVCR_SPECTRAL_FW_RELAY_H
#define AVCR_SPECTRAL_FW_RELAY_H


class Relay {
private:
    uint relayPin;
public:
    explicit Relay(uint relayPin);
    void on() const;
    void off() const;
    bool status() const;
};

#endif //AVCR_SPECTRAL_FW_RELAY_H
