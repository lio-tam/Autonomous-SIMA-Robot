/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2012, 2016-2017, Fabian Greif
 * Copyright (c) 2011-2012, Georgi Grinshpun
 * Copyright (c) 2013, 2016, Kevin Läufer
 * Copyright (c) 2014, Sascha Schade
 * Copyright (c) 2014, 2016-2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "timer_15.hpp"
#include <modm/platform/clock/rcc.hpp>

// ----------------------------------------------------------------------------
void
modm::platform::Timer15::clockEnable()
{
	enable();
}

void
modm::platform::Timer15::enable()
{
	Rcc::enable<Peripheral::Tim15>();
}

void
modm::platform::Timer15::disable()
{
	TIM15->CR1 = 0;
	TIM15->DIER = 0;
	TIM15->CCER = 0;

	Rcc::disable<Peripheral::Tim15>();
}

bool
modm::platform::Timer15::isEnabled()
{
	return Rcc::isEnabled<Peripheral::Tim15>();
}

// ----------------------------------------------------------------------------
void
modm::platform::Timer15::setMode(Mode mode, SlaveMode slaveMode,
		SlaveModeTrigger slaveModeTrigger, MasterMode masterMode,
		bool enableOnePulseMode, bool bufferAutoReloadRegister,
		bool limitUpdateEventRequestSource)
{
	// disable timer
	TIM15->CR1 = 0;
	TIM15->CR2 = 0;

	if (slaveMode == SlaveMode::Encoder1 || \
		slaveMode == SlaveMode::Encoder2 || \
		slaveMode == SlaveMode::Encoder3)
	{
		// Prescaler has to be 1 when using the quadrature decoder
		setPrescaler(1);
	}
	uint32_t cr1 = static_cast<uint32_t>(mode);
	if(bufferAutoReloadRegister)
	{
		cr1 |= TIM_CR1_ARPE;
	}
	if(limitUpdateEventRequestSource)
	{
		cr1 |= TIM_CR1_URS;
	}
	if (enableOnePulseMode) {
		TIM15->CR1 = cr1 | TIM_CR1_OPM;
	} else {
		TIM15->CR1 = cr1;
	}
	TIM15->CR2 = static_cast<uint32_t>(masterMode);
	TIM15->SMCR = static_cast<uint32_t>(slaveMode)
						| static_cast<uint32_t>(slaveModeTrigger);
}

// ----------------------------------------------------------------------------
void
modm::platform::Timer15::configureInputChannel(uint32_t channel, uint8_t filter) {
		channel -= 1;	// 1..4 -> 0..3

	// disable channel
	TIM15->CCER &= ~(TIM_CCER_CC1E << (channel * 4));

	uint32_t flags = static_cast<uint32_t>(filter&0xf) << 4;

	if (channel <= 1)
	{
		const uint32_t offset = 8 * channel;

		flags <<= offset;
		flags |= TIM15->CCMR1 & ~(0xf0 << offset);

		TIM15->CCMR1 = flags;
	}
	else {
		const uint32_t offset = 8 * (channel - 2);

		flags <<= offset;
		flags |= TIM15->CCMR2 & ~(0xf0 << offset);

		TIM15->CCMR2 = flags;
	}
	TIM15->CCER |= TIM_CCER_CC1E << (channel * 4);
}

void
modm::platform::Timer15::configureInputChannel(uint32_t channel,
		InputCaptureMapping input, InputCapturePrescaler prescaler,
		InputCapturePolarity polarity, uint8_t filter,
		bool xor_ch1_3)
{
	channel -= 1;	// 1..4 -> 0..3

	// disable channel
	TIM15->CCER &= ~((TIM_CCER_CC1NP | TIM_CCER_CC1P | TIM_CCER_CC1E) << (channel * 4));

	uint32_t flags = static_cast<uint32_t>(input);
	flags |= static_cast<uint32_t>(prescaler) << 2;
	flags |= (static_cast<uint32_t>(filter) & 0xf) << 4;

	if (channel <= 1)
	{
		uint32_t offset = 8 * channel;

		flags <<= offset;
		flags |= TIM15->CCMR1 & ~(0xff << offset);

		TIM15->CCMR1 = flags;

		if(channel == 0) {
			if(xor_ch1_3)
				TIM15->CR2 |= TIM_CR2_TI1S;
			else
				TIM15->CR2 &= ~TIM_CR2_TI1S;
		}
	}
	else {
		uint32_t offset = 8 * (channel - 2);

		flags <<= offset;
		flags |= TIM15->CCMR2 & ~(0xff << offset);

		TIM15->CCMR2 = flags;
	}

	TIM15->CCER |=
		(TIM_CCER_CC1E | static_cast<uint32_t>(polarity)) << (channel * 4);
}

// ----------------------------------------------------------------------------
void
modm::platform::Timer15::configureOutputChannel(uint32_t channel,
		OutputCompareMode_t mode, Value compareValue, PinState out,
		bool enableComparePreload)
{
	channel -= 1;	// 1..4 -> 0..3

	// disable channel
	TIM15->CCER &= ~((TIM_CCER_CC1NP | TIM_CCER_CC1P | TIM_CCER_CC1E) << (channel * 4));

	setCompareValue(channel + 1, compareValue);

	uint32_t flags = mode.value;
	if(enableComparePreload)
	{
		// enable preload (the compare value is loaded at each update event)
		flags |= TIM_CCMR1_OC1PE;
	}

	if (channel <= 1)
	{
		uint32_t offset = 8 * channel;

		flags <<= offset;
		flags |= TIM15->CCMR1 & ~(0xff << offset);

		TIM15->CCMR1 = flags;
	}
	else {
		uint32_t offset = 8 * (channel - 2);

		flags <<= offset;
		flags |= TIM15->CCMR2 & ~(0xff << offset);

		TIM15->CCMR2 = flags;
	}

	if (mode != OutputCompareMode::Inactive && out == PinState::Enable) {
		TIM15->CCER |= (TIM_CCER_CC1E) << (channel * 4);
	}
}

void
modm::platform::Timer15::configureOutputChannel(uint32_t channel,
OutputCompareMode mode, Value compareValue,
PinState out, OutputComparePolarity polarity,
OutputComparePreload preload)
{
	// disable output
	TIM15->CCER &= ~(0xf << ((channel-1) * 4));
	setCompareValue(channel, compareValue);
	configureOutputChannel(channel, mode, out, polarity,  PinState::Disable, OutputComparePolarity::ActiveHigh, preload);
}

void
modm::platform::Timer15::configureOutputChannel(uint32_t channel,
OutputCompareMode mode,
PinState out, OutputComparePolarity polarity,
PinState out_n, OutputComparePolarity polarity_n,
OutputComparePreload preload)
{
	modm_assert(channel == 1, "Timer15", "This timer has complementary output only on channel 1!", "15");
	channel -= 1;	// 1..4 -> 0..3

	// disable output
	TIM15->CCER &= ~(0xf << (channel * 4));

	uint32_t flags = static_cast<uint32_t>(mode) | static_cast<uint32_t>(preload);

	if (channel <= 1)
	{
		const uint32_t offset = 8 * channel;

		flags <<= offset;
		flags |= TIM15->CCMR1 & ~(0xff << offset);

		TIM15->CCMR1 = flags;
	}
	else {
		const uint32_t offset = 8 * (channel - 2);

		flags <<= offset;
		flags |= TIM15->CCMR2 & ~(0xff << offset);

		TIM15->CCMR2 = flags;
	}

	// CCER Flags (Enable/Polarity)
	flags = (static_cast<uint32_t>(polarity_n) << 2) |
			(static_cast<uint32_t>(out_n)      << 2) |
			 static_cast<uint32_t>(polarity) | static_cast<uint32_t>(out);

	TIM15->CCER |= flags << (channel * 4);
}

// ----------------------------------------------------------------------------
void
modm::platform::Timer15::enableInterruptVector(bool enable, uint32_t priority)
{
	if (enable)
	{
		NVIC_SetPriority(TIM1_BRK_TIM15_IRQn, priority);
		NVIC_EnableIRQ(TIM1_BRK_TIM15_IRQn);
	}
	else
	{
		NVIC_DisableIRQ(TIM1_BRK_TIM15_IRQn);
	}
}

// ----------------------------------------------------------------------------
