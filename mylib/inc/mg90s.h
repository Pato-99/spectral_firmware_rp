#ifndef MG90S_H
#define MG90S_H

#define MG90S_PWM_CLK_DIV 40.0
#define MG90S_PWM_WRAP 62499
#define MG90S_PWM_LEVEL_MIN 1562
#define MG90S_PWM_LEVEL_MID 4167
#define MG90S_PWM_LEVEL_MAX 7812

#include "pico/stdlib.h"

class MG90S
{
private:
	const uint pin;
	const uint pwmSlice;
	const uint pwmChannel;

	// uint currentLevel;

public:
	explicit MG90S(uint pin);
	void enable() const;
	void disable() const;

	// void setAngle(uint level);
	void setLevel(uint16_t level) const;
// private:
	// uint angleToLevel(uint angle);
	// uint dutyToAngle(uint duty);
};

#endif  // MG90S_H
