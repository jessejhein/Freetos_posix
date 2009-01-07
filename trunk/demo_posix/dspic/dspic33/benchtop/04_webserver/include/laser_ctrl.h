/************************************************************************************************
 * File:            laser_ctrl.h
 * Description:     structures for laser_ctrl variables
 ***********************************************************************************************/

#ifndef LASER_CTRL_H_
#define LASER_CTRL_H_



#define LASER_NUM             2

struct laser_ctrl_t {
    unsigned int enable[LASER_NUM];
};

struct laser_set_t {
    float current_set[LASER_NUM];
};

struct laser_get_t {
    float power[LASER_NUM];
    float tec[LASER_NUM];
};

extern struct laser_ctrl_t laser_ctrl;
extern struct laser_set_t laser_set;
extern struct laser_get_t laser_get;

#endif /* LASER_CTRL_H_ */
