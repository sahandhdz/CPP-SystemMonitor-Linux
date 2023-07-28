#include <string>

#include "format.h"

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
    int HH = seconds/3600;
    int MM = (seconds-3600*HH)/60;
    int SS = seconds%60;
    string hh = std::to_string(HH);
    string mm = std::to_string(MM);
    string ss = std::to_string(SS);
    hh.insert(0, 2- hh.length(), '0');
    mm.insert(0, 2- mm.length(), '0');
    ss.insert(0, 2- ss.length(), '0');
    return hh + ":" + mm + ":" + ss;
}