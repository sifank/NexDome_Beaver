Lunatico NexDome Beaver INDI Driver
=========================================
This package provides the INDI driver for Lunatico NexDome Beaver controller.

NexDome is a classic observatory dome with more than 100 degrees of shutter opening, allowing you to point your telescope to the zenith while remaining shielded from stray light and wind. The 2.2 meter inside diameter provides plenty of space to operate up to a 14” Schmidt Cass or a refractor 1400mm long.

The driver is compatible with Beaver firmware version 1.0 or higher.

INDI NexDome Beaver driver is currently only released in beta form from a separate git repository. It is compatible with libindi >= v1.8.2

INSTALL
=======
- Reference the INSTALL document (above)

FEATURES
========
NexDome is a fully automatic observatory dome control system. Link your dome to a computer for complete automation including telescope slaving and shutter control. It supports the following features:

    Slave dome rotation to your telescope
    Rotation-only and full shutter-and-rotation available
    Dome and Shutter status
    Rotator homing and park
    Motor control (Mmx/min speeds, acceleration and timeouts)
    Direct confirmation of shutter open/closed state
    Slave dome rotation to your telescope
    Failsafe feature for shutter on low battery, disconnection with controller or computer
    Park-before-close option to avoid mechanical interferences
    Manual override controls for shutter and rotation control
    (Not implemented yet) Field-upgradable firmware

How to Use
==========

You can use the driver in any INDI-compatible client (such as KStars or Xephem) - just select 
Beaver from the Dome list

To run the driver from the command line:
$ indiserver indi_beaver # plus other needed drivers (telescope, camera, etc)

You can then connect to the driver from any client, the default port is 7624.
If you're using KStars, the drivers will be automatically listed in KStars' Device Manager.

Before you Start
================
- Shutter controls will not show unless the rotator unit is in communication with the
  shutter unit.
  - This can take up to 20 secs after turning on the shutter
- Under the Slaving tab: you need to set the parameters for your dome:
- ![Slaving Tab](Assets/SlavingTab.png)
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

ISSUES
============
- Reference the Release Notes (above)


