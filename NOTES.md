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


### Kernel Headers

+ Modules are compiled using kernel, not user space, Headers
+ You need the headers that correspond to your running kernel
+ These headers will include kernel configuration choices

#### write simple module

> sudo apt install --reinstall linux-headers-$(uname -r)


> sudo make -C /lib/modules/$(uname -r)/build M=$PWD modules
> sudo insmod simplemodule.ko
> sudo rmmod simplemodule.ko

> dmesg

## Linux Kernel Source Code

### Obtain a kernel source code

#### Official Kernel Source 

+ https://mirrors.edge.kernel.org/pub/linux/kernel/

> wget https://mirrors.edge.kernel.org/pub/linux/kernel/v6.x/linux-6.9.9.tar.xz

> git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git

#### Ubuntu Kernel Source


> cat /etc/lsb-release

> source /etc/lsb-release

> name=$(echo $DISTRIB_CODENAME | tr "[[:upper:]]" "[[:lower:]]" )


> git clone git://git.launchpad.net/~ubuntu-kernel/ubuntu/+source/linux/+git/$name


The Git source may be more than 4GB, it has tags for the different kernel releases, see the Ubuntu Kernel Git guid for more info: https://wiki.ubuntu.com/Kernel/Dev/KernelGitGuide


### The Kernel Makefile

+ make help
+ make menuconfig and make xconfig
+ All configuration choices are stored in .config after you do a configuration step
+ Current kernel config may by in /boot with a name like config-$(uname -r)
+ Other important target are bzImage, modules, modules_install, install, clean

### Version info in Makefile


VERSION = 6
PATCHLEVEL = 8
SUBLEVEL = 4
EXTRAVERSION = 
NAME = Hur xxx


### Make help

#### cleaning targets

+ clean: remove most generated files but keep the config
+ mrproper: remove all generated files, config, and various backup files
+ distclean: mrproper, plus removing editor backup and patch files

#### config targets:

+ config: update current config utilizing a line-oriented program
+ nconfig: update current config utilizing an ncurses menu-based program
+ menuconfig: update current config utilizing a menu-based program
+ xconfig: update current config utilizing a QT-based front end
+ gconfig: update current config utilizing a GTK-based front end

#### Other generic targets:


+ all: build all targets marked with [*]
+ vmlinux*: build the bare kernel
+ modules*: build all modules
+ modules_install: install all modules to INSTALL_MOD_PATH(default:/)

Architecture-specific targets(x86)

+ bzImage*: compressed kernel image(arch/x86/boot/bzImage)
+ install: install kernel using(your)~/bin/installkernell or (distribution) /sbin/installkernel, or install to $(INSTALL_PATH) and run LILO

make by itself or make all will build the targets marked with an asterisk(*)

## Linux Kernel Source Code Documentation


Documentation subdirectory

> grep -rl in Documentation(handy)


Example: devices.txt

> Documentation/admin-guide/devices.txt


DocBooks



> make SPHINXDIRS="scheduler" pdfdocs

> make htmldocs

> make SPHINXDIRS="locking" pdfdocs

pdf in Documentation/output/locking/pdf


#### Searching

> grep -rli sys_read include


#### cscope

> sudo apt install cscope

> make cscope

> cscope -d to launch

move up and down with Tab key

Exit with Ctrl+D

Launches editor, usually vi


search for

copy_to_user




#### Tags


> sudo apt install universal-ctags

> make tags
> vi -t <TAG>

> vim -t sys_read

> ctrl + ] on a symbol
> Ctrl + T to go back
> :tn (tag next)

> du -ms .

> git tag -l Ubuntu-* | wc -l

#### Drivers


net/ethernet

> ip addr # see names of interfaces
> ethtool -i $interface

char


Character drivers include meme.c, the 'memory' driver, which include /dev/null and its siblings

> find  . -name meme.c | grep drivers/char


> cat /dev/null

mem.c -> read_null

> echo hi > /dev/null

mem.c -> write_null


how to find ethernet's driver code in the kernel


> ip addr 
> ethtool -i $interface

get driver: e1000e like this

in /kernel/linux-6.9.9/drivers/net/ethernet dir

> fdfind  e1000e


#### Selected directories

##### include

+ Kernel code does not use files from /usr/include
+ Some files in /usr/include, such as some needed by glibc, are derived from kernel include files ,though


##### fs

Linux has a wide variety of file systems

+ Virtual(proc and sys)
+ On-disk(ext{2,3,4}, btrfs, and xfs)
+ Network(nfs)
+ compatible(ntfs, fat, and hfs)

##### arch


##### security

+ security.c provides the fundamental hooks that SELinux, AppArmor, and other security systems use
+ security.c essentially provides a hook into all system calls so that extra checks can be made
+ The kernel portions of SELinux and AppArmor are also in the security directory

find defination of the two functions

> write_zero
> read_zero




