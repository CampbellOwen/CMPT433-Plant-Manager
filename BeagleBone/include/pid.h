// CMPT 433: Project
// References:
// - "PID Without PhD" by Tim Wescott
//     - http://www.wescottdesign.com/articles/pid/pidWithoutAPhd.pdf
// - "A Method for Precision Closed-loop Irrigation Using a Modified PID Control Algorithm" by Martin Goodchild and Malcolm Jenkins
//     - http://www.sensorsportal.com/HTML/DIGEST/may_2015/Vol_188/P_2662.pdf

#ifndef _PID_H_
#define _PID_H_

#include <include/device_array.h>

typedef struct {
  uint32_t id;
  long long timestamp;
  float derivativeState;  // Last position input
  float integratorState;  // Integrator state
} pid_row_t;

typedef struct {
  pid_row_t* rows;
  int length;
  int max;
} pid_callback_args_t;

void PID_SavePIDdata( device_t* device, float derivativeState, float integratorState);
void PID_Update(device_t* device);
_Bool PID_Init(void);
void PID_Shutdown(void);

#endif
