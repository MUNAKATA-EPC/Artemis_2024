#ifndef _ATTACKER_HPP
#define _ATTACKER_HPP

extern void attacker_init();
extern void attacker_process(int speed);
extern void attacker_process_cam(int deg, int distance, int e_deg, int e_distance, int speed, int mode);

#endif