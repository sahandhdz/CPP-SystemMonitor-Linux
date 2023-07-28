#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <unistd.h> // added to system's clock tick value

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

using std::stoi; // changing string object to int

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}



// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, vers, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> vers >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {
  string line, key, value;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  vector<float> utilizationInfo {}; // {MemTotal, MemFree, MemAvailable, Buffers}
  if (filestream.is_open()){
    for (int i=0; i<4; i++){
       std::getline(filestream, line);
       std::istringstream linestream(line);
       linestream >> key >> value;
       utilizationInfo.push_back(stof(value));
    }
    float MemTotal = utilizationInfo[0];
    float MemFree = utilizationInfo[1];
    float MemAvailable = utilizationInfo[2];

    return MemTotal-MemFree/ MemTotal;
   }
   return 0.0;
}


long LinuxParser::UpTime(){
  string line, up_time, idle_time;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> up_time >> idle_time;
    return stoi(up_time);
  }
  return 0; 
}

long LinuxParser::Jiffies() {
  vector<string> CPU_stats = LinuxParser::CpuUtilization();
  long sum_stats = 0;
  for (int i=0; i<10; i++){
    sum_stats += stoi(CPU_stats[i]);
  }
  return sum_stats;
}


long LinuxParser::ActiveJiffies(int pid) {
  string line, val;
  long active_jiff = 0;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int i=1; i<18; i++){
      linestream >> val;
      if (i>=14){
        active_jiff += stoi(val);
      }
    }
    return active_jiff;
  }
  return active_jiff;
}


long LinuxParser::ActiveJiffies(){ 
  vector<string> times = LinuxParser::CpuUtilization();
  vector<int> INDEX = {kUser_, kNice_, kSystem_, kIRQ_, kSoftIRQ_, kSteal_};
  long active_jiffs = 0;
  for (int i : INDEX){
    active_jiffs += stoi(times[i]);
  }

  return active_jiffs;
}

long LinuxParser::IdleJiffies(){
  vector<string> times = LinuxParser::CpuUtilization();
  long idle_jiffs = 0;
  idle_jiffs = stoi(times[CPUStates::kIdle_]);
  idle_jiffs += stoi(times[CPUStates::kIOwait_]);
  return idle_jiffs;
}


vector<string> LinuxParser::CpuUtilization() {
  string line, val;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> val;
    vector<string> CPU_stats = {};
    for (int i=0; i<10; i++){
      linestream >> val;
      CPU_stats.emplace_back(val);
    }
    return CPU_stats;
  }
  return {};
}


int LinuxParser::TotalProcesses() {
  string line, key, val;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()){
    while (std::getline(filestream, line)){
      std::istringstream linestream(line);
      linestream >> key >> val;
      if (key == "processes"){
        return stoi(val);
      }
    }
  }
  return 0; 
}


int LinuxParser::RunningProcesses() {
  string line, key, val;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()){
    while (std::getline(filestream, line)){
      std::istringstream linestream(line);
      linestream >> key >> val;
      if (key == "procs_running"){
        return stoi(val);
      }
    }
  }
  return 0; 
}

string LinuxParser::Command(int pid) {
  string line;
  string comm {};
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> comm;
    return comm;
  }
  return comm; 
}

string LinuxParser::Ram(int pid){
  string line, key;
  string val {};
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()){
    while (std::getline(filestream, line)){
      std::istringstream linestream(line);
      linestream >> key >> val;
      if (key == "VmRSS:"){   // I have used "VmRSS" instead of "VmSize" as recommended by the reviewer
        long v = stoi(val);
        v = v/1024;
        val = std::to_string(v);
        return val;
      }
    }
  }
  return val;
}

string LinuxParser::Uid(int pid) {
  string line, key;
  string val = "Sahand";
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()){
    while (std::getline(filestream, line)){
      std::istringstream linestream(line);
      linestream >> key >> val;
      if (key == "Uid:"){
        return val;
      }
    }
  }
  return val;
}

string LinuxParser::User(int pid) {
  string line, user, x, uid;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()){
    while (std::getline(filestream, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> user >> x >> uid;
      if (uid == LinuxParser::Uid(pid)){
        return user;
      }
    }
  }
  return user;
}

long LinuxParser::UpTime(int pid){ 
  string line, val;
  long uptime;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int i=0; i< 22; i++){
      linestream >> val;
    }
    uptime = stoi(val);
    return uptime/sysconf(_SC_CLK_TCK);
  }
  return 0;
}