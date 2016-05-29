// наборы поправочных коэффициентов по температуре и влажности
float ham[2] = {35.0, 65.0};
float t[4] = {10.0, 20.0, 30.0, 40.0};
float kk[4][2] = {
  {1.078, -0.0016666667},
  {1.02, -0.0006666667},
  {1.00, -0.0060000000},
  {0.90, -0.0066666667}
};
float RoT[2];

const float rl = 10000.0; //Сопротивление в делителе на tgs2600
const float v = 5.0;   //напряжение питания датчика

const int sensorPin = A0;    // select the input pin for the tgs2600
boolean voc_enable=false;
void initVOC(bool enab){
  voc_enable=enab;
}

word getVOC(float curt, float curh){
  if (voc_enable){
  return (word)calcR(curt, curh, get_R(analogRead(sensorPin)));
  }else{
    return 0;
  }
}

float get_R(int sen) {
  return (v / (float(sen) / 1024.0 * v) - 1.0) * rl;
}

float calcR(float new_t, float new_h, float r) {
  int i0 = getI0(new_t);
  calcRoT(i0, new_h);
  return r / calcK(i0, new_t);
}

int getI0(float new_t) {
  for (int i = 1; i < 4; i++) {
    if (new_t <= t[i]) {
      return i;
    }
  }
  return 2;
}

float get_k(float k1[2], float new_h) {
  return k1[0] + k1[1] * (new_h - ham[0]);
}

void calcRoT(int i0, float new_h) {
  RoT[0] = get_k(kk[i0 - 1], new_h);
  RoT[1] = get_k(kk[i0], new_h);
}

float calcK(int i0, float new_t) {
  float dt = new_t - t[i0 - 1];
  return RoT[0] + dt * (RoT[1] - RoT[0]) / (t[i0] - t[i0 - 1]);
}

