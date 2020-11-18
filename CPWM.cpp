/*
 * Author: Erdet Nasufi <erdet.nasufi@gmail.com>
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to <http://unlicense.org>
 */

#include "CPWM.h"

using namespace pigpio_wcpp;

PWM::PWM(unsigned int pin)
{
    m_pin = pin;
}

bool PWM::drive(unsigned int pulseWidth)
{
    m_isAttached = gpioPWM(m_pin, pulseWidth) == 0;
    return m_isAttached;
}

bool PWM::off()
{
    if(!m_isAttached) return false;
    return gpioServo(m_pin, 0) == 0;
}

bool PWM::setFrequency(unsigned int frequencyHz)
{
    return (gpioSetPWMfrequency(m_pin, frequencyHz) > 4);
}

unsigned int PWM::frequency()
{
    return gpioGetPWMfrequency(m_pin);
}

bool PWM::setRange(PWMRange range)
{
    return gpioSetPWMrange(m_pin, (unsigned int)range) > 24;
}
unsigned int PWM::range()
{
    return gpioGetPWMrange(m_pin);
}

unsigned int PWM::realRange()
{
    return gpioGetPWMrealRange(m_pin);
}

unsigned int PWM::pulseWidth()
{
    return gpioGetServoPulsewidth(m_pin);
}

bool PWM::isAttached() const
{
    return m_isAttached;
}
