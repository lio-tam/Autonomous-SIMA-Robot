/*
 * Copyright (c) 2016, Sascha Schade
 * Copyright (c) 2016-2017, Fabian Greif
 * Copyright (c) 2016-2017, 2019, 2024, Niklas Hauser
 * Copyright (c) 2021, Raphael Lehmann
 * Copyright (c) 2021, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "../device.hpp"

/**
 * This code should _only_ enable internal memories and nothing else.
 * Since this is the first code executed after a reset, you do not
 * have access to _any_ data stored in RAM, since it has not yet been
 * initialized.
 * In the worst case you won't even have access to the stack, if the
 * memory containing the stack is not physically enabled yet.
 * In that case, consider using inline assembly to manage stack access
 * manually, until the memory is enabled.
 */
void
__modm_initialize_platform(void)
{
	// Enable SYSCFG
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; __DSB();
	// Enable power to backup domain
	RCC->APB1ENR1 |= RCC_APB1ENR1_PWREN; __DSB();
	PWR->CR1 |= PWR_CR1_DBP;
#ifdef PWR_CR2_IOSV
	// Enable VDDIO2
	PWR->CR2 |= PWR_CR2_IOSV;
#endif
}