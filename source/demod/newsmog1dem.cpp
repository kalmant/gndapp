#include "newsmog1dem.h"

//The initial code was written by HA7WEN

DEMVariables *create_and_initialize_sdr_variables(long doppler_freq, long datarate){
    DEMVariables *sdr_vars = (DEMVariables*) malloc(sizeof(DEMVariables));
    sdr_vars->cnco_vars.fs = S1DEM_SDR_SAMPLING_FREQ;
    sdr_vars->cnco_vars.f = -doppler_freq;
    sdr_vars->cnco_vars.lo = (std::complex<float>*) malloc(sizeof(std::complex<float>)*sdr_vars->cnco_vars.fs);
    if (sdr_vars->cnco_vars.lo == nullptr) {
        printf("COULD NOT ALLOCATE MEMORY FOR SDR CNCO");
        exit(1);
    }
    for (int i = 0; i < sdr_vars->cnco_vars.fs; i++) {
        float p=2.0*M_PI/sdr_vars->cnco_vars.fs*i;
        if (sdr_vars->cnco_vars.f > 0){
            sdr_vars->cnco_vars.lo[i]=std::complex<float>(cos(p), sin(p));
        }
        else {
            sdr_vars->cnco_vars.lo[i]=std::complex<float>(cos(p), -sin(p));
        }
    }
    if (sdr_vars->cnco_vars.f < 0) {sdr_vars->cnco_vars.f *= -1;}
    sdr_vars->cnco_vars.loi = 0;
    sdr_vars->avg_vars.n = S1DEM_SDR_SAMPLING_FREQ / datarate / 2;
    sdr_vars->avg_vars.buf = (std::complex<float>*)malloc(sizeof(std::complex<float>)*sdr_vars->avg_vars.n);
    if (sdr_vars->avg_vars.buf == nullptr) {
        printf("COULD NOT ALLOCATE MEMORY FOR SDR AVG");
        exit(1);
    }
    for (int i = 0; i < sdr_vars->avg_vars.n; i++){
        sdr_vars->avg_vars.buf[i] = std::complex<float>(0, 0);
    }
    sdr_vars->avg_vars.index = 0;
    sdr_vars->avg_vars.out = std::complex<float>(0,0);
    sdr_vars->demod_vars.bpsindex = 0;
    sdr_vars->demod_vars.integral = std::complex<float>(0, 0);
    sdr_vars->demod_vars.m = std::complex<float>(0, 0);
    sdr_vars->demod_vars.decmem = std::complex<float>(0, 0);
    sdr_vars->demod_vars.lpf = 0;
    sdr_vars->dec_vars.a = 0;
    sdr_vars->dec_vars.b = 0;
    sdr_vars->dec_vars.sm = 0;
    sdr_vars->dec_vars.ca = 0;
    sdr_vars->dec_vars.cb = 0;
    sdr_vars->dec_vars.d = 0;
    return sdr_vars;
}

DEMVariables *create_and_initialize_audio_variables(){
    DEMVariables *audio_vars = (DEMVariables*) malloc(sizeof(DEMVariables));
    audio_vars->cnco_vars.fs = S1DEM_AUDIO_SAMPLING_FREQ;
    audio_vars->cnco_vars.f = -S1DEM_AUDIO_FREQ_CENTER_OFFSET;
    audio_vars->cnco_vars.lo = (std::complex<float>*)malloc(sizeof(std::complex<float>)*audio_vars->cnco_vars.fs);
    if (audio_vars->cnco_vars.lo == nullptr) {
        printf("COULD NOT ALLOCATE MEMORY AUDIO CNCO");
        exit(1);
    }
    for (int i = 0; i < audio_vars->cnco_vars.fs; i++) {
        float p=2.0*M_PI/audio_vars->cnco_vars.fs*i;
        if (audio_vars->cnco_vars.f > 0){
            audio_vars->cnco_vars.lo[i]=std::complex<float>(cos(p), sin(p));
        }
        else {
            audio_vars->cnco_vars.lo[i]=std::complex<float>(cos(p), -sin(p));
        }
    }
    if (audio_vars->cnco_vars.f < 0) {audio_vars->cnco_vars.f *= -1;}
    audio_vars->cnco_vars.loi = 0;
    audio_vars->avg_vars.n = S1DEM_AVG_N;
    audio_vars->avg_vars.buf = (std::complex<float>*)malloc(sizeof(std::complex<float>)*audio_vars->avg_vars.n);
    if (audio_vars->avg_vars.buf == nullptr) {
        printf("COULD NOT ALLOCATE MEMORY FOR AUDIO AVG");
        exit(1);
    }
    for (int i = 0; i < audio_vars->avg_vars.n; i++){
        audio_vars->avg_vars.buf[i] = std::complex<float>(0, 0);
    }
    audio_vars->avg_vars.index = 0;
    audio_vars->avg_vars.out = std::complex<float>(0,0);
    audio_vars->demod_vars.bpsindex = 0;
    audio_vars->demod_vars.integral = std::complex<float>(0, 0);
    audio_vars->demod_vars.m = std::complex<float>(0, 0);
    audio_vars->demod_vars.decmem = std::complex<float>(0, 0);
    audio_vars->demod_vars.lpf = 0;
    audio_vars->dec_vars.a = 0;
    audio_vars->dec_vars.b = 0;
    audio_vars->dec_vars.sm = 0;
    audio_vars->dec_vars.ca = 0;
    audio_vars->dec_vars.cb = 0;
    audio_vars->dec_vars.d = 0;
    return audio_vars;
}

void free_dem_variables(DEMVariables *dem_vars){
    free(dem_vars->cnco_vars.lo);
    free(dem_vars->avg_vars.buf);
    free(dem_vars);
}

void SDR_set_doppler_frequency(DEMVariables *sdr_vars, long doppler_freq){
    sdr_vars->cnco_vars.f = -doppler_freq; // If the doppler is +100Hz, we have to apply a -100Hz CNCO
    for (int i = 0; i < sdr_vars->cnco_vars.fs; i++) {
        float p=2.0*M_PI/sdr_vars->cnco_vars.fs*i;
            if (sdr_vars->cnco_vars.f > 0){
                sdr_vars->cnco_vars.lo[i]=std::complex<float>(cos(p), sin(p));
            }
            else {
                sdr_vars->cnco_vars.lo[i]=std::complex<float>(cos(p), -sin(p));
        }
    }
    if (sdr_vars->cnco_vars.f < 0) {sdr_vars->cnco_vars.f *= -1;}
}

void SDR_set_datarate(DEMVariables *sdr_vars, long datarate){
    sdr_vars->avg_vars.n = S1DEM_SDR_SAMPLING_FREQ / datarate / 2;
    if (sdr_vars->avg_vars.buf != nullptr){
        free(sdr_vars->avg_vars.buf);
    }
    sdr_vars->avg_vars.buf = (std::complex<float>*)malloc(sizeof(std::complex<float>)*sdr_vars->avg_vars.n);
    if (sdr_vars->avg_vars.buf == nullptr) {
        printf("COULD NOT ALLOCATE MEMORY FOR SDR AVG");
        exit(1);
    }
    for (int i = 0; i < sdr_vars->avg_vars.n; i++){
        sdr_vars->avg_vars.buf[i] = std::complex<float>(0, 0);
    }
}

void reset_internal_state(DEMVariables* dem_vars){
    dem_vars->cnco_vars.loi = 0;

    reset_avg_state(&dem_vars->avg_vars);

    reset_demod_state(&dem_vars->demod_vars);

    dem_vars->dec_vars.a = 0;
    dem_vars->dec_vars.b = 0;
    dem_vars->dec_vars.sm = 0;
    dem_vars->dec_vars.ca = 0;
    dem_vars->dec_vars.cb = 0;
    dem_vars->dec_vars.d = 0;
}

void reset_avg_state(AveragingVariables *avg_vars){
    for (int i = 0; i < avg_vars->n; i++){
        avg_vars->buf[i] = std::complex<float>(0, 0);
    }
    avg_vars->index = 0;
    avg_vars->out = std::complex<float>(0,0);
}

void reset_demod_state(DemodulationVariables *demod_vars){
    demod_vars->bpsindex = 0;
    demod_vars->integral = std::complex<float>(0, 0);
    demod_vars->m = std::complex<float>(0, 0);
    demod_vars->decmem = std::complex<float>(0, 0);
    demod_vars->lpf = 0;
}

std::complex<float> s16le2cf(int16_t input){
    return std::complex<float>((float)input/(1<<15), 0);
}

/*
// Rational resampling for audio input
// Input must have a length of S1DEM_ORIGINAL_AUDIO_SAMPLING_FREQ
// Output must have a length of S1DEM_AUDIO_SAMPLING_FREQ
 */
void rational_resample(std::complex<float> input[], std::complex<float> output[]){
    long f1 = S1DEM_ORIGINAL_AUDIO_SAMPLING_FREQ;
    long f2 = S1DEM_AUDIO_SAMPLING_FREQ;
    for(long i=0;i<f1;i++){
        output[(long)round((float)i*f2/f1)]=input[i];
    }
}

std::complex<float> cnco(CncoVariables *cnco_vars, std::complex<float> input){
    std::complex<float> out;
    out=input*cnco_vars->lo[cnco_vars->loi];
    cnco_vars->loi=(cnco_vars->loi+cnco_vars->f)%cnco_vars->fs;
    return out;
}

std::complex<float> average(DEMVariables *dem_vars, std::complex<float> input){
    dem_vars->avg_vars.out -= dem_vars->avg_vars.buf[dem_vars->avg_vars.index];
    dem_vars->avg_vars.buf[dem_vars->avg_vars.index]=input;
    dem_vars->avg_vars.out += dem_vars->avg_vars.buf[dem_vars->avg_vars.index];
    dem_vars->avg_vars.index=((dem_vars->avg_vars.index)+1)%dem_vars->avg_vars.n;
    return dem_vars->avg_vars.out;
}

/* SMOG & ATL GMSK (MSK) demodulator by ha7wen
// Input: complex sample with sampling_freq sampling frequency
// Audio and RTL-SDR sampling_freq/datarate must be integer!
// Output: 2 complex number / databit (Shannon-theorem)
// If creal(out)>cimag(out) = 1 else 0
// output is set if we demodulated successfully
// Return value is true if we demodulated, false otherwise
*/
bool smog_atl_demodulate(DEMVariables *dem_vars, std::complex<float> input, unsigned long sampling_freq, unsigned long datarate, std::complex<float> *output){
    // derivate the resampled signal: decimated signal
    std::complex<float> dec=std::complex<float>(0, 0);
    float t=0;
    // integrate the incoming complex signal
    dem_vars->demod_vars.integral+=input;
    bool demodulated = false;
    // decimate
    if(dem_vars->demod_vars.bpsindex==0){
        // derive
        dec=dem_vars->demod_vars.integral-dem_vars->demod_vars.m;
        dem_vars->demod_vars.m=dem_vars->demod_vars.integral;
        // MSK demod
        t=imag(dec*conj(dem_vars->demod_vars.decmem));
        dem_vars->demod_vars.decmem=dec;
        // low pass filtering
        dem_vars->demod_vars.lpf=1.0/S1DEM_LPF*t+(1.0-1.0/S1DEM_LPF)*dem_vars->demod_vars.lpf;
        // output complex sample: if creal()>cimag() means 1, else 0 bit
        *output=std::complex<float>(t,dem_vars->demod_vars.lpf);
        demodulated = true;
    }
    // index increment for decimation
    dem_vars->demod_vars.bpsindex=(dem_vars->demod_vars.bpsindex+1)%(sampling_freq/datarate/2);
    return demodulated;
}

/* Hard-decision by ha7wen
// Input: 2 complex number / databit
// (da) 1st shifted into a, (db) 2nd into b uint16
// Output: if a == SYNC or b == SYNC the demodulated data as an integer. Otherwise -1. Packet data has to be pieced together outside this function
*/
int make_hard_decision(DecisionVariables *dec_vars, std::complex<float> input_a, std::complex<float> input_b, unsigned long packet_length){
    uint8_t da=(real(input_a)>imag(input_a));
    uint8_t db=(real(input_b)>imag(input_b));
    int out = -1;
    long n = packet_length*8;
    if(dec_vars->sm==1){
        dec_vars->d=(dec_vars->d<<1)|da;
        dec_vars->ca++;
        if(dec_vars->ca%8==0) {out = dec_vars->d;}
        if(dec_vars->ca>=n){
            dec_vars->sm=0;
            dec_vars->ca=0;
        // End of packet
        }
    }
    if(dec_vars->sm==2){
        dec_vars->d=(dec_vars->d<<1)|db;
        dec_vars->cb++;
        if(dec_vars->cb%8==0) {out = dec_vars->d;}
        if(dec_vars->cb>=n){
            dec_vars->sm=0;
            dec_vars->cb=0;
            // End of packet
        }
    }
    if(dec_vars->sm==0){
        dec_vars->a=(dec_vars->a<<1)|da;
        dec_vars->b=(dec_vars->b<<1)|db;
    }
    if(dec_vars->a==S1DEM_SYNC && dec_vars->ca==0 && dec_vars->sm==0) {dec_vars->sm=1;}
    if(dec_vars->b==S1DEM_SYNC && dec_vars->cb==0 && dec_vars->sm==0) {dec_vars->sm=2;}
    return out;
}
