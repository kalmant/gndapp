#ifndef NEWSMOG1DEM_H
#define NEWSMOG1DEM_H

#include <complex>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define S1DEM_ORIGINAL_AUDIO_SAMPLING_FREQ 44100
#define S1DEM_AUDIO_BPS 1250
#define S1DEM_AUDIO_SAMPLING_FREQ 42500
#define S1DEM_SYNC 0x2dd4
#define S1DEM_AUDIO_FREQ_CENTER_OFFSET 1500
#define S1DEM_LL 64

struct CncoStruct {
    long fs;
    long f;
    std::complex<float> *lo = nullptr;
    long loi;
};

struct AveragingStruct {
    int n;
    std::complex<float> *buf = nullptr;
    std::complex<float> out;
    int index;
};

struct AveragingDecStruct {
    int n;
    std::complex<float> *buf = nullptr;
    int index;
};

struct DemodulationStruct {
    std::complex<float> m;
    float x;
};

struct DecisionStruct {
    uint16_t a;
    uint16_t b;
    uint8_t sm;
    uint32_t ca;
    uint32_t cb;
    uint8_t d;
};

typedef struct CncoStruct CncoVariables;
typedef struct AveragingStruct AveragingVariables;
typedef struct AveragingDecStruct AveragingDecVariables;
typedef struct DemodulationStruct DemodulationVariables;
typedef struct DecisionStruct DecisionVariables;

void change_cnco_sampling_rate(CncoVariables *cnco_vars, long sampling_rate);
void change_cnco_offset_frequency(CncoVariables *cnco_vars, long offset_frequency);
void reinitialize_avg_vars(AveragingVariables *avg_vars, int n);
void reinitialize_avg_dec_vars(AveragingDecVariables *avg_dec_vars, int n);
void reinitialize_demod_vars(DemodulationVariables *demod_vars);
void reinitialize_dec_vars(DecisionVariables *dec_vars);

std::complex<float> s16le2cf(int16_t input);
void rational_resample(std::complex<float> input[], std::complex<float> output[]);
std::complex<float> cnco(CncoVariables *cnco_vars, std::complex<float> input);
std::complex<float> average(AveragingVariables *avg_vars, std::complex<float> input);
bool average_dec(AveragingDecVariables *avg_dec_vars, std::complex<float> input, std::complex<float> *output);
std::complex<float> smog_atl_demodulate(DemodulationVariables *demod_vars, std::complex<float> input);
int make_hard_decision(
    DecisionVariables *dec_vars, std::complex<float> input_a, std::complex<float> input_b, unsigned long packet_length);

#endif
