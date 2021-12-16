Lunatico NexDome Beaver INDI Driver
=========================================

This package provides the INDI driver for Lunatico NexDome Beaver controller.

Requirements
============

As of this beta release, this has only been compiled on a Raspberry Pi 
running Ubuntu Bionic (V18.04) and on a laptop running the same OS.

You need to install both indi and indi-dev to build this package. The best way
is to install libindi-dev package from the PPA. If you cannot use
the PPA you need to install libindi-dev from your distribution or compile the
indi libraries yourself using instructions from the INDI website. To compile 
the driver you will need also: cmake, libindi-dev, and git (maybe more ...)
	
Installation
============

In a working directory of your choosing:
1) $ git clone https://github.com/sifank/Beaver.git
2) $ cd Beaver
3) $ mkdir build
4) $ cd build
5) $ cmake -DCMAKE_INSTALL_PREFIX=/usr . ../
6) $ make
7) $ sudo make install

Potential Issue
===============
Since this will build 'outside' of the indi-3rdparty structure, you might get
the error: *** No rule to make target '/usr/lib/libindidriver.so'
As long as you have indilib installed, it's on your system, just not under /usr/lib.
To fix:
1) $ locate libindidriver.so
    1) If you don't have locate, it can be installed with: $ sudo apt install locate
    2) Then update it's db: $ sudo updatedb
2) $ cd /usr/lib; ls -ld libindidriver*   # make sure what this link points to does not exist (eg libindidriver.so.1), eg, only one libindidriver
3) $ sudo rm /usr/lib/libindidriver.so    # remove empty link
4) $ sudo ln -s /usr/lib/x86_(replace from locate above)/libindidriver.so  libindidriver.so
   1) (example on my Raspberry Pi):  sudo ln -s /usr/lib/x86_64-linux-gnu/libindidriver.so  libindidriver.so)

That's it - you'll have the Beaver driver listed in the Dome section
... and you can remove the "build" folder.
	
How to Use
==========

You can use the driver in any INDI-compatible client (such as KStars or Xephem) - just select 
Beaver from the Dome list

To run the driver from the command line:

$ indiserver indi_beaver

You can then connect to the driver from any client, the default port is 7624.
If you're using KStars, the drivers will be automatically listed in KStars' Device Manager.

Before you Start
================
- Shutter controls will not show unless the rotator unit is in communication with the
  shutter unit.
- Under the Slaving tab: you need to set the parameters for your dome:
  + [Reference] (https://www.nexdome.com/_files/ugd/8a866a_9cd260bfa6de414aacdc7a9e26b0a607.pdf)
  - Autosync (m): .5 (already set)
  - Radius (m):  2.2 (Nexdome)
  - Shutter width (m):  0.6 (Nexdome)
  - N displacement (m):  ?? (0 if pier/mount directly in center of dome)
  - E displacement (m):  ?? (0 if pier/mount directly in center of dome)
  - Up displacement (m): ?? (height of the ra/dec intersection of your mount above the dome ring)
  - OTA offset (m):  ?? (distance between center of telescope and ra/dec intersection of mount)
- Under the Main tab, click on the 'Measure Home' button to initialize the rotator.  This will
  find and measure the home sensor on the rotator and set all the parameters.
- Under the Main tab, set the home position degrees (Home ...ition).  This tells the software
  home many degrees the home sensor magnet is from the North.

