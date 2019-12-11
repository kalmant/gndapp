#ifndef PREDICTMOD_H
#define PREDICTMOD_H
#include <QList>
#include <QObject>
#include <QString>
/**
 * @brief Struct for the data of a satellite.
 */
struct satStruct {
    char line1[70];      //!< First line of TLE
    char line2[70];      //!< First line of TLE
    long catnum;         //!< NORAD catalog number
    long setnum;         //!< Element set number
    char designator[10]; //!< International designator
    int year;            //!< Last 2 number of the year, works until 2056
    double refepoch;     //!< Date of the year, whole + fraction
    double incl;         //!< Inclination in degrees
    double raan;         //!< Right ascension of the ascending node (degrees)
    double eccn;         //!< Excentricity (degrees)
    double argper;       //!< Argument of perigee (degrees)
    double meanan;       //!< Mean anomaly (degrees)
    double meanmo;       //!< Mean motion (degrees)
    double drag;         //!< First Time Derivative of the Mean Motion divided by two
    double nddot6;       //!< Second Time Derivative of Mean Motion divided by six
    double bstar;        //!< BSTAR drag term
    long orbitnum;       //!< Revolution number at epoch
};

/**
 * @brief Struct for the data of the station.
 */
struct qthStruct {
    double stnlat;  //!< Station latitude. North is positive, south is negative.
    double stnlong; //!< Station longitude. West is positive, south is negative.
    int stnalt;     //!< Station altitude in metres.
};

/**
 * @brief Struct for the current tracking data of the satellite
 */
struct trackData {
    double azimuth;       //!< Azimuth in degrees
    double elevation;     //!< Elevation in degrees
    double uplink_freq;   //!< Uplink frequency (Hz)
    double downlink_freq; //!< Downlink frequecy (Hz)
    double doppler100;    //!< Doppler freq. at @100Mhz (Hz)
    QString AOSTime;      //!< AOS in HH:mm:ss t format
    QString LOSTime;      //!< HH:mm:ss t format
};

/**
 * @brief Initializes variables used for satellite tracking
 * @param line1 First line of the TLE.
 * @param line2 Second line of the TLE.
 */
void initSat(const char line1[], const char line2[]);

/**
 * @brief Creates and returns a qthStruct with \p lat for latitude, \p az for azimuth and \p alt for altitude.
 * @param lat Latitude of the station.
 * @param lon Longitude of the station.
 * @param alt Altitude of the station.
 * @return The qthStruct object.
 */
qthStruct createQthStruct(double lat, double lon, int alt);

/**
 * @brief Initializes variables used for satellite tracking (observer's position)
 * @param qths The qthStruct containing the data.
 */
void initQth(qthStruct qths);

/**
 * @brief Initializes tracking structures
 * @return 0 if everything is OK, -1 if the satellite appears to have decayed, -2 if the satellite appears to be never
 * visible from the station.
 */
int trackInit();

/**
 * @brief Calculates tracking data for the satellite for this moment (current time is read in the function) with \p
 * baseFrequency base frequency
 * @return Returns the trackData sturcture with the calculated values.
 */
trackData track(unsigned long baseFrequency);

/**
 * @brief Returns a QList<unsigned int> representing the next pass's path: a list of the azimuths.
 * @return The QList<unsigned int> containing the next pass azimuths.
 */
QList<unsigned int> predictNextPassPath();

/**
 * @brief Returns the timestamp(seconds since UTC epoch) for the next(afer this moment) Acquisition Of Signal
 *
 * Should be used when there is no signal currently
 * @return UTC timestamp for the next AOS.
 */
long nextAOS();

/**
 * @brief Returns the timestamp(seconds since UTC epoch) for the next(afer this moment) Loss Of Signal
 *
 * Should be used when there is no signal currently
 * @return UTC timestamp for the next LOS.
 */
long nextLOS();

/**
 * @brief Performs some checking on \p line1 and \p line2 and returns 1 if it appears to be a valid TLE.
 * @param line1 First line of the TLE.
 * @param line2 First line of the TLE.
 * @return
 */
char KepCheck(char *line1, char *line2);

/**
 * @brief Returns the closest AOS in HH:mm:ss or "We have signal"
 * @return A QString in the specified format.
 */
QString AOSQString();

/**
 * @brief Returns the closest LOS in HH:mm:ss or "We don't have signal"
 * @return A QString in the specified format.
 */
QString LOSQString();

#endif // PREDICTMOD_H
