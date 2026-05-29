int ledpin[10] = {2,3,4,5,6,7,8,9,10,11};

void setup() {

  Serial.begin(9600);

  // 設定 LED 腳位
  for(int i=0; i<10; i++){
    pinMode(ledpin[i], OUTPUT);
  }
}

void loop() {

  // 讀取可變電阻
  int sensorValue = analogRead(A0);

  // 轉換成 0~10
  int ledBar = map(sensorValue, 0, 1023, 0, 10);

  // 控制 LED
  for(int i=0; i<10; i++){

    if(i < ledBar){
      digitalWrite(ledpin[i], HIGH);
    }
    else{
      digitalWrite(ledpin[i], LOW);
    }
  }
  // 顯示數值
  Serial.println(sensorValue);
  delay(10);
}