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

## Exercise #2: lunix:TNG
We are given the codebase of a Linux device driver for a wireless sensor network, the station/receiver of which is connected to our computer through TTY (Serial) over USB. We must add code to the kernel program to achieve safe and proper driver functionality.

The exercise is performed entirely inside a QEMU Debian virtual machine. The driver is loaded as a module onto the system.


Sensor sets can't possibly be provided to every participant. Instead, a TCP/IP server has been set up at lunix.cslab.ece.ntua.grn to constantly forward sensor measurements to connected clients.
All participants have to do is redirect a virtual serial port to the server using QEMU command line arguments. Two sensor sets are available at ports 49152 and 49153, respectively.

Only the driver code is included in this repository. For the virtual machine files, see below.

### Downloads
Root file system for QEMU virtual machine: [cslab_rootfs_20221102-0.raw.gz](https://newsite.cslab.ece.ntua.gr/assets/oslab/cslab_rootfs_20221102-0.raw.gz)

VM management files: [utopia-20221103-0.tgz](https://helios.ntua.gr/mod/resource/view.php?id=10239)

*Links are not guaranteed to work in the future as they are managed by the lab team.*

Report: Available in [English](./exercise_2/report_en.pdf), [Greek](./exercise_2/report_el.pdf)



School of Electrical and Computer Engineering, National Technical University of Athens  
Winter Semester 2022-2023