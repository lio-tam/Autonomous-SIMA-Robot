// coding: utf-8
/*
 * Copyright (c) 2017, Sascha Schade
 * Copyright (c) 2017-2018, Niklas Hauser
 * Copyright (c) 2018, 2023, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_STM32_I2C_1_HPP
#define MODM_STM32_I2C_1_HPP

#include <modm/architecture/interface/interrupt.hpp>
#include <modm/architecture/interface/i2c_master.hpp>
#include <modm/architecture/interface/clock.hpp>
#include <modm/platform/gpio/connector.hpp>

#include "i2c_timing_calculator.hpp"

namespace modm
{

namespace platform
{

/**
 * I2cMaster implementation of I2C1 module.
 *
 * Interrupts must be enabled.
 *
 * @author		Georgi Grinshpun
 * @author		Niklas Hauser
 * @author		Sascha Schade (strongly-typed)
 * @ingroup		modm_platform_i2c modm_platform_i2c_1
 */
class I2cMaster1 : public ::modm::I2cMaster
{
public:
	static constexpr size_t TransactionBufferSize = 8;

private:
	template<class SystemClock, baudrate_t baudrate, percent_t tolerance>
	static constexpr std::optional<uint32_t>
	calculateTimings()
	{
		constexpr I2cParameters parameters = {
			.peripheralClock = SystemClock::I2c1,
			.targetSpeed = baudrate,
			.tolerance = tolerance,
			.digitalFilterLength = 0,
			.enableAnalogFilter = true,
			.riseTime = 0,
			.fallTime = 0
		};

		auto calculator = I2cTimingCalculator{parameters};

		std::optional<I2cMasterTimings> timings = calculator.calculateTimings();
		if(timings) {
			return I2cTimingCalculator::timingsToRegisterValue(timings.value());
		} else {
			return std::nullopt;
		}
	}

public:
	template<class... Signals>
	static void
	connect(PullUps pullups = PullUps::External, ResetDevices reset = ResetDevices::Standard)
	{
		using Connector = GpioConnector<Peripheral::I2c1, Signals...>;
		using Scl = typename Connector::template GetSignal<Gpio::Signal::Scl>;
		using Sda = typename Connector::template GetSignal<Gpio::Signal::Sda>;
		static_assert(sizeof...(Signals) == 2 and
					  Connector::template IsValid<Scl> and Connector::template IsValid<Sda>,
					  "I2cMaster1::connect() requires one Scl and one Sda signal!");
		const Gpio::InputType input =
			(pullups == PullUps::Internal) ? Gpio::InputType::PullUp : Gpio::InputType::Floating;

		Connector::disconnect();
		Scl::configure(input);
		Sda::configure(input);
		Scl::setOutput(Gpio::OutputType::OpenDrain);
		Sda::setOutput(Gpio::OutputType::OpenDrain);
		if (reset != ResetDevices::NoReset) resetDevices<Scl>(uint32_t(reset));
		Connector::connect();
	}

	/**
	 * Set up the I2C module for master operation.
	 *
	 * @param	rate
	 *		`Standard` or `Fast`, `High` datarate is not supported
	 */
	template<class SystemClock, baudrate_t baudrate=kBd(100), percent_t tolerance=pct(5)>
	static void
	initialize(uint8_t isrPriority = 10u)
	{
		constexpr std::optional<uint32_t> timingRegisterValue = calculateTimings<SystemClock, baudrate, tolerance>();
		static_assert(bool(timingRegisterValue), "Could not find a valid clock configuration for the requested baudrate");

		initializeWithPrescaler(timingRegisterValue.value(), isrPriority);
	}

	static bool
	start(I2cTransaction *transaction, ConfigurationHandler handler = nullptr);

	static Error
	getErrorState();

	static void
	reset();

private:
	static void
	initializeWithPrescaler(uint32_t timingRegisterValue, uint8_t isrPriority = 10u);

};


} // namespace platform

} // namespace modm

#endif // MODM_STM32_I2C_1_HPP