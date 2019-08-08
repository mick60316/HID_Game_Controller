#include <SoftwareSerial.h>
#include <Wire.h>
#include <Timer.h>
  Timer timer;
  SoftwareSerial BTSerial(A2, A3); // RX | TX
  int I2C_Address = 0xA7 >> 1; // ADXL345 的 I2C 地址
  int X0, X1, Y0, Y1, Z1, Z0;
  float X, Y, Z;
  long cont = 0;
  byte buffer_move [] = {0xFD, 0x06, 127, 0, 0, 0, 0x00, 0x00,
                        };//data = {0xFD,0x06,x1,y1,x2,y2,btnByte1,btnByte1}
  int fps = 10 ;
  void setup()
  {
    Serial.begin(115200);
    timer.every(50, TimerCounter);//us
    BTSerial.begin(115200);
    Wire.begin();  //初始化 I2C
    setReg(0x2D, 0xA); // (打開電源, 設定輸出資料速度為 100 Hz)
    buffer_move[2] = 0;
    BTSerial.write(buffer_move , sizeof(buffer_move));
  }
  void loop()
  {
    timer.update();
    X0 = getData(0x32); // 取得 X 軸 低位元資料
    X1 = getData(0x33); // 取得 X 軸 高位元資料
    X = ((X1 << 8)  + X0) / 256.0;

    Y0 = getData(0x34); // 取得 Y 軸 低位元資料
    Y1 = getData(0x35); // 取得 Y 軸 高位元資料
    Y = ((Y1 << 8)  + Y0) / 256.0;

    Z0 = getData(0x36); // 取得 Z 軸 低位元資料
    Z1 = getData(0x37); // 取得 Y 軸 高位元資料
    Z = ((Z1 << 8)  + Z0) / 256.0;
    X = X * 10;
    Z = Z * 10;
    Serial.print(X);
    Serial.print(" ");
    Serial.println(X * 12.7);
    float angleValue = X * 12.7;
    if (angleValue < 15 && angleValue > -15)  // Go Forword
    {
      buffer_move[2] = 0;

    }
    else  // Turn left or right
    {
      buffer_move[2] = angleValue;
    }
    if (Z > 1)
    {
      buffer_move[6] = 8;
    }
    else
    {
      buffer_move[6] = 0;
    }
    setMsgToAndroid();
    delay(17);
  }

  void setMsgToAndroid ()
  {
    BTSerial.write(buffer_move , sizeof(buffer_move));

  }

  void setReg(int reg, int data) {
    Wire.beginTransmission(I2C_Address);
    Wire.write(reg); // 指定佔存器
    Wire.write(data); // 寫入資料
    Wire.endTransmission();
  }
  int getData(int reg) {
    Wire.beginTransmission(I2C_Address);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom(I2C_Address, 1);
    if (Wire.available() <= 1) {
      return Wire.read();
    }
  }
  void TimerCounter()
  {
    cont++;
    // Serial.println(cont);
  }
