- Version 0.4 20211222:
  - Default baudrate is now set to 115200
  - Removed shutter timeout - cmd not avail in the cntlr, it's autocalculated by the cntlr now
  - Added flag to prevent updating the rotator and shutter settings until everything in initialized
  - Cleaned up INFO statements, added DEBUG statements in most places
  - KNOWN ISSUES:
    - Setting the Dome and Shutter max/min/accel/safevoltage is disabled for now (reports correct values tho)
    - Cntlr (Beaver) version is not working, reports N/A for now
    - Have not tested network connection, use USB
    - Dome CW and CCW are working now, but I need to test those more
    
- Version 0.2, 29211219: 
  - Added driver version to Connection tab, updated config.h.cmake with a Beaver version
  - ISSUES:
    - Can only connect with USB (I'm not able to test the network connection yet - issue with my controller)
    - I caught the dome rotating to Park about 10 secs after bringing up the driver.  This only happened once and I'm not able to reproduce.  Please contact me if you experience this.
    - The Motion fields (Dome CW and Dome CCW) occasionally have issues and crash all of Kstars.  The work-around is to enter the number of degrees you want to move in the "Relative Position" field, using either positive (CW) or negative (CCW) numbers.
    
- Version 0.1, 20211218:  Initial release in beta.
