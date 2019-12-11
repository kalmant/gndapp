#ifndef HELPER_H
#define HELPER_H

#include <cmath>
#include <cstdint>

const double Ibat_calibr[4][2] = {
    {0.0091578069, 305.6611509242},
    {0.0092536381, 308.7354923643},
    {0.0092357733, 306.997781812},
    {0.0092097441, 308.1479044269},
};

uint16_t static helper_DOWNLINK_PCKT_TYPE_ATL_TELEMETRY_3_calc_battery_current_mA(int panel, uint16_t adc) {
    if (panel < 1)
        return 0;
    if (panel > 4)
        return 0;
    // 'panel' will be used for compensation by calibration values...

    double Ibat;
    Ibat = adc * Ibat_calibr[panel - 1][0] - Ibat_calibr[panel - 1][1];

    return static_cast<uint16_t>(Ibat);
}

const double resist_correction[4][5][3] = {
    {
        {-0.0006651322, -0.0122560584, 0.751697002},
        {-0.0001612352, -0.054850064, 0.4956892858},
        {0.0005175386, -0.0151862359, 1.584156215},
        {-0.0005616397, -0.0035293076, 1.2312336719},
        {-0.0006161208, 0.0329681997, 2.3084111427},
    },
    {
        {-0.0008810698, -0.0005768639, 1.4462896472},
        {0.0002554855, -0.0497453659, 1.2655688326},
        {-0.0002372522, 0.0156908555, 2.3968181466},
        {-0.0005689415, 0.0082033445, 2.05142517512},
        {-0.000324599, 0.0315826991, 2.973868843},
    },
    {
        {-0.0005846387, -0.0212703774, 1.6960469081},
        {-0.0005227575, -0.029222141, 0.6464118577},
        {-0.0003972915, 0.0164286754, 2.2502388179},
        {0.0003169663, -0.0174945998, 1.7588053065},
        {-0.0004615105, 0.0385790667, 2.7849059873},
    },
    {
        {-0.0006318885, -0.0047932305, 1.6301837316},
        {-0.000244109, -0.039820197, 1.3241749543},
        {-0.0005641744, 0.022177914, 2.4338965601},
        {-0.0004342717, 0.0057748108, 1.9869618945},
        {-0.0005652303, 0.0447351507, 3.0252586698},
    },
};

uint16_t static helper_DOWNLINK_PCKT_TYPE_ATL_TELEMETRY_3_calc_temperature_mC(
    int panel, uint16_t adc_ref, uint16_t adc, int channel) {
    if (panel < 1)
        return 0;
    if (panel > 4)
        return 0;
    if (channel < 1)
        return 0;
    if (channel > 5)
        return 0;
    // 'panel' will be used for compensation by calibration values...

    const double Urefp = 2.046;
    const double Urefm = 0.0;
    const double R0 = 100.0;
    const double Rref = 72.9;
    const double A = 3.9083e-3;
    const double B = -5.775e-7;
    const double lsb = (Urefp - Urefm) / 65536.0;

    double Itemp, Rtemp;
    double Rt;
    double Tpt100;

    Itemp = (adc_ref * lsb) / (100.0 * Rref);
    Rtemp = ((adc * lsb) / (Itemp * 100.0));
    Rtemp = Rtemp - ((Rtemp * Rtemp * resist_correction[panel - 1][channel - 1][0]) +
                        (Rtemp * resist_correction[panel - 1][channel - 1][1]) +
                        (resist_correction[panel - 1][channel - 1][2]));
    Rt = Rref + Rtemp;
    Tpt100 = ((-1.0 * R0 * A) + sqrt((R0 * R0 * A * A) - (4.0 * R0 * B * (R0 - Rt)))) / (2.0 * R0 * B);

    return static_cast<uint16_t>(Tpt100 * 1000);
}

#endif // HELPER_H
