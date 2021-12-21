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

The driver is only compatible with firmware version 3.1.0 or higher. Please update the firmware for the Azimuth & Shutter controllers before using this driver.

INDI NexDome driver is released as a 3rd party driver in source and binary packages. It is compatible with libindi >= v1.8.2

Under Ubuntu, you can install the driver via:

sudo add-apt-repository ppa:mutlaqja/ppa

To be entered ...


	
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
