#include "hardware.hpp"
#include <modm/architecture/interface/delay.hpp>
#include <modm/debug/logger.hpp>

using namespace Board;

/**
 * @brief Drives the motor at a given speed percentage.
 *
 * Sets the PWM duty cycle for the motor channels.
 */
void driveForward(uint16_t speedPercent)
{
    if (speedPercent > 100) {
        speedPercent = 100;
    }

    // (Assumes that the Direction pins are set by the caller.)
    uint16_t duty1 = (MotorTimer3::getOverflow() * speedPercent) / 100;
    uint16_t duty2 = (MotorTimer2::getOverflow() * speedPercent) / 100;

    MotorTimer3::configureOutputChannel<GpioB0::Ch3>(
        MotorTimer3::OutputCompareMode::Pwm, duty1);
    MotorTimer2::configureOutputChannel<GpioA0::Ch1>(
        MotorTimer2::OutputCompareMode::Pwm, duty2);

    MotorTimer3::applyAndReset();
    MotorTimer3::start();
    MotorTimer2::applyAndReset();
    MotorTimer2::start();
}

/**
 * @brief Logs PWM settings.
 *
 * Displays key parameters (here, the overflow values) so you can verify
 * that the PWM is configured as expected.
 */
void logPwmSettings()
{
    uint16_t overflow1 = MotorTimer3::getOverflow();
    uint16_t overflow2 = MotorTimer2::getOverflow();

    MODM_LOG_INFO << "PWM Settings:" << modm::endl;
    MODM_LOG_INFO << "  MotorTimer3 overflow = " << overflow1 << modm::endl;
    MODM_LOG_INFO << "  MotorTimer2 overflow = " << overflow2 << modm::endl;
    modm::delay_ms(100);
}

/**
 * @brief Drives the motor with a full (or zero) duty cycle.
 *
 * Useful for verifying that the PWM output itself is working.
 *
 * @param fullOn If true, sets duty cycle to 100%; if false, sets to 0%.
 */
void driveAtFullDuty(bool fullOn = true)
{
    uint16_t duty = fullOn ? MotorTimer3::getOverflow() : 0;
    MODM_LOG_INFO << "Setting PWM duty to " << (fullOn ? "100%" : "0%")
                  << " (" << duty << ")" << modm::endl;
    modm::delay_ms(100);

    MotorTimer3::configureOutputChannel<GpioB0::Ch3>(
        MotorTimer3::OutputCompareMode::Pwm, duty);
    MotorTimer2::configureOutputChannel<GpioA0::Ch1>(
        MotorTimer2::OutputCompareMode::Pwm, duty);

    MotorTimer3::applyAndReset();
    MotorTimer2::applyAndReset();
}

/**
 * @brief Toggles the PWM pin as a digital output.
 *
 * Temporarily reconfigures the PWM pin (assumed to be GpioB0) as a digital output and toggles it.
 * After testing, restores PWM functionality.
 */
void testPwmPinToggle()
{
    MODM_LOG_INFO << "Starting PWM pin toggle test..." << modm::endl;
    modm::delay_ms(100);

    // Temporarily disable PWM on M1_Pwm and reconfigure it as a standard output.
    M1_Pwm::setOutput();

    // Toggle the pin 10 times.
    for (int i = 0; i < 10; ++i) {
        M1_Pwm::toggle();
        MODM_LOG_INFO << "  Toggling PWM pin (" << (i + 1) << "/10)" << modm::endl;
        modm::delay_ms(500);
    }

    // Restore PWM functionality.
    MotorTimer3::connect<GpioB0::Ch3>();
    MotorTimer3::start();

    MODM_LOG_INFO << "PWM pin toggle test complete." << modm::endl;
    modm::delay_ms(100);
}

/**
 * @brief Tests various enable combinations while allowing manual rotation.
 *
 * This function cycles through four specific combinations:
 *
 * 1. Normal Operation: nSLEEP = HIGH, BRAKE = LOW, DIR = LOW.
 * 2. Reverse Operation: nSLEEP = HIGH, BRAKE = LOW, DIR = HIGH.
 * 3. Brake Active: nSLEEP = HIGH, BRAKE = HIGH, DIR = LOW.
 * 4. Sleep Mode: nSLEEP = LOW, BRAKE = LOW, DIR = LOW.
 *
 * For each combination, the motor is driven at a preset PWM duty cycle (75%)
 * and the program waits 10 seconds to allow you to manually rotate the wheel
 * to see if the motor begins commutating.
 */
void testEnableModes(uint16_t speedPercent)
{
    // const uint16_t speedPercent = 75;
    const int testDuration_ms = 10000;  // 10 seconds per configuration

    // Test 1: Normal Operation
    MODM_LOG_INFO << "01" << modm::endl; // "Test 1: Normal Operation (nSLEEP=HIGH, BRAKE=LOW, DIR=LOW)"
    M1_Sleep::setOutput(true);  // nSLEEP high: driver active.
    M2_Sleep::setOutput(true);
    M1_Brake::setOutput(false); // BRAKE low: normal operation.
    M2_Brake::setOutput(false);
    M1_Dir::setOutput(false);   // DIR low: choose clockwise (for example).
    M2_Dir::setOutput(false);
    driveForward(speedPercent);
    MODM_LOG_INFO << "cw" << modm::endl; // "Rotate wheel manually (clockwise) now."
    modm::delay_ms(testDuration_ms);

    // Test 2: Reverse Operation
    MODM_LOG_INFO << "02" << modm::endl;  // "Test 2: Reverse Operation (nSLEEP=HIGH, BRAKE=LOW, DIR=HIGH)"
    M1_Sleep::setOutput(true);
    M2_Sleep::setOutput(true);
    M1_Brake::setOutput(false);
    M2_Brake::setOutput(false);
    M1_Dir::setOutput(true);   // DIR high: reverse rotation.
    M2_Dir::setOutput(true);
    driveForward(speedPercent);
    MODM_LOG_INFO << "cc" << modm::endl; //  "Rotate wheel manually (counterclockwise) now."
    modm::delay_ms(testDuration_ms);

    // Test 3: Brake Active
    MODM_LOG_INFO << "03" << modm::endl; // "Test 3: Brake Active (nSLEEP=HIGH, BRAKE=HIGH, DIR=LOW)"
    M1_Sleep::setOutput(true);
    M2_Sleep::setOutput(true);
    M1_Brake::setOutput(true); // BRAKE high: motor is braked.
    M2_Brake::setOutput(true);
    M1_Dir::setOutput(false);   // Direction doesn't matter here.
    M2_Dir::setOutput(false);
    driveForward(speedPercent);
    MODM_LOG_INFO << "br" << modm::endl; // "Rotate wheel manually; motor should not turn (brake active)."
    modm::delay_ms(testDuration_ms);

    // Test 4: Sleep Mode
    MODM_LOG_INFO << "04" << modm::endl; // "Test 4: Sleep Mode (nSLEEP=LOW, BRAKE=LOW, DIR=LOW)"
    M1_Sleep::setOutput(false); // nSLEEP low: driver in sleep.
    M2_Sleep::setOutput(false);
    M1_Brake::setOutput(false);
    M2_Brake::setOutput(false);
    M1_Dir::setOutput(false);
    M2_Dir::setOutput(false);
    driveForward(speedPercent);
    MODM_LOG_INFO << "sl" << modm::endl; // "Rotate wheel manually; motor should not run (sleep mode)."
    modm::delay_ms(testDuration_ms);
}

/**
 * @brief Main function.
 *
 * Initializes the board, performs a baseline drive test, then cycles through
 * enable combinations with manual rotation.
 */
int main()
{
    Board::initialize();
    MODM_LOG_INFO << "In" << modm::endl; // "Init."

    // Blink a heartbeat LED during startup.
    for (int i = 0; i < 5; i++) {
        Led_D2::toggle();
        modm::delay_ms(1000);
    }

    MODM_LOG_INFO << "75" << modm::endl; // "Starting baseline drive at 75% duty."
    modm::delay_ms(100);
    driveForward(75);
    modm::delay_ms(2000);

    // Run the enable mode tests.
    testEnableModes(50);

    MODM_LOG_INFO << "00" << modm::endl; // "End of tests."

    // After testing, continue with a heartbeat loop.
    while (true) {
        Led_D2::toggle();
        modm::delay_ms(1000);
    }

    return 0;
}
