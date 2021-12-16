Lunatico NexDome Beaver INDI Driver
=========================================

This package provides the INDI driver for Lunatico NexDome Beaver controller.

Requirements
============

You need to install both indi and indi-dev to build this package. The best way
is to install libindi-dev package from the PPA. If you cannot use
the PPA you need to install libindi-dev from your distribution or compile the
indi libraries yourself using instructions from the INDI website. To compile 
the driver you will need also: cmake, libindi-dev, and git (maybe more ...)
	
Installation
============

	See INSTALL.md
	
How to Use
==========

You can use the driver in any INDI-compatible client (such as KStars or Xephem) - just select 
Beaver from the Dome list

To run the driver from the command line:

$ indiserver indi_beaver

You can then connect to the driver from any client, the default port is 7624.
If you're using KStars, the drivers will be automatically listed in KStars' Device Manager.

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
  where the home sensor magnet is from the North.

