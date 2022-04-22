/***************************************************************************
 *
 * Homework for chapter 3 -- "overlap-save method"
 *
 * Here is the declaration of FFT function.
 * The input/output data are stored in fftdata. FFT order should also be
 * assigned. It equals to log2(fft_length)
 *
 **************************************************************************/

#ifndef __BASELIB_H_
#define __BASELIB_H_

#include "typedefs.h"

/** @brief FFT */
void fft(COMPLEX *fftdata, int fftorder);

COMPLEX conj(COMPLEX input)
{
    COMPLEX output;
    output.real = input.real;
    output.image = -input.image;

    return output;
}

#endif //__BASELIB_H_
