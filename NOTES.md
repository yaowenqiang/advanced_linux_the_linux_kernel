# The Kernel is an API

+ System calls
+ Virtual file system entries
  + proc
  + sys
  + debugfs
+ Device files(system calls)

Kernel image is relatively small
Kernel image is sufficient to boot to user space


## Discovering and controlling hardware

+ lshw and lspic
+ lsusb and lsblk
+ lscpu and lsdev

Commands for hardware control and config

+ hdparm
+ Write(for example, echo) to proc, dev, or sys files
+ inb and outb
+ setpci


> hdparm -I /dev/sda
> lspic | grep -i ethernet

## System Calls

There are about 300 system calls

> include/uapi/asm-generic/unistd.h

> man 2 read

They are called through the standard library(libc)


> strace -c date


Messages from the kernel and /proc

printk

printk() is the kernel's function for code to print messages, it is like printf() in c

dmesg show RAM buffer message from kernel
log file has kernel messages and more
journalctl -k # with -f to follow as new msgs are added


/proc: Process Info

+ Each process has a directory named with its PID
+ It has info on memory, program, files, and lots more
+ There are hundreds of files and directories per process
+ Threads have entries under the directory /task

> cat /proc/cmdline
> dmesg | grep -i boot_image
> grep -i boot_image /var/log
> journalctl -k | grep BOOT_IMAGE
> cat /proc/meminfo


/sys

+ sysfs is for kernel object info
+ in particular, it is for hardware info (for example, PCI device info)
+ has tens of thousands of entries

debugfs, Trace under /sys

+ /sys also has the debugfs filesysm mounted under it : /sys/kernel/debug
+ /sys provides powerful tracing in /sys/kernel/tracing

Module information

+ /sys/module contains information about all of the currently loaded linux kernel loadable modules
+ Whether they were dynamically loaded or statically linked into the kernel image

Device information

+ Information about block and character device files in /sys/dev
+ Information about block devices in /sys/block
+ Classes of devices in /sys/class
  + like /sys/class/net


> strace -e openat lspci 
> strace -e openat -o /tmm/strace.out fdisk -l

Device files

+ Character and block drivers use device files
+ Device files have a major number, minor number ,and type(c or b)
+ The kernel maintains a relationship between the three characteristics and what driver to call
+ The driver can implement different functions for different minor numbers
+ Device files are often created when a driver is loaded

> ls -l /dev/sda, for example

A character driver, for example, can implement open(), read(), write() and ioctl()

A process opens a device file and then can read, write, etc. with the file descriptor: the kernel arranges to have th driver's function called

echo hi > /dev/null would open and then write

ls -l /dev/null
ls -l /dev | grep " 1, "

strace -o strace.out cat /dev/null

## Booting

### GRUB:Role

+ GRUB comes after power-on self-test(POST) and the BIOS
+ GRUB is installed in a special place on disk
+ GRUB loads the kernel and initial root file system, sets up the kernel command line and then transfers control to the kernel

### GRUB:Flexibility

+ GRUB is built with support for file systems: therefore, GRUB can find files, like kernel files, by name
+ GRUB can do file name completion
+ GRUB has lots of utilities (do main -k grub)


/etc/default/grub
/etc/grub.d

Run grub2-mkconfig to generate a new config file

### Kernel parameters

in kernel source tree, it is Documentation/kernel-parameters.txt

> grep "^   [a-zA-Z0-9]" kernel-parameters.txt | wc -l

many are registered with _setup() in source

+ cachesize: used to work around a hardware bug
+ clearcpuid: disables CPU features
+ crashkernel: reserves emory for a crash kernel
+ enforceing: enable SELinux
+ ftrace: starts a tracer early to help debug boot problems
+ init: what to run instead of /sbin/init
+ rdinit: what to run instead of /init in the ramdisk/ramfs

/etc/systemd/system
ps -el

add init=/bin/bash on the grub parameter will login to the system without a password


## Loadable Kernel Modules

What is a loadable Kernel Module(LKM)

+ An object file with a .ko suffix; compressed files have .zst after that
+ Contains code to run in kernel space
+ dynamically adds functionally to the runing kernel
+ Typically written in C and compiled for a particular kernel version - not binary compatible with other kernels

### Advantages

+ Can be a relatively inial main kernel file
+ Adds functionality without rebuilding or rebooting
+ Allows for only the needed functionality to be loaded
+ Live updates
+ Accelerated development

### Kernel Module Installation


+ Modules are installed in a directory under /lib/modules, with each installed kernel version having its own directory
+ The modules are organized in different subdirectories under the kernel version
+ There are also config files

### Module files


+ Each module should have a unique name
+ Module files can be in any directory, but the modprob utility is designed to look only under /lib/modules$(uname -r)
+ only modules built for the kernel version - and how it was configured - should be loaded
+ Modules run in kernel mode with all privileges

```bash

cd /lib/modules/6.8.0-40-generic
find -name "*.zst"  | wc -l
fd -e .zst | wc -l
wc -l modules.dep


```


### Module commands1


+ lsmod
+ rmmod
+ modinfo
+ depmod
+ insmod
+ modprobe



rmod -f may let you remove a module that the kernel think is in use


#### modinfo

+ Module info
+ Author
+ parameters
+ Alias
+ vermagic
+ more


lsmod | wc -l
lsmod | tail
lsmod | grep msdos
modprobe msdos # load mod
lsmod | grep msdos
rmmod mscos
modinfo sunrpc | less
grep garp.ko /lib/modules/$(uname -r)/modules.dep

Number of development modules on a line: echo $line | sed 's/^.*://' | wc -w
Number of used by modules on a line: echo $line | sed 's/[^,]//g' | wc -c


```bash
# mostusedby.sh

lsmod | while
    read line
do
    commas=$(echo $line | sed 's/[^,]//g' | wc -c)
    echo $commas $line
done | sort -n


```


```bash
# largestcount.sh
lsmod | while
    read line
do
    commas=$(echo $line | sed 's/[^,]//g' | wc -c)
    echo $commas $line
done | sort -n
```

```bash
# mostdepend.sh
while
    read line
do
    mods=$(echo $line | sed 's/^.*://' | wc -w)
    echo $mods $line
done < /lib/modules/$(uname -r)/modules.dep | sort -n

```

### Write an LKM

#### Compiling Modules

make -C /lib/modules/$(uanme -r)/build M=$PWD modules


