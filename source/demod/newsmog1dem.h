#ifndef NEWSMOG1DEM_H
#define NEWSMOG1DEM_H

#include <complex>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

#define S1DEM_SDR_SAMPLING_FREQ 250000
#define S1DEM_ORIGINAL_AUDIO_SAMPLING_FREQ 44100
#define S1DEM_AUDIO_BPS 1250
#define S1DEM_AUDIO_SAMPLING_FREQ 42500
#define S1DEM_SYNC 0x2dd4
#define S1DEM_AVG_N (S1DEM_AUDIO_SAMPLING_FREQ/S1DEM_AUDIO_BPS/2)
#define S1DEM_AUDIO_FREQ_CENTER_OFFSET 1500
#define S1DEM_LPF 32

struct CncoStruct{
    long fs;
    long f;
    std::complex<float> *lo;
    long loi;
};

struct AveragingStruct{
    int n;
    std::complex<float> *buf;
    std::complex<float> out;
    int index;
};

struct DemodulationStruct{
    unsigned long bpsindex;
    std::complex<float> integral;
    std::complex<float> m; // memory for the prev integral value - differential
    std::complex<float> decmem; // decimated signal prev sample (MSK demodulation)
    float lpf; // IIR type low pass filter - freq error!
};

struct DecisionStruct{
    uint16_t a;
    uint16_t b;
    uint8_t sm;
    uint32_t ca;
    uint32_t cb;
    uint8_t d;
};

typedef struct CncoStruct CncoVariables;
typedef struct AveragingStruct AveragingVariables;
typedef struct DemodulationStruct DemodulationVariables;
typedef struct DecisionStruct DecisionVariables;

struct DEMStruct{
    CncoVariables cnco_vars;
    AveragingVariables avg_vars;
    DemodulationVariables demod_vars;
    DecisionVariables dec_vars;
};

typedef struct DEMStruct DEMVariables;

DEMVariables* create_and_initialize_sdr_variables(long doppler_freq, long datarate);
DEMVariables* create_and_initialize_audio_variables();
void free_dem_variables(DEMVariables *sdr_vars);
void SDR_set_doppler_frequency(DEMVariables *sdr_vars, long doppler_freq);
void SDR_set_datarate(DEMVariables *sdr_vars, long datarate);
void reset_avg_state(AveragingVariables *avg_vars);
void reset_demod_state(DemodulationVariables *demod_vars);
void reset_internal_state(DEMVariables* dem_vars);

std::complex<float> s16le2cf(int16_t input);
void rational_resample(std::complex<float> input[], std::complex<float> output[]);
std::complex<float> cnco(CncoVariables *cnco_vars, std::complex<float> input);
std::complex<float> average(DEMVariables *dem_vars, std::complex<float> input);
bool smog_atl_demodulate(DEMVariables *dem_vars, std::complex<float> input, unsigned long sampling_freq, unsigned long datarate, std::complex<float> *output);
int make_hard_decision(DecisionVariables *dec_vars, std::complex<float> input_a, std::complex<float> input_b, unsigned long packet_length);

#endif
