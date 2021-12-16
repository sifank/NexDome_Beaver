/*
    NexDome Beaver Controller

    Copyright (C) 2021 Jasem Mutlaq (mutlaqja@ikarustech.com)

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

#pragma once

#include <memory>
#include <indidome.h>
#include <indipropertytext.h>
#include <indipropertyswitch.h>
#include <indipropertynumber.h>

class Beaver : public INDI::Dome
{
    public:
        Beaver();
        virtual ~Beaver() override = default;

        const char *getDefaultName() override;
        virtual bool initProperties() override;
        virtual bool updateProperties() override;
        virtual bool ISNewNumber(const char *dev, const char *name, double values[], char *names[], int n) override;
        virtual bool ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int n) override;

    protected:
        bool Handshake() override;
        virtual void TimerHit() override;

        // Rotator
        virtual IPState MoveAbs(double az) override;
        virtual IPState MoveRel(double azDiff) override;
        virtual bool Sync(double az) override;

        // Shutter
        virtual IPState ControlShutter(ShutterOperation operation) override;

        // Abort
        virtual bool Abort() override;

        // Config
        virtual bool saveConfigItems(FILE * fp) override;

        // Parking
        virtual IPState Park() override;
        virtual IPState UnPark() override;

        // Beaver dome status
        enum
        {
            DOME_STATUS_IDLE,
            DOME_STATUS_ROTATOR_MOVING,
            DOME_STATUS_SHUTTER_MOVING,
            DOME_STATUS_BOTH_MOVING,
            DOME_STATUS_ROTATOR_ERROR = 4,
            DOME_STATUS_SHUTTER_ERROR = 8,
            DOME_STATUS_SHUTTER_COMM = 16,
            DOME_STATUS_UNSAFE_CW = 32,
            DOME_STATUS_UNSAFE_RG = 64,
        };

        // Beaver shutter status
        enum
        {
            SHUTTER_STATUS_OPENED,
            SHUTTER_STATUS_CLOSED,
            SHUTTER_STATUS_OPENING,
            SHUTTER_STATUS_CLOSING,
            SHUTTER_STATUS_ERROR,
        };

    private:

        ///////////////////////////////////////////////////////////////////////////////
        /// Set & Query Functions
        ///////////////////////////////////////////////////////////////////////////////
        bool echo();

        ///////////////////////////////////////////////////////////////////////////////
        /// Rotator Motion Control
        ///////////////////////////////////////////////////////////////////////////////
        bool rotatorGotoAz(double az);
        bool rotatorGetAz();
        bool rotatorSyncAZ(double az);
        bool rotatorSetHome(double az);
        bool rotatorGotoPark();
        bool rotatorGotoHome();
        bool rotatorMeasureHome();
        bool rotatorFindHome();
        bool rotatorIsHome();
        bool rotatorIsParked();
        bool rotatorUnPark();
        bool rotatorSetPark();
        //bool rotatorGetStatus();  // ALERT should return actual status, int
        bool abortAll();

        bool rotatorGetSettings();
        bool rotatorSetSettings(double maxSpeed, double minSpeed, double acceleration, double timeout);

        ///////////////////////////////////////////////////////////////////////////////
        /// Shutter Motion Control
        ///////////////////////////////////////////////////////////////////////////////
        bool shutterSetSettings(double maxSpeed, double minSpeed, double acceleration, double timeout, double voltage);
        bool shutterGetSettings();
        bool shutterFindHome();
        bool shutterAbort();
        //bool shutterGetStatus(); // ALERT should return actual status, int
        bool shutterIsUp();

        ///////////////////////////////////////////////////////////////////////////////
        /// Communication Functions
        ///////////////////////////////////////////////////////////////////////////////
        bool sendCommand(const char * cmd, double &res);
        void hexDump(char * buf, const char * data, int size);
        std::vector<std::string> split(const std::string &input, const std::string &regex);

        ///////////////////////////////////////////////////////////////////////////////
        /// Properties
        ///////////////////////////////////////////////////////////////////////////////
        // Firmware Version
        INDI::PropertyText FirmwareVersionTP {1};
        // Home offset from north
        INDI::PropertyNumber HomePositionNP {1};
        // Shutter voltage
        INDI::PropertyNumber ShutterVoltsNP {1};
        // Rotator Status        
        INDI::PropertyText RotatorStatusTP {1};
        // Rotator Calibration
        INDI::PropertySwitch RotatorCalibrationSP {3};
        enum
        {
            ROTATOR_HOME_FIND,
            ROTATOR_HOME_MEASURE,
            ROTATOR_HOME_GOTO
        };
        /****** NOTE DOME_CAN_PARK removes the need for this (?)
        INDI::PropertySwitch RotatorParkSP {3};
        enum
        {
            ROTATOR_HOME_PARK,
            ROTATOR_HOME_UNPARK,
            ROTATOR_HOME_SETPARK
        };
        ******/
        // Shutter Calibration
        INDI::PropertySwitch ShutterCalibrationSP {1};
        enum
        {
            SHUTTER_HOME_FIND
        };
        // Shutter Configuration
        INDI::PropertyNumber ShutterSettingsNP {5};
        enum
        {
            SHUTTER_MAX_SPEED,
            SHUTTER_MIN_SPEED,
            SHUTTER_ACCELERATION,
            SHUTTER_TIMEOUT,
            SHUTTER_SAFE_VOLTAGE
        };
        // Rotator Configuration
        INDI::PropertyNumber RotatorSettingsNP {4};
        enum
        {
            ROTATOR_MAX_SPEED,
            ROTATOR_MIN_SPEED,
            ROTATOR_ACCELERATION,
            ROTATOR_TIMEOUT
        };

        ///////////////////////////////////////////////////////////////////////
        /// Private Variables
        ///////////////////////////////////////////////////////////////////////
        double m_TargetRotatorAz {-1};

        /////////////////////////////////////////////////////////////////////////////
        /// Static Helper Values
        /////////////////////////////////////////////////////////////////////////////        
        static constexpr const char * ROTATOR_TAB = "Rotator";
        static constexpr const char * SHUTTER_TAB = "Shutter";
        // '#' is the stop char
        static const char DRIVER_STOP_CHAR { 0x23 };
        // Wait up to a maximum of 3 seconds for serial input
        static constexpr const uint8_t DRIVER_TIMEOUT {3};
        // Maximum buffer for sending/receving.
        static constexpr const uint8_t DRIVER_LEN {128};
        bool shutterPresent;
};
