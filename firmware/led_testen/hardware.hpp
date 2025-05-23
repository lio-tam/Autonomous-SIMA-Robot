/*
 * Copyright (c) 2024, Raphael Lehmann
 *
 * This file is part of the RCA project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef HARDWARE_HPP
#define HARDWARE_HPP

#include <modm/platform.hpp>
#include <modm/architecture/interface/clock.hpp>
#include <modm/platform/i2c/i2c_master_1.hpp>
#include <modm/platform/uart/uart_hal_1.hpp>

using namespace modm::platform;

namespace Board
{

	using namespace modm::literals;

	/// STM32G474RE running at 170MHz generated from the internal 16MHz crystal
	struct SystemClock
	{
		static constexpr uint32_t Frequency = 170_MHz;
		static constexpr uint32_t Ahb1      = Frequency;
		static constexpr uint32_t Ahb2      = Frequency;
		static constexpr uint32_t Apb1      = Frequency;
		static constexpr uint32_t Apb2      = Frequency;

		static constexpr uint32_t Cordic    = Ahb1;
		static constexpr uint32_t Crc       = Ahb1;
		static constexpr uint32_t Dma       = Ahb1;
		static constexpr uint32_t Dma1      = Dma;
		static constexpr uint32_t Dma2      = Dma;
		static constexpr uint32_t DmaMux    = Dma;
		static constexpr uint32_t Fmac      = Ahb1;

		static constexpr uint32_t Adc       = Ahb2;
		static constexpr uint32_t Adc1      = Adc;
		static constexpr uint32_t Adc2      = Adc;
		static constexpr uint32_t Adc3      = Adc;
		static constexpr uint32_t Adc4      = Adc;
		static constexpr uint32_t Adc5      = Adc;
		static constexpr uint32_t Dac       = Ahb2;
		static constexpr uint32_t Dac1      = Dac;
		static constexpr uint32_t Dac2      = Dac;
		static constexpr uint32_t Dac3      = Dac;
		static constexpr uint32_t Dac4      = Dac;
		static constexpr uint32_t Rng       = Ahb2;

		static constexpr uint32_t Can       = Apb1;
		static constexpr uint32_t Fdcan1    = Can;
		static constexpr uint32_t Fdcan2    = Can;
		static constexpr uint32_t Fdcan3    = Can;
		static constexpr uint32_t I2c       = Apb1;
		static constexpr uint32_t I2c1      = I2c;
		static constexpr uint32_t I2c2      = I2c;
		static constexpr uint32_t I2c3      = I2c;
		static constexpr uint32_t I2c4      = I2c;
		static constexpr uint32_t Lptim     = Apb1;
		static constexpr uint32_t Lpuart    = Apb1;
		static constexpr uint32_t Rtc       = Apb1;
		static constexpr uint32_t Spi2      = Apb1;
		static constexpr uint32_t Spi3      = Apb1;
		static constexpr uint32_t Uart4     = Apb1;
		static constexpr uint32_t Uart5     = Apb1;
		static constexpr uint32_t Usart2    = Apb1;
		static constexpr uint32_t Usart3    = Apb1;
		static constexpr uint32_t Usb       = Apb1;
		static constexpr uint32_t Apb1Timer = Apb1 * 1;
		static constexpr uint32_t Timer2    = Apb1Timer;
		static constexpr uint32_t Timer3    = Apb1Timer;
		static constexpr uint32_t Timer4    = Apb1Timer;
		static constexpr uint32_t Timer5    = Apb1Timer;
		static constexpr uint32_t Timer6    = Apb1Timer;
		static constexpr uint32_t Timer7    = Apb1Timer;

		static constexpr uint32_t Sai1      = Apb2;
		static constexpr uint32_t Spi1      = Apb2;
		static constexpr uint32_t Usart1    = Apb2;
		static constexpr uint32_t Apb2Timer = Apb2 * 1;
		static constexpr uint32_t Timer1    = Apb2Timer;
		static constexpr uint32_t Timer8    = Apb2Timer;
		static constexpr uint32_t Timer15   = Apb2Timer;
		static constexpr uint32_t Timer16   = Apb2Timer;
		static constexpr uint32_t Timer17   = Apb2Timer;
		static constexpr uint32_t Timer20   = Apb2Timer;
		static constexpr uint32_t Iwdg = Rcc::LsiFrequency;

		static bool inline enable()
		{
			Rcc::enableInternalClock();	// 16MHz

			Rcc::PllFactors pllFactors
			{
				.pllM = 4,	//  16MHz / M= 4 ->   4MHz
				.pllN = 85,	//   4MHz * N=85 -> 340MHz
				.pllR = 2,	// 340MHz / R= 2 -> 170MHz = F_cpu
			};

			Rcc::enablePll(Rcc::PllSource::InternalClock, pllFactors);
			Rcc::setFlashLatency<Frequency>();
			Rcc::setVoltageScaling(Rcc::VoltageScaling::Boost); // recommended for >150 MHz
			// switch system clock to PLL output
			Rcc::enableSystemClock(Rcc::SystemClockSource::Pll);
			Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);
			// APB1 has max. 170MHz
			Rcc::setApb1Prescaler(Rcc::Apb1Prescaler::Div1);
			Rcc::setApb2Prescaler(Rcc::Apb2Prescaler::Div1);
			// update frequencies for busy-wait delay functions
			Rcc::updateCoreFrequency<Frequency>();

			Rcc::setCanClockSource(Rcc::CanClockSource::Pclk);
			return true;
		}
	};

	using Led_D2 = GpioA11; //update the pin here

//	struct I2c
//	{
//		using Sda = GpioB5;
//		using Scl = GpioA8;
//		using Master = I2cMaster1;
//
//		static void initialize()
//		{
//			Master::connect<Sda::Sda, Scl::Scl>();
//			Master::initialize<SystemClock, 400_kBd>();
//		}
//	};


	namespace DebugUart 
	{
		using DebugUartTx	= GpioA9;
		//using DebugUartRx	= GpioA10;

		using DebugUart 	= BufferedUart<UsartHal1>;
		static constexpr uint32_t DebugUartBaudrate = 115200_Bd;

		static void inline initialize()
		{
			DebugUart::connect<DebugUartTx::Tx>();
			DebugUart::initialize<Board::SystemClock, 115200_Bd>();
		}
	}

	inline void initialize()
	{
		SystemClock::enable();
		SysTickTimer::initialize<SystemClock>();
		Led_D2::setOutput();
	}

}

#endif	// HARDWARE_HPP
