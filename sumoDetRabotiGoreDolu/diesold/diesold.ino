#include <Servo.h>

#define spin 1800
#define escape 1000

Servo servor;
Servo servol;
// Servo servo_flag_l;
// Servo servo_flag_d;
Servo turn_servo;
Servo push_rear_left;
Servo push_rear_right;

// enemy detection sensors
#define front_left      2
#define front_right     7
#define front_center    4
#define rear_left       12
#define rear_right      8
#define rear_center     13

// servo turning small wheels
#define TurningServo    9
#define TurnForward     90
#define TurnLeft        0
#define TurnRight       180

// line sensors
int f_left;
int f_right;
int r_left;
int r_right;

#define detect_line_fl() (f_left >= 200)
#define detect_line_fr() (f_right >= 100)
#define detect_line_rl() (r_left >= 100)
#define detect_line_rr() (r_right >= 100)

char detect_front_left = 1;
char detect_front_center = 1;
char detect_front_right = 1;
char detect_rear_left = 1;
char detect_rear_center = 1;
char detect_rear_right = 1;

#define detect_in_front() ((!detect_front_left && !detect_front_right && !detect_front_center) || (!detect_front_center))
#define detect_in_back() ((!detect_rear_left && !detect_rear_right && !detect_rear_center) || (!detect_rear_center))
#define detect_on_front_left() (!detect_front_left)
#define detect_on_rear_left() (!detect_rear_left)
#define detect_on_front_right() (!detect_front_right)
#define detect_on_rear_right() (!detect_rear_right)

void setup()
{ 
    servol.attach(5); // sreda - 1369
    servor.attach(6); // sreda - 1381
    turn_servo.attach(TurningServo);
    push_rear_left.attach(3);
    push_rear_right.attach(11);
    // servo_flag_l.attach(3);
    // servo_flag_d.attach(11);

    servol.writeMicroseconds(1375); // sreda1 
    servor.writeMicroseconds(1361); // sreda2

    push_rear_left.writeMicroseconds(1369);  // sreda1 
    push_rear_right.writeMicroseconds(1381); // sreda2

    pinMode(front_left, INPUT);
    pinMode(front_right, INPUT);
    pinMode(front_center, INPUT);
    pinMode(rear_left, INPUT);
    pinMode(rear_right, INPUT);
    pinMode(rear_center, INPUT);
    
    servol.writeMicroseconds(2000); // sreda1 
    servor.writeMicroseconds(2000); // sreda2

    // servo_flag_l.write(80);
    // servo_flag_d.write(75);
    
    // Serial.begin(9600);
}

void read_sensors()
{
  f_right = analogRead(A2); // bqlo: >100, 4erno:20<
  f_left = analogRead(A3);  // bqlo: >400, 4erno:40<
  r_left = analogRead(A0);  // bqlo: >100, 4erno:20<
  r_right = analogRead(A1); // bqlo: >100, 4erno:20<
  
  detect_front_left = digitalRead(front_left);
  detect_front_center = digitalRead(front_center);
  detect_front_right = digitalRead(front_right);
  detect_rear_left = digitalRead(rear_left);
  detect_rear_center = digitalRead(rear_center);
  detect_rear_right = digitalRead(rear_right);
  
  /*
  Serial.print(f_right);
  Serial.print(" ");
  Serial.print(f_left);
  Serial.print(" ");
  Serial.print(r_left);
  Serial.print(" ");
  Serial.println(r_right);
  */
}

void forward()
{
  servol.writeMicroseconds(2000);
  servor.writeMicroseconds(1000);
  
  turn_servo.write(TurnForward);
  push_rear_left.writeMicroseconds(1000); //sreda1
  push_rear_right.writeMicroseconds(2000); //sreda2
}

void backward()
{
  servol.writeMicroseconds(1000);
  servor.writeMicroseconds(2000);
  
  turn_servo.write(TurnForward);
  push_rear_left.writeMicroseconds(2000); //sreda1
  push_rear_right.writeMicroseconds(1000); //sreda2
}

void spin_left()
{
  servol.writeMicroseconds(2000);
  servor.writeMicroseconds(2000);
  
  turn_servo.write(TurnLeft);
  push_rear_left.writeMicroseconds(1000); //sreda1
  push_rear_right.writeMicroseconds(2000); //sreda2
}

void spin_right()
{
  servol.writeMicroseconds(1000);
  servor.writeMicroseconds(1000);
  
  turn_servo.write(TurnRight);
  push_rear_left.writeMicroseconds(1000); //sreda1
  push_rear_right.writeMicroseconds(2000); //sreda2
}

int detect_oponent()
{
  if(detect_in_front()){
    for (int i = 0; i<=spin; i++){
      forward();
      if(detect_line_rr() && detect_line_rl()){
        for (int i = 0; i<=escape; i++){
          spin_left();
        }
        break;
      }
      else if (detect_line_rl()){
        for (int i = 0; i<=escape; i++){
          spin_left();
        }
        break;
      }
      else if(detect_line_rr()){
        for (int i = 0; i<=escape; i++){
          spin_right();
        }
        break;
      }
      else if(detect_line_fr() || detect_line_fl()){
        for (int i = 0; i<=escape; i++){
          backward();
        }
        break;
      } 
    }
    return 1;
  }
  else if(detect_in_back()){
    for (int i = 0; i<=spin; i++){
      backward();
      if(detect_line_rr() && detect_line_rl()){
        for (int i = 0; i<=escape; i++){
          forward();
        }
        break;
      }
      else if (detect_line_rl()){
        for (int i = 0; i<=escape; i++){
          spin_left();
        }
        break;
      }
      else if(detect_line_rr()){
        for (int i = 0; i<=escape; i++){
          spin_right();
        }
        break;
      }
      else if(detect_line_fr() || detect_line_fl()){
        for (int i = 0; i<=escape; i++){
          backward();
        }
        break;
      } 
    }
    return 1;
  }
  else if(detect_on_front_left()){
    for (int i = 0; i<=spin; i++){
      spin_left();
      if (detect_line_rl()){
        for (int i = 0; i<=escape; i++){
          spin_left();
        }
        break;
      }
      else if(detect_line_rr()){
        for (int i = 0; i<=escape; i++){
          spin_left();
        }
        break;
      }
      else if(detect_line_fr() || detect_line_fl()){
        for (int i = 0; i<=escape; i++){
          spin_left();
        }
        break;
      }
    }
    return 1;
  }
    else if(detect_on_rear_left()){
    for (int i = 0; i<=spin; i++){
      spin_right();
      if (detect_line_rl()){
        for (int i = 0; i<=escape; i++){
          spin_right();
        }
        break;
      }
      else if(detect_line_rr()){
        for (int i = 0; i<=escape; i++){
          spin_right();
        }
        break;
      }
      else if(detect_line_fr() || detect_line_fl()){
        for (int i = 0; i<=escape; i++){
          backward();
        }
        break;
      }
    }
    return 1;
  }
  else if(detect_on_front_right()){
    for (int i = 0; i<=spin; i++){
      spin_right();
      if (detect_line_rl()){
        for (int i = 0; i<=escape; i++){
          spin_right();
        }
        break;
      }
      else if(detect_line_rr()){
        for (int i = 0; i<=escape; i++){
          spin_right();
        }
        break;
      }
      else if(detect_line_fr() || detect_line_fl()){
        for (int i = 0; i<=escape; i++){
          spin_right();
        }
        break;
      }
    }
    return 1;
  }
    else if(detect_on_rear_right()){
    for (int i = 0; i<=spin; i++){
      spin_left();
      if (detect_line_rl()){
        for (int i = 0; i<=escape; i++){
          spin_left();
        }
        break;
      }
      else if(detect_line_rr()){
        for (int i = 0; i<=escape; i++){
          spin_left();
        }
        break;
      }
      else if(detect_line_fr() || detect_line_fl()){
        for (int i = 0; i<=escape; i++){
          spin_left();
        }
        break;
      }
    }
    return 1;
  }
  else{
    return 0;
  }
}

int detect_end_of_ring()
{
  if (detect_line_fl() && detect_line_fr()){
    for (int i = 0; i<=spin; i++){
      backward();
      if (detect_oponent()){
        detect_oponent();
      }
        break;
      }
    return 1;
  }
  else if (detect_line_rl() && detect_line_rr()){
    for (int i = 0; i<=spin; i++){
      forward();
      if (detect_oponent()){
        detect_oponent();
      }
       break;
      }
      return 1;
    }
    else if (detect_line_fl()){
    for (int i = 0; i<=spin; i++){
      spin_right();
      if (detect_on_front_right()||detect_on_rear_right()){
        detect_oponent();
      }
      break;
    }
    return 1;
  }
  else if(detect_line_fr()){
    for (int i = 0; i<=spin; i++){
      spin_left();
      if (detect_on_front_left()||detect_on_rear_left()){
        detect_oponent();
      }
      break;
    }
    return 1;
  }
  else if (detect_line_rl()){
    for (int i = 0; i<=spin; i++){
      spin_right();
      if (detect_oponent()){
        for (int i = 0; i<=spin; i++){
          spin_left();
        }
        break;
      }
    }
    return 1;
  }
  else if (detect_line_rr()){
    for (int i = 0; i<=spin; i++){
      spin_left();
      if (detect_oponent()){
        for (int i = 0; i<=spin; i++){
          spin_right();
        }
        break;
      }
    }
    return 1;
  }else{
    forward();
   return 0;
  }
}

void loop()
{
  read_sensors();
  detect_end_of_ring();
  detect_oponent();
}
