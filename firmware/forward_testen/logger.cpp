#include <modm/debug/logger.hpp>
#include "hardware.hpp"

modm::IODeviceWrapper<Board::DebugUart::DebugUart, modm::IOBuffer::DiscardIfFull> loggerDevice;
modm::log::Logger modm::log::debug(loggerDevice);
modm::log::Logger modm::log::info(loggerDevice);
modm::log::Logger modm::log::warning(loggerDevice);
modm::log::Logger modm::log::error(loggerDevice);