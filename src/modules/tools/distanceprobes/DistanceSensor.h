/*
    This file is part of Smoothie (http://smoothieware.org/). The motion control part is heavily based on Grbl (https://github.com/simen/grbl).
    Smoothie is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
    Smoothie is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
    You should have received a copy of the GNU General Public License along with Smoothie. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DISTANCESENSOR_H
#define DISTANCESENSOR_H

#include <map>
#include <stdint.h>

class DistanceSensor
{
public:
    virtual ~DistanceSensor() {}

    // Load config parameters using provided "base" names.
    virtual void UpdateConfig(uint16_t module_checksum, uint16_t name_checksum) {}

    // Return Distance
    virtual float get_distance() { return -1.0F; }

    typedef std::map<char, float> sensor_options_t;
    virtual bool set_optional(const sensor_options_t& options) { return false; }
    virtual bool get_optional(sensor_options_t& options) { return false; }
    virtual void get_raw() {}
};

#endif
