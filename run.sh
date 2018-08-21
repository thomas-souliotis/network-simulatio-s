time ./waf --run scratch/task1 > task1.txt
time ./waf --run scratch/task2 > task2.txt
time ./waf --run scratch/task3 > task3.txt
python3 plot.py
