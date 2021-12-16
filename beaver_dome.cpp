/*
    NexDome Beaver Controller

    Copyright (C) 2021 Jasem Mutlaq (mutlaqja@ikarustech.com)
    Modified 2021 Sifan Kahale (sifan.kahale@gmail.com)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*/

#include "beaver_dome.h"

#include "indicom.h"
#include "connectionplugins/connectiontcp.h"

#include <cmath>
#include <cstring>
#include <cassert>
#include <memory>
#include <regex>

#include <termios.h>
#include <unistd.h>
#include "config.h"

static std::unique_ptr<Beaver> dome(new Beaver());

Beaver::Beaver()
{
    setVersion(LUNATICO_VERSION_MAJOR, LUNATICO_VERSION_MINOR);
    // TBD consider implementing CAN_PARK instead of having sepearte tab ... or, consolodate on Site Mgmt tab ...
    SetDomeCapability(DOME_CAN_ABORT |
                      DOME_CAN_ABS_MOVE |
                      DOME_CAN_REL_MOVE |
                      DOME_CAN_PARK |
                      DOME_CAN_SYNC);
    setDomeConnection(CONNECTION_TCP | CONNECTION_SERIAL);
}

bool Beaver::initProperties()
{
    INDI::Dome::initProperties();

    SetParkDataType(PARK_AZ);

    ///////////////////////////////////////////////////////////////////////////////
    /// Main Tab
    ///////////////////////////////////////////////////////////////////////////////
    // Rotator status
    RotatorStatusTP[0].fill("STATUS", "Status", "Idle");
    RotatorStatusTP.fill(getDeviceName(), "ROTATORSTATUS", "Dome", MAIN_CONTROL_TAB, IP_RO, 60, IPS_IDLE);

    // Shutter Voltage
    ShutterVoltsNP[0].fill("SHUTTERvolts", "Volts", "%.1f", 0.0, 15, 0.0, 0);
    ShutterVoltsNP.fill(getDeviceName(), "SHUTTERVOLTS", "Shutter", MAIN_CONTROL_TAB, IP_RO, 60, IPS_OK);

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Rototor settings tab
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Home position (offset from North)
    HomePositionNP[0].fill("POSITON", "Degrees", "%.2f", 0.0, 360.0, 0.0, 0);
    // TODO add set to current
    HomePositionNP.fill(getDeviceName(), "HOME_POSITION", "Home Sensor Position", ROTATOR_TAB, IP_RW, 60, IPS_IDLE);

    // Rotator
    RotatorCalibrationSP[ROTATOR_HOME_FIND].fill("ROTATOR_HOME_FIND", "Find Home", ISS_OFF);
    RotatorCalibrationSP[ROTATOR_HOME_MEASURE].fill("ROTATOR_HOME_MEASURE", "Measure Home", ISS_OFF);
    RotatorCalibrationSP[ROTATOR_HOME_GOTO].fill("ROTATOR_HOME_GOTO", "Goto Home", ISS_OFF);
    RotatorCalibrationSP.fill(getDefaultName(), "ROTATOR_CALIBRATION", "Rotator", ROTATOR_TAB, IP_RW, ISR_ATMOST1, 60,
                              IPS_IDLE);
    
    /******* NOTE DOME_CAN_PARK removes the need for this (?)
    RotatorParkSP[ROTATOR_HOME_PARK].fill("ROTATOR_HOME_PARK", "Goto Park", ISS_OFF);
    RotatorParkSP[ROTATOR_HOME_UNPARK].fill("ROTATOR_HOME_UNPARK", "UnPark", ISS_OFF);
    RotatorParkSP[ROTATOR_HOME_SETPARK].fill("ROTATOR_HOME_SETPARK", "Set Park", ISS_OFF);
    RotatorParkSP.fill(getDefaultName(), "ROTATOR_PARK", "Rotator", MAIN_CONTROL_TAB, IP_RW, ISR_ATMOST1, 60,
                              IPS_IDLE);
    ******/

    // Rotator Settings
    RotatorSettingsNP[ROTATOR_MAX_SPEED].fill("SHUTTER_MAX_SPEED", "Max Speed (m/s)", "%.f", 1, 1000, 10, 800);
    RotatorSettingsNP[ROTATOR_MIN_SPEED].fill("SHUTTER_MIN_SPEED", "Min Speed (m/s)", "%.f", 1, 1000, 10, 400);
    RotatorSettingsNP[ROTATOR_ACCELERATION].fill("SHUTTER_ACCELERATION", "Acceleration (m/s^2)", "%.f", 1, 1000, 10, 500);
    RotatorSettingsNP[ROTATOR_TIMEOUT].fill("SHUTTER_TIMEOUT", "Timeout (s)", "%.f", 1, 1000, 10, 83);
    RotatorSettingsNP.fill(getDeviceName(), "SHUTTER_SETTINGS", "Settings", ROTATOR_TAB, IP_RW, 60, IPS_IDLE);

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Shutter settings tab
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Shutter Home (calibrate, reset)
    ShutterCalibrationSP[SHUTTER_HOME_FIND].fill("SHUTTER_HOME_FIND", "Find home", ISS_OFF);
    ShutterCalibrationSP.fill(getDeviceName(), "SHUTTER_CALIBRATION", "Shutter", SHUTTER_TAB, IP_RW, ISR_ATMOST1, 60, IPS_IDLE);

    // Shutter Settings
    ShutterSettingsNP[SHUTTER_MAX_SPEED].fill("SHUTTER_MAX_SPEED", "Max Speed (m/s)", "%.f", 1, 10, 1, 0);
    ShutterSettingsNP[SHUTTER_MIN_SPEED].fill("SHUTTER_MIN_SPEED", "Min Speed (m/s)", "%.f", 1, 10, 1, 0);
    ShutterSettingsNP[SHUTTER_ACCELERATION].fill("SHUTTER_ACCELERATION", "Acceleration (m/s^2)", "%.f", 1, 10, 1, 0);
    ShutterSettingsNP[SHUTTER_TIMEOUT].fill("SHUTTER_TIMEOUT", "Timeout (s)", "%.f", 1, 10, 1, 0);
    ShutterSettingsNP[SHUTTER_SAFE_VOLTAGE].fill("SHUTTER_SAFE_VOLTAGE", "Safe Voltage", "%.f", 1, 10, 1, 0);
    ShutterSettingsNP.fill(getDeviceName(), "SHUTTER_SETTINGS", "Settings", SHUTTER_TAB, IP_RW, 60, IPS_IDLE);

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Info tab (firmware/driver)
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Firmware Version
    FirmwareVersionTP[0].fill("VERSION", "Version", "");
    FirmwareVersionTP.fill(getDeviceName(), "DOME_FIRMWARE", "Firmware", INFO_TAB, IP_RO, 0, IPS_IDLE);
    //add driver version

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Communication
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // NOTE need to figure out how to get network connection working
    //    tcpConnection->setDefaultHost("192.168.1.1");
    //    tcpConnection->setDefaultPort(10000);
    //    tcpConnection->setConnectionType(Connection::TCP::TYPE_UDP);
    //    tty_set_generic_udp_format(1);
    // NOTE how to set default baudrate
    addDebugControl();
    return true;
}

bool Beaver::updateProperties()
{
    INDI::Dome::updateProperties();

    if (isConnected())
    {
        InitPark();

        defineProperty(&FirmwareVersionTP);

        //defineProperty(&RotatorParkSP);
        defineProperty(&HomePositionNP);
        defineProperty(&RotatorCalibrationSP);
        defineProperty(&RotatorSettingsNP);
        defineProperty(&RotatorStatusTP);
        if (shutterPresent) {
            defineProperty(ShutterCalibrationSP);
            defineProperty(ShutterSettingsNP);
            defineProperty(&ShutterVoltsNP);
        }
    }
    else
    {
        deleteProperty(FirmwareVersionTP.getName());
        deleteProperty(RotatorCalibrationSP.getName());
        //deleteProperty(RotatorParkSP.getName());
        deleteProperty(ShutterCalibrationSP.getName());
        deleteProperty(ShutterSettingsNP.getName());
        deleteProperty(HomePositionNP.getName());
        deleteProperty(RotatorSettingsNP.getName());
        deleteProperty(RotatorStatusTP.getName());
        deleteProperty(ShutterVoltsNP.getName());
    }

    return true;
}


//////////////////////////////////////////////////////////////////////////////
///
//////////////////////////////////////////////////////////////////////////////
bool Beaver::Handshake()
{
    if (echo())
    {
        shutterPresent = false;
        // Check if shutter is online
        if (shutterIsUp()) {
            SetDomeCapability(GetDomeCapability() | DOME_HAS_SHUTTER);
            shutterPresent = true;
        }

        return true;
    }

    return false;
}

//////////////////////////////////////////////////////////////////////////////
///
//////////////////////////////////////////////////////////////////////////////
const char *Beaver::getDefaultName()
{
    return  "Beaver Dome";
}

//////////////////////////////////////////////////////////////////////////////
///
//////////////////////////////////////////////////////////////////////////////
bool Beaver::echo()
{
    // retrieve the version from the dome
    double res = 0;
    if (!sendCommand("!seletek tversion#", res))
        return false;     //NOTE perhaps set a value for a false, then check at end to return false, thereby allowing other checks to occur
    else {
        char firmwareText[MAXINDILABEL] = {0};
        snprintf(firmwareText, MAXINDILABEL, "%.f", res);
        FirmwareVersionTP[0].setText(firmwareText);
        LOGF_INFO("Detected firmware version %s", firmwareText);
    }
    // retrieve the current home offset from the dome
    if (!sendCommand("!domerot gethome#", res)) 
        return false;
    else {
        HomePositionNP[0].setValue(res);
        LOGF_INFO("Dome reports home offset: %f", res);
    }
    // retrieve the current az from the dome
    if (!sendCommand("!dome getaz#", res)) 
        return false;
    else {
        DomeAbsPosN[0].value = res;
        LOGF_INFO("Dome reports currently at az: %.1f", res);
    }
    // retrieve the current park position from the dome
    if (!sendCommand("!domerot getpark#", res)) 
        return false;
    else {
        DomeAbsPosN[0].value = res;
        LOGF_INFO("Dome reports park az as: %.1f", res);
    }
    
    // get current shutter settings
    if (!shutterGetSettings())
        return false;
    // get current rotator settings
    if (!rotatorGetSettings())
        return false;
    
    return true;
}

//////////////////////////////////////////////////////////////////////////////
///
//////////////////////////////////////////////////////////////////////////////
bool Beaver::ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int n)
{
    if (dev != nullptr && strcmp(dev, getDeviceName()) == 0)
    {
        /////////////////////////////////////////////
        // Rotator Calibration
        /////////////////////////////////////////////
        if (RotatorCalibrationSP.isNameMatch(name))
        {
            RotatorCalibrationSP.update(states, names, n);
            bool rc = false;
            switch (RotatorCalibrationSP.findOnSwitchIndex())
            {
                case ROTATOR_HOME_FIND:
                    rc = rotatorFindHome();
                    break;

                case ROTATOR_HOME_MEASURE:
                    rc = rotatorMeasureHome();
                    break;

                case ROTATOR_HOME_GOTO:
                    rc = rotatorGotoHome();
                    break;
            }

            RotatorCalibrationSP.setState(rc ? IPS_BUSY : IPS_ALERT);
            RotatorCalibrationSP.apply();
            return true;
        }
        
        // TODO add set park to current and set home to current

        /*******  NOTE I think setting the DOME_CAN_PARK removes the need for this
        /////////////////////////////////////////////
        // Rotator Park
        /////////////////////////////////////////////
        if (RotatorParkSP.isNameMatch(name))
        {
            RotatorParkSP.update(states, names, n);
            bool rc = false;
            switch (RotatorParkSP.findOnSwitchIndex())
            {
                case ROTATOR_HOME_PARK:
                    rc = rotatorGotoPark();
                    break;
                    case ROTATOR_HOME_UNPARK:
                    rc = rotatorUnPark();
                    break;
                 case ROTATOR_HOME_SETPARK:
                    rc = rotatorSetPark();
                    break;
            }

            RotatorParkSP.setState(rc ? IPS_BUSY : IPS_ALERT);
            RotatorParkSP.apply();
            return true;
        }
        ******/

        /////////////////////////////////////////////
        // Shutter Calibration
        /////////////////////////////////////////////
        if (ShutterCalibrationSP.isNameMatch(name))
        {  //TEST
            ShutterCalibrationSP.update(states, names, n);
            bool rc = shutterFindHome();
            if (rc)
                setShutterState(SHUTTER_MOVING);
            ShutterCalibrationSP.setState(rc ? IPS_BUSY : IPS_ALERT);
            ShutterCalibrationSP.apply();
            return true;
        }
    }

    return INDI::Dome::ISNewSwitch(dev, name, states, names, n);
}

//////////////////////////////////////////////////////////////////////////////
///
//////////////////////////////////////////////////////////////////////////////
bool Beaver::ISNewNumber(const char *dev, const char *name, double values[], char *names[], int n)
{
    if (dev != nullptr && strcmp(dev, getDeviceName()) == 0)
    {
        /////////////////////////////////////////////
        // Rotator Settings
        /////////////////////////////////////////////
        if (RotatorSettingsNP.isNameMatch(name))
        {
            RotatorSettingsNP.update(values, names, n);
            RotatorSettingsNP.setState(rotatorSetSettings(RotatorSettingsNP[SHUTTER_MAX_SPEED].getValue(),
                                       RotatorSettingsNP[SHUTTER_MIN_SPEED].getValue(),
                                       RotatorSettingsNP[SHUTTER_ACCELERATION].getValue(),
                                       RotatorSettingsNP[SHUTTER_TIMEOUT].getValue()) ? IPS_OK : IPS_ALERT);
            RotatorSettingsNP.apply();
            return true;
        }
        /////////////////////////////////////////////
        // Shutter Settings
        /////////////////////////////////////////////
        if (ShutterSettingsNP.isNameMatch(name))
        {
            ShutterSettingsNP.update(values, names, n);
            ShutterSettingsNP.setState(shutterSetSettings(ShutterSettingsNP[SHUTTER_MAX_SPEED].getValue(),
                                       ShutterSettingsNP[SHUTTER_MIN_SPEED].getValue(),
                                       ShutterSettingsNP[SHUTTER_ACCELERATION].getValue(),
                                       ShutterSettingsNP[SHUTTER_TIMEOUT].getValue(),
                                       ShutterSettingsNP[SHUTTER_SAFE_VOLTAGE].getValue()) ? IPS_OK : IPS_ALERT);
            ShutterCalibrationSP.apply();
            return true;
        }
        ///////////////////////////////////////////////////////////////////////////////
        /// Home Position
        ///////////////////////////////////////////////////////////////////////////////
        // TODO read initial value from dome
        if (HomePositionNP.isNameMatch(name))
        {
            HomePositionNP.update(values, names, n);
            if (rotatorSetHome(HomePositionNP[0].getValue())) {
                LOGF_INFO("Home position is updated to %.1f degrees.", HomePositionNP[0].getValue());
                HomePositionNP.apply();
                return true;
            }
            else
                return false;
        }
        
    }

    return INDI::Dome::ISNewNumber(dev, name, values, names, n);
    
}

///////////////////////////////////////////////////////////////////////////
/// Read position and update accordingly
///////////////////////////////////////////////////////////////////////////
void Beaver::TimerHit()
{
    if (!isConnected()) {
        return;
    }
    
    // Query rotator status
    //rres = rotatorGetStatus();
    //LOGF_INFO("DIAG: Rotator status: %f", rres);
    double res = 0;
    if (!sendCommand("!dome status#", res))
        LOG_ERROR("Status command error");
    
    
    uint16_t domestatus = static_cast<uint16_t>(res);
    //LOGF_INFO("DIAG: Dome status: %0x", domestatus);
    // TODO change this to use enum defined in .h 
    // Test for errors
    if (domestatus & 0x0020) {
        LOG_ERROR("CW Unsafe Error");
        setDomeState(DOME_ERROR);
        RotatorStatusTP.apply();
    }
    if (domestatus & 0x0040) {
        LOG_ERROR("RGx Unsafe Error");
        setDomeState(DOME_ERROR);
        RotatorStatusTP.apply();
    }
    
    // Get Position
    rotatorGetAz();
    //DomeAbsPosN[0].value = sendCommand("!dome getaz#", res);
    //LOGF_INFO("DIAG: Rotator position: %f", DomeAbsPosN[0].value);

    // Check rotator
    if (getDomeState() == DOME_MOVING || getDomeState() == DOME_UNPARKING) {
        // TEST
        LOGF_INFO("dome status: %00x", domestatus);
        if ((domestatus & 0x0001) == 0) {
            setDomeState(DOME_IDLE);
            RotatorStatusTP[0].setText("Idle");
            RotatorStatusTP.apply();
            LOGF_INFO("DIAG: Dome state set to IDLE, domestatus: %00x", domestatus);
        }
        if (domestatus & 0x800) {
            setDomeState(DOME_IDLE);
            RotatorStatusTP[0].setText("At Home/Idle");
            RotatorStatusTP.apply();
            LOG_INFO("DIAG: Dome state set to HOME");
        }
        if (domestatus & 0x0004) {
            setDomeState(DOME_ERROR);
            RotatorStatusTP[0].setText("Error");
            RotatorStatusTP.apply();
            LOG_ERROR("Rotation Mechanical Error");
        }
    }
    // Dome parked is a special case
    if (getDomeState() == DOME_PARKING) {
        if (domestatus & 0x1000) {   //ALERT only do this if asked to park
            setDomeState(DOME_PARKED);
            RotatorStatusTP[0].setText("Parked");
            RotatorStatusTP.apply();
            LOG_INFO("DIAG: Dome state set to PARKED");
        }
    }
    
    // Check shutter
    // TODO add field in Main tab to say what the current shutter state is
    if (getShutterState() == SHUTTER_MOVING)
    {
        // TEST
        if (domestatus & 0x0080) {
            setShutterState(SHUTTER_OPENED);
            LOG_INFO("DIAG: Shutter state set to OPENED");
        }
        if (domestatus & 0x0100) {
            setShutterState(SHUTTER_CLOSED);
            LOG_INFO("DIAG: Shutter state set to CLOSED");
        }
        if (domestatus & 0x0008) {
            LOG_ERROR("Shutter Mechanical Error");
            setShutterState(SHUTTER_ERROR);
        }
        if (domestatus & 0x00010) {
            LOG_ERROR("Shutter Communications Error");
            setShutterState(SHUTTER_ERROR);
        }
    }
    
    // Update shutter voltage
    if (shutterPresent) {
        if (!sendCommand("!dome getshutterbatvoltage#", res))
            LOG_ERROR("Shutter voltage command error");
        else {
            //LOGF_INFO("Shutter voltage is currently: %.1f", res);
            ShutterVoltsNP[0].setValue(res);
            (res < ShutterSettingsNP[SHUTTER_SAFE_VOLTAGE].getValue()) ? ShutterVoltsNP.setState(IPS_ALERT) : ShutterVoltsNP.setState(IPS_OK);  //TASK how can I change the button, not the circle?
            ShutterVoltsNP.apply();
        }
    }
        

    SetTimer(getCurrentPollingPeriod());
}

//////////////////////////////////////////////////////////////////////////////
///
//////////////////////////////////////////////////////////////////////////////
IPState Beaver::MoveAbs(double az)
{
    // ALERT updates abs location 'after' next move (always one behind)
    if (rotatorGotoAz(az))
    {
        m_TargetRotatorAz = az;
        setDomeState(DOME_MOVING);
        RotatorStatusTP[0].setText("Moving");
        RotatorStatusTP.apply();
        return IPS_BUSY;
    }

    return IPS_ALERT;
}

//////////////////////////////////////////////////////////////////////////////
///
//////////////////////////////////////////////////////////////////////////////
IPState Beaver::MoveRel(double azDiff)
{
    // ALERT not updating abs location field
    m_TargetRotatorAz = DomeAbsPosN[0].value + azDiff;

    if (m_TargetRotatorAz < DomeAbsPosN[0].min)
        m_TargetRotatorAz += DomeAbsPosN[0].max;
    if (m_TargetRotatorAz > DomeAbsPosN[0].max)
        m_TargetRotatorAz -= DomeAbsPosN[0].max;

    // It will take a few cycles to reach final position
    return MoveAbs(m_TargetRotatorAz);
}

//////////////////////////////////////////////////////////////////////////////
///
//////////////////////////////////////////////////////////////////////////////
bool Beaver::Sync(double az)
{
    return rotatorSyncAZ(az);
}

//////////////////////////////////////////////////////////////////////////////
///
//////////////////////////////////////////////////////////////////////////////
IPState Beaver::ControlShutter(ShutterOperation operation)
{
    double res = 0;
    if (operation == SHUTTER_OPEN)
    {
        if (sendCommand("!dome openshutter#", res))
            return IPS_BUSY;
    }
    else if (operation == SHUTTER_CLOSE)
    {
        if (sendCommand("!dome closeshutter#", res))
            return IPS_BUSY;
    }
    return IPS_ALERT;
}

//////////////////////////////////////////////////////////////////////////////
///
//////////////////////////////////////////////////////////////////////////////
bool Beaver::Abort()
{
    return abortAll();
}

//////////////////////////////////////////////////////////////////////////////
///
//////////////////////////////////////////////////////////////////////////////
IPState Beaver::Park()
{
    return rotatorGotoPark() ? IPS_BUSY : IPS_ALERT;
}

//////////////////////////////////////////////////////////////////////////////
///
//////////////////////////////////////////////////////////////////////////////
IPState Beaver::UnPark()
{
    return IPS_OK;
}

//////////////////////////////////////////////////////////////////////////////
///
//////////////////////////////////////////////////////////////////////////////
bool Beaver::saveConfigItems(FILE *fp)
{
    INDI::Dome::saveConfigItems(fp);
    IUSaveConfigNumber(fp, &ShutterSettingsNP);
    IUSaveConfigNumber(fp, &HomePositionNP);
    return true;
}

/////////////////////////////////////////////////////////////////////////////
///
/////////////////////////////////////////////////////////////////////////////
bool Beaver::rotatorGotoAz(double az)
{
    char cmd[DRIVER_LEN] = {0};
    double res = 0;
    snprintf(cmd, DRIVER_LEN, "!dome gotoaz %.2f#", az);
    setDomeState(DOME_MOVING);
    RotatorStatusTP[0].setText("Moving");
    RotatorStatusTP.apply();
    return sendCommand(cmd, res);
}

/////////////////////////////////////////////////////////////////////////////
///
/////////////////////////////////////////////////////////////////////////////
bool Beaver::rotatorGetAz()
{
    double res = 0;
    if (sendCommand("!dome getaz#", res))
    {
        DomeAbsPosN[0].value = res;
        //DomeAbsPosNP.apply(); //Need to update Abs position on main tab
        return true;
    }

    return false;
}

/////////////////////////////////////////////////////////////////////////////
///  Sync
/////////////////////////////////////////////////////////////////////////////
bool Beaver::rotatorSyncAZ(double az)
{
    char cmd[DRIVER_LEN] = {0};
    double res = 0;
    snprintf(cmd, DRIVER_LEN, "!dome setaz %.2f#", az);
    return sendCommand(cmd, res);
}

/////////////////////////////////////////////////////////////////////////////
/// Set home offset from north
/////////////////////////////////////////////////////////////////////////////
// NOTE indi_dome has a HOME_POSITION, described as: dome home position in absolute degrees azimuth, should use this instead?
bool Beaver::rotatorSetHome(double az)
{
    char cmd[DRIVER_LEN] = {0};
    double res = 0;
    snprintf(cmd, DRIVER_LEN, "!domerot sethome %.2f#", az);
    return sendCommand(cmd, res);
}

// NOTE what about a set park?

/////////////////////////////////////////////////////////////////////////////
/// Rotator park
/////////////////////////////////////////////////////////////////////////////
bool Beaver::rotatorGotoPark()
{
    double res = 0;
    setDomeState(DOME_PARKING);
    RotatorStatusTP[0].setText("Parking");
    RotatorStatusTP.apply();
    return sendCommand("!dome gopark#", res);
}

/////////////////////////////////////////////////////////////////////////////
/// Rotator unpark
/////////////////////////////////////////////////////////////////////////////
bool Beaver::rotatorUnPark()
{
    setDomeState(DOME_IDLE);
    RotatorStatusTP[0].setText("Idle @ park");
    RotatorStatusTP.apply();
    return true;
}

/////////////////////////////////////////////////////////////////////////////
/// Rotator set park
/////////////////////////////////////////////////////////////////////////////
bool Beaver::rotatorSetPark()
{
    double res = 0;
    setDomeState(DOME_PARKED);
    RotatorStatusTP[0].setText("Parked");
    RotatorStatusTP.apply();
    return sendCommand("!dome setpark#", res);
}

/////////////////////////////////////////////////////////////////////////////
///
/////////////////////////////////////////////////////////////////////////////
bool Beaver::rotatorGotoHome()
{
    double res = 0;
    setDomeState(DOME_MOVING);
    RotatorStatusTP[0].setText("Homing");
    RotatorStatusTP.apply();
    return sendCommand("!dome gohome#", res);
}

/////////////////////////////////////////////////////////////////////////////
///
/////////////////////////////////////////////////////////////////////////////
bool Beaver::rotatorMeasureHome()
{
    double res = 0;
    setDomeState(DOME_MOVING);
    RotatorStatusTP[0].setText("Measuring Home");
    RotatorStatusTP.apply();
    return sendCommand("!dome autocalrot 1#", res);
}

/////////////////////////////////////////////////////////////////////////////
///
/////////////////////////////////////////////////////////////////////////////
bool Beaver::rotatorFindHome()
{
    double res = 0;
    setDomeState(DOME_MOVING);
    RotatorStatusTP[0].setText("Finding Home");
    RotatorStatusTP.apply();
    return sendCommand("!dome autocalrot 0#", res);
}

/////////////////////////////////////////////////////////////////////////////
///
/////////////////////////////////////////////////////////////////////////////
bool Beaver::rotatorIsHome()
{
    double res = 0;
    if (sendCommand("!dome athome#", res))
    {
        RotatorStatusTP[0].setText("Home");
        RotatorStatusTP.apply();
        return res == 1;
    }
    return false;
}

/////////////////////////////////////////////////////////////////////////////
///
/////////////////////////////////////////////////////////////////////////////
bool Beaver::rotatorIsParked()
{
    double res = 0;
    if (sendCommand("!dome atpark#", res))
    {
        RotatorStatusTP[0].setText("Parked");
        RotatorStatusTP.apply();
        return res == 1;
    }
    return false;
}

/////////////////////////////////////////////////////////////////////////////
///
/////////////////////////////////////////////////////////////////////////////
//bool Beaver::rotatorGetStatus()
//{
    // TODO
    double res = 0;
    //if (sendCommand("!dome status#", res))
    //{
        //LOGF_INFO("Rotator status: %f", res);
        //return res;
    //}
    //return false;
//}

/////////////////////////////////////////////////////////////////////////////
///
/////////////////////////////////////////////////////////////////////////////
//bool Beaver::shutterGetStatus()
//{
    // TODO
    // 0 open, 1 closed, 2 opening, 3 closing, 4 error
    //double res = 0;
    //if (sendCommand("!dome shutterstatus#", res))
    //{
        //LOGF_INFO("Shutter status: %f", res);
        //return res;
    //}
    //return false;
//}

/////////////////////////////////////////////////////////////////////////////
///
/////////////////////////////////////////////////////////////////////////////
bool Beaver::shutterIsUp()
{
    double res = 0;
    if (sendCommand("!dome shutterisup#", res))
    {
        return res == 1;
    }
    return false;
}

/////////////////////////////////////////////////////////////////////////////
///
/////////////////////////////////////////////////////////////////////////////
bool Beaver::abortAll()
{
    double res = 0;
    if (sendCommand("!dome abort 1 1 1#", res)) {
        RotatorStatusTP[0].setText("Idle");
        RotatorStatusTP.apply();
        if (!rotatorGetAz())
            return false;
        return true;
    }
        
    return false;
}

/////////////////////////////////////////////////////////////////////////////
///
/////////////////////////////////////////////////////////////////////////////
bool Beaver::shutterAbort()
{
    double res = 0;
    return sendCommand("!dome abort 0 0 1#", res);
}

/////////////////////////////////////////////////////////////////////////////
///
/////////////////////////////////////////////////////////////////////////////
bool Beaver::shutterSetSettings(double maxSpeed, double minSpeed, double acceleration, double timeout,
                                double voltage)
{
    // TEST
    if (shutterPresent) {
        if (!sendCommand("!dome setshuttermaxspeed#", maxSpeed))
            return false;
        if (!sendCommand("!dome setshutterminspeed#", minSpeed))
            return false;
        if (!sendCommand("!dome setshutteracceleration#", acceleration))
            return false;
        if (!sendCommand("!dome setshuttertimeoutopenclose#", timeout))
            return false;
        if (!sendCommand("!dome setshuttersafevoltage#", voltage))
            return false;
    }
    return true;
}

/////////////////////////////////////////////////////////////////////////////
///
/////////////////////////////////////////////////////////////////////////////
bool Beaver::shutterGetSettings()
{
    // TEST
    if (shutterPresent) {
        if (!sendCommand("!dome getshuttermaxspeed#", res))
            return false;
        else {
            ShutterSettingsNP[SHUTTER_MAX_SPEED].setValue(res);
            LOGF_INFO("Shutter reports max speed of: %.1f", res);
        }
        if (!sendCommand("!dome getshutterminspeed#", res))
            return false;
        else {
            ShutterSettingsNP[SHUTTER_MIN_SPEED].setValue(res);
            LOGF_INFO("Shutter reports min speed of: %.1f", res);
        }
        if (!sendCommand("!dome getshutteracceleration#", res))
            return false;
        else {
            ShutterSettingsNP[SHUTTER_ACCELERATION].setValue(res);
            LOGF_INFO("Shutter reports acceleration of: %.1f", res);
        }
        if (!sendCommand("!dome getshuttertimeoutopenclose#", res))
            return false;
        else {
            ShutterSettingsNP[SHUTTER_TIMEOUT].setValue(res);
            LOGF_INFO("Shutter reports timeout(s) of: %.1f", res);
        }
        if (!sendCommand("!dome getshuttersafevoltage#", res))
            return false;
        else {
            ShutterSettingsNP[SHUTTER_SAFE_VOLTAGE].setValue(res);
            LOGF_INFO("Shutter reports safe voltage of: %.1f", res);
        }
        ShutterSettingsNP.apply();
    }

    return true;
}

/////////////////////////////////////////////////////////////////////////////
///
/////////////////////////////////////////////////////////////////////////////
bool Beaver::rotatorSetSettings(double maxSpeed, double minSpeed, double acceleration, double timeout)
{
    // TEST
    if (shutterPresent) {
        if (!sendCommand("!domerot setmaxspeed#", maxSpeed))
            return false;
        if (!sendCommand("!domerot setminspeed#", minSpeed))
            return false;
        if (!sendCommand("!domerot setacceleration#", acceleration))
            return false;
        if (!sendCommand("!domerot setfullrotsecs#", timeout))
            return false;
    }
    return true;
}

/////////////////////////////////////////////////////////////////////////////
///
/////////////////////////////////////////////////////////////////////////////
bool Beaver::rotatorGetSettings()
{
    // TEST
    if (shutterPresent) {
        if (!sendCommand("!domerot getmaxspeed#", res))
            return false;
        else {
            RotatorSettingsNP[ROTATOR_MAX_SPEED].setValue(res);
            LOGF_INFO("Rotator reports max speed of: %.1f", res);
        }
        if (!sendCommand("!domerot getminspeed#", res))
            return false;
        else {
            RotatorSettingsNP[ROTATOR_MIN_SPEED].setValue(res);
            LOGF_INFO("Rotator reports min speed of: %.1f", res);
        }
        if (!sendCommand("!domerot getacceleration#", res))
            return false;
        else {
            RotatorSettingsNP[ROTATOR_ACCELERATION].setValue(res);
            LOGF_INFO("Rotator reports acceleration of: %.1f", res);
        }
        if (!sendCommand("!domerot getmaxfullfotsecs#", res))
            return false;
        else {
            RotatorSettingsNP[ROTATOR_TIMEOUT].setValue(res);
            LOGF_INFO("Rotator reports timeout(s) of: %.1f", res);
        }
        RotatorSettingsNP.apply();
    }

    return true;
}

/////////////////////////////////////////////////////////////////////////////
///
/////////////////////////////////////////////////////////////////////////////
bool Beaver::shutterFindHome()
{
    double res = 0;
    return sendCommand("!dome autocalshutter#", res);
}


/////////////////////////////////////////////////////////////////////////////
/// Send Command
/////////////////////////////////////////////////////////////////////////////
bool Beaver::sendCommand(const char * cmd, double &res)
{
    int rc = TTY_OK;
    for (int i = 0; i < 3; i++)
    {
        int nbytes_written = 0, nbytes_read = 0;
        char response[DRIVER_LEN] = {0};

        LOGF_DEBUG("CMD <%s>", cmd);

        rc = tty_write_string(PortFD, cmd, &nbytes_written);

        if (rc != TTY_OK)
        {
            char errstr[MAXRBUF] = {0};
            tty_error_msg(rc, errstr, MAXRBUF);
            LOGF_ERROR("Serial write error: %s.", errstr);
            return false;
        }

        rc = tty_nread_section(PortFD, response, DRIVER_LEN, DRIVER_STOP_CHAR, DRIVER_TIMEOUT, &nbytes_read);

        if (rc != TTY_OK)
        {
            usleep(100000);
            continue;
        }

        // Remove extra #
        response[nbytes_read - 1] = 0;
        LOGF_DEBUG("RES <%s>", response);


        std::regex rgx(R"(.*:(\d+))");
        std::smatch match;
        std::string input(response);

        if (std::regex_search(input, match, rgx))
        {
            try
            {
                res = std::stof(match.str(1));
                return true;
            }
            catch (...)
            {
                LOGF_ERROR("Failed to process response: %s.", response);
                return false;
            }
        }
    }

    if (rc != TTY_OK)
    {
        char errstr[MAXRBUF] = {0};
        tty_error_msg(rc, errstr, MAXRBUF);
        LOGF_ERROR("Serial read error: %s.", errstr);
    }

    return false;
}
