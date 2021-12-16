Lunatico NexDome Beaver INDI Driver
=========================================

This package provides the INDI driver for Lunatico NexDome Beaver controller.
As of this version, it covers focuser functionality, in one port of the controller. 

Requirements
============

+ INDI >= v0.6 (http://indi.sf.net)

	You need to install both indi and indi-devel to build this package.
	
	
Installation
============

	See INSTALL
	
How to Use
==========

You can use the driver in any INDI-compatible client (such as KStars or Xephem) - just select 
Beaver from the focuser list

To run the driver from the command line:

$ indiserver indi_beaver

You can then connect to the driver from any client, the default port is 7624.
If you're using KStars, the drivers will be automatically listed in KStars' Device Manager.

- Shutter controls will not show unless the rotator unit is in communication with the
  shutter unit.
- Under the Slaving tab: you need to set the parameters for your dome:
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

