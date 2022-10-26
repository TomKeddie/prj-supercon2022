/*
 * Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2011 Stephen Caudle <scaudle@doceme.com>
 * Copyright (C) 2012 Karl Palsson <karlp@tweak.net.au>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

typedef struct {
    uint32_t port;
    uint32_t pin;
} portpin_t;

#define SEG_A 0
#define SEG_B 1
#define SEG_C 2
#define SEG_D 3
#define SEG_E 4
#define SEG_F 5
#define SEG_G 6

#define PORTA_ALL_OFF 0
#define PORTB_ALL_OFF 0

static const portpin_t segments[SEG_G+1] = {
    {GPIOA, GPIO0}, // A
    {GPIOA, GPIO2}, // B
    {GPIOA, GPIO5}, // C
    {GPIOA, GPIO4}, // D
    {GPIOA, GPIO3}, // E
    {GPIOA, GPIO1}, // F
    {GPIOA, GPIO6}, // G
};
static const portpin_t digits[4] = {
    {GPIOA, GPIO10}, // 1
    {GPIOA, GPIO9},  // 2
    {GPIOA, GPIO7},  // 3
    {GPIOB, GPIO1},  // 4
};
static const portpin_t inputs[4] = {
    {GPIOF, GPIO0},  // 0
    {GPIOF, GPIO1},  // 1
    {GPIOA, GPIO13}, // 2
    {GPIOA, GPIO14}, // 3
};

#define CHARACTER_BLANK 16
#define CHARACTER_H     17
#define CHARACTER_K     18
#define CHARACTER_Y     19
#define CHARACTER_S     20
#define CHARACTER_U     21
#define CHARACTER_P     22
#define CHARACTER_R     23
#define CHARACTER_N     24

static const portpin_t characters[][sizeof(segments)/sizeof(segments[0])+1] = {
    // 0
    { segments[SEG_A], segments[SEG_B], segments[SEG_C], segments[SEG_D], segments[SEG_E], segments[SEG_F], {0,0}},
    // 1
    { segments[SEG_B], segments[SEG_C], {0,0}},
    // 2
    { segments[SEG_A], segments[SEG_B], segments[SEG_D], segments[SEG_E], segments[SEG_G], {0,0}},
    // 3
    { segments[SEG_A], segments[SEG_B], segments[SEG_C], segments[SEG_D], segments[SEG_G], {0,0}},
    // 4
    { segments[SEG_B], segments[SEG_C], segments[SEG_F], segments[SEG_G], {0,0}},
    // 5
    { segments[SEG_A], segments[SEG_C], segments[SEG_D], segments[SEG_F], segments[SEG_G], {0,0}},
    // 6
    { segments[SEG_A], segments[SEG_C], segments[SEG_D], segments[SEG_E], segments[SEG_F], segments[SEG_G], {0,0}},
    // 7
    { segments[SEG_A], segments[SEG_B], segments[SEG_C], {0,0}},
    // 8
    { segments[SEG_A], segments[SEG_B], segments[SEG_C], segments[SEG_D], segments[SEG_E], segments[SEG_F], segments[SEG_G], {0,0}},
    // 9
    { segments[SEG_A], segments[SEG_B], segments[SEG_C], segments[SEG_D], segments[SEG_F], segments[SEG_G], {0,0}},
    // A
    { segments[SEG_A], segments[SEG_B], segments[SEG_C], segments[SEG_E], segments[SEG_F], segments[SEG_G], {0,0}},
    // B
    { segments[SEG_C], segments[SEG_D], segments[SEG_E], segments[SEG_F], segments[SEG_G], {0,0}},
    // C
    { segments[SEG_A], segments[SEG_D], segments[SEG_E], segments[SEG_F], {0,0}},
    // D
    { segments[SEG_B], segments[SEG_C], segments[SEG_D], segments[SEG_E], segments[SEG_G], {0,0}},
    // E
    { segments[SEG_A], segments[SEG_D], segments[SEG_E], segments[SEG_F], segments[SEG_G], {0,0}},
    // F
    { segments[SEG_A], segments[SEG_E], segments[SEG_F], segments[SEG_G], {0,0}},
    // blank
    { {0,0}},
    // H
    { segments[SEG_C], segments[SEG_E], segments[SEG_F], segments[SEG_G], {0,0}},
    // K
    { segments[SEG_B], segments[SEG_C], segments[SEG_E], segments[SEG_F], segments[SEG_G], {0,0}},
    // Y
    { segments[SEG_B], segments[SEG_C], segments[SEG_D], segments[SEG_F], segments[SEG_G], {0,0}},
    // S
    { segments[SEG_A], segments[SEG_C], segments[SEG_D], segments[SEG_F], segments[SEG_G], {0,0}},
    // U
    { segments[SEG_B], segments[SEG_C], segments[SEG_D], segments[SEG_E], segments[SEG_F], {0,0}},
    // P
    { segments[SEG_A], segments[SEG_B], segments[SEG_E], segments[SEG_F], segments[SEG_G], {0,0}},
    // r
    { segments[SEG_E], segments[SEG_G], {0,0}},
    // n
    { segments[SEG_C], segments[SEG_E], segments[SEG_G], {0,0}},

};

static uint8_t digits_active[] = { CHARACTER_BLANK, CHARACTER_BLANK, CHARACTER_BLANK, CHARACTER_BLANK };

static const uint8_t hackaday_string[] = {
    CHARACTER_BLANK, CHARACTER_BLANK, CHARACTER_BLANK,
    CHARACTER_H, 0xA, 0xC, CHARACTER_K, 0xA, 0xD, 0xA, CHARACTER_Y,
    CHARACTER_BLANK,
    CHARACTER_S, CHARACTER_U, CHARACTER_P, 0xE, CHARACTER_R, 0xc, 0, CHARACTER_N,
    CHARACTER_BLANK,
    2, 0, 2, 2,
    CHARACTER_BLANK, CHARACTER_BLANK, CHARACTER_BLANK, CHARACTER_BLANK, CHARACTER_BLANK,
};

static uint8_t input_prev = 255;

static uint8_t read_inputs(void) {
    return ((gpio_get(inputs[3].port, (inputs[3].pin)) != 0 ? 1 : 0) << 3 |
            (gpio_get(inputs[2].port, (inputs[2].pin)) != 0 ? 1 : 0) << 2 |
            (gpio_get(inputs[1].port, (inputs[1].pin)) != 0 ? 1 : 0) << 1 |
            (gpio_get(inputs[0].port, (inputs[0].pin)) != 0 ? 1 : 0) << 0);
}

static void gpio_setup(void)
{
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_GPIOF);

    for (uint32_t ix=0; ix < sizeof(segments)/sizeof(segments[0]); ++ix) {
        gpio_mode_setup(segments[ix].port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, segments[ix].pin);
    }
    for (uint32_t ix=0; ix < sizeof(digits)/sizeof(digits[0]); ++ix) {
        gpio_mode_setup(digits[ix].port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, digits[ix].pin);
    }
    for (uint32_t ix=0; ix < sizeof(digits)/sizeof(digits[0]); ++ix) {
        gpio_mode_setup(inputs[ix].port, GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN, inputs[ix].pin);
    }

    input_prev = read_inputs();
}

int main(void)
{
    gpio_setup();
    unsigned counter = 0;
    uint8_t initial_display = 1;
    uint8_t initial_index = 0;

    while (1) {
        for (unsigned digit=0; digit < sizeof(digits)/sizeof(digits[0]); ++digit) {
            // blank segments and columns
            gpio_port_write(GPIOA, PORTA_ALL_OFF);
            gpio_port_write(GPIOB, PORTB_ALL_OFF);
            // write the segment value
            for (unsigned segment=0; segment < sizeof(segments)/sizeof(segments[0]); ++segment) {
                if (characters[digits_active[digit]][segment].port == 0) {
                    break;
                } else {
                    gpio_set(characters[digits_active[digit]][segment].port, characters[digits_active[digit]][segment].pin);
                }
            }
            // enable the digit
            gpio_set(digits[digit].port, digits[digit].pin);
            for (unsigned i = 0; i < 2500; i++) {     /* Wait a bit. */
                __asm__("nop");
            }
        }
        if (initial_display) {
            if (counter++ == 25) {
                // scroll through the text until all displayed
                counter = 0;
                memcpy(digits_active, hackaday_string+initial_index, sizeof(digits_active));

                initial_index++;
                if (initial_index+sizeof(digits_active) >= sizeof(hackaday_string)) {
                    initial_display = 0;
                }
            }
        } else {
            // read the input and look for change
            uint8_t input_current = read_inputs();

            if (input_prev != input_current) {
                // shift data
                digits_active[0] = digits_active[1];
                digits_active[1] = digits_active[2];
                digits_active[2] = digits_active[3];
                // load new data
                digits_active[3] = input_prev = input_current;
            }
        }
    }

    return 0;
}
