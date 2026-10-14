#include <iostream>
#include <cstdlib>
#include <cmath>
#include <iomanip>
using namespace std;

int unity(float);
int main()
{
   float target1=0.5,target2=90000000;
   int a=target2;
   unity(target1);cout<<endl;
   unity(target2);cout<<endl;
    return 0;
}
int unity(float target)
{
    float log_target=0;
    int log_target2;
    log_target=log10(target);
    log_target=floor(log_target/3);
    log_target+=3;
    log_target2=log_target;
      switch(log_target2)
        {
        case 1:cout<<setprecision(3)<<fixed<<target*pow(10,6)<<"u";break;
        case 2:cout<<setprecision(3)<<fixed<<target*pow(10,3)<<"m";break;
        case 4:cout<<setprecision(3)<<fixed<<target/pow(10,3)<<"k";break;
        case 5:cout<<setprecision(3)<<fixed<<target/pow(10,6)<<"Meg";break;
        case 6:cout<<setprecision(3)<<fixed<<target/pow(10,9)<<"G";break;
        default: cout<<setprecision(3)<<fixed<<target;;
        }

     return log_target;
}

