# ece-ntua-oslab
Exercise repository for the Operating Systems Lab course.  
This is an optional Computer Systems course in the 7th semester of the undergraduate studies program.  

Only for those who dare! :sunglasses:


## Exercise #1: Riddle
We are given a Linux x86-64 executable which consists of a series of challenges.  
Those challenges require us to modify the execution environment of the file in order to progress.

We are only allowed to use Unix tools like strace, ltrace, strings and OS features like the /proc pseudo-file system.

### Rules
* We may not use the LD_PRELOAD trick to solve the challenges
* We may use a debugger to analyze the executable's behavior, but not to solve the challenges
* We may not modify the binary file

The primary focus of this exercise is for us to better understand Unix system calls and the underlying mechanisms of the operating system.  
The binary was created for academic purposes only and [is in no way malicious](https://www.virustotal.com/gui/file/a913670a0675aa6554bdfee66c7a8f90d5a23ca92e10bb74bb70ffb41f5d1998/).

Report: Available in [English](./exercise_1/report_en.pdf), [Greek](./exercise_1/report_el.pdf)

School of Electrical and Computer Engineering, National Technical University of Athens  
Winter Semester 2022-2023