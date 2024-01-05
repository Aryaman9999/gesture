#include <Servo.h>;
#include <math.h>;
Servo servo[4][3];
const int servo_pin[4][3] = { {2, 3, 4}, {5, 6, 7}, {8, 9, 10}, {11, 12, 13} };
const double pi = 3.14159265;
bool active = true;
bool left = true;

double const T = 77.0; // length of tibia
double const F = 55.5; //length of femur
double const C = 27.5; // length of coxa
double height = 28; //height of robot ; can be adjusted

double loc_a(double a, double b, double c) {
  double angle_in_deg = acos((a * a + b * b - c * c) / (2 * a * b)) * 180 / pi;
  return angle_in_deg ;


}
double loc_s(double a, double b, double ang) {
  double side = sqrt((a * a) + (b * b) - 2 * a * b * cos(ang * pi / 180));
  return side ;


}
double itan(double a, double b) {
  double m = atan(a / b) * 180 / pi;
  return m;
}
double icos(double a, double b) {
  double m = acos(a / b) * 180 / pi;
  return m;
}

class roboservo {
  public:
    int leg;
    int no;
    double ang;
    int offset;
    int multi;
    int cc;
    void init(int b, int j) {
      leg = b;
      no = j;
    };
    void calib( int o, int m, int c) {
      offset = o;
      multi = m;
      cc = c;
    }
    void write(double a) {
      ang = offset + a * multi + cc;
      servo[leg][no].write(ang);
    }
};


class leg {
  public:
    int s_no;
    int x_coord;
    int y_coord;
    double length;
    double c_pos;
    double f_pos;
    double t_pos;
    roboservo coxa;
    roboservo femur;
    roboservo tibia;

    //        roboservo femur; //solve
    //        roboservo tibia; //solve
    leg(int n) {
      s_no = n;
      coxa.init(s_no, 0);
      femur.init(s_no, 1);
      tibia.init(s_no, 2);
    }
    move(int x, int y) {
      x_coord = x;
      y_coord = y;
      double l = sqrt((x * x) + (y * y));
      double m = l - C;
      double L = sqrt((height * height) + (m * m));
      c_pos = itan(y, x);
      f_pos = icos(height, L) + loc_a(F, L, T);
      t_pos = loc_a(F, T, L);
      coxa.write(c_pos);
      femur.write(f_pos);
      tibia.write(t_pos);
      //      Serial.println(l);
    }

    step(int x, int y) {
      x_coord = x;
      y_coord = y;
      f_pos +=  20;
      femur.write(f_pos);
      delay(100);
      double l = sqrt((x * x) + (y * y));
      double m = l - C;
      double L = sqrt((height * height) + (m * m));
      double c_pos = itan(y, x);
      double f_pos = icos(height, L) + loc_a(F, L, T);
      double t_pos = loc_a(F, T, L);
      coxa.write(c_pos);
      delay(50);
      femur.write(f_pos);
      tibia.write(t_pos);
      //      delay(1000);
    }
    void rotate_u() {
      double xi = x_coord + 35;
      double yi = y_coord + 35;
      length = sqrt((xi * xi) + (yi * yi));
    }
    rotate_m(int a) {
      double xi = x_coord + 35;
      double yi = y_coord + 35;
      double theta = itan(yi , xi);
      double new_x = length * cos((theta - a) * pi / 180);
      double new_y = length * sin((theta - a) * pi / 180);
      move(new_x - 35, new_y - 35);
      //      Serial.println(length);
    }
    rotate_s(int a) {
      double xi = x_coord + 35;
      double yi = y_coord + 35;
      length = sqrt((xi * xi) + (yi * yi));
      double theta = itan(yi , xi);
      double new_x = length * cos((a) * pi / 180);
      double new_y = length * sin((a) * pi / 180);
      step(new_x - 35, new_y - 35);
      //      Serial.println(length);
    }
};



leg leg_a(0);
leg leg_b(1);
leg leg_c(2);
leg leg_d(3);

void calibrate() {
  leg_a.coxa.calib(90, -1, 6);
  leg_a.femur.calib(0, 1, 6);
  leg_a.tibia.calib(180, -1, -6);

  leg_b.coxa.calib(90, 1, -6);
  leg_b.femur.calib(180, -1, 0);
  leg_b.tibia.calib(0, 1, 0);

  leg_c.coxa.calib(90, -1, 0);
  leg_c.femur.calib(0, 1, 0);
  leg_c.tibia.calib(180, -1, 0);

  leg_d.coxa.calib(90, 1, 0);
  leg_d.femur.calib(180, -1, -4);
  leg_d.tibia.calib(0, 1, -4);
};

void attach() {
  for (int i = 0; i < 4; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      servo[i][j].attach(servo_pin[i][j]);
      //      delay(100);
    }
  }
}


void set_height(int h) {
  for (int i = 0; height <= h; i++) {
    leg_a.move(leg_a.x_coord , leg_a.y_coord );
    leg_b.move(leg_b.x_coord , leg_b.y_coord );
    leg_c.move(leg_c.x_coord , leg_c.y_coord );
    leg_d.move(leg_d.x_coord , leg_d.y_coord );
    delay(15);
    height ++;
  }
}

void push_forward( int y) {
  for (int i = 0; i <= y; i++) {
    leg_a.move(leg_a.x_coord , leg_a.y_coord - 1);
    leg_b.move(leg_b.x_coord , leg_b.y_coord - 1);
    leg_c.move(leg_c.x_coord , leg_c.y_coord + 1);
    leg_d.move(leg_d.x_coord , leg_d.y_coord + 1);
    delay(3);
  }
}
void push_back( int y) {
  for (int i = 0; i <= y; i++) {
    leg_a.move(leg_a.x_coord , leg_a.y_coord + 1);
    leg_b.move(leg_b.x_coord , leg_b.y_coord + 1);
    leg_c.move(leg_c.x_coord , leg_c.y_coord - 1);
    leg_d.move(leg_d.x_coord , leg_d.y_coord - 1);
    delay(3);
  }
}

void rotate_r(int ang) {

  leg_a.rotate_u();
  leg_b.rotate_u();
  leg_c.rotate_u();
  leg_d.rotate_u();
  for (int i = 0; i <= ang; i++) {
    leg_a.rotate_m(1);
    leg_b.rotate_m(-1);
    leg_c.rotate_m(1);
    leg_d.rotate_m(-1);
    delay(10);
  }

}
void rotate_l(int ang) {
  leg_a.rotate_u();
  leg_b.rotate_u();
  leg_c.rotate_u();
  leg_d.rotate_u();
  for (int i = 0; i <= ang; i++) {
    leg_a.rotate_m(-1);
    leg_b.rotate_m(1);
    leg_c.rotate_m(-1);
    leg_d.rotate_m(1);
    delay(10);
  }
}


void walk_f() {
  if (left == false) {
    leg_a.step(50, 100);
    leg_c.step(50, 20);
    delay(150);
    push_forward(40);
    delay(150);
    left = ! left;
  }
  else if (left == true) {
    leg_b.step(50, 100);
    leg_d.step(50, 20);
    delay(150);
    push_forward(40);
    delay(150);
    left = ! left;
  }
  if (Serial.available() > 0) {
    while (Serial.available() > 0) {
      if (Serial.read() == 'f') {
        if (left == false) {
          leg_a.step(50, 100);
          leg_c.step(50, 20);
          delay(150);
          push_forward(40);
          delay(150);
          left = ! left;
        }
        else if (left == true) {
          leg_b.step(50, 100);
          leg_d.step(50, 20);
          delay(150);
          push_forward(40);
          delay(150);
          left = ! left;
        }

      }
    }
  }
  if (Serial.available() == 0) {
    if (left == false) {
      leg_a.step(50, 50);
      leg_c.step(50, 50);
      delay(150);

      left = ! left;
    }
    else if (left == true) {
      leg_b.step(50, 50);
      leg_d.step(50, 50);
      delay(150);

      left = ! left;
    }
  }
  active = true;
}
void walk_b() {
  if (left == false) {
    leg_c.step(50, 100);
    leg_a.step(50, 20);
    delay(150);
    push_back(40);
    delay(150);
    left = ! left;
  }
  else if (left == true) {
    leg_d.step(50, 100);
    leg_b.step(50, 20);
    delay(150);
    push_back(40);
    delay(150);
    left = ! left;
  }
  if (Serial.available() > 0) {
    while (Serial.available() > 0) {
      if (Serial.read() == 'b') {
        if (left == false) {
          leg_c.step(50, 100);
          leg_a.step(50, 20);
          delay(150);
          push_back(40);
          delay(150);
          left = ! left;
        }
        else if (left == true) {
          leg_d.step(50, 100);
          leg_b.step(50, 20);
          delay(150);
          push_back(40);
          delay(150);
          left = ! left;
        }
      }
    }
  }
  if (Serial.available() == 0) {
    if (left == false) {
      leg_c.step(50, 50);
      leg_a.step(50, 50);
      delay(150);

      left = ! left;
    }
    else if (left == true) {
      leg_d.step(50, 50);
      leg_b.step(50, 50);
      delay(150);

      left = ! left;
    }
  }
  active = true;
}

void turn_l() {
  if (left == false) {
    leg_a.rotate_s(25);
    leg_c.rotate_s(25);
    rotate_l(20);
    delay(150);
    left = ! left;
  }
  else if (left == true) {
    leg_b.rotate_s(65);
    leg_d.rotate_s(65);
    rotate_l(20);
    delay(150);
    left = ! left;
  }
  if (Serial.available() > 0) {
    while (Serial.available() > 0) {
      if (Serial.read() == 'l') {
        if (left == false) {
          leg_a.rotate_s(25);
          leg_c.rotate_s(25);
          rotate_l(20);
          delay(150);
          left = ! left;
        }
        else if (left == true) {
          leg_b.rotate_s(65);
          leg_d.rotate_s(65);
          rotate_l(20);
          delay(150);
          left = ! left;
        }
      }
    }
  }
  if (Serial.available() == 0) {
    if (left == false) {
      leg_a.rotate_s(45);
      leg_c.rotate_s(45);
      delay(150);
      left = ! left;
    }
    else if (left == true) {
      leg_b.rotate_s(45);
      leg_d.rotate_s(45);
      delay(150);
      left = ! left;
    }
  }
  active = true;
}

void turn_r() {
  if (left == false) {
    leg_a.rotate_s(65);
    leg_c.rotate_s(65);
    rotate_r(20);
    delay(150);
    left = ! left;
  }
  else if (left == true) {
    leg_b.rotate_s(25);
    leg_d.rotate_s(25);
    rotate_r(20);
    delay(150);
    left = ! left;
  }
  if (Serial.available() > 0) {
    while (Serial.available() > 0) {
      if (Serial.read() == 'r') {
        if (left == false) {
          leg_a.rotate_s(65);
          leg_c.rotate_s(65);
          rotate_r(20);
          delay(150);
          left = ! left;
        }
        else if (left == true) {
          leg_b.rotate_s(25);
          leg_d.rotate_s(25);
          rotate_r(20);
          delay(150);
          left = ! left;
        }
      }
    }
  }
  if (Serial.available() == 0) {
    if (left == false) {
      leg_a.rotate_s(45);
      leg_c.rotate_s(45);
      delay(150);
      left = ! left;
    }
    else if (left == true) {
      leg_b.rotate_s(45);
      leg_d.rotate_s(45);
      delay(150);
      left = ! left;
    }
  }
  active = true;
}
void setup() {
  Serial.begin(9600);
  attach();

  calibrate();

  leg_a.move(60, 60);
  leg_b.move(60, 60);
  leg_c.move(60, 60);
  leg_d.move(60, 60);
  delay(2000);
  set_height(50);
  delay(1500);
  //  turn_l();
  





}



void loop() {
  char command;
  if (Serial.available() > 0) {
    //    delay(300);
    if (active == true ) {
      //      active = false;
      command = Serial.read();
    }
    //    if (command == '\n'&&active==true) {
    switch (command) {
      case 'f' : active = false;
        command = ' ';
        walk_f();
        break;
      case 'l' :  active = false;
        command = ' ';
        turn_l();
        break;
      case 'r' : active = false;
        command = ' ';
        turn_r();
        break;
      case 'b' : active = false;
        command = ' ';
        walk_b();
        break;


        //      }
    }
  }
}
