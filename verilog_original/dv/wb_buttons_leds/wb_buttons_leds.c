/*
 * SPDX-FileCopyrightText: 2020 Efabless Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * SPDX-License-Identifier: Apache-2.0
 */

// This include is relative to $CARAVEL_PATH (see Makefile)
#include <defs.h>
#include <stub.c>
// #include "verilog/dv/caravel/defs.h"
// #include "verilog/dv/caravel/stub.c"

#define reg_wb_suma (*(volatile uint32_t*)0x30000000)
#define reg_wb_buttons (*(volatile uint32_t*)0x30000004)
#define reg_wb_salidas (*(volatile uint32_t*)0x30000008)
#define reg_wb_sumb (*(volatile uint32_t*)0x3000000C)
#define reg_wb_op (*(volatile uint32_t*)0x30000010)
/*
	Wishbone Test:
		- Configures MPRJ lower 8-IO pins as outputs
		- Checks counter value through the wishbone port
*/

void main()
{

	/* 
	IO Control Registers
	| DM     | VTRIP | SLOW  | AN_POL | AN_SEL | AN_EN | MOD_SEL | INP_DIS | HOLDH | OEB_N | MGMT_EN |
	| 3-bits | 1-bit | 1-bit | 1-bit  | 1-bit  | 1-bit | 1-bit   | 1-bit   | 1-bit | 1-bit | 1-bit   |
	Output: 0000_0110_0000_1110  (0x1808) = GPIO_MODE_USER_STD_OUTPUT
	| DM     | VTRIP | SLOW  | AN_POL | AN_SEL | AN_EN | MOD_SEL | INP_DIS | HOLDH | OEB_N | MGMT_EN |
	| 110    | 0     | 0     | 0      | 0      | 0     | 0       | 1       | 0     | 0     | 0       |
	
	 
	Input: 0000_0001_0000_1111 (0x0402) = GPIO_MODE_USER_STD_INPUT_NOPULL
	| DM     | VTRIP | SLOW  | AN_POL | AN_SEL | AN_EN | MOD_SEL | INP_DIS | HOLDH | OEB_N | MGMT_EN |
	| 001    | 0     | 0     | 0      | 0      | 0     | 0       | 0       | 0     | 1     | 0       |
	*/

	/* Set up the housekeeping SPI to be connected internally so	*/
	/* that external pin changes don't affect it.			*/

    reg_spi_enable = 1;
    reg_wb_enable = 1;
	// reg_spimaster_config = 0xa002;	// Enable, prescaler = 2,
    //                                     // connect to housekeeping SPI

	// Connect the housekeeping SPI to the SPI master
	// so that the CSB line is not left floating.  This allows
	// all of the GPIO pins to be used for user functions.

    //buttons as input to testbench
    reg_mprj_io_7 = GPIO_MODE_USER_STD_INPUT_NOPULL;
    reg_mprj_io_8 = GPIO_MODE_USER_STD_INPUT_NOPULL;
    reg_mprj_io_9 = GPIO_MODE_USER_STD_INPUT_NOPULL;

    //leds as output to testbench
    reg_mprj_io_10 = GPIO_MODE_USER_STD_OUTPUT;
    reg_mprj_io_11 = GPIO_MODE_USER_STD_OUTPUT;
    reg_mprj_io_12 = GPIO_MODE_USER_STD_OUTPUT;
    reg_mprj_io_13 = GPIO_MODE_USER_STD_OUTPUT;
    reg_mprj_io_14 = GPIO_MODE_USER_STD_OUTPUT;
    reg_mprj_io_15 = GPIO_MODE_USER_STD_OUTPUT;
    reg_mprj_io_16 = GPIO_MODE_USER_STD_OUTPUT;
    reg_mprj_io_17 = GPIO_MODE_USER_STD_OUTPUT;

     /* Apply configuration */
    reg_mprj_xfer = 1;
    while (reg_mprj_xfer == 1);//wait until the configuration is done

	//reg_la2_oenb = reg_la2_iena = 0x00000000;    // [95:64]

    // Flag start of the test
	//reg_mprj_datal = 0xAB600000;

    // reg_mprj_slave = 0x00002710;
    // reg_mprj_datal = 0xAB610000;
    // if (reg_mprj_slave == 0x2B3D) {
    //     reg_mprj_datal = 0xAB610000;
    // }

    //wait for all 3 buttons get pressed
    while(reg_wb_buttons != 7);

    //the set all the leds signaling the end of the test
    //reg_wb_leds = 0x5;
    //reg_wb_leds = 0x6;

    reg_wb_op = 0x0;
    reg_wb_suma = 0x05134518;
    reg_wb_sumb = 0x00000004;
    while(reg_wb_salidas != 0xFAECBAE7); // op = 0
    reg_wb_op = 0x1;
    while(reg_wb_salidas != 0x00000000); // op = 1
    reg_wb_op = 0x2;
    while(reg_wb_salidas != 0x05134518); // op = 2
    reg_wb_op = 0x3;
    while(reg_wb_salidas != 0x0513451C); // op = 3
    reg_wb_op = 0x4;
    while(reg_wb_salidas != 0x05134517); // op = 4
    reg_wb_op = 0x5;
    while(reg_wb_salidas != 0x0513451C); // op = 5
    reg_wb_op = 0x6;
    while(reg_wb_salidas != 0x05134514); // op = 6
    reg_wb_op = 0x7;
    while(reg_wb_salidas != 0x05134519); // op = 7
    reg_wb_suma = 0xD;
}