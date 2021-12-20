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
the driver you will need also: cmake, libindi-dev, and git (maybe more, do let me
what other dependencies you find - I'll update the list)
	
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

Potential Build Issue
=====================
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
5) You can then rerun from the make step onwards

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
  - This can take up to 20 secs after turning on the shutter
- Under the Slaving tab: you need to set the parameters for your dome:
  + [See Reference for more infomation on these settings](https://www.nexdome.com/_files/ugd/8a866a_9cd260bfa6de414aacdc7a9e26b0a607.pdf)
  - Autosync (m): .5 (already set)
  - Radius (m):  2.2 (Nexdome)
  - Shutter width (m):  0.6 (Nexdome)
  - N displacement (m):  ?? (0 if pier/mount directly in center of dome)
  - E displacement (m):  ?? (0 if pier/mount directly in center of dome)
  - Up displacement (m): ?? (height of the ra/dec intersection of your mount above the dome ring)
  - OTA offset (m):  ?? (distance between center of telescope and ra/dec intersection of mount)
- Under the Rotator tab, click on the 'Measure Home' button to initialize the rotator.  This will
  find and measure the home sensor on the rotator and set all the parameters.
- Under the Rotator tab, set the home position degrees (Home ...ition).  This tells the software
  how many degrees the home sensor magnet is from the North.
  - One method to do this, depending on whether the controller knows where things are:
    - If the controller has some other value for home (other than zero), do a find home and set it to zero
    - Either with the controller's buttons or via the INDI driver, move the dome slit to north
    - Read the current az position from the Main tab of the INDI driver and set the Home Position field on the Rotator tab to this value
- *NOTE* In general the Shutter Home and Measure Home functions will calculate and set all the max/min/acceleration/timeout/voltage settings, you should not change these unless you really know what you are doing!
  - If either the rotator or shutter unit is turned off, you may need to redo these home procedures
  - To reset shutter parameters back to defaults, click the 'Shutter Home' button on the Shutter tab
  - To reset the rotator parameters back to defaults, click the 'Measure Home' button on the Rotator tab
- I placed a shutter battery voltage indicator on the bottom of the Main tab, so that we can be alert to a low battery and take action.

KNOWN ISSUES
============
- Can only connect with USB (I'm not able to test the network connection yet - issue with my controller)
- I caught the dome rotating to Park about 10 secs after bringing up the driver.  This only happened once and I'm not able to reproduce.  Please contact me if you experience this.
- The Motion fields (Dome CW and Dome CCW) occasionally have issues and crash all of Kstars.  You can accomplish the same effect as these buttons by entering the number of degrees you want to move in the Relative Position field, using either positive (CW) or negative (CCW) numbers.


