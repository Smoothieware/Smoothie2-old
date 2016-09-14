/*
    This file is part of Smoothie (http://smoothieware.org/). The motion control part is heavily based on Grbl (https://github.com/simen/grbl).
    Smoothie is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
    Smoothie is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
    You should have received a copy of the GNU General Public License along with Smoothie. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SRC_MODULES_TOOLS_DISTANCEPROBES_DISTANCEPROBES_H_
#define SRC_MODULES_TOOLS_DISTANCEPROBES_DISTANCEPROBES_H_

#include "libs/Module.h"
#include "libs/Pin.h"
#include "DistanceSensor.h"
#include "DistanceProbesPublicAccess.h"

class Gcode;

class DistanceProbes : public Module{
    public:
        DistanceProbes();
        ~DistanceProbes();
        void on_module_loaded();
        void on_gcode_received(void* argument);


    private:
        void load_config();
        void on_get_public_data(void* argument);
        void on_set_public_data(void* argument);
        uint16_t set_m_code;
        uint16_t get_m_code;
        uint16_t name_checksum;
        struct {
                    bool is_corexy:1;
                    bool is_delta:1;
                    bool is_rdelta:1;
                    bool is_scara:1;
                };
        DistanceSensor *sensor[3];
 };



#endif /* SRC_MODULES_TOOLS_DISTANCEPROBES_DISTANCEPROBES_H_ */
