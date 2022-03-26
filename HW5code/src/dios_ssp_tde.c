/***************************************************************************
 *
 * Homework for chapter 5 -- Acoustic Echo Cancellation
 *
 * Here is the realization of Time Delay Estimation api.
 *
 **************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "dios_ssp_tde.h"
#include "dios_ssp_share_rfft.h"

/**
 * @brief tde process
 *
 * You are going to finish this function of TDE, dios_ssp_tde().
 * Obviously the time delay is within [0, inputdata_length).
 * Since we don't know the ground truth delay, we usually need
 * a longer buffer for TDE. In this lab we use about 1s buffer
 * for TDE. You can apply GCC method to estimate the time delay
 * between input signal and reference signal.
 *
 * @param inputdata          input data buffer.
 * @param refdata            reference data buffer.
 * @param inputdata_length   input data length (samples), also the reference data length.
 * @return                   estimated time delay (samples) between input signal and reference signal.
 */
int dios_ssp_tde(short *inputdata, short *refdata, long int inputdata_length)
{
  int delay = 0;

  // please finish your code here...
  int fftlen = inputdata_length * 0.5 + 1;
  void *rfft_handle;
  rfft_handle = dios_ssp_share_rfft_init(inputdata_length);

  /* transform the (short) data to (float) */
  float *inbuffer_y = NULL;
  float *inbuffer_x = NULL;
  float *outbuffer_y = NULL;
  float *outbuffer_x = NULL;
  inbuffer_y = (float *)calloc(inputdata_length, sizeof(float));  // y(n)
  inbuffer_x = (float *)calloc(inputdata_length, sizeof(float));  // x(n)
  outbuffer_y = (float *)calloc(inputdata_length, sizeof(float)); // Y(k)
  outbuffer_x = (float *)calloc(inputdata_length, sizeof(float)); // X(k)
  // printf("valuehaha = %f\n", *(inbuffer_x + 288));

  for (int i = 0; i < inputdata_length; i++)
  {
    inbuffer_y[i] = (float)inputdata[i];
    inbuffer_x[i] = (float)refdata[i];
  }
  // printf("value_y = %f\n", *(inbuffer_y + 288));
  // printf("value_data = %f\n", *(inputdata + 288));
  // printf("value_input = %d\n", *(inputdata + 55));
  // printf("value_ref = %d\n", *(refdata + 55));
  // printf("value_y = %f\n", *(inbuffer_y + 55));
  // printf("value_x = %f\n", *(inbuffer_x + 55));
  // printf("value_Y = %f\n", *(outbuffer_y + 55));
  // printf("value_X = %f\n", *(outbuffer_x + 55));

  /* calculating the FFT of input and reference signal, to get the cross PSD */
  dios_ssp_share_rfft_process(rfft_handle, inbuffer_y, outbuffer_y);
  dios_ssp_share_rfft_process(rfft_handle, inbuffer_x, outbuffer_x);

  /* calculating the cross PSD and store it in to an approperiate pointer */
  float *inbuffer_yx = NULL;  // Pyx(k)
  float *outbuffer_yx = NULL; // ryx(m)
  inbuffer_yx = (float *)calloc(inputdata_length, sizeof(float));
  outbuffer_yx = (float *)calloc(inputdata_length, sizeof(float));
  // conjugation
  for (int i = fftlen; i < inputdata_length; i++)
  {
    outbuffer_x[i] = outbuffer_x[i] * (-1);
  }
  // printf("value_input = %d\n", *(inputdata + 55));
  // printf("value_ref = %d\n", *(refdata + 55));
  // printf("value_y = %f\n", *(inbuffer_y + 55));
  // printf("value_x = %f\n", *(inbuffer_x + 55));
  // printf("value_Y = %f\n", *(outbuffer_y + 55));
  // printf("value_X = %f\n", *(outbuffer_x + 55));
  // cross PSD
  for (int i = 0; i < inputdata_length; i++)
  {
    if (i == 0)
    {
      inbuffer_yx[0] = outbuffer_x[0] * outbuffer_y[0]; // Pyx(k) = X*(k)Y(k)
    }
    else if (i > 0 && i < fftlen - 1)
    {
      inbuffer_yx[i] = outbuffer_x[i] * outbuffer_y[i] - outbuffer_x[inputdata_length - i] * outbuffer_y[inputdata_length - i];
    }
    else if (i == fftlen - 1)
    {
      inbuffer_yx[i] = outbuffer_x[i] * outbuffer_y[i];
    }
    else
    {
      inbuffer_yx[i] = outbuffer_x[inputdata_length - i] * outbuffer_y[i] + outbuffer_x[i] * outbuffer_y[inputdata_length - i];
    }
  }
  // printf("valueRyx = %f\n", *(inbuffer_yx + 55));
  // normalizaion, Pyx(k) = X*(k)Y(k)
  for (int i = 0; i < inputdata_length; i++)
  {
    if (i == 0)
    {
      inbuffer_yx[0] = 1;
    }
    else if (i > 0 && i < fftlen - 1)
    {
      inbuffer_yx[i] = inbuffer_yx[i] / sqrt((*(outbuffer_x + i) * *(outbuffer_y + i) - *(outbuffer_x + inputdata_length - i) * *(outbuffer_y + inputdata_length - i)) *
                                                 (*(outbuffer_x + i) * *(outbuffer_y + i) - *(outbuffer_x + inputdata_length - i) * *(outbuffer_y + inputdata_length - i)) +
                                             (*(outbuffer_x + i) * *(outbuffer_y + inputdata_length - i) + *(outbuffer_y + i) * *(outbuffer_x + inputdata_length - i)) *
                                                 (*(outbuffer_x + i) * *(outbuffer_y + inputdata_length - i) + *(outbuffer_y + i) * *(outbuffer_x + inputdata_length - i)));
    }
    else if (i == fftlen - 1)
    {
      inbuffer_yx[i] = 1;
    }
    else
    {
      inbuffer_yx[i] = inbuffer_yx[i] / sqrt((*(outbuffer_x + i) * *(outbuffer_y + i) - *(outbuffer_x + inputdata_length - i) * *(outbuffer_y + inputdata_length - i)) *
                                                 (*(outbuffer_x + i) * *(outbuffer_y + i) - *(outbuffer_x + inputdata_length - i) * *(outbuffer_y + inputdata_length - i)) +
                                             (*(outbuffer_x + i) * *(outbuffer_y + inputdata_length - i) + *(outbuffer_y + i) * *(outbuffer_x + inputdata_length - i)) *
                                                 (*(outbuffer_x + i) * *(outbuffer_y + inputdata_length - i) + *(outbuffer_y + i) * *(outbuffer_x + inputdata_length - i)));
    }
  }

  // calculating r_yx
  // float *outbuffer_Ryx = NULL;
  // outbuffer_Ryx = (float *)calloc(inputdata_length, sizeof(float));
  dios_ssp_share_irfft_process(rfft_handle, inbuffer_yx, outbuffer_yx);
  float max_value = (float)0;
  // finding the position of the maximum value of r_yx
  for (int i = 0; i < inputdata_length; i++)
  {
    if (*(outbuffer_yx + i) > max_value)
    {
      max_value = *(outbuffer_yx + i);
      delay = i;
    }
    else
    {
      i = i;
    }
  }
  // printf("value_ryx = %f\n", *(outbuffer_yx + 550));
  // dios_ssp_share_rfft_uninit(rfft_handle);
  //   end TDE
  //   delay = 3000;
  return delay;
}
