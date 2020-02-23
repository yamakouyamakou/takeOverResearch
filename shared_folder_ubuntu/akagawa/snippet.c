#define USE_USBCON
#if (ARDUINO >= 100)
 #include <Arduino.h>
#else
 #include <WProgram.h>
#endif

#include <ros.h>
#include <std_msgs/Int16.h>
#include <MsTimer2.h>

#define START_A 200
#define END_A 753
#define START_B 210
#define END_B 763
#define DEG_180 553
#define START_OFFSET 3

int analogPinA = 0;     //ポテンショメーターの真ん中の端子をアナログピンの0番に接続
int analogPinB = 1;     //ポテンショメーターの真ん中の端子をアナログピンの1番に接続
                       //両端の端子は，GNDと+5Vに接続
int step_val = 0;
float deg_before;

int valA = 0;           //読み取った値を格納する変数
int valB = 0;           //読み取った値を格納する変数
int valC = 0;

ros::NodeHandle  nh;

void flash() {
  float raw_deg = 0.0;
  int diff_deg = 0;
  static boolean output = HIGH;
  valA = analogRead(analogPinA);
  valB = analogRead(analogPinB);
  if(valA < END_A && valA > START_A){
    valA = analogRead(analogPinA) - START_A;
    valB = 0;
  }else{
    valB = analogRead(analogPinB) - START_B + DEG_180;
    valA = 0;
  }
  valC = valA + valB;
  valC = valC + START_OFFSET;
 
  diff_deg = valC - deg_before;
  if(diff_deg > 200 && diff_deg < 700){
    valC = analogRead(analogPinA) - START_A;
  } else if( diff_deg < -200 && diff_deg > -700 ){
    valC = analogRead(analogPinB) - START_B + DEG_180;
  }
  deg_before = valC;
  digitalWrite(13, output);
  output = !output;
}

void servo_cb( const std_msgs::Int16& cmd_msg){
  // step_motor
  //cr
  if(cmd_msg.data > 0){
    digitalWrite(8,HIGH); 
    for(int x = 0; x < cmd_msg.data * 10; x++) {
      digitalWrite(9,HIGH); 
      delay(1); 
      digitalWrite(9,LOW); 
      delay(1); 
    }
  }else if(cmd_msg.data < 0){
    //ccr
    digitalWrite(8,LOW); 
    for(int x = 0; x < cmd_msg.data * 10; x++) {
      digitalWrite(9,HIGH);
      delay(1);
      digitalWrite(9,LOW);
      delay(1);
    }
  }
  //end step
}

void init_servo(){
  step_val = servo_control();
  if(step_val > 0.0){
        digitalWrite(8,LOW);
        for(int x = 0; x < step_val*10; x++) {
          digitalWrite(9,HIGH); 
          delay(1); 
          digitalWrite(9,LOW); 
          delay(1); 
        }
      }else{
        step_val = step_val * -1;
        digitalWrite(8,HIGH);
        for(int x = 0; x < step_val*10; x++) {
          digitalWrite(9,HIGH); 
          delay(1); 
          digitalWrite(9,LOW); 
          delay(1); 
        }
      }  
}
/*
int get_analogsensor(){
  int valA = 0;           //読み取った値を格納する変数
  int valB = 0;           //読み取った値を格納する変数
  int valC = 0;
  valA = analogRead(analogPinA);
  valB = analogRead(analogPinB);
  if(valA < END_A && valA > START_A){
    valA = analogRead(analogPinA) - START_A;
    valB = 0;
  }else{
    valB = analogRead(analogPinB) - START_B + DEG_180;
    valA = 0;
  }
  valC = valA + valB;
  valC = valC + START_OFFSET;
  return valC;
}
*/
int servo_control(){
  double step_pulse = 0.0;
  int deg = 0;
  deg = valC;
  if(deg < 553){
    step_pulse = deg / 3.072;
    step_pulse *= -1;
  } else {
    step_pulse = (int)( 1106 - deg ) / 3.072;
  }
  return step_pulse;
}

ros::Subscriber<std_msgs::Int16> sub("servo", servo_cb);
std_msgs::Int16 deg;
ros::Publisher pub_deg("degree", &deg);
void setup(){
  pinMode(8,OUTPUT); 
  pinMode(9,OUTPUT);
  pinMode(13, OUTPUT);
  MsTimer2::set(10, flash);
  MsTimer2::start();
  //Serial.begin(9600);
  
  init_servo();
  //位置が微妙にずれている場合があるので、少し時間をおいてから、もう一度初期化を行う
  delay(500);
  init_servo();
  //位置が微妙にずれている場合があるので、少し時間をおいてから、もう一度初期化を行う(3回くらいやれば、だいたいいい感じになる)
  delay(500);
  init_servo();
  nh.initNode();
  nh.subscribe(sub);
  nh.advertise(pub_deg);
  
}

void loop(){
  
  float raw_deg = 0.0;
  raw_deg = (valC / 3.072) - 360;
  if(raw_deg < 0){
    raw_deg *= -1;
  }
  //Serial.println(raw_deg);
  deg.data = raw_deg;
  pub_deg.publish( &deg );
  nh.spinOnce();
  delay(1);
  
}