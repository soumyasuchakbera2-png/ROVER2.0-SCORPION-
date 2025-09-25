/*
MAKE THIS FOR MY ROVER USING TINKERCAD(SOUMYASUCHAK...)
*/
// Motor A
int enA = 5;
int in1 = 3;
int in2 = 2;

// Motor B
int enB = 6;
int in3 = 8;
int in4 = 7;

// RC receiver pins
int receiver_pins[] = {A0, A1, A2, A3, A4, A5}; // CH0–CH5
int receiver_values[] = {0, 0, 0, 0, 0, 0};
int res_min = 1100;
int res_max = 1900;
int working_range = 255;

// CH6 and CH7 used for LEDs (not relays anymore)
int ch6_pin = A6;
int ch7_pin = A7;
int led_ch6 = 4;
int led_ch7 = 9;

boolean prt = true;
int mode = 0; // -1: no signal, 0: ready, 1: low speed, 2: high speed

void setup() {
  // Motor control pins
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  // Mode LEDs
  pinMode(11, OUTPUT); // LED1
  pinMode(12, OUTPUT); // LED2
  pinMode(13, OUTPUT); // LED3

  // CH6/CH7 output LEDs
  pinMode(led_ch6, OUTPUT);
  pinMode(led_ch7, OUTPUT);

  Serial.begin(115200);

  // Startup LED animation
  setLED(1); delay(300);
  setLED(1); delay(300);
  setLED(2); delay(300);
  setLED(3); delay(300);
  setLED(0);
}

void loop() {
  receive();
  setModeLED();
  readChannelLEDs();

  int m1 = 0;
  int m2 = 0;
  int rot = receiver_values[0];  // CH0 = steering

  // Mode-specific motor logic
  if (mode == 1) {
    m1 = receiver_values[1] / 2 - rot;  // CH1 = throttle
    m2 = receiver_values[1] / 2 + rot;
  } else if (mode == 2) {
    m1 = receiver_values[1] - rot / 2;
    m2 = receiver_values[1] + rot / 2;
  }

  mpower(1, m1);
  mpower(2, m2);
}

int rp = 0;
void receive() {
  receiver_values[rp] = map(pulseIn(receiver_pins[rp], HIGH, 25000), res_min, res_max, -working_range, working_range);
  rp++;
  if (rp == 6) rp = 0;

  boolean activevalues = true;

  for (int i = 0; i < 6; i++) {
    if (prt) {
      Serial.print("CH");
      Serial.print(i);
      Serial.print(" : ");
      Serial.print(receiver_values[i]);
      Serial.print(",\t");
    }
    if (receiver_values[i] < -500) {
      activevalues = false;
    }
  }

  mode = 0;
  if (!activevalues) {
    mode = -1;
  } else if (receiver_values[4] > -100) {
    mode = 2;
  } else if (receiver_values[5] > -100) {
    mode = 1;
  }

  if (prt) {
    Serial.println();
  }
}

void setModeLED() {
  if (mode == -1) {
    setLED(0);
  } else if (mode == 0) {
    setLED(1);
  } else if (mode == 1) {
    setLED(2);
  } else if (mode == 2) {
    setLED(3);
  }
}

void setLED(int led) {
  for (int i = 1; i < 4; i++) {
    digitalWrite(10 + i, (led == i) ? LOW : HIGH);
  }
}

void mpower(int motor, int spd) {
  int rotation = 0;
  if (spd > 0) {
    rotation = 1;
  } else if (spd < 0) {
    rotation = -1;
    spd *= -1;
  }
  if (spd > 255) spd = 255;

  int pwm, pA, pB;
  if (motor == 1) {
    pwm = enA;
    pA = in1;
    pB = in2;
  } else if (motor == 2) {
    pwm = enB;
    pA = in3;
    pB = in4;
  } else {
    return;
  }

  if (rotation == 0) {
    digitalWrite(pA, LOW);
    digitalWrite(pB, LOW);
  } else if (rotation == 1) {
    digitalWrite(pA, HIGH);
    digitalWrite(pB, LOW);
  } else if (rotation == -1) {
    digitalWrite(pA, LOW);
    digitalWrite(pB, HIGH);
  }

  analogWrite(pwm, spd);
}

// Read CH6/CH7 and control LEDs
void readChannelLEDs() {
  int val_ch6 = pulseIn(ch6_pin, HIGH, 25000);
  int val_ch7 = pulseIn(ch7_pin, HIGH, 25000);

  digitalWrite(led_ch6, (val_ch6 > 1200 && val_ch6 < 1800) ? HIGH : LOW);
  digitalWrite(led_ch7, (val_ch7 > 1200 && val_ch7 < 1800) ? HIGH : LOW);
}
