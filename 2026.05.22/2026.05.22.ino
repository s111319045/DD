int speakerPin = 10; 

int length = 15; // 音符總數
char notes[] = "ccggaagffeeddc "; // 樂譜
int beats[] = { 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 4 };
int tempo = 300;
  
// 定義 8 個音階對應的 LED 腳位
int ledPins[] = { 2, 3, 4, 5, 6, 7, 8, 9 };

void playTone(int tone, int duration) {
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
    digitalWrite(speakerPin, HIGH);
    delayMicroseconds(tone);
    digitalWrite(speakerPin, LOW);
    delayMicroseconds(tone);
  }
}
  
void playNote(char note, int duration) {
  char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
  int tones[] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956 };
  
  // 檢查目前的音符是哪一個音階
  for (int i = 0; i < 8; i++) {
    if (names[i] == note) {
      
      digitalWrite(ledPins[i], HIGH); // 1. 撥放前，點亮對應的 LED
      
      playTone(tones[i], duration);   // 2. 撥放音符
      
      digitalWrite(ledPins[i], LOW);  // 3. 撥放完，熄滅 LED
    }
  }
}
  
void setup() {
  pinMode(speakerPin, OUTPUT);
  
  // 使用迴圈將 Pin 2 到 Pin 9 全部設定為輸出模式
  for (int i = 0; i < 8; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
}
  
void loop() {
  for (int i = 0; i < length; i++) {
    if (notes[i] == ' ') {
      delay(beats[i] * tempo); // 休止符
    } else {
      playNote(notes[i], beats[i] * tempo);
    }
  
    // 音符之間的微小停頓
    delay(tempo / 2); 
  }
}