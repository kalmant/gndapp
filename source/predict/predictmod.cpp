/***************************************************************************\
*          PREDICT: A satellite tracking/orbital prediction program         *
*          Project started 26-May-1991 by John A. Magliacane, KD2BD         *
*                        Last update: 14-May-2006                           *
*****************************************************************************
*                                                                           *
* This program is free software; you can redistribute it and/or modify it   *
* under the terms of the GNU General Public License as published by the     *
* Free Software Foundation; either version 2 of the License or any later    *
* version.                                                                  *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
* General Public License for more details.                                  *
*                                                                           *
*****************************************************************************
*          See the "CREDITS" file for the names of those who have           *
*   generously contributed their time, talent, and effort to this project.  *
\***************************************************************************/

#include "predictmod.h"
#include <QDateTime>
#include <QList>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

/* Constants used by SGP4/SDP4 code */

#define km2mi 0.621371               /* km to miles */
#define deg2rad 1.745329251994330E-2 /* Degrees to radians */
#define pi 3.14159265358979323846    /* Pi */
#define pio2 1.57079632679489656     /* Pi/2 */
#define x3pio2 4.71238898038468967   /* 3*Pi/2 */
#define twopi 6.28318530717958623    /* 2*Pi  */
#define e6a 1.0E-6
#define tothrd 6.6666666666666666E-1 /* 2/3 */
#define xj2 1.0826158E-3             /* J2 Harmonic (WGS '72) */
#define xj3 -2.53881E-6              /* J3 Harmonic (WGS '72) */
#define xj4 -1.65597E-6              /* J4 Harmonic (WGS '72) */
#define xke 7.43669161E-2
#define xkmper 6.378137E3 /* WGS 84 Earth radius km */
#define xmnpda 1.44E3     /* Minutes per day */
#define ae 1.0
#define ck2 5.413079E-4
#define ck4 6.209887E-7
#define f 3.35281066474748E-3 /* Flattening factor */
#define ge 3.986008E5         /* Earth gravitational constant (WGS '72) */
#define s 1.012229
#define qoms2t 1.880279E-09
#define secday 8.6400E4       /* Seconds per day */
#define omega_E 1.00273790934 /* Earth rotations/siderial day */
#define omega_ER 6.3003879    /* Earth rotations, rads/siderial day */
#define zns 1.19459E-5
#define c1ss 2.9864797E-6
#define zes 1.675E-2
#define znl 1.5835218E-4
#define c1l 4.7968065E-7
#define zel 5.490E-2
#define zcosis 9.1744867E-1
#define zsinis 3.9785416E-1
#define zsings -9.8088458E-1
#define zcosgs 1.945905E-1
#define zcoshs 1
#define zsinhs 0
#define q22 1.7891679E-6
#define q31 2.1460748E-6
#define q33 2.2123015E-7
#define g22 5.7686396
#define g32 9.5240898E-1
#define g44 1.8014998
#define g52 1.0508330
#define g54 4.4108898
#define root22 1.7891679E-6
#define root32 3.7393792E-7
#define root44 7.3636953E-9
#define root52 1.1428639E-7
#define root54 2.1765803E-9
#define thdt 4.3752691E-3
#define rho 1.5696615E-1
#define mfactor 7.292115E-5
#define sr 6.96000E5       /* Solar radius - km (IAU 76) */
#define AU 1.49597870691E8 /* Astronomical unit - km (IAU 76) */

/* Entry points of Deep() */

#define dpinit 1 /* Deep-space initialization code */
#define dpsec 2  /* Deep-space secular code        */
#define dpper 3  /* Deep-space periodic code       */

/* Flow control flag definitions */

#define ALL_FLAGS -1
#define SGP_INITIALIZED_FLAG 0x000001 /* not used */
#define SGP4_INITIALIZED_FLAG 0x000002
#define SDP4_INITIALIZED_FLAG 0x000004
#define SGP8_INITIALIZED_FLAG 0x000008 /* not used */
#define SDP8_INITIALIZED_FLAG 0x000010 /* not used */
#define SIMPLE_FLAG 0x000020
#define DEEP_SPACE_EPHEM_FLAG 0x000040
#define LUNAR_TERMS_DONE_FLAG 0x000080
#define NEW_EPHEMERIS_FLAG 0x000100 /* not used */
#define DO_LOOP_FLAG 0x000200
#define RESONANCE_FLAG 0x000400
#define SYNCHRONOUS_FLAG 0x000800
#define EPOCH_RESTART_FLAG 0x001000
#define VISIBLE_FLAG 0x002000
#define SAT_ECLIPSED_FLAG 0x004000

satStruct sat;

qthStruct qth;

/* Global variables for sharing data among functions... */

double tsince, jul_epoch, jul_utc, eclipse_depth = 0, sat_azi, sat_ele, sat_range, sat_range_rate, sat_lat, sat_lon,
                                   sat_alt, sat_vel, phase, sun_azi, sun_ele, daynum, fm, fk, age, aostime, lostime, ax,
                                   ay, az, rx, ry, rz, squint, alat, alon, sun_ra, sun_dec, sun_lat, sun_lon, sun_range,
                                   sun_range_rate, moon_az, moon_el, moon_dx, moon_ra, moon_dec, moon_gha, moon_dv;

char qthfile[50], tlefile[50], dbfile[50], temp[80], output[25], serial_port[15],
    resave = 0, reload_tle = 0, netport[6], once_per_second = 0, ephem[5], sat_sun_status, findsun, calc_squint,
    database = 0, xterm, io_lat = 'N', io_lon = 'W';

int indx, antfd, iaz, iel, ma256, isplat, isplong, socket_flag = 0, Flags = 0;

long rv, irk;

unsigned char val[256];

/* The following variables are used by the socket server.  They
   are updated in the MultiTrack() and SingleTrack() functions. */

char visibility_array[24], tracking_mode[30];

float az_array[24], el_array[24], long_array[24], lat_array[24], footprint_array[24], range_array[24],
    altitude_array[24], velocity_array[24], eclipse_depth_array[24], phase_array[24], squint_array[24];

double doppler[24], nextevent[24];

long aos_array[24], orbitnum_array[24];

unsigned short portbase = 0;

/** Type definitions **/

/* Two-line-element satellite orbital data
   structure used directly by the SGP4/SDP4 code. */

typedef struct {
    double epoch, xndt2o, xndd6o, bstar, xincl, xnodeo, eo, omegao, xmo, xno;
    int catnr, elset, revnum;
    char idesg[9];
} tle_t;

/* Geodetic position structure used by SGP4/SDP4 code. */

typedef struct {
    double lat, lon, alt, theta;
} geodetic_t;

/* General three-dimensional vector structure used by SGP4/SDP4 code. */

typedef struct {
    double x, y, z, w;
} vector_t;

/* Common arguments between deep-space functions used by SGP4/SDP4 code. */

typedef struct {
    /* Used by dpinit part of Deep() */
    double eosq, sinio, cosio, betao, aodp, theta2, sing, cosg, betao2, xmdot, omgdot, xnodot, xnodp;

    /* Used by dpsec and dpper parts of Deep() */
    double xll, omgadf, xnode, em, xinc, xn, t;

    /* Used by thetg and Deep() */
    double ds50;
} deep_arg_t;

/* Global structure used by SGP4/SDP4 code. */

geodetic_t obs_geodetic;

/* Two-line Orbital Elements for the satellite used by SGP4/SDP4 code. */

tle_t tle;

/* Functions for testing and setting/clearing flags used in SGP4/SDP4 code */

int isFlagSet(int flag) {
    return (Flags & flag);
}

int isFlagClear(int flag) {
    return (~Flags & flag);
}

void SetFlag(int flag) {
    Flags |= flag;
}

void ClearFlag(int flag) {
    Flags &= ~flag;
}

/* Remaining SGP4/SDP4 code follows... */

int Sign(double arg) {
    /* Returns sign of a double */

    if (arg > 0)
        return 1;

    else if (arg < 0)
        return -1;

    else
        return 0;
}

double Sqr(double arg) {
    /* Returns square of a double */
    return (arg * arg);
}

double Radians(double arg) {
    /* Returns angle in radians from argument in degrees */
    return (arg * deg2rad);
}

double Degrees(double arg) {
    /* Returns angle in degrees from argument in radians */
    return (arg / deg2rad);
}

double ArcSin(double arg) {
    /* Returns the arcsine of the argument */

    if (fabs(arg) >= 1.0)
        return (Sign(arg) * pio2);
    else

        return (atan(arg / sqrt(1.0 - arg * arg)));
}

double ArcCos(double arg) {
    /* Returns arccosine of argument */
    return (pio2 - ArcSin(arg));
}

void Magnitude(vector_t *v) {
    /* Calculates scalar magnitude of a vector_t argument */
    v->w = sqrt(Sqr(v->x) + Sqr(v->y) + Sqr(v->z));
}

void Vec_Sub(vector_t *v1, vector_t *v2, vector_t *v3) {
    /* Subtracts vector v2 from v1 to produce v3 */
    v3->x = v1->x - v2->x;
    v3->y = v1->y - v2->y;
    v3->z = v1->z - v2->z;
    Magnitude(v3);
}

void Scalar_Multiply(double k, vector_t *v1, vector_t *v2) {
    /* Multiplies the vector v1 by the scalar k to produce the vector v2 */
    v2->x = k * v1->x;
    v2->y = k * v1->y;
    v2->z = k * v1->z;
    v2->w = fabs(k) * v1->w;
}

void Scale_Vector(double k, vector_t *v) {
    /* Multiplies the vector v1 by the scalar k */
    v->x *= k;
    v->y *= k;
    v->z *= k;
    Magnitude(v);
}

double Dot(vector_t *v1, vector_t *v2) {
    /* Returns the dot product of two vectors */
    return (v1->x * v2->x + v1->y * v2->y + v1->z * v2->z);
}

double Angle(vector_t *v1, vector_t *v2) {
    /* Calculates the angle between vectors v1 and v2 */
    Magnitude(v1);
    Magnitude(v2);
    return (ArcCos(Dot(v1, v2) / (v1->w * v2->w)));
}

double AcTan(double sinx, double cosx) {
    /* Four-quadrant arctan function */

    if (cosx == 0.0) {
        if (sinx > 0.0)
            return (pio2);
        else
            return (x3pio2);
    }

    else {
        if (cosx > 0.0) {
            if (sinx > 0.0)
                return (atan(sinx / cosx));
            else
                return (twopi + atan(sinx / cosx));
        }

        else
            return (pi + atan(sinx / cosx));
    }
}

double FMod2p(double x) {
    /* Returns mod 2PI of argument */

    int i;
    double ret_val;

    ret_val = x;
    i = ret_val / twopi;
    ret_val -= i * twopi;

    if (ret_val < 0.0)
        ret_val += twopi;

    return ret_val;
}

double Modulus(double arg1, double arg2) {
    /* Returns arg1 mod arg2 */

    int i;
    double ret_val;

    ret_val = arg1;
    i = ret_val / arg2;
    ret_val -= i * arg2;

    if (ret_val < 0.0)
        ret_val += arg2;

    return ret_val;
}

double Frac(double arg) {
    /* Returns fractional part of double argument */
    return (arg - floor(arg));
}

void Convert_Sat_State(vector_t *pos, vector_t *vel) {
    /* Converts the satellite's position and velocity  */
    /* vectors from normalized values to km and km/sec */
    Scale_Vector(xkmper, pos);
    Scale_Vector(xkmper * xmnpda / secday, vel);
}

double Julian_Date_of_Year(double year) {
    /* The function Julian_Date_of_Year calculates the Julian Date  */
    /* of Day 0.0 of {year}. This function is used to calculate the */
    /* Julian Date of any date by using Julian_Date_of_Year, DOY,   */
    /* and Fraction_of_Day. */

    /* Astronomical Formulae for Calculators, Jean Meeus, */
    /* pages 23-25. Calculate Julian Date of 0.0 Jan year */

    long A, B, i;
    double jdoy;

    year = year - 1;
    i = year / 100;
    A = i;
    i = A / 4;
    B = 2 - A + i;
    i = 365.25 * year;
    i += 30.6001 * 14;
    jdoy = i + 1720994.5 + B;

    return jdoy;
}

double Julian_Date_of_Epoch(double epoch) {
    /* The function Julian_Date_of_Epoch returns the Julian Date of     */
    /* an epoch specified in the format used in the NORAD two-line      */
    /* element sets. It has been modified to support dates beyond       */
    /* the year 1999 assuming that two-digit years in the range 00-56   */
    /* correspond to 2000-2056. Until the two-line element set format   */
    /* is changed, it is only valid for dates through 2056 December 31. */

    double year, day;

    /* Modification to support Y2K */
    /* Valid 1957 through 2056     */

    day = modf(epoch * 1E-3, &year) * 1E3;

    if (year < 57)
        year = year + 2000;
    else
        year = year + 1900;

    return (Julian_Date_of_Year(year) + day);
}

double Delta_ET(double year) {
    /* The function Delta_ET has been added to allow calculations on   */
    /* the position of the sun.  It provides the difference between UT */
    /* (approximately the same as UTC) and ET (now referred to as TDT).*/
    /* This function is based on a least squares fit of data from 1950 */
    /* to 1991 and will need to be updated periodically. */

    /* Values determined using data from 1950-1991 in the 1990
    Astronomical Almanac.  See DELTA_ET.WQ1 for details. */

    double delta_et;

    delta_et = 26.465 + 0.747622 * (year - 1950) + 1.886913 * sin(twopi * (year - 1975) / 33);

    return delta_et;
}

double ThetaG(double epoch, deep_arg_t *deep_arg) {
    /* The function ThetaG calculates the Greenwich Mean Sidereal Time */
    /* for an epoch specified in the format used in the NORAD two-line */
    /* element sets. It has now been adapted for dates beyond the year */
    /* 1999, as described above. The function ThetaG_JD provides the   */
    /* same calculation except that it is based on an input in the     */
    /* form of a Julian Date. */

    /* Reference:  The 1992 Astronomical Almanac, page B6. */

    double year, day, UT, jd, TU, GMST, ThetaG;

    /* Modification to support Y2K */
    /* Valid 1957 through 2056     */

    day = modf(epoch * 1E-3, &year) * 1E3;

    if (year < 57)
        year += 2000;
    else
        year += 1900;

    UT = modf(day, &day);
    jd = Julian_Date_of_Year(year) + day;
    TU = (jd - 2451545.0) / 36525;
    GMST = 24110.54841 + TU * (8640184.812866 + TU * (0.093104 - TU * 6.2E-6));
    GMST = Modulus(GMST + secday * omega_E * UT, secday);
    ThetaG = twopi * GMST / secday;
    deep_arg->ds50 = jd - 2433281.5 + UT;
    ThetaG = FMod2p(6.3003880987 * deep_arg->ds50 + 1.72944494);

    return ThetaG;
}

double ThetaG_JD(double jd) {
    /* Reference:  The 1992 Astronomical Almanac, page B6. */

    double UT, TU, GMST;

    UT = Frac(jd + 0.5);
    jd = jd - UT;
    TU = (jd - 2451545.0) / 36525;
    GMST = 24110.54841 + TU * (8640184.812866 + TU * (0.093104 - TU * 6.2E-6));
    GMST = Modulus(GMST + secday * omega_E * UT, secday);

    return (twopi * GMST / secday);
}

void Calculate_Solar_Position(double time, vector_t *solar_vector) {
    /* Calculates solar position vector */

    double mjd, year, T, M, L, e, C, O, Lsa, nu, R, eps;

    mjd = time - 2415020.0;
    year = 1900 + mjd / 365.25;
    T = (mjd + Delta_ET(year) / secday) / 36525.0;
    M = Radians(Modulus(358.47583 + Modulus(35999.04975 * T, 360.0) - (0.000150 + 0.0000033 * T) * Sqr(T), 360.0));
    L = Radians(Modulus(279.69668 + Modulus(36000.76892 * T, 360.0) + 0.0003025 * Sqr(T), 360.0));
    e = 0.01675104 - (0.0000418 + 0.000000126 * T) * T;
    C = Radians((1.919460 - (0.004789 + 0.000014 * T) * T) * sin(M) + (0.020094 - 0.000100 * T) * sin(2 * M) +
                0.000293 * sin(3 * M));
    O = Radians(Modulus(259.18 - 1934.142 * T, 360.0));
    Lsa = Modulus(L + C - Radians(0.00569 - 0.00479 * sin(O)), twopi);
    nu = Modulus(M + C, twopi);
    R = 1.0000002 * (1.0 - Sqr(e)) / (1.0 + e * cos(nu));
    eps = Radians(23.452294 - (0.0130125 + (0.00000164 - 0.000000503 * T) * T) * T + 0.00256 * cos(O));
    R = AU * R;
    solar_vector->x = R * cos(Lsa);
    solar_vector->y = R * sin(Lsa) * cos(eps);
    solar_vector->z = R * sin(Lsa) * sin(eps);
    solar_vector->w = R;
}

int Sat_Eclipsed(vector_t *pos, vector_t *sol, double *depth) {
    /* Calculates satellite's eclipse status and depth */

    double sd_sun, sd_earth, delta;
    vector_t Rho, earth;

    /* Determine partial eclipse */

    sd_earth = ArcSin(xkmper / pos->w);
    Vec_Sub(sol, pos, &Rho);
    sd_sun = ArcSin(sr / Rho.w);
    Scalar_Multiply(-1, pos, &earth);
    delta = Angle(sol, &earth);
    *depth = sd_earth - sd_sun - delta;

    if (sd_earth < sd_sun)
        return 0;
    else if (*depth >= 0)
        return 1;
    else
        return 0;
}

void select_ephemeris(tle_t *tle) {
    /* Selects the apropriate ephemeris type to be used */
    /* for predictions according to the data in the TLE */
    /* It also processes values in the tle set so that  */
    /* they are apropriate for the sgp4/sdp4 routines   */

    double ao, xnodp, dd1, dd2, delo, temp, a1, del1, r1;

    /* Preprocess tle set */
    tle->xnodeo *= deg2rad;
    tle->omegao *= deg2rad;
    tle->xmo *= deg2rad;
    tle->xincl *= deg2rad;
    temp = twopi / xmnpda / xmnpda;
    tle->xno = tle->xno * temp * xmnpda;
    tle->xndt2o *= temp;
    tle->xndd6o = tle->xndd6o * temp / xmnpda;
    tle->bstar /= ae;

    /* Period > 225 minutes is deep space */
    dd1 = (xke / tle->xno);
    dd2 = tothrd;
    a1 = pow(dd1, dd2);
    r1 = cos(tle->xincl);
    dd1 = (1.0 - tle->eo * tle->eo);
    temp = ck2 * 1.5f * (r1 * r1 * 3.0 - 1.0) / pow(dd1, 1.5);
    del1 = temp / (a1 * a1);
    ao = a1 * (1.0 - del1 * (tothrd * .5 + del1 * (del1 * 1.654320987654321 + 1.0)));
    delo = temp / (ao * ao);
    xnodp = tle->xno / (delo + 1.0);

    /* Select a deep-space/near-earth ephemeris */

    if (twopi / xnodp / xmnpda >= 0.15625)
        SetFlag(DEEP_SPACE_EPHEM_FLAG);
    else
        ClearFlag(DEEP_SPACE_EPHEM_FLAG);
}

void SGP4(double tsince, tle_t *tle, vector_t *pos, vector_t *vel) {
    /* This function is used to calculate the position and velocity */
    /* of near-earth (period < 225 minutes) satellites. tsince is   */
    /* time since epoch in minutes, tle is a pointer to a tle_t     */
    /* structure with Keplerian orbital elements and pos and vel    */
    /* are vector_t structures returning ECI satellite position and */
    /* velocity. Use Convert_Sat_State() to convert to km and km/s. */

    static double aodp, aycof, c1, c4, c5, cosio, d2, d3, d4, delmo, omgcof, eta, omgdot, sinio, xnodp, sinmo, t2cof,
        t3cof, t4cof, t5cof, x1mth2, x3thm1, x7thm1, xmcof, xmdot, xnodcf, xnodot, xlcof;

    double cosuk, sinuk, rfdotk, vx, vy, vz, ux, uy, uz, xmy, xmx, cosnok, sinnok, cosik, sinik, rdotk, xinck, xnodek,
        uk, rk, cos2u, sin2u, u, sinu, cosu, betal, rfdot, rdot, r, pl, elsq, esine, ecose, epw, cosepw, x1m5th, xhdot1,
        tfour, sinepw, capu, ayn, xlt, aynl, xll, axn, xn, beta, xl, e, a, tcube, delm, delomg, templ, tempe, tempa,
        xnode, tsq, xmp, omega, xnoddf, omgadf, xmdf, a1, a3ovk2, ao, betao, betao2, c1sq, c2, c3, coef, coef1, del1,
        delo, eeta, eosq, etasq, perigee, pinvsq, psisq, qoms24, s4, temp, temp1, temp2, temp3, temp4, temp5, temp6,
        theta2, theta4, tsi;

    int i;

    /* Initialization */

    if (isFlagClear(SGP4_INITIALIZED_FLAG)) {
        SetFlag(SGP4_INITIALIZED_FLAG);

        /* Recover original mean motion (xnodp) and   */
        /* semimajor axis (aodp) from input elements. */

        a1 = pow(xke / tle->xno, tothrd);
        cosio = cos(tle->xincl);
        theta2 = cosio * cosio;
        x3thm1 = 3 * theta2 - 1.0;
        eosq = tle->eo * tle->eo;
        betao2 = 1.0 - eosq;
        betao = sqrt(betao2);
        del1 = 1.5 * ck2 * x3thm1 / (a1 * a1 * betao * betao2);
        ao = a1 * (1.0 - del1 * (0.5 * tothrd + del1 * (1.0 + 134.0 / 81.0 * del1)));
        delo = 1.5 * ck2 * x3thm1 / (ao * ao * betao * betao2);
        xnodp = tle->xno / (1.0 + delo);
        aodp = ao / (1.0 - delo);

        /* For perigee less than 220 kilometers, the "simple"     */
        /* flag is set and the equations are truncated to linear  */
        /* variation in sqrt a and quadratic variation in mean    */
        /* anomaly.  Also, the c3 term, the delta omega term, and */
        /* the delta m term are dropped.                          */

        if ((aodp * (1 - tle->eo) / ae) < (220 / xkmper + ae))
            SetFlag(SIMPLE_FLAG);

        else
            ClearFlag(SIMPLE_FLAG);

        /* For perigees below 156 km, the      */
        /* values of s and qoms2t are altered. */

        s4 = s;
        qoms24 = qoms2t;
        perigee = (aodp * (1 - tle->eo) - ae) * xkmper;

        if (perigee < 156.0) {
            if (perigee <= 98.0)
                s4 = 20;
            else
                s4 = perigee - 78.0;

            qoms24 = pow((120 - s4) * ae / xkmper, 4);
            s4 = s4 / xkmper + ae;
        }

        pinvsq = 1 / (aodp * aodp * betao2 * betao2);
        tsi = 1 / (aodp - s4);
        eta = aodp * tle->eo * tsi;
        etasq = eta * eta;
        eeta = tle->eo * eta;
        psisq = fabs(1 - etasq);
        coef = qoms24 * pow(tsi, 4);
        coef1 = coef / pow(psisq, 3.5);
        c2 = coef1 * xnodp *
             (aodp * (1 + 1.5 * etasq + eeta * (4 + etasq)) +
                 0.75 * ck2 * tsi / psisq * x3thm1 * (8 + 3 * etasq * (8 + etasq)));
        c1 = tle->bstar * c2;
        sinio = sin(tle->xincl);
        a3ovk2 = -xj3 / ck2 * pow(ae, 3);
        c3 = coef * tsi * a3ovk2 * xnodp * ae * sinio / tle->eo;
        x1mth2 = 1 - theta2;

        c4 = 2 * xnodp * coef1 * aodp * betao2 *
             (eta * (2 + 0.5 * etasq) + tle->eo * (0.5 + 2 * etasq) -
                 2 * ck2 * tsi / (aodp * psisq) *
                     (-3 * x3thm1 * (1 - 2 * eeta + etasq * (1.5 - 0.5 * eeta)) +
                         0.75 * x1mth2 * (2 * etasq - eeta * (1 + etasq)) * cos(2 * tle->omegao)));
        c5 = 2 * coef1 * aodp * betao2 * (1 + 2.75 * (etasq + eeta) + eeta * etasq);

        theta4 = theta2 * theta2;
        temp1 = 3 * ck2 * pinvsq * xnodp;
        temp2 = temp1 * ck2 * pinvsq;
        temp3 = 1.25 * ck4 * pinvsq * pinvsq * xnodp;
        xmdot = xnodp + 0.5 * temp1 * betao * x3thm1 + 0.0625 * temp2 * betao * (13 - 78 * theta2 + 137 * theta4);
        x1m5th = 1 - 5 * theta2;
        omgdot = -0.5 * temp1 * x1m5th + 0.0625 * temp2 * (7 - 114 * theta2 + 395 * theta4) +
                 temp3 * (3 - 36 * theta2 + 49 * theta4);
        xhdot1 = -temp1 * cosio;
        xnodot = xhdot1 + (0.5 * temp2 * (4 - 19 * theta2) + 2 * temp3 * (3 - 7 * theta2)) * cosio;
        omgcof = tle->bstar * c3 * cos(tle->omegao);
        xmcof = -tothrd * coef * tle->bstar * ae / eeta;
        xnodcf = 3.5 * betao2 * xhdot1 * c1;
        t2cof = 1.5 * c1;
        xlcof = 0.125 * a3ovk2 * sinio * (3 + 5 * cosio) / (1 + cosio);
        aycof = 0.25 * a3ovk2 * sinio;
        delmo = pow(1 + eta * cos(tle->xmo), 3);
        sinmo = sin(tle->xmo);
        x7thm1 = 7 * theta2 - 1;

        if (isFlagClear(SIMPLE_FLAG)) {
            c1sq = c1 * c1;
            d2 = 4 * aodp * tsi * c1sq;
            temp = d2 * tsi * c1 / 3;
            d3 = (17 * aodp + s4) * temp;
            d4 = 0.5 * temp * aodp * tsi * (221 * aodp + 31 * s4) * c1;
            t3cof = d2 + 2 * c1sq;
            t4cof = 0.25 * (3 * d3 + c1 * (12 * d2 + 10 * c1sq));
            t5cof = 0.2 * (3 * d4 + 12 * c1 * d3 + 6 * d2 * d2 + 15 * c1sq * (2 * d2 + c1sq));
        }
    }

    /* Update for secular gravity and atmospheric drag. */
    xmdf = tle->xmo + xmdot * tsince;
    omgadf = tle->omegao + omgdot * tsince;
    xnoddf = tle->xnodeo + xnodot * tsince;
    omega = omgadf;
    xmp = xmdf;
    tsq = tsince * tsince;
    xnode = xnoddf + xnodcf * tsq;
    tempa = 1 - c1 * tsince;
    tempe = tle->bstar * c4 * tsince;
    templ = t2cof * tsq;

    if (isFlagClear(SIMPLE_FLAG)) {
        delomg = omgcof * tsince;
        delm = xmcof * (pow(1 + eta * cos(xmdf), 3) - delmo);
        temp = delomg + delm;
        xmp = xmdf + temp;
        omega = omgadf - temp;
        tcube = tsq * tsince;
        tfour = tsince * tcube;
        tempa = tempa - d2 * tsq - d3 * tcube - d4 * tfour;
        tempe = tempe + tle->bstar * c5 * (sin(xmp) - sinmo);
        templ = templ + t3cof * tcube + tfour * (t4cof + tsince * t5cof);
    }

    a = aodp * pow(tempa, 2);
    e = tle->eo - tempe;
    xl = xmp + omega + xnode + xnodp * templ;
    beta = sqrt(1 - e * e);
    xn = xke / pow(a, 1.5);

    /* Long period periodics */
    axn = e * cos(omega);
    temp = 1 / (a * beta * beta);
    xll = temp * xlcof * axn;
    aynl = temp * aycof;
    xlt = xl + xll;
    ayn = e * sin(omega) + aynl;

    /* Solve Kepler's Equation */
    capu = FMod2p(xlt - xnode);
    temp2 = capu;
    i = 0;

    do {
        sinepw = sin(temp2);
        cosepw = cos(temp2);
        temp3 = axn * sinepw;
        temp4 = ayn * cosepw;
        temp5 = axn * cosepw;
        temp6 = ayn * sinepw;
        epw = (capu - temp4 + temp3 - temp2) / (1 - temp5 - temp6) + temp2;

        if (fabs(epw - temp2) <= e6a)
            break;

        temp2 = epw;

    } while (i++ < 10);

    /* Short period preliminary quantities */
    ecose = temp5 + temp6;
    esine = temp3 - temp4;
    elsq = axn * axn + ayn * ayn;
    temp = 1 - elsq;
    pl = a * temp;
    r = a * (1 - ecose);
    temp1 = 1 / r;
    rdot = xke * sqrt(a) * esine * temp1;
    rfdot = xke * sqrt(pl) * temp1;
    temp2 = a * temp1;
    betal = sqrt(temp);
    temp3 = 1 / (1 + betal);
    cosu = temp2 * (cosepw - axn + ayn * esine * temp3);
    sinu = temp2 * (sinepw - ayn - axn * esine * temp3);
    u = AcTan(sinu, cosu);
    sin2u = 2 * sinu * cosu;
    cos2u = 2 * cosu * cosu - 1;
    temp = 1 / pl;
    temp1 = ck2 * temp;
    temp2 = temp1 * temp;

    /* Update for short periodics */
    rk = r * (1 - 1.5 * temp2 * betal * x3thm1) + 0.5 * temp1 * x1mth2 * cos2u;
    uk = u - 0.25 * temp2 * x7thm1 * sin2u;
    xnodek = xnode + 1.5 * temp2 * cosio * sin2u;
    xinck = tle->xincl + 1.5 * temp2 * cosio * sinio * cos2u;
    rdotk = rdot - xn * temp1 * x1mth2 * sin2u;
    rfdotk = rfdot + xn * temp1 * (x1mth2 * cos2u + 1.5 * x3thm1);

    /* Orientation vectors */
    sinuk = sin(uk);
    cosuk = cos(uk);
    sinik = sin(xinck);
    cosik = cos(xinck);
    sinnok = sin(xnodek);
    cosnok = cos(xnodek);
    xmx = -sinnok * cosik;
    xmy = cosnok * cosik;
    ux = xmx * sinuk + cosnok * cosuk;
    uy = xmy * sinuk + sinnok * cosuk;
    uz = sinik * sinuk;
    vx = xmx * cosuk - cosnok * sinuk;
    vy = xmy * cosuk - sinnok * sinuk;
    vz = sinik * cosuk;

    /* Position and velocity */
    pos->x = rk * ux;
    pos->y = rk * uy;
    pos->z = rk * uz;
    vel->x = rdotk * ux + rfdotk * vx;
    vel->y = rdotk * uy + rfdotk * vy;
    vel->z = rdotk * uz + rfdotk * vz;

    /* Phase in radians */
    phase = xlt - xnode - omgadf + twopi;

    if (phase < 0.0)
        phase += twopi;

    phase = FMod2p(phase);
}

void Deep(int ientry, tle_t *tle, deep_arg_t *deep_arg) {
    /* This function is used by SDP4 to add lunar and solar */
    /* perturbation effects to deep-space orbit objects.    */

    static double thgr, xnq, xqncl, omegaq, zmol, zmos, savtsn, ee2, e3, xi2, xl2, xl3, xl4, xgh2, xgh3, xgh4, xh2, xh3,
        sse, ssi, ssg, xi3, se2, si2, sl2, sgh2, sh2, se3, si3, sl3, sgh3, sh3, sl4, sgh4, ssl, ssh, d3210, d3222,
        d4410, d4422, d5220, d5232, d5421, d5433, del1, del2, del3, fasx2, fasx4, fasx6, xlamo, xfact, xni, atime,
        stepp, stepn, step2, preep, pl, sghs, xli, d2201, d2211, sghl, sh1, pinc, pe, shs, zsingl, zcosgl, zsinhl,
        zcoshl, zsinil, zcosil;

    double a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, ainv2, alfdp, aqnv, sgh, sini2, sinis, sinok, sh, si, sil, day,
        betdp, dalf, bfact, c, cc, cosis, cosok, cosq, ctem, f322, zx, zy, dbet, dls, eoc, eq, f2, f220, f221, f3, f311,
        f321, xnoh, f330, f441, f442, f522, f523, f542, f543, g200, g201, g211, pgh, ph, s1, s2, s3, s4, s5, s6, s7, se,
        sel, ses, xls, g300, g310, g322, g410, g422, g520, g521, g532, g533, gam, sinq, sinzf, sis, sl, sll, sls, stem,
        temp, temp1, x1, x2, x2li, x2omi, x3, x4, x5, x6, x7, x8, xl, xldot, xmao, xnddt, xndot, xno2, xnodce, xnoi,
        xomi, xpidot, z1, z11, z12, z13, z2, z21, z22, z23, z3, z31, z32, z33, ze, zf, zm, zn, zsing, zsinh, zsini,
        zcosg, zcosh, zcosi, delt = 0, ft = 0;

    switch (ientry) {
    case dpinit: /* Entrance for deep space initialization */
        thgr = ThetaG(tle->epoch, deep_arg);
        eq = tle->eo;
        xnq = deep_arg->xnodp;
        aqnv = 1 / deep_arg->aodp;
        xqncl = tle->xincl;
        xmao = tle->xmo;
        xpidot = deep_arg->omgdot + deep_arg->xnodot;
        sinq = sin(tle->xnodeo);
        cosq = cos(tle->xnodeo);
        omegaq = tle->omegao;

        /* Initialize lunar solar terms */
        day = deep_arg->ds50 + 18261.5; /* Days since 1900 Jan 0.5 */

        if (day != preep) {
            preep = day;
            xnodce = 4.5236020 - 9.2422029E-4 * day;
            stem = sin(xnodce);
            ctem = cos(xnodce);
            zcosil = 0.91375164 - 0.03568096 * ctem;
            zsinil = sqrt(1 - zcosil * zcosil);
            zsinhl = 0.089683511 * stem / zsinil;
            zcoshl = sqrt(1 - zsinhl * zsinhl);
            c = 4.7199672 + 0.22997150 * day;
            gam = 5.8351514 + 0.0019443680 * day;
            zmol = FMod2p(c - gam);
            zx = 0.39785416 * stem / zsinil;
            zy = zcoshl * ctem + 0.91744867 * zsinhl * stem;
            zx = AcTan(zx, zy);
            zx = gam + zx - xnodce;
            zcosgl = cos(zx);
            zsingl = sin(zx);
            zmos = 6.2565837 + 0.017201977 * day;
            zmos = FMod2p(zmos);
        }

        /* Do solar terms */
        savtsn = 1E20;
        zcosg = zcosgs;
        zsing = zsings;
        zcosi = zcosis;
        zsini = zsinis;
        zcosh = cosq;
        zsinh = sinq;
        cc = c1ss;
        zn = zns;
        ze = zes;
        xnoi = 1 / xnq;

        /* Loop breaks when Solar terms are done a second */
        /* time, after Lunar terms are initialized        */

        for (;;) {
            /* Solar terms done again after Lunar terms are done */
            a1 = zcosg * zcosh + zsing * zcosi * zsinh;
            a3 = -zsing * zcosh + zcosg * zcosi * zsinh;
            a7 = -zcosg * zsinh + zsing * zcosi * zcosh;
            a8 = zsing * zsini;
            a9 = zsing * zsinh + zcosg * zcosi * zcosh;
            a10 = zcosg * zsini;
            a2 = deep_arg->cosio * a7 + deep_arg->sinio * a8;
            a4 = deep_arg->cosio * a9 + deep_arg->sinio * a10;
            a5 = -deep_arg->sinio * a7 + deep_arg->cosio * a8;
            a6 = -deep_arg->sinio * a9 + deep_arg->cosio * a10;
            x1 = a1 * deep_arg->cosg + a2 * deep_arg->sing;
            x2 = a3 * deep_arg->cosg + a4 * deep_arg->sing;
            x3 = -a1 * deep_arg->sing + a2 * deep_arg->cosg;
            x4 = -a3 * deep_arg->sing + a4 * deep_arg->cosg;
            x5 = a5 * deep_arg->sing;
            x6 = a6 * deep_arg->sing;
            x7 = a5 * deep_arg->cosg;
            x8 = a6 * deep_arg->cosg;
            z31 = 12 * x1 * x1 - 3 * x3 * x3;
            z32 = 24 * x1 * x2 - 6 * x3 * x4;
            z33 = 12 * x2 * x2 - 3 * x4 * x4;
            z1 = 3 * (a1 * a1 + a2 * a2) + z31 * deep_arg->eosq;
            z2 = 6 * (a1 * a3 + a2 * a4) + z32 * deep_arg->eosq;
            z3 = 3 * (a3 * a3 + a4 * a4) + z33 * deep_arg->eosq;
            z11 = -6 * a1 * a5 + deep_arg->eosq * (-24 * x1 * x7 - 6 * x3 * x5);
            z12 = -6 * (a1 * a6 + a3 * a5) + deep_arg->eosq * (-24 * (x2 * x7 + x1 * x8) - 6 * (x3 * x6 + x4 * x5));
            z13 = -6 * a3 * a6 + deep_arg->eosq * (-24 * x2 * x8 - 6 * x4 * x6);
            z21 = 6 * a2 * a5 + deep_arg->eosq * (24 * x1 * x5 - 6 * x3 * x7);
            z22 = 6 * (a4 * a5 + a2 * a6) + deep_arg->eosq * (24 * (x2 * x5 + x1 * x6) - 6 * (x4 * x7 + x3 * x8));
            z23 = 6 * a4 * a6 + deep_arg->eosq * (24 * x2 * x6 - 6 * x4 * x8);
            z1 = z1 + z1 + deep_arg->betao2 * z31;
            z2 = z2 + z2 + deep_arg->betao2 * z32;
            z3 = z3 + z3 + deep_arg->betao2 * z33;
            s3 = cc * xnoi;
            s2 = -0.5 * s3 / deep_arg->betao;
            s4 = s3 * deep_arg->betao;
            s1 = -15 * eq * s4;
            s5 = x1 * x3 + x2 * x4;
            s6 = x2 * x3 + x1 * x4;
            s7 = x2 * x4 - x1 * x3;
            se = s1 * zn * s5;
            si = s2 * zn * (z11 + z13);
            sl = -zn * s3 * (z1 + z3 - 14 - 6 * deep_arg->eosq);
            sgh = s4 * zn * (z31 + z33 - 6);
            sh = -zn * s2 * (z21 + z23);

            if (xqncl < 5.2359877E-2)
                sh = 0;

            ee2 = 2 * s1 * s6;
            e3 = 2 * s1 * s7;
            xi2 = 2 * s2 * z12;
            xi3 = 2 * s2 * (z13 - z11);
            xl2 = -2 * s3 * z2;
            xl3 = -2 * s3 * (z3 - z1);
            xl4 = -2 * s3 * (-21 - 9 * deep_arg->eosq) * ze;
            xgh2 = 2 * s4 * z32;
            xgh3 = 2 * s4 * (z33 - z31);
            xgh4 = -18 * s4 * ze;
            xh2 = -2 * s2 * z22;
            xh3 = -2 * s2 * (z23 - z21);

            if (isFlagSet(LUNAR_TERMS_DONE_FLAG))
                break;

            /* Do lunar terms */
            sse = se;
            ssi = si;
            ssl = sl;
            ssh = sh / deep_arg->sinio;
            ssg = sgh - deep_arg->cosio * ssh;
            se2 = ee2;
            si2 = xi2;
            sl2 = xl2;
            sgh2 = xgh2;
            sh2 = xh2;
            se3 = e3;
            si3 = xi3;
            sl3 = xl3;
            sgh3 = xgh3;
            sh3 = xh3;
            sl4 = xl4;
            sgh4 = xgh4;
            zcosg = zcosgl;
            zsing = zsingl;
            zcosi = zcosil;
            zsini = zsinil;
            zcosh = zcoshl * cosq + zsinhl * sinq;
            zsinh = sinq * zcoshl - cosq * zsinhl;
            zn = znl;
            cc = c1l;
            ze = zel;
            SetFlag(LUNAR_TERMS_DONE_FLAG);
        }

        sse = sse + se;
        ssi = ssi + si;
        ssl = ssl + sl;
        ssg = ssg + sgh - deep_arg->cosio / deep_arg->sinio * sh;
        ssh = ssh + sh / deep_arg->sinio;

        /* Geopotential resonance initialization for 12 hour orbits */
        ClearFlag(RESONANCE_FLAG);
        ClearFlag(SYNCHRONOUS_FLAG);

        if (!((xnq < 0.0052359877) && (xnq > 0.0034906585))) {
            if ((xnq < 0.00826) || (xnq > 0.00924))
                return;

            if (eq < 0.5)
                return;

            SetFlag(RESONANCE_FLAG);
            eoc = eq * deep_arg->eosq;
            g201 = -0.306 - (eq - 0.64) * 0.440;

            if (eq <= 0.65) {
                g211 = 3.616 - 13.247 * eq + 16.290 * deep_arg->eosq;
                g310 = -19.302 + 117.390 * eq - 228.419 * deep_arg->eosq + 156.591 * eoc;
                g322 = -18.9068 + 109.7927 * eq - 214.6334 * deep_arg->eosq + 146.5816 * eoc;
                g410 = -41.122 + 242.694 * eq - 471.094 * deep_arg->eosq + 313.953 * eoc;
                g422 = -146.407 + 841.880 * eq - 1629.014 * deep_arg->eosq + 1083.435 * eoc;
                g520 = -532.114 + 3017.977 * eq - 5740 * deep_arg->eosq + 3708.276 * eoc;
            }

            else {
                g211 = -72.099 + 331.819 * eq - 508.738 * deep_arg->eosq + 266.724 * eoc;
                g310 = -346.844 + 1582.851 * eq - 2415.925 * deep_arg->eosq + 1246.113 * eoc;
                g322 = -342.585 + 1554.908 * eq - 2366.899 * deep_arg->eosq + 1215.972 * eoc;
                g410 = -1052.797 + 4758.686 * eq - 7193.992 * deep_arg->eosq + 3651.957 * eoc;
                g422 = -3581.69 + 16178.11 * eq - 24462.77 * deep_arg->eosq + 12422.52 * eoc;

                if (eq <= 0.715)
                    g520 = 1464.74 - 4664.75 * eq + 3763.64 * deep_arg->eosq;

                else
                    g520 = -5149.66 + 29936.92 * eq - 54087.36 * deep_arg->eosq + 31324.56 * eoc;
            }

            if (eq < 0.7) {
                g533 = -919.2277 + 4988.61 * eq - 9064.77 * deep_arg->eosq + 5542.21 * eoc;
                g521 = -822.71072 + 4568.6173 * eq - 8491.4146 * deep_arg->eosq + 5337.524 * eoc;
                g532 = -853.666 + 4690.25 * eq - 8624.77 * deep_arg->eosq + 5341.4 * eoc;
            }

            else {
                g533 = -37995.78 + 161616.52 * eq - 229838.2 * deep_arg->eosq + 109377.94 * eoc;
                g521 = -51752.104 + 218913.95 * eq - 309468.16 * deep_arg->eosq + 146349.42 * eoc;
                g532 = -40023.88 + 170470.89 * eq - 242699.48 * deep_arg->eosq + 115605.82 * eoc;
            }

            sini2 = deep_arg->sinio * deep_arg->sinio;
            f220 = 0.75 * (1 + 2 * deep_arg->cosio + deep_arg->theta2);
            f221 = 1.5 * sini2;
            f321 = 1.875 * deep_arg->sinio * (1 - 2 * deep_arg->cosio - 3 * deep_arg->theta2);
            f322 = -1.875 * deep_arg->sinio * (1 + 2 * deep_arg->cosio - 3 * deep_arg->theta2);
            f441 = 35 * sini2 * f220;
            f442 = 39.3750 * sini2 * sini2;
            f522 = 9.84375 * deep_arg->sinio *
                   (sini2 * (1 - 2 * deep_arg->cosio - 5 * deep_arg->theta2) +
                       0.33333333 * (-2 + 4 * deep_arg->cosio + 6 * deep_arg->theta2));
            f523 = deep_arg->sinio * (4.92187512 * sini2 * (-2 - 4 * deep_arg->cosio + 10 * deep_arg->theta2) +
                                         6.56250012 * (1 + 2 * deep_arg->cosio - 3 * deep_arg->theta2));
            f542 = 29.53125 * deep_arg->sinio *
                   (2 - 8 * deep_arg->cosio + deep_arg->theta2 * (-12 + 8 * deep_arg->cosio + 10 * deep_arg->theta2));
            f543 = 29.53125 * deep_arg->sinio *
                   (-2 - 8 * deep_arg->cosio + deep_arg->theta2 * (12 + 8 * deep_arg->cosio - 10 * deep_arg->theta2));
            xno2 = xnq * xnq;
            ainv2 = aqnv * aqnv;
            temp1 = 3 * xno2 * ainv2;
            temp = temp1 * root22;
            d2201 = temp * f220 * g201;
            d2211 = temp * f221 * g211;
            temp1 = temp1 * aqnv;
            temp = temp1 * root32;
            d3210 = temp * f321 * g310;
            d3222 = temp * f322 * g322;
            temp1 = temp1 * aqnv;
            temp = 2 * temp1 * root44;
            d4410 = temp * f441 * g410;
            d4422 = temp * f442 * g422;
            temp1 = temp1 * aqnv;
            temp = temp1 * root52;
            d5220 = temp * f522 * g520;
            d5232 = temp * f523 * g532;
            temp = 2 * temp1 * root54;
            d5421 = temp * f542 * g521;
            d5433 = temp * f543 * g533;
            xlamo = xmao + tle->xnodeo + tle->xnodeo - thgr - thgr;
            bfact = deep_arg->xmdot + deep_arg->xnodot + deep_arg->xnodot - thdt - thdt;
            bfact = bfact + ssl + ssh + ssh;
        }

        else {
            SetFlag(RESONANCE_FLAG);
            SetFlag(SYNCHRONOUS_FLAG);

            /* Synchronous resonance terms initialization */
            g200 = 1 + deep_arg->eosq * (-2.5 + 0.8125 * deep_arg->eosq);
            g310 = 1 + 2 * deep_arg->eosq;
            g300 = 1 + deep_arg->eosq * (-6 + 6.60937 * deep_arg->eosq);
            f220 = 0.75 * (1 + deep_arg->cosio) * (1 + deep_arg->cosio);
            f311 =
                0.9375 * deep_arg->sinio * deep_arg->sinio * (1 + 3 * deep_arg->cosio) - 0.75 * (1 + deep_arg->cosio);
            f330 = 1 + deep_arg->cosio;
            f330 = 1.875 * f330 * f330 * f330;
            del1 = 3 * xnq * xnq * aqnv * aqnv;
            del2 = 2 * del1 * f220 * g200 * q22;
            del3 = 3 * del1 * f330 * g300 * q33 * aqnv;
            del1 = del1 * f311 * g310 * q31 * aqnv;
            fasx2 = 0.13130908;
            fasx4 = 2.8843198;
            fasx6 = 0.37448087;
            xlamo = xmao + tle->xnodeo + tle->omegao - thgr;
            bfact = deep_arg->xmdot + xpidot - thdt;
            bfact = bfact + ssl + ssg + ssh;
        }

        xfact = bfact - xnq;

        /* Initialize integrator */
        xli = xlamo;
        xni = xnq;
        atime = 0;
        stepp = 720;
        stepn = -720;
        step2 = 259200;

        return;

    case dpsec: /* Entrance for deep space secular effects */
        deep_arg->xll = deep_arg->xll + ssl * deep_arg->t;
        deep_arg->omgadf = deep_arg->omgadf + ssg * deep_arg->t;
        deep_arg->xnode = deep_arg->xnode + ssh * deep_arg->t;
        deep_arg->em = tle->eo + sse * deep_arg->t;
        deep_arg->xinc = tle->xincl + ssi * deep_arg->t;

        if (deep_arg->xinc < 0) {
            deep_arg->xinc = -deep_arg->xinc;
            deep_arg->xnode = deep_arg->xnode + pi;
            deep_arg->omgadf = deep_arg->omgadf - pi;
        }

        if (isFlagClear(RESONANCE_FLAG))
            return;

        do {
            if ((atime == 0) || ((deep_arg->t >= 0) && (atime < 0)) || ((deep_arg->t < 0) && (atime >= 0))) {
                /* Epoch restart */

                if (deep_arg->t >= 0)
                    delt = stepp;
                else
                    delt = stepn;

                atime = 0;
                xni = xnq;
                xli = xlamo;
            }

            else {
                if (fabs(deep_arg->t) >= fabs(atime)) {
                    if (deep_arg->t > 0)
                        delt = stepp;
                    else
                        delt = stepn;
                }
            }

            do {
                if (fabs(deep_arg->t - atime) >= stepp) {
                    SetFlag(DO_LOOP_FLAG);
                    ClearFlag(EPOCH_RESTART_FLAG);
                }

                else {
                    ft = deep_arg->t - atime;
                    ClearFlag(DO_LOOP_FLAG);
                }

                if (fabs(deep_arg->t) < fabs(atime)) {
                    if (deep_arg->t >= 0)
                        delt = stepn;
                    else
                        delt = stepp;

                    SetFlag(DO_LOOP_FLAG | EPOCH_RESTART_FLAG);
                }

                /* Dot terms calculated */
                if (isFlagSet(SYNCHRONOUS_FLAG)) {
                    xndot = del1 * sin(xli - fasx2) + del2 * sin(2 * (xli - fasx4)) + del3 * sin(3 * (xli - fasx6));
                    xnddt =
                        del1 * cos(xli - fasx2) + 2 * del2 * cos(2 * (xli - fasx4)) + 3 * del3 * cos(3 * (xli - fasx6));
                }

                else {
                    xomi = omegaq + deep_arg->omgdot * atime;
                    x2omi = xomi + xomi;
                    x2li = xli + xli;
                    xndot = d2201 * sin(x2omi + xli - g22) + d2211 * sin(xli - g22) + d3210 * sin(xomi + xli - g32) +
                            d3222 * sin(-xomi + xli - g32) + d4410 * sin(x2omi + x2li - g44) + d4422 * sin(x2li - g44) +
                            d5220 * sin(xomi + xli - g52) + d5232 * sin(-xomi + xli - g52) +
                            d5421 * sin(xomi + x2li - g54) + d5433 * sin(-xomi + x2li - g54);
                    xnddt = d2201 * cos(x2omi + xli - g22) + d2211 * cos(xli - g22) + d3210 * cos(xomi + xli - g32) +
                            d3222 * cos(-xomi + xli - g32) + d5220 * cos(xomi + xli - g52) +
                            d5232 * cos(-xomi + xli - g52) +
                            2 * (d4410 * cos(x2omi + x2li - g44) + d4422 * cos(x2li - g44) +
                                    d5421 * cos(xomi + x2li - g54) + d5433 * cos(-xomi + x2li - g54));
                }

                xldot = xni + xfact;
                xnddt = xnddt * xldot;

                if (isFlagSet(DO_LOOP_FLAG)) {
                    xli = xli + xldot * delt + xndot * step2;
                    xni = xni + xndot * delt + xnddt * step2;
                    atime = atime + delt;
                }
            } while (isFlagSet(DO_LOOP_FLAG) && isFlagClear(EPOCH_RESTART_FLAG));
        } while (isFlagSet(DO_LOOP_FLAG) && isFlagSet(EPOCH_RESTART_FLAG));

        deep_arg->xn = xni + xndot * ft + xnddt * ft * ft * 0.5;
        xl = xli + xldot * ft + xndot * ft * ft * 0.5;
        temp = -deep_arg->xnode + thgr + deep_arg->t * thdt;

        if (isFlagClear(SYNCHRONOUS_FLAG))
            deep_arg->xll = xl + temp + temp;
        else
            deep_arg->xll = xl - deep_arg->omgadf + temp;

        return;

    case dpper: /* Entrance for lunar-solar periodics */
        sinis = sin(deep_arg->xinc);
        cosis = cos(deep_arg->xinc);

        if (fabs(savtsn - deep_arg->t) >= 30) {
            savtsn = deep_arg->t;
            zm = zmos + zns * deep_arg->t;
            zf = zm + 2 * zes * sin(zm);
            sinzf = sin(zf);
            f2 = 0.5 * sinzf * sinzf - 0.25;
            f3 = -0.5 * sinzf * cos(zf);
            ses = se2 * f2 + se3 * f3;
            sis = si2 * f2 + si3 * f3;
            sls = sl2 * f2 + sl3 * f3 + sl4 * sinzf;
            sghs = sgh2 * f2 + sgh3 * f3 + sgh4 * sinzf;
            shs = sh2 * f2 + sh3 * f3;
            zm = zmol + znl * deep_arg->t;
            zf = zm + 2 * zel * sin(zm);
            sinzf = sin(zf);
            f2 = 0.5 * sinzf * sinzf - 0.25;
            f3 = -0.5 * sinzf * cos(zf);
            sel = ee2 * f2 + e3 * f3;
            sil = xi2 * f2 + xi3 * f3;
            sll = xl2 * f2 + xl3 * f3 + xl4 * sinzf;
            sghl = xgh2 * f2 + xgh3 * f3 + xgh4 * sinzf;
            sh1 = xh2 * f2 + xh3 * f3;
            pe = ses + sel;
            pinc = sis + sil;
            pl = sls + sll;
        }

        pgh = sghs + sghl;
        ph = shs + sh1;
        deep_arg->xinc = deep_arg->xinc + pinc;
        deep_arg->em = deep_arg->em + pe;

        if (xqncl >= 0.2) {
            /* Apply periodics directly */
            ph = ph / deep_arg->sinio;
            pgh = pgh - deep_arg->cosio * ph;
            deep_arg->omgadf = deep_arg->omgadf + pgh;
            deep_arg->xnode = deep_arg->xnode + ph;
            deep_arg->xll = deep_arg->xll + pl;
        }

        else {
            /* Apply periodics with Lyddane modification */
            sinok = sin(deep_arg->xnode);
            cosok = cos(deep_arg->xnode);
            alfdp = sinis * sinok;
            betdp = sinis * cosok;
            dalf = ph * cosok + pinc * cosis * sinok;
            dbet = -ph * sinok + pinc * cosis * cosok;
            alfdp = alfdp + dalf;
            betdp = betdp + dbet;
            deep_arg->xnode = FMod2p(deep_arg->xnode);
            xls = deep_arg->xll + deep_arg->omgadf + cosis * deep_arg->xnode;
            dls = pl + pgh - pinc * deep_arg->xnode * sinis;
            xls = xls + dls;
            xnoh = deep_arg->xnode;
            deep_arg->xnode = AcTan(alfdp, betdp);

            /* This is a patch to Lyddane modification */
            /* suggested by Rob Matson. */

            if (fabs(xnoh - deep_arg->xnode) > pi) {
                if (deep_arg->xnode < xnoh)
                    deep_arg->xnode += twopi;
                else
                    deep_arg->xnode -= twopi;
            }

            deep_arg->xll = deep_arg->xll + pl;
            deep_arg->omgadf = xls - deep_arg->xll - cos(deep_arg->xinc) * deep_arg->xnode;
        }
        return;
    }
}

void SDP4(double tsince, tle_t *tle, vector_t *pos, vector_t *vel) {
    /* This function is used to calculate the position and velocity */
    /* of deep-space (period > 225 minutes) satellites. tsince is   */
    /* time since epoch in minutes, tle is a pointer to a tle_t     */
    /* structure with Keplerian orbital elements and pos and vel    */
    /* are vector_t structures returning ECI satellite position and */
    /* velocity. Use Convert_Sat_State() to convert to km and km/s. */

    int i;

    static double x3thm1, c1, x1mth2, c4, xnodcf, t2cof, xlcof, aycof, x7thm1;

    double a, axn, ayn, aynl, beta, betal, capu, cos2u, cosepw, cosik, cosnok, cosu, cosuk, ecose, elsq, epw, esine, pl,
        theta4, rdot, rdotk, rfdot, rfdotk, rk, sin2u, sinepw, sinik, sinnok, sinu, sinuk, tempe, templ, tsq, u, uk, ux,
        uy, uz, vx, vy, vz, xinck, xl, xlt, xmam, xmdf, xmx, xmy, xnoddf, xnodek, xll, a1, a3ovk2, ao, c2, coef, coef1,
        x1m5th, xhdot1, del1, r, delo, eeta, eta, etasq, perigee, psisq, tsi, qoms24, s4, pinvsq, temp, tempa, temp1,
        temp2, temp3, temp4, temp5, temp6, bx, by, bz, cx, cy, cz;

    static deep_arg_t deep_arg;

    /* Initialization */

    if (isFlagClear(SDP4_INITIALIZED_FLAG)) {
        SetFlag(SDP4_INITIALIZED_FLAG);

        /* Recover original mean motion (xnodp) and   */
        /* semimajor axis (aodp) from input elements. */

        a1 = pow(xke / tle->xno, tothrd);
        deep_arg.cosio = cos(tle->xincl);
        deep_arg.theta2 = deep_arg.cosio * deep_arg.cosio;
        x3thm1 = 3 * deep_arg.theta2 - 1;
        deep_arg.eosq = tle->eo * tle->eo;
        deep_arg.betao2 = 1 - deep_arg.eosq;
        deep_arg.betao = sqrt(deep_arg.betao2);
        del1 = 1.5 * ck2 * x3thm1 / (a1 * a1 * deep_arg.betao * deep_arg.betao2);
        ao = a1 * (1 - del1 * (0.5 * tothrd + del1 * (1 + 134 / 81 * del1)));
        delo = 1.5 * ck2 * x3thm1 / (ao * ao * deep_arg.betao * deep_arg.betao2);
        deep_arg.xnodp = tle->xno / (1 + delo);
        deep_arg.aodp = ao / (1 - delo);

        /* For perigee below 156 km, the values */
        /* of s and qoms2t are altered.         */

        s4 = s;
        qoms24 = qoms2t;
        perigee = (deep_arg.aodp * (1 - tle->eo) - ae) * xkmper;

        if (perigee < 156.0) {
            if (perigee <= 98.0)
                s4 = 20.0;
            else
                s4 = perigee - 78.0;

            qoms24 = pow((120 - s4) * ae / xkmper, 4);
            s4 = s4 / xkmper + ae;
        }

        pinvsq = 1 / (deep_arg.aodp * deep_arg.aodp * deep_arg.betao2 * deep_arg.betao2);
        deep_arg.sing = sin(tle->omegao);
        deep_arg.cosg = cos(tle->omegao);
        tsi = 1 / (deep_arg.aodp - s4);
        eta = deep_arg.aodp * tle->eo * tsi;
        etasq = eta * eta;
        eeta = tle->eo * eta;
        psisq = fabs(1 - etasq);
        coef = qoms24 * pow(tsi, 4);
        coef1 = coef / pow(psisq, 3.5);
        c2 = coef1 * deep_arg.xnodp *
             (deep_arg.aodp * (1 + 1.5 * etasq + eeta * (4 + etasq)) +
                 0.75 * ck2 * tsi / psisq * x3thm1 * (8 + 3 * etasq * (8 + etasq)));
        c1 = tle->bstar * c2;
        deep_arg.sinio = sin(tle->xincl);
        a3ovk2 = -xj3 / ck2 * pow(ae, 3);
        x1mth2 = 1 - deep_arg.theta2;
        c4 = 2 * deep_arg.xnodp * coef1 * deep_arg.aodp * deep_arg.betao2 *
             (eta * (2 + 0.5 * etasq) + tle->eo * (0.5 + 2 * etasq) -
                 2 * ck2 * tsi / (deep_arg.aodp * psisq) *
                     (-3 * x3thm1 * (1 - 2 * eeta + etasq * (1.5 - 0.5 * eeta)) +
                         0.75 * x1mth2 * (2 * etasq - eeta * (1 + etasq)) * cos(2 * tle->omegao)));
        theta4 = deep_arg.theta2 * deep_arg.theta2;
        temp1 = 3 * ck2 * pinvsq * deep_arg.xnodp;
        temp2 = temp1 * ck2 * pinvsq;
        temp3 = 1.25 * ck4 * pinvsq * pinvsq * deep_arg.xnodp;
        deep_arg.xmdot = deep_arg.xnodp + 0.5 * temp1 * deep_arg.betao * x3thm1 +
                         0.0625 * temp2 * deep_arg.betao * (13 - 78 * deep_arg.theta2 + 137 * theta4);
        x1m5th = 1 - 5 * deep_arg.theta2;
        deep_arg.omgdot = -0.5 * temp1 * x1m5th + 0.0625 * temp2 * (7 - 114 * deep_arg.theta2 + 395 * theta4) +
                          temp3 * (3 - 36 * deep_arg.theta2 + 49 * theta4);
        xhdot1 = -temp1 * deep_arg.cosio;
        deep_arg.xnodot = xhdot1 + (0.5 * temp2 * (4 - 19 * deep_arg.theta2) + 2 * temp3 * (3 - 7 * deep_arg.theta2)) *
                                       deep_arg.cosio;
        xnodcf = 3.5 * deep_arg.betao2 * xhdot1 * c1;
        t2cof = 1.5 * c1;
        xlcof = 0.125 * a3ovk2 * deep_arg.sinio * (3 + 5 * deep_arg.cosio) / (1 + deep_arg.cosio);
        aycof = 0.25 * a3ovk2 * deep_arg.sinio;
        x7thm1 = 7 * deep_arg.theta2 - 1;

        /* initialize Deep() */

        Deep(dpinit, tle, &deep_arg);
    }

    /* Update for secular gravity and atmospheric drag */
    xmdf = tle->xmo + deep_arg.xmdot * tsince;
    deep_arg.omgadf = tle->omegao + deep_arg.omgdot * tsince;
    xnoddf = tle->xnodeo + deep_arg.xnodot * tsince;
    tsq = tsince * tsince;
    deep_arg.xnode = xnoddf + xnodcf * tsq;
    tempa = 1 - c1 * tsince;
    tempe = tle->bstar * c4 * tsince;
    templ = t2cof * tsq;
    deep_arg.xn = deep_arg.xnodp;

    /* Update for deep-space secular effects */
    deep_arg.xll = xmdf;
    deep_arg.t = tsince;

    Deep(dpsec, tle, &deep_arg);

    xmdf = deep_arg.xll;
    a = pow(xke / deep_arg.xn, tothrd) * tempa * tempa;
    deep_arg.em = deep_arg.em - tempe;
    xmam = xmdf + deep_arg.xnodp * templ;

    /* Update for deep-space periodic effects */
    deep_arg.xll = xmam;

    Deep(dpper, tle, &deep_arg);

    xmam = deep_arg.xll;
    xl = xmam + deep_arg.omgadf + deep_arg.xnode;
    beta = sqrt(1 - deep_arg.em * deep_arg.em);
    deep_arg.xn = xke / pow(a, 1.5);

    /* Long period periodics */
    axn = deep_arg.em * cos(deep_arg.omgadf);
    temp = 1 / (a * beta * beta);
    xll = temp * xlcof * axn;
    aynl = temp * aycof;
    xlt = xl + xll;
    ayn = deep_arg.em * sin(deep_arg.omgadf) + aynl;

    /* Solve Kepler's Equation */
    capu = FMod2p(xlt - deep_arg.xnode);
    temp2 = capu;
    i = 0;

    do {
        sinepw = sin(temp2);
        cosepw = cos(temp2);
        temp3 = axn * sinepw;
        temp4 = ayn * cosepw;
        temp5 = axn * cosepw;
        temp6 = ayn * sinepw;
        epw = (capu - temp4 + temp3 - temp2) / (1 - temp5 - temp6) + temp2;

        if (fabs(epw - temp2) <= e6a)
            break;

        temp2 = epw;

    } while (i++ < 10);

    /* Short period preliminary quantities */
    ecose = temp5 + temp6;
    esine = temp3 - temp4;
    elsq = axn * axn + ayn * ayn;
    temp = 1 - elsq;
    pl = a * temp;
    r = a * (1 - ecose);
    temp1 = 1 / r;
    rdot = xke * sqrt(a) * esine * temp1;
    rfdot = xke * sqrt(pl) * temp1;
    temp2 = a * temp1;
    betal = sqrt(temp);
    temp3 = 1 / (1 + betal);
    cosu = temp2 * (cosepw - axn + ayn * esine * temp3);
    sinu = temp2 * (sinepw - ayn - axn * esine * temp3);
    u = AcTan(sinu, cosu);
    sin2u = 2 * sinu * cosu;
    cos2u = 2 * cosu * cosu - 1;
    temp = 1 / pl;
    temp1 = ck2 * temp;
    temp2 = temp1 * temp;

    /* Update for short periodics */
    rk = r * (1 - 1.5 * temp2 * betal * x3thm1) + 0.5 * temp1 * x1mth2 * cos2u;
    uk = u - 0.25 * temp2 * x7thm1 * sin2u;
    xnodek = deep_arg.xnode + 1.5 * temp2 * deep_arg.cosio * sin2u;
    xinck = deep_arg.xinc + 1.5 * temp2 * deep_arg.cosio * deep_arg.sinio * cos2u;
    rdotk = rdot - deep_arg.xn * temp1 * x1mth2 * sin2u;
    rfdotk = rfdot + deep_arg.xn * temp1 * (x1mth2 * cos2u + 1.5 * x3thm1);

    /* Orientation vectors */
    sinuk = sin(uk);
    cosuk = cos(uk);
    sinik = sin(xinck);
    cosik = cos(xinck);
    sinnok = sin(xnodek);
    cosnok = cos(xnodek);
    xmx = -sinnok * cosik;
    xmy = cosnok * cosik;
    ux = xmx * sinuk + cosnok * cosuk;
    uy = xmy * sinuk + sinnok * cosuk;
    uz = sinik * sinuk;
    vx = xmx * cosuk - cosnok * sinuk;
    vy = xmy * cosuk - sinnok * sinuk;
    vz = sinik * cosuk;

    /* Position and velocity */
    pos->x = rk * ux;
    pos->y = rk * uy;
    pos->z = rk * uz;
    vel->x = rdotk * ux + rfdotk * vx;
    vel->y = rdotk * uy + rfdotk * vy;
    vel->z = rdotk * uz + rfdotk * vz;

    /* Calculations for squint angle begin here... */

    if (calc_squint) {
        bx = cos(alat) * cos(alon + deep_arg.omgadf);
        by = cos(alat) * sin(alon + deep_arg.omgadf);
        bz = sin(alat);
        cx = bx;
        cy = by * cos(xinck) - bz * sin(xinck);
        cz = by * sin(xinck) + bz * cos(xinck);
        ax = cx * cos(xnodek) - cy * sin(xnodek);
        ay = cx * sin(xnodek) + cy * cos(xnodek);
        az = cz;
    }

    /* Phase in radians */
    phase = xlt - deep_arg.xnode - deep_arg.omgadf + twopi;

    if (phase < 0.0)
        phase += twopi;

    phase = FMod2p(phase);
}

void Calculate_User_PosVel(double time, geodetic_t *geodetic, vector_t *obs_pos, vector_t *obs_vel) {
    /* Calculate_User_PosVel() passes the user's geodetic position
       and the time of interest and returns the ECI position and
       velocity of the observer.  The velocity calculation assumes
       the geodetic position is stationary relative to the earth's
       surface. */

    /* Reference:  The 1992 Astronomical Almanac, page K11. */

    double c, sq, achcp;

    geodetic->theta = FMod2p(ThetaG_JD(time) + geodetic->lon); /* LMST */
    c = 1 / sqrt(1 + f * (f - 2) * Sqr(sin(geodetic->lat)));
    sq = Sqr(1 - f) * c;
    achcp = (xkmper * c + geodetic->alt) * cos(geodetic->lat);
    obs_pos->x = achcp * cos(geodetic->theta); /* kilometers */
    obs_pos->y = achcp * sin(geodetic->theta);
    obs_pos->z = (xkmper * sq + geodetic->alt) * sin(geodetic->lat);
    obs_vel->x = -mfactor * obs_pos->y; /* kilometers/second */
    obs_vel->y = mfactor * obs_pos->x;
    obs_vel->z = 0;
    Magnitude(obs_pos);
    Magnitude(obs_vel);
}

void Calculate_LatLonAlt(double time, vector_t *pos, geodetic_t *geodetic) {
    /* Procedure Calculate_LatLonAlt will calculate the geodetic  */
    /* position of an object given its ECI position pos and time. */
    /* It is intended to be used to determine the ground track of */
    /* a satellite.  The calculations  assume the earth to be an  */
    /* oblate spheroid as defined in WGS '72.                     */

    /* Reference:  The 1992 Astronomical Almanac, page K12. */

    double r, e2, phi, c;

    geodetic->theta = AcTan(pos->y, pos->x);                   /* radians */
    geodetic->lon = FMod2p(geodetic->theta - ThetaG_JD(time)); /* radians */
    r = sqrt(Sqr(pos->x) + Sqr(pos->y));
    e2 = f * (2 - f);
    geodetic->lat = AcTan(pos->z, r); /* radians */

    do {
        phi = geodetic->lat;
        c = 1 / sqrt(1 - e2 * Sqr(sin(phi)));
        geodetic->lat = AcTan(pos->z + xkmper * c * e2 * sin(phi), r);

    } while (fabs(geodetic->lat - phi) >= 1E-10);

    geodetic->alt = r / cos(geodetic->lat) - xkmper * c; /* kilometers */

    if (geodetic->lat > pio2)
        geodetic->lat -= twopi;
}

void Calculate_Obs(double time, vector_t *pos, vector_t *vel, geodetic_t *geodetic, vector_t *obs_set) {
    /* The procedures Calculate_Obs and Calculate_RADec calculate         */
    /* the *topocentric* coordinates of the object with ECI position,     */
    /* {pos}, and velocity, {vel}, from location {geodetic} at {time}.    */
    /* The {obs_set} returned for Calculate_Obs consists of azimuth,      */
    /* elevation, range, and range rate (in that order) with units of     */
    /* radians, radians, kilometers, and kilometers/second, respectively. */
    /* The WGS '72 geoid is used and the effect of atmospheric refraction */
    /* (under standard temperature and pressure) is incorporated into the */
    /* elevation calculation; the effect of atmospheric refraction on     */
    /* range and range rate has not yet been quantified.                  */

    /* The {obs_set} for Calculate_RADec consists of right ascension and  */
    /* declination (in that order) in radians.  Again, calculations are   */
    /* based on *topocentric* position using the WGS '72 geoid and        */
    /* incorporating atmospheric refraction.                              */

    double sin_lat, cos_lat, sin_theta, cos_theta, el, azim, top_s, top_e, top_z;

    vector_t obs_pos, obs_vel, range, rgvel;

    Calculate_User_PosVel(time, geodetic, &obs_pos, &obs_vel);

    range.x = pos->x - obs_pos.x;
    range.y = pos->y - obs_pos.y;
    range.z = pos->z - obs_pos.z;

    /* Save these values globally for calculating squint angles later... */

    rx = range.x;
    ry = range.y;
    rz = range.z;

    rgvel.x = vel->x - obs_vel.x;
    rgvel.y = vel->y - obs_vel.y;
    rgvel.z = vel->z - obs_vel.z;

    Magnitude(&range);

    sin_lat = sin(geodetic->lat);
    cos_lat = cos(geodetic->lat);
    sin_theta = sin(geodetic->theta);
    cos_theta = cos(geodetic->theta);
    top_s = sin_lat * cos_theta * range.x + sin_lat * sin_theta * range.y - cos_lat * range.z;
    top_e = -sin_theta * range.x + cos_theta * range.y;
    top_z = cos_lat * cos_theta * range.x + cos_lat * sin_theta * range.y + sin_lat * range.z;
    azim = atan(-top_e / top_s); /* Azimuth */

    if (top_s > 0.0)
        azim = azim + pi;

    if (azim < 0.0)
        azim = azim + twopi;

    el = ArcSin(top_z / range.w);
    obs_set->x = azim;    /* Azimuth (radians)   */
    obs_set->y = el;      /* Elevation (radians) */
    obs_set->z = range.w; /* Range (kilometers)  */

    /* Range Rate (kilometers/second) */

    obs_set->w = Dot(&range, &rgvel) / range.w;

    /* Corrections for atmospheric refraction */
    /* Reference:  Astronomical Algorithms by Jean Meeus, pp. 101-104    */
    /* Correction is meaningless when apparent elevation is below horizon */

    /*** The following adjustment for
         atmospheric refraction is bypassed ***/

    /* obs_set->y=obs_set->y+Radians((1.02/tan(Radians(Degrees(el)+10.3/(Degrees(el)+5.11))))/60); */

    obs_set->y = el;

    /**** End bypass ****/

    if (obs_set->y >= 0.0)
        SetFlag(VISIBLE_FLAG);
    else {
        obs_set->y = el; /* Reset to true elevation */
        ClearFlag(VISIBLE_FLAG);
    }
}

/* .... SGP4/SDP4 functions end .... */

char *SubString(char *string, char start, int end) {
    /* This function returns a substring based on the starting
       and ending positions provided.  It is used heavily in
       the AutoUpdate function when parsing 2-line element data. */

    int x, y;

    if (end >= start) {
        for (x = start, y = 0; x <= end && string[x] != 0; x++)
            if (string[x] != ' ') {
                temp[y] = string[x];
                y++;
            }

        temp[y] = 0;
        return temp;
    }
    else
        return NULL;
}

char KepCheck(char *line1, char *line2) {
    /* This function scans line 1 and line 2 of a NASA 2-Line element
       set and returns a 1 if the element set appears to be valid or
       a 0 if it does not.  If the data survives this torture test,
       it's a pretty safe bet we're looking at a valid 2-line
       element set and not just some random text that might pass
       as orbital data based on a simple checksum calculation alone. */

    int x;
    unsigned sum1, sum2;

    /* Compute checksum for each line */

    for (x = 0, sum1 = 0, sum2 = 0; x <= 67; sum1 += val[(int) line1[x]], sum2 += val[(int) line2[x]], x++)
        ;

    /* Perform a "torture test" on the data */

    x = (val[(int) line1[68]] ^ (sum1 % 10)) | (val[(int) line2[68]] ^ (sum2 % 10)) | (line1[0] ^ '1') |
        (line1[1] ^ ' ') | (line1[7] ^ 'U') | (line1[8] ^ ' ') | (line1[17] ^ ' ') | (line1[23] ^ '.') |
        (line1[32] ^ ' ') | (line1[34] ^ '.') | (line1[43] ^ ' ') | (line1[52] ^ ' ') | (line1[61] ^ ' ') |
        (line1[62] ^ '0') | (line1[63] ^ ' ') | (line2[0] ^ '2') | (line2[1] ^ ' ') | (line2[7] ^ ' ') |
        (line2[11] ^ '.') | (line2[16] ^ ' ') | (line2[20] ^ '.') | (line2[25] ^ ' ') | (line2[33] ^ ' ') |
        (line2[37] ^ '.') | (line2[42] ^ ' ') | (line2[46] ^ '.') | (line2[51] ^ ' ') | (line2[54] ^ '.') |
        (line1[2] ^ line2[2]) | (line1[3] ^ line2[3]) | (line1[4] ^ line2[4]) | (line1[5] ^ line2[5]) |
        (line1[6] ^ line2[6]) | (isdigit(line1[68]) ? 0 : 1) | (isdigit(line2[68]) ? 0 : 1) |
        (isdigit(line1[18]) ? 0 : 1) | (isdigit(line1[19]) ? 0 : 1) | (isdigit(line2[31]) ? 0 : 1) |
        (isdigit(line2[32]) ? 0 : 1);

    return (x ? 0 : 1);
}

// Loads data from the satellite's TLE
void InternalUpdate() {
    /* Updates data in TLE structure based on
       line1 and line2 stored in structure. */

    double tempnum;

    strncpy(sat.designator, SubString(sat.line1, 9, 16), 8);
    sat.designator[9] = 0;
    sat.catnum = atol(SubString(sat.line1, 2, 6));
    sat.year = atoi(SubString(sat.line1, 18, 19));
    sat.refepoch = QString(SubString(sat.line1, 20, 31)).toDouble();
    tempnum = 1.0e-5 * QString(SubString(sat.line1, 44, 49)).toDouble();
    sat.nddot6 = tempnum / pow(10.0, (sat.line1[51] - '0'));
    tempnum = 1.0e-5 * QString(SubString(sat.line1, 53, 58)).toDouble();
    sat.bstar = tempnum / pow(10.0, (sat.line1[60] - '0'));
    sat.setnum = atol(SubString(sat.line1, 64, 67));
    sat.incl = QString(SubString(sat.line2, 8, 15)).toDouble();
    sat.raan = QString(SubString(sat.line2, 17, 24)).toDouble();
    sat.eccn = 1.0e-07 * QString(SubString(sat.line2, 26, 32)).toDouble();
    sat.argper = QString(SubString(sat.line2, 34, 41)).toDouble();
    sat.meanan = QString(SubString(sat.line2, 43, 50)).toDouble();
    sat.meanmo = QString(SubString(sat.line2, 52, 62)).toDouble();
    sat.drag = QString(SubString(sat.line1, 33, 42)).toDouble();
    sat.orbitnum = QString(SubString(sat.line2, 63, 67)).toDouble();
}

double ReadBearing(char *input) {
    /* This function takes numeric input in the form of a character
       string, and returns an equivalent bearing in degrees as a
       decimal number (double).  The input may either be expressed
       in decimal format (74.2467) or degree, minute, second
       format (74 14 48).  This function also safely handles
       extra spaces found either leading, trailing, or
       embedded within the numbers expressed in the
       input string.  Decimal seconds are permitted. */

    char string[20];
    double bearing = 0.0, seconds;
    int a, b, length, degrees, minutes;

    /* Copy "input" to "string", and ignore any extra
       spaces that might be present in the process. */

    string[0] = 0;
    length = strlen(input);

    for (a = 0, b = 0; a < length && a < 18; a++) {
        if ((input[a] != 32 && input[a] != '\n') || (input[a] == 32 && input[a + 1] != 32 && b != 0)) {
            string[b] = input[a];
            b++;
        }
    }

    string[b] = 0;

    /* Count number of spaces in the clean string. */

    length = strlen(string);

    for (a = 0, b = 0; a < length; a++)
        if (string[a] == 32)
            b++;

    if (b == 0) /* Decimal Format (74.2467) */
        sscanf(string, "%lf", &bearing);

    if (b == 2) /* Degree, Minute, Second Format (74 14 48) */
    {
        sscanf(string, "%d %d %lf", &degrees, &minutes, &seconds);

        if (degrees < 0.0) {
            minutes = -minutes;
            seconds = -seconds;
        }

        bearing = (double) degrees + ((double) minutes / 60) + (seconds / 3600);
    }

    /* Bizarre results return a 0.0 */

    if (bearing > 360.0 || bearing < -360.0)
        bearing = 0.0;

    return bearing;
}

long DayNum(int m, int d, int y) {
    /* This function calculates the day number from m/d/y. */

    long dn;
    double mm, yy;

    if (m < 3) {
        y--;
        m += 12;
    }

    if (y < 57)
        y += 100;

    yy = (double) y;
    mm = (double) m;
    dn = (long) (floor(365.25 * (yy - 80.0)) - floor(19.0 + yy / 100.0) + floor(4.75 + yy / 400.0) - 16.0);
    dn += d + 30 * m + (long) floor(0.6 * mm - 0.3);
    return dn;
}

double CurrentDaynum() {
    /* Read the system clock and return the number
       of days since 31Dec79 00:00:00 UTC (daynum 0) */

    // 3651: days between 1970.01.01 000000 (utc epoch) and daynum0
    return ((QDateTime::currentMSecsSinceEpoch() / (1000.0 * 86400.0)) - 3651.0);
}

// char *Daynum2String(double daynum)
//{
//    /* This function takes the given epoch as a fractional number of
//       days since 31Dec79 00:00:00 UTC and returns the corresponding
//       date as a string of the form "Tue 12Oct99 17:22:37". */

//    char timestr[26];
//    time_t t;
//    int x;

//    /* Convert daynum to Unix time (seconds since 01-Jan-70) */
//    t=(time_t)(86400.0*(daynum+3651.0));

//    sprintf(timestr,"%s",asctime(gmtime(&t)));

//    if (timestr[8]==' ')
//        timestr[8]='0';

//    for (x=0; x<=3; output[x]=timestr[x], x++);

//    output[4]=timestr[8];
//    output[5]=timestr[9];
//    output[6]=timestr[4];
//    output[7]=timestr[5];
//    output[8]=timestr[6];
//    output[9]=timestr[22];
//    output[10]=timestr[23];
//    output[11]=' ';

//    for (x=12; x<=19; output[x]=timestr[x-1], x++);

//    output[20]=0;
//    return output;
//}

void PreCalc() {
    /* This function copies TLE data from PREDICT's sat structure
       to the SGP4/SDP4's single dimensioned tle structure, and
       prepares the tracking code for the update. */

    strcpy(tle.idesg, sat.designator);
    tle.catnr = sat.catnum;
    tle.epoch = (1000.0 * (double) sat.year) + sat.refepoch;
    tle.xndt2o = sat.drag;
    tle.xndd6o = sat.nddot6;
    tle.bstar = sat.bstar;
    tle.xincl = sat.incl;
    tle.xnodeo = sat.raan;
    tle.eo = sat.eccn;
    tle.omegao = sat.argper;
    tle.xmo = sat.meanan;
    tle.xno = sat.meanmo;
    tle.revnum = sat.orbitnum;

    // our satellite has no squint
    //    if (sat_db.squintflag)
    //    {
    //        calc_squint=1;
    //        alat=deg2rad*sat_db.alat;
    //        alon=deg2rad*sat_db.alon;
    //    }
    //    else
    calc_squint = 0;

    /* Clear all flags */

    ClearFlag(ALL_FLAGS);

    /* Select ephemeris type.  This function will set or clear the
       DEEP_SPACE_EPHEM_FLAG depending on the TLE parameters of the
       satellite.  It will also pre-process tle members for the
       ephemeris functions SGP4 or SDP4, so this function must
       be called each time a new tle set is used. */

    select_ephemeris(&tle);
}

void Calc() {
    /* This is the stuff we need to do repetitively while tracking. */

    /* Zero vector for initializations */
    vector_t zero_vector = {0, 0, 0, 0};

    /* Satellite position and velocity vectors */
    vector_t vel = zero_vector;
    vector_t pos = zero_vector;

    /* Satellite Az, El, Range, Range rate */
    vector_t obs_set;

    /* Solar ECI position vector  */
    vector_t solar_vector = zero_vector;

    /* Solar observed azi and ele vector  */
    vector_t solar_set;

    /* Satellite's predicted geodetic position */
    geodetic_t sat_geodetic;

    jul_utc = daynum + 2444238.5;

    /* Convert satellite's epoch time to Julian  */
    /* and calculate time since epoch in minutes */

    jul_epoch = Julian_Date_of_Epoch(tle.epoch);
    tsince = (jul_utc - jul_epoch) * xmnpda;
    age = jul_utc - jul_epoch;

    /* Copy the ephemeris type in use to ephem string. */

    if (isFlagSet(DEEP_SPACE_EPHEM_FLAG))
        strcpy(ephem, "SDP4");
    else
        strcpy(ephem, "SGP4");

    /* Call NORAD routines according to deep-space flag. */

    if (isFlagSet(DEEP_SPACE_EPHEM_FLAG))
        SDP4(tsince, &tle, &pos, &vel);
    else
        SGP4(tsince, &tle, &pos, &vel);

    /* Scale position and velocity vectors to km and km/sec */

    Convert_Sat_State(&pos, &vel);

    /* Calculate velocity of satellite */

    Magnitude(&vel);
    sat_vel = vel.w;

    /** All angles in rads. Distance in km. Velocity in km/s **/
    /* Calculate satellite Azi, Ele, Range and Range-rate */

    Calculate_Obs(jul_utc, &pos, &vel, &obs_geodetic, &obs_set);

    /* Calculate satellite Lat North, Lon East and Alt. */

    Calculate_LatLonAlt(jul_utc, &pos, &sat_geodetic);

    /* Calculate squint angle */

    if (calc_squint)
        squint = (acos(-(ax * rx + ay * ry + az * rz) / obs_set.z)) / deg2rad;

    /* Calculate solar position and satellite eclipse depth. */
    /* Also set or clear the satellite eclipsed flag accordingly. */

    Calculate_Solar_Position(jul_utc, &solar_vector);
    Calculate_Obs(jul_utc, &solar_vector, &zero_vector, &obs_geodetic, &solar_set);

    if (Sat_Eclipsed(&pos, &solar_vector, &eclipse_depth))
        SetFlag(SAT_ECLIPSED_FLAG);
    else
        ClearFlag(SAT_ECLIPSED_FLAG);

    if (isFlagSet(SAT_ECLIPSED_FLAG))
        sat_sun_status = 0; /* Eclipse */
    else
        sat_sun_status = 1; /* In sunlight */

    /* Convert satellite and solar data */
    sat_azi = Degrees(obs_set.x);
    sat_ele = Degrees(obs_set.y);
    sat_range = obs_set.z;
    sat_range_rate = obs_set.w;
    sat_lat = Degrees(sat_geodetic.lat);
    sat_lon = Degrees(sat_geodetic.lon);
    sat_alt = sat_geodetic.alt;

    fk = 12756.33 * acos(xkmper / (xkmper + sat_alt));
    fm = fk / 1.609344;

    rv = (long) floor((tle.xno * xmnpda / twopi + age * tle.bstar * ae) * age + tle.xmo / twopi) + tle.revnum;

    sun_azi = Degrees(solar_set.x);
    sun_ele = Degrees(solar_set.y);

    irk = (long) rint(sat_range);
    isplat = (int) rint(sat_lat);
    isplong = (int) rint(360.0 - sat_lon);
    iaz = (int) rint(sat_azi);
    iel = (int) rint(sat_ele);
    ma256 = (int) rint(256.0 * (phase / twopi));

    if (sat_sun_status) {
        if (sun_ele <= -12.0 && rint(sat_ele) >= 0.0)
            findsun = '+';
        else
            findsun = '*';
    }
    else
        findsun = ' ';
}

char AosHappens() {
    /* This function returns a 1 if the satellite
       can ever rise above the horizon of the ground station. */

    double lin, sma, apogee;

    if (sat.meanmo == 0.0)
        return 0;
    else {
        lin = sat.incl;

        if (lin >= 90.0)
            lin = 180.0 - lin;

        sma = 331.25 * exp(log(1440.0 / sat.meanmo) * (2.0 / 3.0));
        apogee = sma * (1.0 + sat.eccn) - xkmper;

        if ((acos(xkmper / (apogee + xkmper)) + (lin * deg2rad)) > fabs(qth.stnlat * deg2rad))
            return 1;
        else
            return 0;
    }
}

char Decayed(double time) {
    /* This function returns a 1 if it appears that the
       satellite has decayed at the
       time of 'time'.  If 'time' is 0.0, then the
       current date/time is used. */

    double satepoch;

    if (time == 0.0)
        time = CurrentDaynum();

    satepoch = DayNum(1, 0, sat.year) + sat.refepoch;

    if (satepoch + ((16.666666 - sat.meanmo) / (10.0 * fabs(sat.drag))) < time)
        return 1;
    else
        return 0;
}

char Geostationary() {
    /* This function returns a 1 if the satellite
        appears to be in a geostationary orbit */

    if (fabs(sat.meanmo - 1.0027) < 0.0002)

        return 1;
    else
        return 0;
}

double FindAOS() {
    /* This function finds and returns the time of AOS (aostime). */

    aostime = 0.0;

    if (AosHappens() && Geostationary() == 0 && Decayed(daynum) == 0) {
        Calc();

        /* Get the satellite in range */

        while (sat_ele < -1.0) {
            daynum -= 0.00035 * (sat_ele * ((sat_alt / 8400.0) + 0.46) - 2.0);
            Calc();
        }

        /* Find AOS */

        while (aostime == 0.0) {
            if (fabs(sat_ele) < 0.03)
                aostime = daynum;
            else {
                daynum -= sat_ele * sqrt(sat_alt) / 530000.0;
                Calc();
            }
        }
    }

    return aostime;
}

double FindLOS() {
    lostime = 0.0;

    if (Geostationary() == 0 && AosHappens() == 1 && Decayed(daynum) == 0) {
        Calc();

        do {
            daynum += sat_ele * sqrt(sat_alt) / 502500.0;
            Calc();

            if (fabs(sat_ele) < 0.03)
                lostime = daynum;

        } while (lostime == 0.0);
    }

    return lostime;
}

double FindLOS2() {
    /* This function steps through the pass to find LOS.
       FindLOS() is called to "fine tune" and return the result. */

    do {
        daynum += cos((sat_ele - 1.0) * deg2rad) * sqrt(sat_alt) / 25000.0;
        Calc();

    } while (sat_ele >= 0.0);

    return (FindLOS());
}

double NextAOS() {
    /* This function finds and returns the time of the next
       AOS for a satellite that is currently in range. */

    aostime = 0.0;

    if (AosHappens() && Geostationary() == 0 && Decayed(daynum) == 0)
        daynum = FindLOS2() + 0.014; /* Move to LOS + 20 minutes */

    return (FindAOS());
}

qthStruct createQthStruct(double lat, double lon, int alt) {
    qthStruct ret;
    ret.stnalt = alt;
    ret.stnlat = lat;
    ret.stnlong = lon;
    return ret;
}

void initQth(qthStruct qths) {
    qth.stnalt = qths.stnalt;
    qth.stnlat = qths.stnlat;
    qth.stnlong = qths.stnlong;

    obs_geodetic.lat = qth.stnlat * deg2rad;
    obs_geodetic.lon = -qth.stnlong * deg2rad;
    obs_geodetic.alt = ((double) qth.stnalt) / 1000.0;
    obs_geodetic.theta = 0.0;
}

void initSat(const char line1[70], const char line2[70]) {
    // maybe KepCheck(line1, line2) should be used too
    strncpy(sat.line1, line1, 69);
    strncpy(sat.line2, line2, 69);
    InternalUpdate(); // setting the other values based on line1 and line2
}

int trackInit() {
    PreCalc();
    if (Decayed(0.0) == 1)
        return -1;
    if (AosHappens() != 1)
        return -2;
    return 0;
}

trackData track(unsigned long baseFrequency) {
    trackData temp;
    double downlink = 0.0, uplink = 0.0, doppler100 = 0.0;

    downlink = static_cast<double>(baseFrequency) / 1000000; // our satellite uses baseFrequency in both directions
    uplink = static_cast<double>(baseFrequency) / 1000000;
    (void) uplink;
    (void) downlink;

    daynum = CurrentDaynum();

    Calc(); // Handles the calculations. Uses the variable tle so PreCalc() should have been run before this one

    doppler100 = -100.0e06 * ((sat_range_rate * 1000.0) / 299792458.0);

    temp.azimuth = sat_azi;
    temp.elevation = sat_ele;
    temp.downlink_freq = baseFrequency + doppler100 * (static_cast<double>(baseFrequency) / 100000000);
    temp.uplink_freq = baseFrequency - doppler100 * (static_cast<double>(baseFrequency) / 100000000);
    temp.doppler100 = doppler100;
    temp.AOSTime = AOSQString();
    temp.LOSTime = LOSQString();
    return temp;
}

QList<unsigned int> predictNextPassPath() {
    QList<unsigned int> retList;
    daynum = CurrentDaynum();
    PreCalc();
    Calc();

    if (AosHappens() && Geostationary() == 0 && Decayed(daynum) == 0) {
        /* Make Predictions */
        daynum = FindAOS();

        /* Calculate the path of the pass */

        while (iel >= 0) {
            retList.append(static_cast<unsigned int>(iaz));
            daynum += cos((sat_ele - 1.0) * deg2rad) * sqrt(sat_alt) / 25000.0;
            Calc();
        }
    }
    return retList;
}

long nextAOS() {
    daynum = CurrentDaynum();
    return static_cast<long>(floor(86400.0 * (3651.0 + FindAOS())));
}

QString AOSQString() {
    long time = nextAOS();
    long secondsToGo = time - (static_cast<long>(QDateTime::currentMSecsSinceEpoch() / 1000.0));
    if (secondsToGo < 0)
        return QString("We have signal");
    int seconds = secondsToGo % 60;
    secondsToGo /= 60;
    int minutes = secondsToGo % 60;
    secondsToGo /= 60;
    int hours = secondsToGo;
    return QString("%1:%2:%3")
        .arg(hours, 2, 10, QChar('0'))
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));
}

long nextLOS() {
    daynum = CurrentDaynum();
    return static_cast<long>(floor(86400.0 * (3651.0 + FindLOS())));
}

QString LOSQString() {
    long time = nextLOS();
    long secondsToGo = time - (static_cast<long>(QDateTime::currentMSecsSinceEpoch() / 1000.0));
    if (secondsToGo < 0)
        return QString("We don't have signal");
    int seconds = secondsToGo % 60;
    secondsToGo /= 60;
    int minutes = secondsToGo % 60;
    secondsToGo /= 60;
    int hours = secondsToGo;
    return QString("%1:%2:%3")
        .arg(hours, 2, 10, QChar('0'))
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));
}
