/* 
 * @File:   ds_filter_database.cpp
 * 
 * @Info:   Created by Onur YILDIRIM
 * 
 * @Author: Baris AYCIN
 *          Gurtac KADEM
 *          Suat Berkant GULEN
 *          Onur YILDIRIM
 * 
 * @Mail:   baris.aycin@altinay.com
 *          gurtac.kadem@altinay.com
 *          berkant.gulen@altinay.com
 *          o.yildirim@altinay.com
 *
 * @Description: This library contains filter functons
 *     
 * 
 * @Note:   Version 1.0.0 - Version updated by: Onur YILDIRIM
 * 
 * Created on 22 May 2019 Wed, 10:00
 * 
 * Copyright (C) 2019 ALTINAY Aerospace and Advanced Technologies
 */


/*
 * Begin of Includes
 */
#include "ds_filter_database.hpp"
#include <math.h>

// End of Includes



/*
 * Begin of Definitions
 */
FILTER_DATABASE filter;
// End of Definitions



/*
 * Default constructor.
 */
FILTER_DATABASE::FILTER_DATABASE() 
{
    
}



/*
 * @brief   Applies the low pass filter to the given signal
 * 
 * @param   float   &output
 *          float   input
 *          float   cutoff_frequency_hz
 *          float   dt_s
 * 
 * @return  void    Nothing
 * 
 */
void FILTER_DATABASE::applyLowPass(float &output, float input, float cutoff_frequency_hz, float dt_s) 
{
    const float DT_LIMIT_S = 0.001;
    float rc    = 0;
    float alpha = 0;
    
    if( cutoff_frequency_hz < 0.01 )
    {
        cutoff_frequency_hz = 0.01;
    }
    if(dt_s > DT_LIMIT_S)
    {
        rc    = 1.0/(2.0*C_PI*cutoff_frequency_hz);
        alpha = dt_s / ( dt_s + rc );

        output = output + alpha*(input - output);
    }
    else
    {
        output = input;
    }
}

/*
 * @brief   Applies the low pass filter to the given signal
 * 
 * @param   float   &output
 *          float   input
 *          float   ex_input 
 *          float   gain 
 *          float   cutoff_frequency_hz
 *          float   dt_s
 * 
 * @return  void    Nothing
 *
 */
void FILTER_DATABASE::applyHighPass(float &output, float input, float ex_input, float gain, float cutoff_frequency_hz, float dt_s) 
{
    float cutoff_frequency;
    float rc;
    float alpha;
   
    if(fabs(input) > (1.0/gain))
    {
        cutoff_frequency = 1.0 / (fabs(input) * gain);
    }
    else
    {
        cutoff_frequency = cutoff_frequency_hz;
    }
    
    
    if(fabs(cutoff_frequency) < 0.001)
    {
        cutoff_frequency = 0.001;
    }
    
    rc = 1.0 / (2.0 * C_PI * cutoff_frequency);
    
    alpha = rc / (rc + dt_s);
    
    output = output * alpha + (input - ex_input) * alpha;
}

/*
 * @brief   Applies the low pass filter to the given signal
 * 
 * @param   float   &output 
 *          float   input 
 *          float   alpha
 * 
 * @return  void    Nothing
 *
 */
void FILTER_DATABASE::applyEMA(float &output, float input, float alpha) 
{
    output = (output * (1.0 - alpha)) + (alpha * input); 
}

/*
 * @brief   Calculates the sine value of a given angle in degrees
 * 
 * @param   float   angle_degrees
 * 
 * @return  float   sine_angle_degrees
 *
 */
float FILTER_DATABASE::sinDegrees(float angle_degrees) 
{
    float result = 0;
    
    result = sinf( ( angle_degrees/180.0 ) * C_PI  );
    
    return result;
}



/*
 * @brief   Calculates the cosine value of a given angle in degrees
 * 
 * @param   float   angle_degrees
 * 
 * @return  float   cosine_angle_degrees
 *
 */
float FILTER_DATABASE::cosDegrees(float angle_degrees) 
{
    float result = 0;
    
    result = cosf( ( angle_degrees/180.0 ) * C_PI  );
    
    return result;
}


/*
 * @brief   Calculates the cosine value of a given angle in degrees
 * 
 * @param   float   * values, 
 *          float   dt_s
 * 
 * @return  float   cosine_angle_degrees
 *
 */
void FILTER_DATABASE::simpsonIntegral(float *values, float *val, float dt_s)
{
    *val += ((values[0] + 4.0 * values[1] + values[2]) / 6.0) * dt_s;
}

FILTER_DATABASE::FILTER_DATABASE(const FILTER_DATABASE& orig) 
{
    
}



/*
 * Default destructor.
 */
FILTER_DATABASE::~FILTER_DATABASE() 
{
    
}


/******************************************************************************/
/******************************************************************************/
/******************************************************************************/



/*
 * Begin of Definitions
 */
NOTCH_FILTER notch_filter_gyro_roll;
NOTCH_FILTER notch_filter_gyro_pitch;
// End of Definitions



/*
 * Default constructor.
 */
NOTCH_FILTER::NOTCH_FILTER() 
{
    
}



/*
 * @brief   Initializes the filter coefficients
 * 
 * @param   float sample_freq_hz, 
            float center_freq_hz, 
            float bandwidth_hz, 
            float attenuation_dB
 * 
 * @return  void  Nothing
 *
 */
void NOTCH_FILTER::initialize(float sample_freq_hz, float center_freq_hz, float bandwidth_hz, float attenuation_dB) 
{
    float omega   = 2.0 * C_PI * center_freq_hz / sample_freq_hz;
    float octaves = log2f(center_freq_hz  / (center_freq_hz - bandwidth_hz/2)) * 2;
    float A       = powf(10, -attenuation_dB/40);
    float Q       = sqrtf(powf(2, octaves)) / (powf(2,octaves) - 1);
    float alpha   = sinf(omega) / (2 * Q/A);
    
    b0 =  1.0 + alpha*A;
    b1 = -2.0 * cosf(omega);
    b2 =  1.0 - alpha*A;
    a0_inv =  1.0/(1.0 + alpha/A);
    a1 = -2.0 * cosf(omega);
    a2 =  1.0 - alpha/A;
    
    if( center_freq_hz < (sample_freq_hz/2) )
    {
        initialised = true;
    }
    else
    {
        initialised = false;
    }
}



/*
 * @brief   Initializes the filter coefficients
 * 
 * @param   float sample_freq_hz, 
            float center_freq_hz, 
            float bandwidth_hz, 
            float attenuation_dB
 * 
 * @return  void  Nothing
 *
 */
float NOTCH_FILTER::update(float sample) 
{
    float result = 0;
    
    if (!initialised) 
    {
        result = sample;
    }
    else
    {
        ntchsig2 = ntchsig1;
        ntchsig1 = ntchsig;
        ntchsig  = sample;
        result   = (ntchsig*b0 + ntchsig1*b1 + ntchsig2*b2 - signal1*a1 - signal2*a2) * a0_inv;
        signal2  = signal1;
        signal1  = result;
    }
    
    return result;
}



NOTCH_FILTER::NOTCH_FILTER(const NOTCH_FILTER& orig) 
{
    
}



/*
 * Default destructor.
 */
NOTCH_FILTER::~NOTCH_FILTER() 
{
    
}

