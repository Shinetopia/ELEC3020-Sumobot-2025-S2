#define ENCODER_LEFT 3
#define ENCODER_RIGHT 18

#define REFLECTANCE_PIN 10
#define PSD_PIN 16

class Motor {
 private:
  int ticks;
  long last_time;
  int encoder_a;
  int drive_plus;
  int drive_minus;
  int ledc_channel;

 public:
  Motor() = default;
  Motor(int drive_plus, int drive_minus, int encoder_a, int channel);
  void incrementTicks();
  int getSpeed();
  void drive(int speed, int desired);
};

class pidController {
 private:
  float Kp;
  float Ki;
  float Kd;
  Motor* motor;
  int err_now;
  int err_old;
  int err_old2;
  int drive_speed_old;
  int desired;
  int actual;

 public:
  pidController(float K_prop, float K_integral, float K_derivative,
                Motor* motor);
  void setDesired(int desired_speed);
  void getMotor();
  void driveMotor();
};

void drive_motors();
void stay_in();