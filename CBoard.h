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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pigpio.h>

#ifdef __GNUC__
    #define PACKED __attribute__((packed));
#else
    #define PACKED
#endif

#ifndef CBOARD_NOND_H
#define CBOARD_NOND_H

#define IOCallback             gpioAlertFunc_t
#define IOCallbackExtended     gpioAlertFuncEx_t

#define ISRCallback			   gpioISRFuncEx_t

#define SPI_LEAST_SIGNIFICANT_BIT  1
#define SPI_MOST_SIGNIFICANT_BIT   0

#define LOW_ACTIVATION         0
#define HIGH_ACTIVATION        1

namespace pigpio_wcpp
{
    enum PWMRange
    {
        Range25    = 25,
        Range50    = 50,
        Range100   = 100,
        Range125   = 125,
        Range200   = 200,
        Range250   = 250,
        Range400   = 400,
        Range500   = 500,
        Range625   = 625,
        Range800   = 800,
        Range1000  = 1000,
        Range1250  = 1250,
        Range2000  = 2000,
        Range2500  = 2500,
        Range4000  = 4000,
        Range5000  = 5000,
        Range10000 = 10000,
        Range20000 = 20000
    } PACKED;

    enum LogicLevel
    {
        High = 1,
        Low  = 0
    } PACKED;

    enum PinState
    {
        HIGH = 1,
        LOW  = 0
    } PACKED;

    enum Pad
    {
        Pad0,
        Pad1,
        Pad2
    } PACKED;

    enum DMAMemoryAllocationMode
    {
        Auto    = PI_MEM_ALLOC_AUTO,
        Pagemap = PI_MEM_ALLOC_PAGEMAP,
        Mailbox = PI_MEM_ALLOC_MAILBOX
    } PACKED;

    enum DMAChannel
    {
        DMAChannel1  = 0,
        DMAChannel2  = 1,
        DMAChannel3  = 2,
        DMAChannel4  = 3,
        DMAChannel5  = 4,
        DMAChannel6  = 5,
        DMAChannel7  = 6,
        DMAChannel8  = 7,
        DMAChannel9  = 8,
        DMAChannel10 = 9,
        DMAChannel11 = 10,
        DMAChannel12 = 11,
        DMAChannel13 = 12,
        DMAChannel14 = 13,
        DMAChannel15 = 14
    } PACKED;

    enum GPIOPort
    {
        Port1,
        Port2
    } PACKED;

    class Board
    {
    private:
        static bool m_isInitialised;

    public:
        Board();
        ~Board();

        bool initialise();
        void kill();

        bool isInitialised() const;

        unsigned int version();
        unsigned int hardwareRevision();

        // Static functions //
        static void delayInSeconds(double seconds);
        static void delayInMicro(double microseconds);
        static int  ticks();

        static int  shell(char *script, char *arguments);

        static int  GetPadDriveStrength(Pad pad);
        static bool SetPadDriveStrength(Pad pad, int driveStrength_mA);

        static bool SetDMAMemoryAllocationMode(DMAMemoryAllocationMode mode);
        static bool SetDMAChannel(DMAChannel channel);
        static bool SetDMAChannel(DMAChannel primaryChannel, DMAChannel secondaryChannel);

        static unsigned int GetGPIOPort(GPIOPort port);
        static bool         SetGPIOPort(GPIOPort port, unsigned int bitmap);
        static bool         ResetGPIOPort(GPIOPort port);

    };
}
#endif

