#include "processor.h"

float Processor::Utilization(){
    return active_jiffies / (float)total_jiffies;
}

void Processor::Set_Total_Jiffies(long int tj){
    total_jiffies = tj;
}

void Processor::Set_Active_Jiffies(long int aj){
    active_jiffies = aj;
}

void Processor::Set_Idle_Jiffies(long int ij){
    idle_jiffies = ij;
}