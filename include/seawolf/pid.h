/**
 * \file
 */

#ifndef __SEAWOLF_PID_INCLUDE_H
#define __SEAWOLF_PID_INCLUDE_H

#include "seawolf/timer.h"

/**
 * PID
 * \private
 */
typedef struct {
    /**
     * Timer used to determine time deltas
     * \private
     */
    Timer* timer;

    /**
     * Proportional coefficient
     * \private
     */
    double p;

    /**
     * Integral coefficient
     * \private
     */
    double i;

    /**
     * Derivative coefficient
     * \private
     */
    double d;

    /**
     * Set point
     * \private
     */
    double sp;

    /**
     * Linear region size
     * \private
     */
    double active_region;

    /**
     * Last error
     * \private
     */
    double e_last;

    /**
     * Error integral
     * \private
     */
    double e_dt;

    /**
     * Run state of the controller
     */
    bool paused;
} PID;

PID* PID_new(double sp, double p, double i, double d);
void PID_pause(PID* pid);
double PID_update(PID* pid, double pv);
void PID_resetIntegral(PID* pid);
void PID_setCoefficients(PID* pid, double p, double i, double d);
void PID_setSetPoint(PID* pid, double sp);
void PID_destroy(PID* pid);
void PID_setActiveRegion(PID* pid, double active_region);

#endif // #ifndef __SEAWOLF_PID_INCLUDE_H
