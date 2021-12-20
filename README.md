ALERTS
======
Do monitor the Release Notes to see what's going on and determine if you need to clone this again.

Anything major I'll document in this space here.

- Do not use the Sync feature (see Main Tab below)
- Do not use the Dome CW and CCW buttons (see Main Tab below), those are a work in process.
- At present, you will need to use the MSWindow's based [Beaver software utility](https://www.nexdome.com/resources) to:
  - Set IP address if you want to use the network
  - Update the firmware

This document is also a work in process...

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
    (Not implemented yet) Field-upgradable firmware (You will need to use the Window's Beaver configuration utility atm)

How to Use
==========

You can use the driver in any INDI-compatible client (such as KStars or Xephem) - just select 
Beaver from the Dome list

To run the driver from the command line:

$ indiserver indi_beaver # plus other needed drivers (telescope, camera, etc)

or using the indiwebserver.

You can then connect to the driver from any client, the default port is 7624.
If you're using KStars, the drivers will be automatically listed in KStars' Device Manager.

Before you Start
================
- Shutter controls will not show unless the rotator unit is in communication with the shutter unit.
  - This can take up to 20 secs after turning on the shutter
- Under the Slaving tab: you need to set the parameters for your dome:
  - (Reference the Slaving Tab below)
- Initialize the rotator and shutter
  - (Reference both the Rotator and Shutter tabs)

- I placed a shutter battery voltage indicator on the bottom of the Main tab, so that we can be alert to a low battery and take action.

OPERATIONS
==========

Connection Tab
--------------

![Connection Tab](Assets/ConnectionTab.png)

You can connect to the Beaver controller via Serial (USB) or Network

USB:
- Look for Silicon_Labs_CP2102N_USB_to_UART_Bridge_Controller_d88d70e0fd44eb11ad70aa52b003b68c-if00-port0 
  - idVendor=10c4, idProduct=ea60 (execute on linux: lsusb)
- BaudRate 115200

Network:
- I am having a problem connecting to the controller, so this is not tested.  Let me know if this works for you.
- Currently you would have to use the Window's Beaver configuration tool to set the IP address.
- Port should be 10000
- Connection type is UDP

The INDI driver version is listed under Driver Info (that's this software)

Beaver controller's firmware version is listed on the Beaver line.


Main Tab
--------

![Main Tab](Assets/MainTab.png)

Shutter Open/Close will only show up if the Shutter controller is up and communicating with the Rotator controller.

CAUTION: I've caught the Dome CW and CCW buttons crashing Kstars!  This part is a work in progress.
- The work-around is to use the Relative Position field.  Fill in the relative amount to move (+ CW, - CCW) and click Set.

CAUTION: 'Sync' should 'not' be used!  Instead make sure your home offset is set correctly (see Rotator Tab)

Absolute Position will move the dome AZ relative to what you set the Home Offset to, which if set according to the instruction in the Rotator Tab, this will be from true north.

Home is defined as where the rotator controller's index magnet is located.  This can be anywhere convenient (like not by the door, over a bay, etc.).  Setting the Home Offset (Rotator Tab) will correctly index this from north.

Park position can be set anywhere.  This is set via the Rotator Tab.
- Usually this is set to where the shutter charger is located, so the shutter will be charging when parked.
  - Some considerations:  What is the dominent wind direction? (I find that placing the front of the shutter into the wind, it keeps the dryest).  Out of the way from doors, maybe bays, etc.
- Note: clicking on Park will move to the set park position and also prevent any rotator movement/action, until UnPark is clicked.

Dome and Shutter Status fields will display any errors, idle, moving, etc.

The Shutter Volts field displays the current voltage of the shutter battery.  In combination with the Safe Voltage on the Shutter tab, this can trigger the rotator controller to go into safety mode and call for the shutter to close.
- Shutter Volts and Status fields will not show if the shutter controller is off or not in communication.

Options Tab
-----------

![Options Tab](Assets/OptionsTab.png)

You can enable or disable automatic shutter opening and closing when dome goes to into or out of the Park position. After changing these settings, go to the Options tabs and click Save configuration to save the driver settings.

Telescope Policy: Telescope policy can be either set to Ignore Telescope (default) or Telescope Locks. When the policy is set to Ignore Telescope then the dome can park/unpark regardless of the mount parking state. When it is set Telescope locks, this disallows the dome from parking when telescope is unparked, and can lead to damage to hardware if it rains.  

Slaving Tab
-----------

![Slaving Tab](Assets/SlavingTab.png)

You can slave the dome to the mount by setting the required slaving parameters (by convention the units are in meters);

- Radius is for the radius of the dome (typically 2.2 for a Nexdome)
- Shutter width is the aperture of the shutter of the dome in meters (0.6m in current models)
- N displacement is for north-south displacement of the intersection of the RA & DEC axis as measured from the center of the dome. Displacement to north is positive, and to south is negative.
- E displacement is for east-west displacement. Similar as the above, displacement to east are positive, and to west are negative.
- Up displacement is for displacement of the RA/DEC intersection in the vertical axis as measured from the origin of the dome (not the walls). Up is positive, down is negative.
- OTA offset is for the distance of the optical axis to the RA/DEC intersection. In fork mount this is generally 0, but for German like mounts is the distance from mount axis cross to the center line of the telescope. West is positive, east is negative.

After settings the parameters above, go to Options tab and click Save in Configurations so that the parameters are used in future sessions. You can also set the Autosync threshold which is the minimum distance autosync will move the dome. Any motion below this threshold will not be triggered. This is to prevent continuous dome moving during telescope tracking.

+ [See Reference for more infomation on these settings](https://www.nexdome.com/_files/ugd/8a866a_9cd260bfa6de414aacdc7a9e26b0a607.pdf)
  
Rotator Tab
-----------

![Rotator Tab](Assets/RotatorTab.png)

- Click on the 'Measure Home' button to initialize the rotator.  This will
  find and measure the home sensor on the rotator and set all the parameters.
- Set the home position degrees (Home ...ition).  This tells the software
  how many degrees the home sensor magnet is from the North.
  - One method to do this, depending on whether the controller knows where things are:
    - If the controller has some other value for home (other than zero), do a find home and set it to zero
    - Then either with the rotator's button or via the INDI driver, move the dome slit to north
    - Read the current az position from the Main tab of the INDI driver and set the Home Position field on the Rotator tab to this value
- The Shutter Home and Measure Home functions will calculate and set all the max/min/acceleration/timeout settings
  - You should not change these unless you really know what you are doing!
  - If either the rotator or shutter unit is turned off (zero values for the max/min, etc), you should redo the home procedures
  - To reset the rotator parameters back to defaults, click the 'Measure Home' button
- Park position can be set in two ways:
  - Enter the degress in the Park Position field
  - Move the dome to where you want park to be and click on 'Set To Current'
- To Park or unPark the dome or goto Home, see the Main tab

After settings the parameters above, go to Options tab and click Save in Configurations so that the parameters are used in future sessions.

Shutter Tab
-----------

![Shutter Tab](Assets/ShutterTab.png)

- NOTE:  This tab will not show up unless the rotator controller is communicating with the shutter
  - When turning the shutter power on, it can take up to 20 seconds for communication to be established
- Reference the Rotator section above for additional information that applies to the shutter as well
- The 'Find Home' button will exercise the shutter through it's full range from closed to open.  It will then set all these fields appropriately.
  - You really should not change these unless you know what you are doing!
  - To reset shutter parameters back to defaults, click the 'Find Home' button.
- If these values are zero, click on the 'Find Home' button to initialize them
- Safe Voltage is the level at which the rotator will execute it's shutter safety proceedure and close the shutter.
- To open or close the shutter, see the Main Tab.

To enable or disable automatic shutter opening and closing when dome goes to into or out of the Park position, see the Options Tab.

After settings the parameters above, go to Options tab and click Save in Configurations so that the parameters are used in future sessions.

Presets Tab
-----------

![Presets Tab](Assets/PresetsTab.png)

Allows you to set 3 rotator positions for convenient locations of your dome.
- Example, maybe you need a ladder to access the dome or shutter.  One preset could rotate the dome so that's it's more convenient.

  
ISSUES
============
- Reference the Release Notes (above)


