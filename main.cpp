#include <iostream>
#include <ctime>
#include <thread>
#include <chrono>
extern "C"
{
    #include <SolTrack.h>
}

int main()
{
    using std::cout;
    using std::endl;

    while(1)
    {
        system("clear");

        time_t now = time(0);   //from ctime lib

        tm *ltm = localtime(&now); //from ctime lib

        Time timeNow //structure from SolTrack library
        { 
            1900 + ltm->tm_year, 
            1 + ltm->tm_mon, 
            ltm->tm_mday, 
            ltm->tm_hour - 1, //time zone correction to UT
            ltm->tm_min, 
            (double)ltm->tm_sec 
        };
        cout << "Time: \n";  
        cout << timeNow.day << '.' << timeNow.month << '.' << timeNow.year << "  " << timeNow.hour + 1 << '.' << timeNow.minute << '.' << timeNow.second << "\n\n";

        Location loc; //structure from SolTrack library
        loc.longitude = 17.062421403208617;
        loc.latitude = 51.11022783585993;    //my location
        loc.pressure = 99.0;   //pressure in kPa
        loc.temperature = 273;  //temp in K

        cout << "My location:\n" << "Longitude: " << loc.longitude << "°  Latitude: " << loc.latitude << "°\n\n";

        Position pos; //structure from SolTrack lib - position of the Sun

        int useDegrees = 1;             // Input (geographic position) and output are in degrees
        int useNorthEqualsZero = 1;     // Azimuth: 0 = South, pi/2 (90deg) = West  ->  0 = North, pi/2 (90deg) = East
        int computeRefrEquatorial = 1;  // Compute refraction-corrected equatorial coordinates (Hour angle, declination): 0-no, 1-yes
        int computeDistance = 1;        // Compute the distance to the Sun in AU: 0-no, 1-yes

        SolTrack(timeNow, loc, &pos, useDegrees, useNorthEqualsZero, computeRefrEquatorial, computeDistance);

        cout << "Postion of the Sun: \n";
        cout << "Altitude: " << pos.altitudeRefract << "°  Azimuth: " << pos.azimuthRefract << "°\n\n";
        
        RiseSet riseSet;
        
        SolTrack_RiseSet(timeNow, loc, &pos, &riseSet, 0.0, useDegrees, useNorthEqualsZero);

        struct HoursMinutes
        {
            double time;
            int hours, minutes;
            std::string wholeTime;
        }Rise, Transit, Set;

        Rise.time = riseSet.riseTime + 1;
        Transit.time = riseSet.transitTime + 1;
        Set.time = riseSet.setTime + 1;

        Rise.hours = (int)Rise.time;
        Rise.minutes = 60 * (Rise.time - Rise.hours);
        Rise.wholeTime = std::to_string(Rise.hours);
        Rise.wholeTime += ":";
        if (Rise.minutes < 10)
            Rise.wholeTime += "0";
        Rise.wholeTime += std::to_string(Rise.minutes);

        Transit.hours = (int)Transit.time;
        Transit.minutes = 60 * (Transit.time - Transit.hours);
        Transit.wholeTime = std::to_string(Transit.hours);
        Transit.wholeTime += ":";
        if (Transit.minutes < 10)
            Transit.wholeTime += "0";
        Transit.wholeTime += std::to_string(Transit.minutes);

        Set.hours = (int)Set.time;
        Set.minutes = 60 * (Set.time - Set.hours);
        Set.wholeTime = std::to_string(Set.hours);
        Set.wholeTime += ":";
        if (Set.minutes < 10)
            Set.wholeTime += "0";
        Set.wholeTime += std::to_string(Set.minutes);
        
        cout << "Rise time: " << Rise.wholeTime << "  Rise azimuth: " << riseSet.riseAzimuth << "°\n\n";
        cout << "Transit time: " << Transit.wholeTime << "  Transit altitude: " << riseSet.transitAltitude << "°\n\n";
        cout << "Set time: " << Set.wholeTime << "  Set azimuth: " << riseSet.setAzimuth << "°\n\n";

        std::chrono::milliseconds timespan(1000);
        std::this_thread::sleep_for(timespan);
    }

    return 0;
}