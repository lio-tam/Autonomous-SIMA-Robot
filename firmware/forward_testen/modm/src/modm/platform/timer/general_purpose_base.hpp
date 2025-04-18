/*
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2014, Fabian Greif
 * Copyright (c) 2014-2017, Niklas Hauser
 * Copyright (c) 2022-2023, Christopher Durand
 * Copyright (c) 2023, Sergey Pluzhnikov
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_STM32_TIMER_GP_BASE_HPP
#define MODM_STM32_TIMER_GP_BASE_HPP

#include "basic_base.hpp"
#include <modm/platform/core/peripherals.hpp>
#include <modm/platform/gpio/data.hpp>

namespace modm
{

namespace platform
{

/// @ingroup	modm_platform_timer
class GeneralPurposeTimer : public BasicTimer
{
public:
	enum class DmaRequestEnable : uint32_t
	{
		Update = TIM_DIER_UDE,
		CaptureCompare1 = TIM_DIER_CC1DE,
		CaptureCompare2 = TIM_DIER_CC2DE,
		CaptureCompare3 = TIM_DIER_CC3DE,
		CaptureCompare4 = TIM_DIER_CC4DE,
		COM = TIM_DIER_COMDE,
		Trigger = TIM_DIER_TDE,
	};

	enum class Interrupt : uint32_t
	{
		Update = TIM_DIER_UIE,
		CaptureCompare1 = TIM_DIER_CC1IE,
		CaptureCompare2 = TIM_DIER_CC2IE,
		CaptureCompare3 = TIM_DIER_CC3IE,
		CaptureCompare4 = TIM_DIER_CC4IE,
		Trigger = TIM_DIER_TIE,
		COM = TIM_DIER_COMIE,
		Break = TIM_DIER_BIE,
	};
	MODM_FLAGS32(Interrupt);

	enum class InterruptFlag : uint32_t
	{
		Update = TIM_SR_UIF,
		CaptureCompare1 = TIM_SR_CC1IF,
		CaptureCompare2 = TIM_SR_CC2IF,
		CaptureCompare3 = TIM_SR_CC3IF,
		CaptureCompare4 = TIM_SR_CC4IF,
		Trigger = TIM_SR_TIF,
		Overcapture1 = TIM_SR_CC1OF,
		Overcapture2 = TIM_SR_CC2OF,
		Overcapture3 = TIM_SR_CC3OF,
		Overcapture4 = TIM_SR_CC4OF,
		COM = TIM_SR_COMIF,
		Break = TIM_SR_BIF,
	};
	MODM_FLAGS32(InterruptFlag);

	enum class SlaveModeTrigger : uint32_t
	{
	};

	enum class SlaveMode : uint32_t
	{
		Disabled = 0,
		/// Counter counts up/down on TI2FP2 edge depending on TI1FP1 level.
		Encoder1	= TIM_SMCR_SMS_0,
		/// Counter counts up/down on TI1FP1 edge depending on TI2FP2 level.
		Encoder2	= TIM_SMCR_SMS_1,
		/// Counter counts up/down on both TI1FP1 and TI2FP2 edges depending on the level of the other input.
		Encoder3	= TIM_SMCR_SMS_1 | TIM_SMCR_SMS_0,
	};

	enum class Mode : uint32_t
	{
		UpCounter = 0,
		DownCounter = TIM_CR1_DIR,
		OneShotUpCounter = TIM_CR1_OPM,
		OneShotDownCounter = TIM_CR1_DIR | TIM_CR1_OPM,
		// Counter counts up and down alternatively
		CenterAligned1 =                 TIM_CR1_CMS_0,	///< Output compare flags only set when counting down
		CenterAligned2 = TIM_CR1_CMS_1, 					///< Output compare flags only set when counting up
		CenterAligned3 = TIM_CR1_CMS_1 | TIM_CR1_CMS_0,	///< Output compare flags set when counting up and down (default)
	};

	enum class OutputCompareMode : uint32_t
	{
		/// Output is independent from the compare result
		Inactive = 0,
		/// Output is forced high on match
		HighOnMatch = TIM_CCMR1_OC1M_0,
		/// Output is forced low on match
		LowOnMatch = TIM_CCMR1_OC1M_1,
		/// Output is toggled on match
		Toggle = TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_0,
		/// Output is forced low
		ForceInactive = TIM_CCMR1_OC1M_2,
		/// Output is forced high
		ForceActive = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_0,

		/**
		 * PWM Mode 1.
		 *
		 * While up-counting, channel is active as long as
		 * count is less than channel capture/compare register, else
		 * inactive.
		 * In downcounting, channel is	inactive as long as count exceeds
		 * capture/compare register, else active
		 */
		Pwm = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1,

		/**
		 * PWM mode 2.
		 *
		 * In upcounting, channel is inactive as long as
		 * count is less than capture/compare register, else active.
		 * In downcounting, channel is active as long as count exceeds
		 * capture/compare	register, else inactive.
		 */
		Pwm2 = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_0,

#ifdef TIM_CCMR1_OC1M_3
		/**
		 * Combined PWM mode 1.
		 *
		 *  OC1REF has the same behavior as in PWM mode 1.
		 *  OC1REFC is the logical OR between OC1REF and OC2REF.
		 */
		Combined1 = TIM_CCMR1_OC1M_3 | TIM_CCMR1_OC1M_2,

		/**
		 * Combined PWM mode 2.
		 *
		 *  OC1REF has the same behavior as in PWM mode 2.
		 * 	OC1REFC is the logical AND between OC1REF and OC2REF.
		 */
		Combined2 = TIM_CCMR1_OC1M_3 | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_0,

		/**
		 *  Asymmetric PWM mode 1.
		 *
		 *  OC1REF has the same behavior as in PWM mode 1.
		 *	OC1REFC outputs OC1REF when the counter is counting up,
		 *	OC2REF when it is counting down.
		 */
		Asymmetric1 = TIM_CCMR1_OC1M_3 | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1,

		/**
		 * Asymmetric PWM mode 2.
		 *
		 * OC1REF has the same behavior as in PWM mode 2.
		 * OC1REFC outputs OC1REF when the counter is counting up,
		 * OC2REF when it is counting down.
		 */
		Asymmetric2 = TIM_CCMR1_OC1M_3 | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_0,
#endif
	};
	MODM_FLAGS32(OutputCompareMode);

	enum class OutputComparePreload : uint32_t
	{
		Disable = 0,
		Enable  = TIM_CCMR1_OC1PE,
	};


	enum class OutputComparePolarity : uint32_t
	{
		ActiveHigh = 0,
		ActiveLow = TIM_CCER_CC1P,
	};

	/**
	 * If more than one Compare Channel is available they are always paired.
	 * 		(A channel with an odd number is paired with the next even numbered channel)
	 * It is possible to choose as trigger for the input capture functionality either
	 * the own timer input or the input associated with the paired channel.
	 *
	 * For working with Internal Triggers, TRC can also be selected.
	 */
	enum class InputCaptureMapping : uint32_t
	{
		InputOwn 			= 1,
		InputOther			= 2,
		InternalTrigger 	= 3,
	};

	enum class InputCapturePolarity : uint32_t
	{
		Rising	= 0,
		Falling	= TIM_CCER_CC1P,
		Both	= TIM_CCER_CC1NP | TIM_CCER_CC1P,
	};

	enum class InputCapturePrescaler : uint32_t
	{
		Div1	= 0,
		Div2	= 1,
		Div4	= 2,
		Div8	= 3,
	};

	enum class PinState : uint32_t
	{
		Disable = 0,
		Enable = 1,
	};

	/* Different Resolution depending on DeadTime[7:5]: */
	enum class DeadTimeResolution : uint32_t
	{
		From0With125nsStep = 0,									//0xx
		From16usWith250nsStep = TIM_BDTR_DTG_7,					//10x
		From32usWith1usStep   = TIM_BDTR_DTG_7 | TIM_BDTR_DTG_6,	//110
		From64usWith2usStep  = TIM_BDTR_DTG_7 | TIM_BDTR_DTG_6	//111
											| TIM_BDTR_DTG_5,
	};
	enum class OffStateForRunMode : uint32_t
	{
		Disable = 0,
		Enable  = TIM_BDTR_OSSR,
	};

	enum class OffStateForIdleMode : uint32_t
	{
		Disable = 0,
		Enable  = TIM_BDTR_OSSI,
	};
public:
	/**
	 * Set operation mode of the timer
	 *
	 * In Encoder mode the encoder channels A and B must be connected
	 * to channel 1 and 2 of the timer (e.g. TIM2_CH1 and TIM2_CH2).
	 */
	static void
	setMode(Mode mode, SlaveMode slaveMode = SlaveMode::Disabled,
			SlaveModeTrigger slaveModeTrigger = (SlaveModeTrigger) 0);



public:
	/**
	 * TODO Description
	 */
	static void
	configureInputChannel(uint32_t channel, InputCaptureMapping input,
			InputCapturePrescaler prescaler, InputCapturePolarity polarity, uint8_t filter);

	/**
	 * Configure output channel 1..4.
	 *
	 * @param	channel			[1..4]
	 * @param	mode			Output compare mode
	 * @param	compareValue	Preloaded output compare value (can be
	 * 							changed later via setCompareValue())
	 */
	static void
	configureOutputChannel(uint32_t channel, OutputCompareMode_t mode,
			uint16_t compareValue);

	/**
	 * Set compare value for channel 1..4.
	 *
	 * @param	channel	[1..4]
	 * @param	value	Compare value
	 */
	static void
	setCompareValue(uint32_t channel, uint16_t value);

	/**
	 * Read compare value for channel 1..4.
	 *
	 * @param	channel	[1..4]
	 * @return	Current compare value
	 */
	static uint16_t
	getCompareValue(uint32_t channel);

public:
	template< typename Signal>
	static consteval bool
	isComplementaryChannel()
	{
		if constexpr (Signal::Signal == Gpio::Signal::Ch1n) {
			return true;
		} else if constexpr (Signal::Signal == Gpio::Signal::Ch2n) {
			return true;
		} else if constexpr (Signal::Signal == Gpio::Signal::Ch3n) {
			return true;
		} else if constexpr (Signal::Signal == Gpio::Signal::Ch4n) {
			return true;
		}
		return false;
	}

protected:
	template<Peripheral p, typename Signal>
	static consteval int
	signalToChannel()
	{
		modm::platform::detail::SignalConnection<Signal, p>{};
		if constexpr (Signal::Signal == Gpio::Signal::Ch1) {
			return 1;
		} else if constexpr (Signal::Signal == Gpio::Signal::Ch1n) {
			return 1;
		} else if constexpr (Signal::Signal == Gpio::Signal::Ch2) {
			return 2;
		} else if constexpr (Signal::Signal == Gpio::Signal::Ch2n) {
			return 2;
		} else if constexpr (Signal::Signal == Gpio::Signal::Ch3) {
			return 3;
		} else if constexpr (Signal::Signal == Gpio::Signal::Ch3n) {
			return 3;
		} else if constexpr (Signal::Signal == Gpio::Signal::Ch4) {
			return 4;
		} else if constexpr (Signal::Signal == Gpio::Signal::Ch4n) {
			return 4;
		} else {
			// assert is always false, static_assert(false) would not compile
			static_assert(!sizeof(Signal), "Invalid timer channel");
		}
	}
};

} // namespace platform

} // namespace modm

#endif // MODM_STM32_TIMER_GP_BASE_HPP