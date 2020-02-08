#include "newsmog1dem.h"

// The initial code was written by HA7WEN

DEMVariables *create_and_initialize_sdr_variables(long offset_freq, long datarate) {
    DEMVariables *sdr_vars = (DEMVariables *) malloc(sizeof(DEMVariables));
    sdr_vars->cnco_vars.fs = S1DEM_SDR_SAMPLING_FREQ;
    sdr_vars->cnco_vars.f = -offset_freq;
    sdr_vars->cnco_vars.lo = (std::complex<float> *) malloc(sizeof(std::complex<float>) * sdr_vars->cnco_vars.fs);
    if (sdr_vars->cnco_vars.lo == nullptr) {
        printf("COULD NOT ALLOCATE MEMORY FOR SDR CNCO");
        exit(1);
    }
    for (int i = 0; i < sdr_vars->cnco_vars.fs; i++) {
        float p = 2.0 * M_PI / sdr_vars->cnco_vars.fs * i;
        if (sdr_vars->cnco_vars.f > 0) {
            sdr_vars->cnco_vars.lo[i] = std::complex<float>(cos(p), sin(p));
        }
        else {
            sdr_vars->cnco_vars.lo[i] = std::complex<float>(cos(p), -sin(p));
        }
    }
    if (sdr_vars->cnco_vars.f < 0) {
        sdr_vars->cnco_vars.f *= -1;
    }
    sdr_vars->cnco_vars.loi = 0;
    sdr_vars->avg_vars.n = S1DEM_SDR_SAMPLING_FREQ / datarate;
    sdr_vars->avg_vars.buf = (std::complex<float> *) malloc(sizeof(std::complex<float>) * sdr_vars->avg_vars.n);
    if (sdr_vars->avg_vars.buf == nullptr) {
        printf("COULD NOT ALLOCATE MEMORY FOR SDR AVG");
        exit(1);
    }
    for (int i = 0; i < sdr_vars->avg_vars.n; i++) {
        sdr_vars->avg_vars.buf[i] = std::complex<float>(0, 0);
    }
    sdr_vars->avg_vars.index = 0;
    sdr_vars->avg_vars.out = std::complex<float>(0, 0);
    sdr_vars->avg_dec_vars.n = S1DEM_SDR_SAMPLING_FREQ / datarate / 2;
    sdr_vars->avg_dec_vars.buf = (std::complex<float> *) malloc(sizeof(std::complex<float>) * sdr_vars->avg_dec_vars.n);
    if (sdr_vars->avg_dec_vars.buf == nullptr) {
        printf("COULD NOT ALLOCATE MEMORY FOR SDR AVG DEC");
        exit(1);
    }
    sdr_vars->avg_dec_vars.index = 0;
    sdr_vars->demod_vars.m = std::complex<float>(0, 0);
    sdr_vars->demod_vars.x = 0;
    sdr_vars->dec_vars.a = 0;
    sdr_vars->dec_vars.b = 0;
    sdr_vars->dec_vars.sm = 0;
    sdr_vars->dec_vars.ca = 0;
    sdr_vars->dec_vars.cb = 0;
    sdr_vars->dec_vars.d = 0;
    return sdr_vars;
}

void free_dem_variables(DEMVariables *dem_vars) {
    free(dem_vars->cnco_vars.lo);
    free(dem_vars->avg_vars.buf);
    free(dem_vars->avg_dec_vars.buf);
    free(dem_vars);
}

void SDR_set_offset(DEMVariables *sdr_vars, long offset_freq) {
    sdr_vars->cnco_vars.f = -offset_freq; // If the offset is +100Hz, we have to apply a -100Hz CNCO
    for (int i = 0; i < sdr_vars->cnco_vars.fs; i++) {
        float p = 2.0 * M_PI / sdr_vars->cnco_vars.fs * i;
        if (sdr_vars->cnco_vars.f > 0) {
            sdr_vars->cnco_vars.lo[i] = std::complex<float>(cos(p), sin(p));
        }
        else {
            sdr_vars->cnco_vars.lo[i] = std::complex<float>(cos(p), -sin(p));
        }
    }
    if (sdr_vars->cnco_vars.f < 0) {
        sdr_vars->cnco_vars.f *= -1;
    }
}

void SDR_set_datarate(DEMVariables *sdr_vars, long datarate) {
    sdr_vars->avg_vars.n = S1DEM_SDR_SAMPLING_FREQ / datarate;
    if (sdr_vars->avg_vars.buf != nullptr) {
        free(sdr_vars->avg_vars.buf);
    }
    sdr_vars->avg_vars.buf = (std::complex<float> *) malloc(sizeof(std::complex<float>) * sdr_vars->avg_vars.n);
    if (sdr_vars->avg_vars.buf == nullptr) {
        printf("COULD NOT ALLOCATE MEMORY FOR SDR AVG");
        exit(1);
    }
    for (int i = 0; i < sdr_vars->avg_vars.n; i++) {
        sdr_vars->avg_vars.buf[i] = std::complex<float>(0, 0);
    }
    sdr_vars->avg_dec_vars.n = S1DEM_SDR_SAMPLING_FREQ / datarate / 2;
    sdr_vars->avg_dec_vars.index = 0;
    if (sdr_vars->avg_dec_vars.buf != nullptr) {
        free(sdr_vars->avg_dec_vars.buf);
    }
    sdr_vars->avg_dec_vars.buf = (std::complex<float> *) malloc(sizeof(std::complex<float>) * sdr_vars->avg_dec_vars.n);
    if (sdr_vars->avg_dec_vars.buf == nullptr) {
        printf("COULD NOT ALLOCATE MEMORY FOR SDR AVG DEC");
        exit(1);
    }
}

std::complex<float> s16le2cf(int16_t input) {
    return std::complex<float>((float) input / (1 << 15), 0);
}

/*
// Rational resampling for audio input
// Input must have a length of S1DEM_ORIGINAL_AUDIO_SAMPLING_FREQ
// Output must have a length of S1DEM_AUDIO_SAMPLING_FREQ
 */
void rational_resample(std::complex<float> input[], std::complex<float> output[]) {
    long f1 = S1DEM_ORIGINAL_AUDIO_SAMPLING_FREQ;
    long f2 = S1DEM_AUDIO_SAMPLING_FREQ;
    for (long i = 0; i < f1; i++) {
        output[(long) round((float) i * f2 / f1)] = input[i];
    }
}

std::complex<float> cnco(CncoVariables *cnco_vars, std::complex<float> input) {
    std::complex<float> out;
    out = input * cnco_vars->lo[cnco_vars->loi];
    cnco_vars->loi = (cnco_vars->loi + cnco_vars->f) % cnco_vars->fs;
    return out;
}

std::complex<float> average(AveragingVariables *avg_vars, std::complex<float> input) {
    avg_vars->out -= avg_vars->buf[avg_vars->index];
    avg_vars->buf[avg_vars->index] = input;
    avg_vars->out += avg_vars->buf[avg_vars->index];
    avg_vars->index = ((avg_vars->index) + 1) % avg_vars->n;
    return avg_vars->out;
}

bool average_dec(AveragingDecVariables *avg_dec_vars, std::complex<float> input, std::complex<float> *output) {
    bool performed = false;
    if (avg_dec_vars->index == avg_dec_vars->n) {
        avg_dec_vars->index = 0;
        auto out = std::complex<float>(0, 0);
        for (int i = 0; i < avg_dec_vars->n; i++) {
            out += avg_dec_vars->buf[i];
        }
        *output = out;
        performed = true;
    }
    avg_dec_vars->buf[avg_dec_vars->index] = input;
    avg_dec_vars->index++;
    return performed;
}

std::complex<float> smog_atl_demodulate(DemodulationVariables *demod_vars, std::complex<float> input) {
    float t = imag(input * conj(demod_vars->m));
    demod_vars->m = input;
    demod_vars->x = 1. / S1DEM_LL * t + (S1DEM_LL - 1.0) / S1DEM_LL * demod_vars->x;
    return std::complex<float>(t, demod_vars->x);
}

/* Hard-decision by ha7wen
// Input: 2 complex number / databit
// (da) 1st shifted into a, (db) 2nd into b uint16
// Output: if a == SYNC or b == SYNC the demodulated data as an integer. Otherwise -1. Packet data has to be pieced
together outside this function
*/
int make_hard_decision(DecisionVariables *dec_vars,
    std::complex<float> input_a,
    std::complex<float> input_b,
    unsigned long packet_length) {
    uint8_t da = (real(input_a) > imag(input_a));
    uint8_t db = (real(input_b) > imag(input_b));
    int out = -1;
    long n = packet_length * 8;
    if (dec_vars->sm == 1) {
        dec_vars->d = (dec_vars->d << 1) | da;
        dec_vars->ca++;
        if (dec_vars->ca % 8 == 0) {
            out = dec_vars->d;
        }
        if (dec_vars->ca >= n) {
            dec_vars->sm = 0;
            dec_vars->ca = 0;
            // End of packet
        }
    }
    if (dec_vars->sm == 2) {
        dec_vars->d = (dec_vars->d << 1) | db;
        dec_vars->cb++;
        if (dec_vars->cb % 8 == 0) {
            out = dec_vars->d;
        }
        if (dec_vars->cb >= n) {
            dec_vars->sm = 0;
            dec_vars->cb = 0;
            // End of packet
        }
    }
    if (dec_vars->sm == 0) {
        dec_vars->a = (dec_vars->a << 1) | da;
        dec_vars->b = (dec_vars->b << 1) | db;
    }
    if (dec_vars->a == S1DEM_SYNC && dec_vars->ca == 0 && dec_vars->sm == 0) {
        dec_vars->sm = 1;
    }
    if (dec_vars->b == S1DEM_SYNC && dec_vars->cb == 0 && dec_vars->sm == 0) {
        dec_vars->sm = 2;
    }
    return out;
}

void reinitialize_avg_vars(AveragingVariables *avg_vars, int n) {
    avg_vars->n = n;
    if (avg_vars->buf) {
        free(avg_vars->buf);
    }
    avg_vars->buf = (std::complex<float> *) malloc(sizeof(std::complex<float>) * avg_vars->n);
    if (avg_vars->buf == nullptr) {
        printf("COULD NOT ALLOCATE MEMORY FOR AVG");
        exit(1);
    }
    for (int i = 0; i < avg_vars->n; i++) {
        avg_vars->buf[i] = std::complex<float>(0, 0);
    }
    avg_vars->index = 0;
    avg_vars->out = std::complex<float>(0, 0);
}

void reinitialize_avg_dec_vars(AveragingDecVariables *avg_dec_vars, int n) {
    avg_dec_vars->n = n;
    if (avg_dec_vars->buf) {
        free(avg_dec_vars->buf);
    }
    avg_dec_vars->buf = (std::complex<float> *) malloc(sizeof(std::complex<float>) * avg_dec_vars->n);
    if (avg_dec_vars->buf == nullptr) {
        printf("COULD NOT ALLOCATE MEMORY FOR AVG DEC");
        exit(1);
    }
    avg_dec_vars->index = 0;
}

void reinitialize_demod_vars(DemodulationVariables *demod_vars) {
    demod_vars->m = std::complex<float>(0, 0);
    demod_vars->x = 0;
}

void reinitialize_dec_vars(DecisionVariables *dec_vars) {
    dec_vars->a = 0;
    dec_vars->b = 0;
    dec_vars->sm = 0;
    dec_vars->ca = 0;
    dec_vars->cb = 0;
    dec_vars->d = 0;
}

void change_cnco_sampling_rate(CncoVariables *cnco_vars, long sampling_rate) {
    cnco_vars->fs = sampling_rate;
    cnco_vars->f = 0;
    if (cnco_vars->lo) {
        free(cnco_vars->lo);
    }
    cnco_vars->lo = (std::complex<float> *) malloc(sizeof(std::complex<float>) * cnco_vars->fs);
    if (cnco_vars->lo == nullptr) {
        printf("COULD NOT ALLOCATE MEMORY FOR CNCO");
        exit(1);
    }
    for (int i = 0; i < cnco_vars->fs; i++) {
        float p = 2.0 * M_PI / cnco_vars->fs * i;
        if (cnco_vars->f > 0) {
            cnco_vars->lo[i] = std::complex<float>(cos(p), sin(p));
        }
        else {
            cnco_vars->lo[i] = std::complex<float>(cos(p), -sin(p));
        }
    }
    if (cnco_vars->f < 0) {
        cnco_vars->f *= -1;
    }
    cnco_vars->loi = 0;
}

void change_cnco_offset_frequency(CncoVariables *cnco_vars, long offset_frequency) {
    cnco_vars->f = -offset_frequency; // If the offset is +100Hz, we have to apply a -100Hz CNCO
    for (int i = 0; i < cnco_vars->fs; i++) {
        float p = 2.0 * M_PI / cnco_vars->fs * i;
        if (cnco_vars->f > 0) {
            cnco_vars->lo[i] = std::complex<float>(cos(p), sin(p));
        }
        else {
            cnco_vars->lo[i] = std::complex<float>(cos(p), -sin(p));
        }
    }
    if (cnco_vars->f < 0) {
        cnco_vars->f *= -1;
    }
}
