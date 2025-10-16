// #define MAIN
#ifndef MAIN
#include <TFT_eSPI.h>
#include <main.h>

#include <iomanip>
#include <sstream>

Motor left_motor = Motor(2, 1, ENCODER_LEFT, 0);
Motor right_motor = Motor(43, 44, ENCODER_RIGHT, 1);
pidController left_pid = pidController(0.15, 0, 0, &left_motor);
pidController right_pid = pidController(0.15, 0, 0, &right_motor);
int update_speed_flag = 0;
int stay_in_flag = 0;
int last_stay_in_time = 0;
TFT_eSPI tft = TFT_eSPI();

void left_motor_ticks_interrupt() {
  left_motor.incrementTicks();
}
void right_motor_ticks_interrupt() {
  right_motor.incrementTicks();
}

void set_drive_flag() {
  update_speed_flag = 1;
}

void set_stay_in_flag() {
  if (millis() - last_stay_in_time > 1000 &&
      digitalRead(REFLECTANCE_PIN) == 0) {
    stay_in_flag = 1;
    last_stay_in_time = millis();
  }
}

void drive_motors() {
  int reflectance = analogRead(PSD_PIN);
  std::stringstream stream;
  stream << std::setw(5) << std::setfill('0') << reflectance;
  tft.drawString(stream.str().c_str(), 0, 0);
  if (update_speed_flag) {
    left_pid.driveMotor();
    right_pid.driveMotor();
    update_speed_flag = 0;
  }
}

void stop() {
  left_pid.setDesired(0);
  right_pid.setDesired(0);
  int start = millis();
  while (800 < millis() - start) {
    drive_motors();
  }
}

void stay_in() {
  /*
  Function to help robot stay in the ring
  we back up and turn around;
  */
  tft.fillScreen(TFT_BLUE);
  left_pid.setDesired(-700);
  right_pid.setDesired(-700);
  left_pid.driveMotor();
  right_pid.driveMotor();
  int start = millis();
  while (2000 > millis() - start) {
    drive_motors();
  }
  right_pid.setDesired(700);
  start = millis();
  while (1500 > millis() - start) {
    drive_motors();
  }
  if (digitalRead(REFLECTANCE_PIN) != 0) {
    stay_in_flag = 0;
  }
}

void push() {
  left_pid.setDesired(900);
  right_pid.setDesired(900);
  while (!stay_in_flag) {
    drive_motors();
  }
  stay_in();
}

void search() {
  tft.fillScreen(TFT_GREEN);
  int count = 0;
  while (true) {
    // TODO: remove debugging

    Serial.print("count:");
    Serial.println(count++);

    // debug end
    int psd = analogRead(PSD_PIN);
    int start = millis();
    left_pid.setDesired(700);
    right_pid.setDesired(700);
    while (2000 > millis() - start) {
      if (stay_in_flag) {
        stay_in();
        left_pid.setDesired(700);
        right_pid.setDesired(700);
      }
      psd = analogRead(PSD_PIN);
      if (psd > 2000) {
        tft.drawNumber(psd, 0, 100);
        push();
      }
      drive_motors();
    }
    if (stay_in_flag) {
      stay_in();
    }
    psd = analogRead(PSD_PIN);
    if (psd > 2000) {
      tft.drawNumber(psd, 0, 100);
      push();
    }
    left_pid.setDesired(700);
    right_pid.setDesired(-700);
    start = millis();
    while (2000 > millis() - start) {
      if (stay_in_flag) {
        stay_in();
        left_pid.setDesired(700);
        right_pid.setDesired(-700);
      }
      psd = analogRead(PSD_PIN);
      if (psd > 2000) {
        tft.drawNumber(psd, 0, 100);
        push();
      }
      drive_motors();
    }
  }
}

void setup() {
  attachInterrupt(ENCODER_LEFT, left_motor_ticks_interrupt, RISING);
  attachInterrupt(ENCODER_RIGHT, right_motor_ticks_interrupt, RISING);
  pinMode(REFLECTANCE_PIN, INPUT);
  pinMode(15, OUTPUT);
  digitalWrite(15, HIGH);
  attachInterrupt(REFLECTANCE_PIN, set_stay_in_flag, FALLING);
  hw_timer_t* drive_timer = NULL;
  drive_timer = timerBegin(0, 80, true);
  timerAttachInterrupt(drive_timer, set_drive_flag, true);
  timerAlarmWrite(drive_timer, 200000, true);
  timerAlarmEnable(drive_timer);
  Serial.begin(115200);
  tft.init();
  tft.fillScreen(TFT_WHITE);
  tft.setTextColor(TFT_BLACK, TFT_WHITE, true);
  tft.setTextSize(2);
  delay(1000);
  search();
}

void loop() {
}

/// @brief Methods for the pid Controller class
/// @param K_prop
/// @param K_int
/// @param K_der
/// @param motor
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
  drive_speed = min(drive_speed, 255);
  drive_speed = max(drive_speed, 0);
  err_old2 = err_old;
  err_old = err_now;
  drive_speed_old = drive_speed;
  motor->drive(drive_speed, desired);
}

/// @brief Methods for the motor class
/// @param drive_plus
/// @param drive_minus
/// @param encoder_a
/// @param channel
Motor::Motor(int drive_plus, int drive_minus, int encoder_a, int channel) {
  this->drive_plus = drive_plus;
  this->drive_minus = drive_minus;
  pinMode(encoder_a, INPUT);
  pinMode(drive_plus, OUTPUT);
  pinMode(drive_minus, OUTPUT);
  ledcSetup(channel, 2000, 8);
  ledc_channel = channel;
}

void Motor::incrementTicks() {
  ticks++;
}

int Motor::getSpeed() {
  int speed = ticks * 1000 / (millis() - last_time);
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

#endif MAIN