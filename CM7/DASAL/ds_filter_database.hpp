/* 
 * @File:   ds_filter_database.hpp
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


#ifndef DS_FILTER_DATABASE_HPP
#define	DS_FILTER_DATABASE_HPP


/*
 * Begin of Includes
 */
#include <stdint.h>
// End of Includes



/*
 * Begin of Macro Definitions
 */
#ifndef C_PI
#define C_PI    3.141592653589793238463L
#endif
//End of Macro Definitions

#define LP_CUTOFF_FREQ      1.0f
#define YAW_LP_CUTOFF_FREQ  10.0f
#define LP_CUTOFF_FREQ_WIND 1.0f
#define HP_GAIN             50.0f
#define HP_CUTOFF_FREQ      2.0f

/*
 * Begin of FILTER_DATABASE Class Definition
 */
class FILTER_DATABASE 
{
    public:
        FILTER_DATABASE();
        
        void    applyLowPass( float &output, 
                              float input, 
                              float cutoff_frequency_hz, 
                              float dt_s );
        void    applyHighPass(float &output, 
                              float input, 
                              float ex_input, 
                              float gain, 
                              float cutoff_frequency_hz, 
                              float dt_s);
        void    applyEMA    ( float &output, 
                              float input, 
                              float alpha);
        float   sinDegrees  ( float  angle_degrees); 
        float   cosDegrees  ( float  angle_degrees);
        
        void   simpsonIntegral( float *values,
                                 float *val,
                                 float dt_s);
        
        
        
        FILTER_DATABASE(const FILTER_DATABASE& orig);
        virtual ~FILTER_DATABASE();
    private:
        
};
// End of of FILTER_DATABASE Class Definition


/*
 * External Linkages
 */
extern FILTER_DATABASE filter;
// End of External Linkages



/*
 * Begin of NOTCH_FILTER Class Definition
 */
class NOTCH_FILTER 
{
    public:
        NOTCH_FILTER();
        
        void initialize(float sample_freq_hz, 
                        float center_freq_hz, 
                        float bandwidth_hz, 
                        float attenuation_dB);
        
        float update    (float sample);
        
        NOTCH_FILTER(const NOTCH_FILTER& orig);
        virtual ~NOTCH_FILTER();
    private:
        bool initialised = false;
        float b0 = 0;
        float b1 = 0; 
        float b2 = 0;
        float a1 = 0;
        float a2 = 0;
        float a0_inv = 0;
        
        float ntchsig  = 0;
        float ntchsig1 = 0; 
        float ntchsig2 = 0;
        float signal2  = 0; 
        float signal1  = 0;
};
// End of of NOTCH_FILTER Class Definition


/*
 * External Linkages
 */
extern NOTCH_FILTER notch_filter_gyro_roll;
extern NOTCH_FILTER notch_filter_gyro_pitch;
// End of External Linkages



#endif	/* AHIT_FILTER_DATABASE_HPP */

