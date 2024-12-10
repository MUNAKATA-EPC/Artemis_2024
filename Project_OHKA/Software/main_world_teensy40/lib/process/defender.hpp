#ifndef _DEFENDER_HPP
#define _DEFENDER_HPP

extern void defender_init();
extern void defender_process(bool cam_mode, int cam_deg_value, int cam_dis_value, int at_cam_deg, int at_cam_dis);

extern int defender_mode;

#endif