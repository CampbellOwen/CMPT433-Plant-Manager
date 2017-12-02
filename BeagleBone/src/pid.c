typedef struct
{
  float derState; // Last position input
  float integratState; // Integrator state
  float integratMax, // Maximum and minimum
  integratMin; // allowable integrator state
  float integratGain, // integral gain
  propGain, // proportional gain
  derGain; // derivative gain
} SPid;

float UpdatePID(SPid * pid, float error, float position) {
  float pTerm, dTerm, iTerm;
  pTerm = pid->propGain * error; // calculate the proportional term

  // calculate the integral state with appropriate limiting
  pid->integratState += error;

  // Limit the integrator state if necessary
  if (pid->integratState > pid->integratMax)
  {
    pid->integratState = pid->integratMax;
  }
  else if (pid->integratState < pid->integratMin)
  {
    pid->integratState = pid->integratMin;
  }

  // calculate the integral term
  iTerm = pid->integratGain * pid->integratState;

  // calculate the derivative
  dTerm = pid->derGain * (pid->derState - position);
  pid->derState = position;
  return pTerm + dTerm + iTerm;
}
