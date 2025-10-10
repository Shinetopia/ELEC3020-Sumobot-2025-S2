#include <TFT_eSPI.h>
#include <main.h>

Motor left_motor = Motor(1, 2, 3, 10, 0);
Motor right_motor = Motor(43, 44, 18, 17, 1);
TFT_eSPI tft = TFT_eSPI();

void left_motor_ticks_interrupt() {
  left_motor.incrementTicks();
}
void right_motor_ticks_interrupt() {
  right_motor.incrementTicks();
}

void setup() {
  attachInterrupt(3, left_motor_ticks_interrupt, RISING);
  attachInterrupt(18, right_motor_ticks_interrupt, RISING);
  pidController left_pid = pidController(0.15, 0, 0, &left_motor);
  pidController right_pid = pidController(0.15, 0, 0, &right_motor);
  tft.init();
  tft.setTextSize(3);
  tft.fillScreen(TFT_WHITE);
  tft.setTextColor(TFT_BLACK, TFT_WHITE, true);
  while (true) {
    left_pid.setDesired(700);
    right_pid.setDesired(700);
    for (int i = 0; i < 50; i++) {
      left_pid.driveMotor();
      right_pid.driveMotor();
      delay(200);
    }
    left_pid.setDesired(-700);
    for (int i = 0; i < 50; i++) {
      left_pid.driveMotor();
      right_pid.driveMotor();
      delay(200);
    }
  }
}

void loop() {
}

pidController::pidController(float K_prop, float K_int, float K_der,
                             Motor* motor) {
  Kp = K_prop;
  Ki = K_prop;
  Kd = K_der;
  this->motor = motor;
  desired = 700;
}

void pidController::setDesired(int desired_speed) {
  desired = desired_speed;
}

void pidController::driveMotor() {
  err_now = abs(desired) - motor->getSpeed();
  int drive_speed = drive_speed_old + Kp * (err_now - err_old) +
                    Ki * (err_now + err_old) / 2 +
                    Kd * (err_now - 2 * err_old + err_old2);
  drive_speed = min(drive_speed, 256);
  drive_speed = max(drive_speed, -256);
  tft.drawNumber(drive_speed, 0, 30);
  err_old2 = err_old;
  err_old = err_now;
  drive_speed_old = drive_speed;
  motor->drive(drive_speed, desired);
}

Motor::Motor(int drive_plus, int drive_minus, int encoder_a, int encoder_b,
             int channel) {
  this->encoder_a = encoder_a;
  this->encoder_b = encoder_b;
  this->drive_plus = drive_plus;
  this->drive_minus = drive_minus;
  pinMode(encoder_a, INPUT);
  pinMode(encoder_b, INPUT);
  pinMode(drive_plus, OUTPUT);
  pinMode(drive_minus, OUTPUT);
  ledcSetup(channel, 2000, 8);
  ledc_channel = channel;
}

void Motor::incrementTicks() {
  ticks++;
  tft.drawNumber(ticks, 0, 60);
}

int Motor::getSpeed() {
  int speed = ticks * 1000 / (millis() - last_time);
  tft.drawNumber(speed, 0, 0);
  this->ticks = 0;

  last_time = millis();
  return speed;
}

void Motor::drive(int speed, int desired) {
  if (desired > 0) {
    ledcDetachPin(drive_minus);
    digitalWrite(drive_minus, LOW);
    ledcAttachPin(drive_plus, ledc_channel);
    ledcWrite(ledc_channel, speed);
  }
  if (desired < 0) {
    ledcDetachPin(drive_plus);
    digitalWrite(drive_plus, LOW);
    ledcAttachPin(drive_minus, ledc_channel);
    ledcWrite(ledc_channel, speed);
  }
}