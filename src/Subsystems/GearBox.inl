// Copyright (c) 2014-2017 FRC Team 3512. All Rights Reserved.

#pragma once

#include <cmath>

#include <Encoder.h>
#include <Solenoid.h>
#include <SpeedController.h>

#include "../PIDController.hpp"

template <class T>
GearBox<T>::GearBox(uint32_t shifterChan, uint32_t encA, uint32_t encB,
                    uint32_t motor1, uint32_t motor2, uint32_t motor3) {
    if (encA != 0 && encB != 0) {
        m_encoder = new Encoder(encA, encB);
        m_pid = new PIDController(0, 0, 0, 0, m_encoder, this);

        m_havePID = true;
    } else {
        m_encoder = NULL;
        m_pid = NULL;

        m_havePID = false;
    }

    if (shifterChan != 0) {
        m_shifter = new Solenoid(shifterChan);
    } else {
        m_shifter = NULL;
    }

    m_isReversed = false;

    m_targetGear = false;

    // Create motor controllers of specified template type
    if (motor1 != 0) {
        m_motors.push_back(new T(motor1));
    }
    if (motor2 != 0) {
        m_motors.push_back(new T(motor2));
    }
    if (motor3 != 0) {
        m_motors.push_back(new T(motor3));
    }
    if (m_havePID) {
        m_encoder->SetPIDSourceParameter(Encoder::kDistance);

        // m_pid->SetPercentTolerance( 5.f );
        m_pid->SetAbsoluteTolerance(1);

        m_encoder->Start();
        m_pid->Enable();
    }
}

template <class T>
GearBox<T>::~GearBox() {
    if (m_havePID) {
        delete m_pid;

        m_encoder->Stop();
        delete m_encoder;
    }

    if (m_shifter != NULL) {
        delete m_shifter;
    }

    // Free motors
    for (uint32_t i = 0; i < m_motors.size(); i++) {
        delete m_motors[i];
    }
    m_motors.clear();
}

template <class T>
void GearBox<T>::setSetpoint(float setpoint) {
    if (m_havePID) {
        if (!m_pid->IsEnabled()) {
            m_pid->Enable();
        }

        m_pid->SetSetpoint(setpoint);
    } else {
        // TODO emit warning since PID doesn't work (possibly through logger?)
    }
}

template <class T>
float GearBox<T>::getSetpoint() const {
    if (m_havePID) {
        return m_pid->GetSetpoint();
    } else {
        // TODO emit warning since PID doesn't work (possibly through logger?)
        return 0.f;
    }
}

template <class T>
void GearBox<T>::setManual(float value) {
    if (m_havePID) {
        if (m_pid->IsEnabled()) {
            m_pid->Disable();
        }
    }

    PIDWrite(value);
}

template <class T>
float GearBox<T>::getManual() const {
    if (!m_isReversed) {
        return m_motors[0]->Get();
    } else {
        return -m_motors[0]->Get();
    }
}

template <class T>
void GearBox<T>::setPID(float p, float i, float d) {
    if (m_havePID) {
        m_pid->SetPID(p, i, d);
    } else {
        // TODO emit warning since PID doesn't work (possibly through logger?)
    }
}

template <class T>
void GearBox<T>::setF(float f) {
    if (m_havePID) {
        m_pid->SetPID(m_pid->GetP(), m_pid->GetI(), m_pid->GetD(), f);
    } else {
        // TODO emit warning since PID doesn't work (possibly through logger?)
    }
}

template <class T>
void GearBox<T>::setDistancePerPulse(double distancePerPulse) {
    if (m_havePID) {
        m_encoder->SetDistancePerPulse(distancePerPulse);
    }
}

template <class T>
void GearBox<T>::setPIDSourceParameter(
    PIDSource::PIDSourceParameter pidSource) {
    if (m_havePID) {
        m_encoder->SetPIDSourceParameter(pidSource);
    }
}

template <class T>
void GearBox<T>::resetEncoder() {
    if (m_havePID) {
        m_encoder->Reset();
    } else {
        // TODO emit warning since PID doesn't work (possibly through logger?)
    }
}

template <class T>
double GearBox<T>::getDistance() const {
    if (m_havePID) {
        return m_encoder->GetDistance();
    } else {
        // TODO emit warning since PID doesn't work (possibly through logger?)
        return 0.f;
    }
}

template <class T>
double GearBox<T>::getRate() const {
    if (m_havePID) {
        return m_encoder->GetRate();
    } else {
        // TODO emit warning since PID doesn't work (possibly through logger?)
        return 0.f;
    }
}

template <class T>
void GearBox<T>::setReversed(bool reverse) {
    m_isReversed = reverse;
}

template <class T>
bool GearBox<T>::isReversed() const {
    return m_isReversed;
}

template <class T>
void GearBox<T>::setGear(bool gear) {
    if (m_shifter != NULL) {
        m_targetGear = gear;
    }
}

template <class T>
bool GearBox<T>::getGear() const {
    if (m_shifter != NULL) {
        return m_shifter->Get();
    } else {
        return false;
    }
}

template <class T>
void GearBox<T>::PIDWrite(float output) {
    for (uint32_t i = 0; i < m_motors.size(); i++) {
        if (!m_isReversed) {
            m_motors[i]->Set(output);
        } else {
            m_motors[i]->Set(-output);
        }
    }

    updateGear();
}

template <class T>
void GearBox<T>::updateGear() {
    if (m_shifter == NULL || m_targetGear == m_shifter->Get()) {
        return;
    }

    for (uint32_t i = 0; i < m_motors.size(); i++) {
        if (std::fabs(m_motors[i]->Get()) < 0.12) {
            return;
        }
    }

    // if ((m_pid->IsEnabled() && std::fabs(m_encoder->GetRate()) > 50) ||
    //     !m_pid->IsEnabled()) {
    m_shifter->Set(m_targetGear);
    // }
}

template <class T>
bool GearBox<T>::onTarget() {
#if 0
    if (!m_havePID) {
        return false;
    }

    return std::fabs(m_pid->GetError()/100.f) <= m_pid->GetTolerance() &&
        std::fabs(m_pid->GetDeltaError()) <= m_pid->GetTolerance();
#endif
    return m_pid->OnTarget();
}

template <class T>
void GearBox<T>::resetPID() {
    m_pid->Reset();
    m_pid->Enable();
}