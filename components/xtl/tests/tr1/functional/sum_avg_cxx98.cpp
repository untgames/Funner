#include "test.h"

void do_sum_avg(int values[], int n, int& sum, float& avg)
{
  sum = 0;
  for (int i = 0; i < n; i++)
    sum += values[i];
  avg = (float)sum / n;
}

int main()
{
    printf ("Results of sum_avg_cxx98_test:\n");

    function<void (int values[], int n, int& sum, float& avg)> sum_avg;
    sum_avg = &do_sum_avg;
    
    int values [] = {1, 2, 3, 4};   
    float avg = 0;
    int sum = 0;
    
    sum_avg (values, 4, sum, avg);
    
    TEST (avg == 2.5);
    TEST (sum == 10);    

    return 0;
}
