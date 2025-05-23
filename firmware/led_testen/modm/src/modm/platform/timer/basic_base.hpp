/*
 * Copyright (c) 2013-2014, Kevin Läufer
 * Copyright (c) 2014-2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_STM32_TIMER_BASIC_BASE_HPP
#define MODM_STM32_TIMER_BASIC_BASE_HPP

#include <chrono>
#include <limits>
#include <stdint.h>
#include "../device.hpp"
#include <modm/architecture/interface/register.hpp>

namespace modm
{

namespace platform
{

/// @ingroup	modm_platform_timer
class BasicTimer
{
public:
	enum class Mode : uint32_t
	{
		UpCounter = 0,
		OneShotUpCounter = TIM_CR1_OPM,
	};

	enum class Interrupt : uint32_t
	{
		Update = TIM_DIER_UIE,
	};
	MODM_FLAGS32(Interrupt);

	enum class InterruptFlag : uint32_t
	{
		Update = TIM_SR_UIF,
	};
	MODM_FLAGS32(InterruptFlag);

	enum class Event : uint32_t
	{
		Break2 						= TIM_EGR_B2G,
		Trigger 					= TIM_EGR_TG,
		CaptureCompare4 			= TIM_EGR_CC4G,
		CaptureCompare3 			= TIM_EGR_CC3G,
		CaptureCompare2 			= TIM_EGR_CC2G,
		CaptureCompare1 			= TIM_EGR_CC1G,
		Update 						= TIM_EGR_UG,
	};

public:
	// This type is the internal size of the counter.
	using Value = uint16_t;

	/**
	 * Enables the clock for the timer and resets all settings
	 *
	 * Has to be called before calling any other function from this
	 * class! Otherwise the settings won't have any effect.
	 */
	static void
	enable();

	/**
	 * Disable clock.
	 *
	 * All settings are ignored in this mode and the timer is
	 * shut down. Calling any function other than enable() won't
	 * have any effect.
	 */
	static void
	disable();

	/**
	 * Check, whether clock has been enabled.
	 */
	static bool
	isEnabled();

	/**
	 * Pause timer operation
	 *
	 * All settings are stored but the timer operation is suspend until
	 * start() is called.
	 */
	static void
	pause();

	/**
	 * Re-enable timer operations
	 *
	 * Has to called after the initial configuration to start timer
	 * or after pause() to restart the timer.
	 */
	static void
	start();

	/**
	 * Set operation mode of the timer
	 */
	static void
	setMode(Mode mode);

	/**
	 * Set new prescaler
	 *
	 * The prescaler can divide the counter clock frequency by any
	 * factor between 1 and 65'536. The new prescaler ratio is taken
	 * into account at the next update event.
	 *
	 * @see		applyAndReset()
	 */
	static inline void
	setPrescaler(uint16_t prescaler);

	/**
	 * Get current prescaler
	 */
	static inline uint16_t
	getPrescaler();

	/**
	 * Set overflow.
	 *
	 * This sets the maximum counter value of the timer.
	 * The timer is blocked if \p overflow is set to zero.
	 *
	 * Takes effect at next update event.
	 *
	 * @see		applyAndReset()
	 */
	static inline void
	setOverflow(Value overflow);

	/**
	 * Get current overflow
	 */
	static inline Value
	getOverflow();

	/**
	 * Set Timer period
	 *
	 * Changes prescaler and overflow values.
	 * Takes effect at next update event.
	 *
	 * @param	duration		Requested duration of period
	 * @param	autoApply		Update the new value immediately and
	 * 							reset the counter value.
	 *
	 * @return	New overflow value.
	 *
	 * @see		applyAndReset()
	 */
	template<class Rep, class Period>
	static Value
	setPeriod(std::chrono::duration<Rep, Period> duration, bool autoApply = true);

	/**
	 * @brief	Reset the counter, and update the prescaler and
	 * 			overflow values.
	 *
	 * Generates an Update-Event without generating an
	 * Update-Interrupt.
	 *
	 * This will reset the counter to 0 in up-counting mode (the
	 * default) or to the maximal value in down-counting mode. It will
	 * also update the timer's prescaler and overflow values if you
	 * have set them up to be changed using setPrescaler() or
	 * setOverflow() (or setPeriod()).
	 *
	 * An Update-Event is also generated when the timer reaches its
	 * maximal (up-counting) or minimal (down-counting) value. The
	 * settings for Prescaler, Overflow and Compare values are applied
	 * then without calling this function.
	 */
	static inline void
	applyAndReset();

	/**
	 * Get the counter value
	 */
	static inline Value
	getValue();

	/**
	 * Set a new counter value
	 */
	static inline void
	setValue(Value value);

	/**
	 * Allows to check, whether the timer has BDTR and DTR2 registers
	 * for PWM deadtime, break and output enable control.
	 */
	static constexpr bool
	hasAdvancedPwmControl();

	/**
	 * Check current count direction
	 */
	static inline bool
	isCountingUp();
	static inline bool
	isCountingDown();

	/**
	 * Enables or disables the Interrupt Vector.
	 *
	 * \param	enable
	 * 				Enable/disable the interrupt vector
	 * \param	priority
	 * 				Priority of the interrupt vector
	 * 				(0=highest to 15=lowest).
	 */
	static void
	enableInterruptVector(bool enable, uint8_t priority);

	/**
	 * Enables interrupts. Don't forget to enable the Interrupt Vector.
	 *
	 * \param	interrupt
	 * 				Interrupts to enable
	 *
	 * \see		enableInterruptVector()
	 */
	static void
	enableInterrupt(Interrupt_t interrupt);

	/**
	 * Disables interrupts.
	 *
	 * \param	interrupt
	 * 				Interrupts to disable
	 */
	static void
	disableInterrupt(Interrupt_t interrupt);

	/**
	 * Returns a bitmap of the enum StateFlag. Use this method while
	 * executing an interrupt or in other situations where you want to
	 * know which of the flags are set.
	 *
	 * If a flag is a cause of an enabled interrupt (and the Interrupt
	 * Vector is enabled) then interrupt will be triggered as long
	 * the flag is set.
	 *
	 * \warning		You cannot use a switch statement for the returned
	 * 				value, since multiple flags may be set!
	 *
	 * \code
	 * InterruptFlag flags = TimerX::getInterruptFlags()
	 * if (flags & TimerX::FLAG_XX){
	 *     // Flag was set, clear the flag since flags are set by
	 *     // hardware, but have to be cleared by software
	 *     TimerX::resetInterruptFlags(TIMx::FLAG_XX)
	 * }
	 * \endcode
	 */
	static InterruptFlag_t
	getInterruptFlags();

	/**
	 * Clears one or multiple flags.
	 *
	 * \param flags		Bitmap of StateFlag
	 */
	static void
	acknowledgeInterruptFlags(InterruptFlag_t flags);
};

} // namespace platform

} // namespace modm

#endif // MODM_STM32_TIMER_BASIC_BASE_HPP