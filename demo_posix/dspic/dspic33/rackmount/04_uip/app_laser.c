/************************************************************************************************
 * File:            app_laser.c
 * Description:     periodically updates laser control values 
 ***********************************************************************************************/

#include <define.h>
#include <unistd.h>
#include <laser_ctrl.h>

struct laser_ctrl_t laser_ctrl;
struct laser_set_t laser_set;
struct laser_get_t laser_get;

/*
 * laser control task
 */
void* tskLaserCtrl(void* ptr)
{
    //Initialization
    laser_get.tec[0] = 25.3;
    laser_get.tec[1] = 25.1;

    laser_set.current_set[0] = 246.3;
    laser_set.current_set[1] = 125.4;
    
    //Process loops here
    start_process();
    {
        laser_get.power[0] += 1.0;
        if(laser_get.power[0] > 5000) laser_get.power[0] = 0;
        laser_get.power[1] += 2.0;
        if(laser_get.power[1] > 5000) laser_get.power[1] = 0;
        
        laser_get.tec[0] += 0.1;
        if(laser_get.tec[0] > 5000) laser_get.tec[0] = 0;
        laser_get.tec[1] += 0.2;
        if(laser_get.tec[1] > 5000) laser_get.tec[1] = 0;

        sleep(1);
    }
    end_process();
}
