#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

  void Set_Total_Jiffies(long int tj);
  void Set_Active_Jiffies(long int aj);
  void Set_Idle_Jiffies(long int ij);


  // Done
 private:
    long int total_jiffies;
    long int active_jiffies;
    long int idle_jiffies;
};

#endif