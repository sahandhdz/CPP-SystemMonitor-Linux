#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h" // added by SHK

using std::string;
using std::to_string;
using std::vector;

Process::Process(int id){
    pid = id;
    util = CpuUtilization();
}

int Process::Pid(){ 
    return pid;
}

float Process::CpuUtilization(){
    long int up_time = LinuxParser::UpTime();
    long int start_time = LinuxParser::UpTime(pid);

    long int total_time = up_time - start_time;

    long int active_time = LinuxParser::ActiveJiffies(pid) / (float)sysconf(_SC_CLK_TCK);

    return active_time/ (float)total_time;
}

string Process::Command() {
    string s = LinuxParser::Command(pid);
    string r = "...";
    if (s.length() <= 40){
        return s;
    }else {
        return s.substr(0,40) + r;
    }
}

string Process::Ram() {
    return LinuxParser::Ram(pid); 
}

string Process::User() {
    return LinuxParser::User(pid);
}

long int Process::UpTime() {
    return LinuxParser::UpTime() - LinuxParser::UpTime(pid);
}

bool Process::operator<(Process const& a) const {
    return util < a.util;
}