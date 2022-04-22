/***************************************************************************
  * 
  * Homework for chapter 3 -- "overlap-save method"
  *
  * Here is the realization of add rir function.
  * You have to finish "conv" function by yourself. It is called in main 
  * function. Also, you may want to use FFT, which is ready for you too.
  *
  * NOTICE: There is no IFFT function. So you have to realize IFFT using FFT.
  * Take some time to think about how to do this please.
  * 
  **************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "conv.h"
#include "baselib.h"

/**
 * @brief add rir using overlap-save method.
 *
 * @param inputdata         input clean audio data buffer
 * @param inputdata_length  length of inputdata(samples)
 * @param rir               room impulse response buffer
 * @param rir_length        length of rir, 4096 by default
 * @param outputdata        processed data, same length as inputdata
 * @return 
 *     @retval 0            successfully
 */
int conv(short* inputdata, long inputdata_length, double* rir, long rir_length, short* outputdata)
{
    // finish overlap-save method here
    long N = rir_length;
    int fft_order = (int) log2(2*N);
    int iteration_number = inputdata_length / N;

    /* manipulating filter data */
    COMPLEX rir_filter_expand[2*N]; 
    for(int j = 0; j < N; j++) // filter expansion
    {
      rir_filter_expand[j].real = (float)*(rir+j);
      rir_filter_expand[j].image = 0.0;
    }
    for(int j = N; j < 2*N; j++)
    {
      rir_filter_expand[j].real = 0.0;
      rir_filter_expand[j].image = 0.0;
    }

    fft(rir_filter_expand, fft_order); // FFT of the filter

    /* manipulating speech data */
    for( int i = 1; i <= iteration_number + 1; i++)
      {
        COMPLEX concatenated_block[2*N];
        if(i==1) // the first block, zero-padding the first N elements
          {
            for(int j = 0; j < N ; j++)
            {
              concatenated_block[j].real = 0.0;
              concatenated_block[j].image = 0.0;
            }
          }
        else //if(i <= iteration_number)
          {
            for(int j = N; j < 2*N; j++) 
            {
             concatenated_block[j].real = (float)(*(inputdata + (i-1)*N + j - N));
             concatenated_block[j].image = 0.0;
            }
          }
  /*      else
          {
            
          }*/
        fft(concatenated_block, fft_order); // FFT of the concatenated block of the speech data, length 4096*2=8192



        COMPLEX fft_product[2*N]; 
        COMPLEX ifft_data[2*N];
        for(int j = 0; j < 2*N; j++)
        {
          fft_product[j].real = (float)(rir_filter_expand[j].real * concatenated_block[j].real - rir_filter_expand[j].image * concatenated_block[j].image); // 待改进，实部和虚部
          fft_product[j].image = (float)(rir_filter_expand[j].real * concatenated_block[j].image + rir_filter_expand[j].image * concatenated_block[j].real);
          ifft_data[j].real = fft_product[j].real; //取共轭
          ifft_data[j].image = -fft_product[j].image;
        }

        fft(ifft_data, fft_order); // IFFT
        short ifft_final[2*N] = {0};
        for(int j = 0; j < 2*N; j++)
        {
          ifft_final[j] = ifft_data[j].real / N;
        }
        
        /* output */
        for(int j = 0; j < N; j++)
        {
          *(outputdata + (i-1)*N + j) = ifft_final[j+N]; // taking the last N elements of IFFT
          //outputdata[(i-1)*N + j] = ifft_final[j+N];
        } 
      }
      for(int j = iteration_number*N; j < inputdata_length; j++)
      {       
        *(outputdata + j) = 0;
        //outputdata[j] = 0;
      }
    return 0;
}
/*      for(int j = 2; j < inputdata_length; j++)
      {       
        *(outputdata + j) = 0;
        //outputdata[j] = 0;
      }
*/

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */

