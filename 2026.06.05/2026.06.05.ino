#define ROW1 13
#define ROW2 12
#define ROW3 11
#define ROW4 10
#define ROW5 9
#define ROW6 8
#define ROW7 7
#define ROW8 6

#define COL1 5
#define COL2 4
#define COL3 3
#define COL4 A4
#define COL5 A3
#define COL6 A2
#define COL7 A1
#define COL8 A0

#define POT_PIN A5    // 可變電阻
#define BTN_PIN 2     // 按鈕 (D2)

// 定義腳位陣列方便用迴圈控制
const int rowPins[8] = {ROW1, ROW2, ROW3, ROW4, ROW5, ROW6, ROW7, ROW8};
const int colPins[8] = {COL1, COL2, COL3, COL4, COL5, COL6, COL7, COL8};

// 遊戲畫面緩衝區 (0: 熄滅, 1: 點亮)
byte screen[8][8] = {0};

// 遊戲變數
int playerX = 3;       // 戰機 X 座標 (0~7)
int bulletX = -1, bulletY = -1; // 子彈座標 (-1 表示不存在)
int enemyX = -1, enemyY = -1;   // 敵人座標 (-1 表示不存在)

unsigned long lastGameUpdate = 0;
const int gameSpeed = 300; // 敵人與子彈移動速度 (毫秒)

// --- 函式宣告與實作 ---

// 8x8 矩陣動態掃描顯示（已反轉電位邏輯）
void refreshScreen() {
  for (int r = 0; r < 8; r++) {
    digitalWrite(rowPins[r], LOW); // 【修改】啟動當前列（從 HIGH 改為 LOW）
    
    for (int c = 0; c < 8; c++) {
      if (screen[r][c] == 1) {
        digitalWrite(colPins[c], HIGH);  // 【修改】點亮（從 LOW 改為 HIGH）
      } else {
        digitalWrite(colPins[c], LOW);   // 【修改】熄滅（從 HIGH 改為 LOW）
      }
    }
    
    delayMicroseconds(500); // 視覺暫留延遲
    
    // 【修改】關閉目前列，避免殘影
    for (int c = 0; c < 8; c++) digitalWrite(colPins[c], LOW);
    digitalWrite(rowPins[r], HIGH);
  }
}

// 遊戲結束特效（閃爍全亮）
void gameOverEffect() {
  for (int k = 0; k < 3; k++) {
    // 全亮
    for (int y = 0; y < 8; y++) {
      for (int x = 0; x < 8; x++) screen[y][x] = 1;
    }
    unsigned long t = millis();
    while(millis() - t < 200) refreshScreen();
    
    // 全暗
    for (int y = 0; y < 8; y++) {
      for (int x = 0; x < 8; x++) screen[y][x] = 0;
    }
    t = millis();
    while(millis() - t < 200) refreshScreen();
  }
  // 重設遊戲
  bulletY = -1; 
  bulletX = -1;
  enemyY = -1;
  enemyX = -1;
}

// 讀取可變電阻與按鈕
void readInputs() {
  // 讀取可變電阻並對應到 0~7 的位置
  int potVal = analogRead(POT_PIN);
  playerX = map(potVal, 0, 1023, 0, 7);
  
  // 讀取按鈕 (低電位表示按下)，且目前畫面上沒有子彈時才能發射
  if (digitalRead(BTN_PIN) == LOW && bulletY == -1) {
    bulletX = playerX;
    bulletY = 6; // 發射在戰機上方一格
  }
}

// 更新遊戲世界狀態
void updateGame() {
  // 清空舊畫面
  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {
      screen[y][x] = 0;
    }
  }
  
  // 更新子彈位置
  if (bulletY != -1) {
    bulletY--;
    if (bulletY < 0) {
      bulletY = -1; // 子彈飛出螢幕
      bulletX = -1;
    }
  }
  
  // 生成或更新敵人
  if (enemyY == -1) {
    // 有 30% 機率生成新敵人
    if (random(0, 10) < 3) {
      enemyX = random(0, 8);
      enemyY = 0;
    }
  } else {
    enemyY++;
    if (enemyY > 7) { // 敵人到底了
      enemyY = -1;
      enemyX = -1;
    }
  }
  
  // 碰撞偵測：子彈打中敵人
  if (bulletY != -1 && enemyY != -1 && bulletX == enemyX && bulletY == enemyY) {
    // 擊中！兩者消失
    bulletY = -1; bulletX = -1;
    enemyY = -1;  enemyX = -1;
  }
  
  // 碰撞偵測：敵人撞到戰機 (Game Over)
  if (enemyY == 7 && enemyX == playerX) {
    gameOverEffect();
    return;
  }

  // 將角色填入畫面緩衝區
  screen[7][playerX] = 1; // 畫戰機
  if (bulletY != -1) screen[bulletY][bulletX] = 1; // 畫子彈
  if (enemyY != -1)  screen[enemyY][enemyX] = 1;   // 畫敵人（已補上分號）
}

// --- Arduino 主核心 ---

void setup() {
  // 初始化 8x8 顯示器腳位
  for (int i = 0; i < 8; i++) {
    pinMode(rowPins[i], OUTPUT);
    pinMode(colPins[i], OUTPUT);
    // 【修改】預設關閉電位對調
    digitalWrite(rowPins[i], HIGH);  
    digitalWrite(colPins[i], LOW); 
  }
  
  pinMode(BTN_PIN, INPUT_PULLUP); // 按鈕使用內建上拉電阻
  randomSeed(analogRead(A1));     // 使用 A1 做隨機種子
}

void loop() {
  // 1. 讀取輸入與遊戲邏輯更新
  readInputs();
  
  if (millis() - lastGameUpdate > gameSpeed) {
    lastGameUpdate = millis();
    updateGame();
  }
  
  // 2. 渲染畫面 (需要不斷掃描才能顯示)
  refreshScreen();
}
