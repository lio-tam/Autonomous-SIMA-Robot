/*
 * Copyright (c) 2024, Raphael Lehmann
 *
 * This file is part of the RCA project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/platform.hpp> //MODM platform library for GPIO and peripheral abstractions.
#include <modm/processing.hpp> //Includes lightweight threading support 
#include <modm/driver/position/vl53l0.hpp>
#include <modm/debug/logger.hpp> //to output debugging information over UART.

#include <type_traits>

#include "hardware.hpp" //hardware-specific configurations (e.g., clock, GPIO pins like GpioA0 for LEDs).

using namespace std::chrono_literals;

//A lightweight thread running independently of the main thread.
modm_faststack modm::Fiber<> fiber_blink([](){
    while (1) {
        Board::Led_D2::toggle();
        MODM_LOG_INFO << "Blink" << modm::endl; //Logs a “Blink” message via UART.
        modm::this_fiber::sleep_for(1s);
    }
});

int main()
{
    Board::initialize(); // Initialize system clock and GPIO pins.
//    Board::I2c::initialize(); // Initialize I2C peripheral.
    Board::DebugUart::initialize(); // Initialize UART for debugging.

    modm::fiber::Scheduler::run(); // Run the fiber scheduler (manages fibers).

    while(1) ; // Infinite loop (not reached since the fiber scheduler runs continuously).
    return 0;
}
