@echo off
train -s 7 -c 0.05 -w1 0.25 training.txt bow.m 

predict -b 1 testing.txt bow.m output.txt
pause
