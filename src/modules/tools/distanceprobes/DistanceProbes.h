/*
 * DistanceProbes.h
 *
 *  Created on: 7/09/2016
 *      Author: douglaspearless
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
