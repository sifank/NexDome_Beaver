Version 1.1 20220129
- Released!  PR sent to INDI
- Fixed shutter open on unpark
- Updated xxx.setText to use strings and fixed overrides to be compatible with MAC and Fedora

Version 0.9 20220124
- Fixed shutter set status
- Fixed sendcommand timer loop
- Ignoring get shutter voltage occasional errors
- Changed shutter timeout field to read only (Lunitco does not support setting it)

Version 0.8 20220121
  - Fixed park/unpark/home status
  - Added shutter open/close policy
  - Not adding rain closure - use Watchdog for that, tested and is working
  - KNOWN ISSUES:
    - None yet ...
  - TODO:
    - Add ability to set rotator and shutter parameters
    - Add test if slaving, don't allow moves
    - Test network connections

Version 0.7 20220119
  - Fixed parking and unparking
  - All options on the EKOS dome tab are now working
  - KNOWN ISSUES:
    - Add test if slaving, don't allow moves
    - Dome unpark status not being shown correctly (but is working)
  - TODO:
    - Add shutter open/close on park policy settings
    - Add option to close if raining (via snoop device)
    - Test network connections

Version 0.6 20220118
  - Fixed a number of issues in the handling of the status and reseting to idle
  - Will not offer setting of dome/shutter low level parameters
  - Will not offer factory resets (use autocalibrate instead)
  - KNOWN ISSUES:
    - EKOS dome page: move abs and rel greyed out (use INDI window instead)
    - EKOS dome page: park and unpark greyed out (use INDI window instead)
  - TODO:
    - Implement park/unpark via INDI calls
    - Add shutter open/close on park policy settings
    - Add option to close if raining (via snoop device)
    - Test network connections

Version 0.5 20211224
  - fixed version (split sendCommand in 2 parts, returns double the other returns char*)
  - fixed status commands to return correctly
  - fixed park so that is doesn't set if rotator is at the park az
  - general cleanup
  - KNOWN ISSUES:
    - Rotator/Shutter settings are set to RO until I find working solution
  - TODO:
    - Find a solution for setting rotator/dome parameters
    - Fix and add factory reset for both rotator and dome
    - Test alt dome rel move
    - Test network connection
    - 'Color' buttons and status correctly

Version 0.4 20211222:
  - Default baudrate is now set to 115200
  - Removed shutter timeout - cmd not avail in the cntlr, it's autocalculated by the cntlr now
  - Added flag to prevent updating the rotator and shutter settings until everything in initialized
  - Cleaned up INFO statements, added DEBUG statements in most places
  - KNOWN ISSUES:
    - Setting the Dome and Shutter max/min/accel/safevoltage is disabled for now (reports correct values tho)
    - Cntlr (Beaver) version is not working, reports N/A for now
    - Have not tested network connection, use USB
    - Dome CW and CCW are working now, but I need to test those more
    
Version 0.2, 29211219: 
  - Added driver version to Connection tab, updated config.h.cmake with a Beaver version
  - ISSUES:
    - Can only connect with USB (I'm not able to test the network connection yet - issue with my controller)
    - I caught the dome rotating to Park about 10 secs after bringing up the driver.  This only happened once and I'm not able to reproduce.  Please contact me if you experience this.
    - The Motion fields (Dome CW and Dome CCW) occasionally have issues and crash all of Kstars.  The work-around is to enter the number of degrees you want to move in the "Relative Position" field, using either positive (CW) or negative (CCW) numbers.
    
Version 0.1, 20211218:  Initial release in beta. 
