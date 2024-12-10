#ifndef _ENGELLINE_HPP
#define _ENGELLINE_HPP

extern void line_process();
extern void reset_line_process();
extern bool is_line_detected();
extern bool is_engelline_detected();
extern bool is_line_halfout();
extern bool is_line_movement();
extern int get_line_deg();
extern int get_raw_line_deg();
extern int get_count_of_detcted_sensors();

#endif