
#ifndef QUACK_SPI_H_
#define QUACK_SPI_H_

/****************************************************************************
quack_spi.h:
  This is just a mock to the SPI arduino library. If SPI_ENABLED is not defi
  ned, it debugs each SPI function. If it is, it includes SPI.h.
****************************************************************************/

#include "config.h"

#ifdef SPI_ENABLED

    #include <SPI.h>

#else

    #include <cstdio>

    struct _SPI {
        void begin() { printf("SPI begin\n"); }
        void transfer(char c) { printf("SPI transfer: '%c'\n", c); }
    };

    _SPI SPI;

#endif

#endif