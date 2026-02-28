#include <math.h>
#include <stdio.h>
#include "dsk6713.h"
#include "dsk6713_aic23.h"
#include "dsk6713_led.h"

/* ------------------- System Parameters ------------------- */
#define FFT_SIZE     256        // Number of samples
#define SAMPLE_RATE  8000       // Sampling frequency (Hz)
#define PI           3.141592653589793

/* ------------------- Global Variables ------------------- */
Uint32 input_sample;            // Raw ADC sample
float real[FFT_SIZE];           // Real part of FFT input/output
float imag[FFT_SIZE];           // Imaginary part
float mag[FFT_SIZE / 2];        // Magnitude spectrum

float lowEnergy, midEnergy, highEnergy;

DSK6713_AIC23_CodecHandle hCodec;

/* ------------------- Codec Configuration ------------------- */
DSK6713_AIC23_Config config = {
    0x0017,  // Left line input
    0x0017,  // Right line input
    0x01F9,  // Left headphone
    0x01F9,  // Right headphone
    0x0011,  // Analog path
    0x0000,  // Digital path
    0x0000,  // Power down
    0x0043,  // Digital format
    0x0081,  // 8 kHz sample rate
    0x0001   // Activate codec
};

/* ------------------- Bit Reversal Function ------------------- */
/* Rearranges FFT input samples in bit-reversed order */
void bit_reverse(float *r, float *i)
{
    int j = 0;
    int k, bit;
    float temp;

    for (k = 1; k < FFT_SIZE; k++)
    {
        bit = FFT_SIZE >> 1;
        while (j & bit)
        {
            j ^= bit;
            bit >>= 1;
        }
        j |= bit;

        if (k < j)
        {
            temp = r[k]; r[k] = r[j]; r[j] = temp;
            temp = i[k]; i[k] = i[j]; i[j] = temp;
        }
    }
}

/* ------------------- FFT Computation ------------------- */
/* Radix-2 Decimation-In-Time FFT */
void fft_compute(float *r, float *i)
{
    int len, p, k;
    int even, odd;
    float ang, wr, wi, tr, ti;

    for (len = 2; len <= FFT_SIZE; len <<= 1)
    {
        ang = -2.0 * PI / len;

        for (p = 0; p < FFT_SIZE; p += len)
        {
            for (k = 0; k < len / 2; k++)
            {
                even = p + k;
                odd  = even + len / 2;

                wr = cos(ang * k);
                wi = sin(ang * k);

                tr = wr * r[odd] - wi * i[odd];
                ti = wr * i[odd] + wi * r[odd];

                r[odd]  = r[even] - tr;
                i[odd]  = i[even] - ti;
                r[even] = r[even] + tr;
                i[even] = i[even] + ti;
            }
        }
    }
}

/* ------------------- Main Function ------------------- */
int main(void)
{
    int i;
    float freq;

    DSK6713_init();
    DSK6713_LED_init();
    hCodec = DSK6713_AIC23_openCodec(0, &config);

    printf("FFT Band Energy Project Running...\n");

    while (1)
    {
        /* Step 1: Collect 256 audio samples */
        for (i = 0; i < FFT_SIZE; i++)
        {
            DSK6713_AIC23_read(hCodec, &input_sample);
            real[i] = (float)((Int16)input_sample);
            imag[i] = 0.0;
        }

        /* Step 2: Perform FFT */
        bit_reverse(real, imag);
        fft_compute(real, imag);

        /* Step 3: Compute magnitude spectrum */
        for (i = 0; i < FFT_SIZE / 2; i++)
        {
            mag[i] = sqrt(real[i]*real[i] + imag[i]*imag[i]);
        }

        /* Step 4: Compute band energies */
        lowEnergy = midEnergy = highEnergy = 0.0;

        for (i = 1; i < FFT_SIZE / 2; i++)
        {
            freq = (i * SAMPLE_RATE) / (float)FFT_SIZE;

            if (freq < 300.0)
                lowEnergy += mag[i];
            else if (freq < 2000.0)
                midEnergy += mag[i];
            else
                highEnergy += mag[i];
        }

        /* Step 5: LED Indication */
        DSK6713_LED_off(0);
        DSK6713_LED_off(1);
        DSK6713_LED_off(2);

        if (lowEnergy > midEnergy && lowEnergy > highEnergy)
            DSK6713_LED_on(0);
        else if (midEnergy > lowEnergy && midEnergy > highEnergy)
            DSK6713_LED_on(1);
        else
            DSK6713_LED_on(2);

        printf("LOW=%f  MID=%f  HIGH=%f\n",
               lowEnergy, midEnergy, highEnergy);
    }
}
