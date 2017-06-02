// Copyright (c) 2014-2017 FRC Team 3512. All Rights Reserved.

#pragma once

#include <Talon.h>

#include "../MotionProfile/TrapezoidProfile.hpp"
#include "../Settings.hpp"
#include "GearBox.hpp"

class DriveTrain : public TrapezoidProfile {
public:
    DriveTrain();
    virtual ~DriveTrain();

    /* Drives robot with given speed and turn values [-1..1].
     * This is a convenience function for use in Operator Control.
     */
    void drive(float throttle, float turn, bool isQuickTurn = false);

    // Sets joystick deadband
    void setDeadband(float band);

    // Set encoder distances to 0
    void resetEncoders();

    // Reload PID constants
    void reloadPID();

    // Set wheel setpoints (see GearBox::setSetpoint(float))
    void setLeftSetpoint(double setpt);
    void setRightSetpoint(double setpt);

    // Directly set wheel speeds [0..1] (see GearBox::setManual(float))
    void setLeftManual(float value);
    void setRightManual(float value);

    // Returns encoder distances
    double getLeftDist();
    double getRightDist();

    // Returns encoder rates
    double getLeftRate();
    double getRightRate();

    // Returns encoder PID loop setpoints
    double getLeftSetpoint();
    double getRightSetpoint();

    // Shifts internal gearboxes
    void setGear(bool gear);

    // Returns gear of internal gearboxes (true assumed to be high gear
    // TODO check if 'true' is high gear
    bool getGear() const;

    void setDefencive(bool defencive);

    bool getDefencive();

    static constexpr double maxWheelSpeed = 274.0;

private:
    Settings m_settings;

    float m_deadband;
    float m_sensitivity;

    bool m_isDefencive;

    // Cheesy Drive variables
    float m_oldTurn;
    float m_quickStopAccumulator;
    float m_negInertiaAccumulator;

    GearBox<Talon>* m_leftGrbx;
    GearBox<Talon>* m_rightGrbx;

    /* Zeroes value if its inside deadband range, and rescales values outside
     * of it
     */
    float applyDeadband(float value);

    // Limits 'value' to within +- 'limit' (limit should be positive)
    template <class T>
    T limit(T value, T limit);
};

#include "DriveTrain.inl"