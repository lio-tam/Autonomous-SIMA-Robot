#ifndef HARDWARE_HPP
#define HARDWARE_HPP

#include <modm/platform.hpp>
#include <modm/architecture/interface/clock.hpp>
#include <modm/platform/i2c/i2c_master_1.hpp>
#include <modm/platform/uart/uart_hal_1.hpp>

#include <modm/platform/timer/timer_3.hpp>
#include <modm/platform/timer/timer_2.hpp>


using namespace modm::platform;

namespace Board
{
    using namespace modm::literals;

    /// STM32G474 running at 170MHz from 16MHz internal clock
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


        static bool inline enable() {
            Rcc::enableInternalClock();    // 16MHz

            Rcc::PllFactors pllFactors
            {
                .pllM = 4,    //  16MHz / M= 4 ->   4MHz
                .pllN = 85,    //   4MHz * N=85 -> 340MHz
                .pllR = 2,    // 340MHz / R= 2 -> 170MHz = F_cpu
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
    

    // ------------------- Led Pin -------------------
    using Led_D2 = GpioA11; //heartbeat led

    // ------------------- Motor 1 Pins (left) -------------------
    using M1_Sleep = GpioA5;    // Sleep pin
    using M1_Fault = GpioA6;    // Fault pin, input from driver
    using M1_Tacho = GpioA7;    // Tachometer/Encoder feedback from driver?
    using M1_Pwm   = GpioB0;    // PWM signal output = TIM3_CH3
    using M1_Dir   = GpioB1;    // Direction pin
    using M1_Brake = GpioF0;    // Brake pin
    // ------------------- Motor 2 Pins (right) -------------------
    using M2_Sleep = GpioC15;
    using M2_Fault = GpioF1;
    using M2_Tacho = GpioB2;
    using M2_Pwm   = GpioA0;    // TIM2_CH1
    using M2_Dir   = GpioA1;
    using M2_Brake = GpioA4;

    // Optional waveforms
    using Wave1 = GpioA3;
    using Wave2 = GpioA2;

    // ------------------- Timer for PWM -------------------
    // Timer for Motor1
    using MotorTimer3 = modm::platform::Timer3; // PB0 can do Timer3 CH3
    // Timer for Motor1
    using MotorTimer2 = modm::platform::Timer2; // PA0 can do Timer2 CH1


    // ------------------- Debug UART -------------------
    namespace DebugUart {
        using DebugUartTx = GpioA9; // TX pin
        using DebugUart   = BufferedUart<UsartHal1>;

        static constexpr uint32_t DebugUartBaudrate = 9600_Bd;  

        static void inline initialize()
        {
            DebugUart::connect<DebugUartTx::Tx>();
            DebugUart::initialize<Board::SystemClock, 115200_Bd>();
        }
    }

//    // ------------------- I2C (to be implemented) -------------------
//    struct I2c
//    {
//        using Sda = GpioB5;
//        using Scl = GpioA8;
//        using Master = I2cMaster1;
//
//        static void initialize()
//        {
//            Master::connect<Sda::Sda, Scl::Scl>();
//            Master::initialize<SystemClock, 400_kBd>();
//        }
//    };

    // ------------------- Board Initialization -------------------
    inline void
    initialize()
    {
        // 1) Clock & SysTick
        SystemClock::enable();
        SysTickTimer::initialize<SystemClock>();

        // 2) --- Setup Motor1 Pins ---
        M1_Sleep::setOutput(true);  // Enable motor driver if active high.
        M1_Fault::setInput();
        M1_Tacho::setInput();
        M1_Dir::setOutput(false);    // false = forward?
        M1_Brake::setOutput(false);  // false = brake released -> led stays off
        // PWM - Timer3 (Motor1) on PB0 => CH3
        M1_Pwm::setOutput();
        MotorTimer3::connect<GpioB0::Ch3>();
        MotorTimer3::enable();
        MotorTimer3::setMode(MotorTimer3::Mode::UpCounter);
        MotorTimer3::setPrescaler(17 - 1);  // => 170 MHz / 17 = 10 MHz
        MotorTimer3::setOverflow(1000);     // => 10 MHz / 1000 = 10 kHz
        // Configure PWM @ 50% initially
        MotorTimer3::configureOutputChannel<GpioB0::Ch3>(
            MotorTimer3::OutputCompareMode::Pwm,
            500 // 50% of 1000
        );
        MotorTimer3::applyAndReset();
        MotorTimer3::start();

        // 3) --- Setup Motor2 Pins ---
        M2_Sleep::setOutput(true);
        M2_Fault::setInput();
        M2_Tacho::setInput();
        M2_Dir::setOutput(false);
        M2_Brake::setOutput(false);
        // Timer2 (Motor2) on PA0 => CH1 --
        M2_Pwm::setOutput();
        MotorTimer2::connect<GpioA0::Ch1>();
        MotorTimer2::enable();
        MotorTimer2::setMode(MotorTimer2::Mode::UpCounter);
        MotorTimer2::setPrescaler(17 - 1);
        MotorTimer2::setOverflow(1000);
        // Configure PWM @ 50% initially
        MotorTimer2::configureOutputChannel<GpioA0::Ch1>(
            MotorTimer2::OutputCompareMode::Pwm,
            500 // 50% of 1000
        );
        MotorTimer2::applyAndReset();
        MotorTimer2::start();

        // 4) --- Led pins ---
        Led_D2::setOutput();

        // 5) Optional wave pins
        Wave1::setOutput();
        Wave2::setOutput();

        // 6) Debug Uart
        DebugUart::initialize();
    }

} // namespace Board

#endif // HARDWARE_HPP
