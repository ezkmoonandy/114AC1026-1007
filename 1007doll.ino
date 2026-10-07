const int buttonPin = 2;  
const int RledPin = 3;    
const int GledPin = 4;    
const int BledPin = 5;    

// 變數設定
int buttonState = 0;      // 讀取按鈕狀態
int ledState = LOW;       // LED 閃爍狀態
bool ButtonPressed = false;

unsigned long previousMillis = 0;   // 記錄 LED 上一次更新的時間
unsigned long previousMillis2 = 0;  // 記錄遊戲計時/結果顯示時間
const long interval = 1000;         // 閃爍間隔 (1000ms)
const long Gametime = 5000;         // 遊戲時間 (5000ms = 5秒)
const long result = 3000;           // 結束白燈維持時間 (3000ms = 3秒)
bool Gamestart = false;             // 遊戲是否開始
int hits = 0;                       // 按壓次數計數
int color = 0;                      // 燈光顏色編號 (0:白, 1:紅, 2:綠, 3:藍)

void setup() {
  // 初始化 RGB LED 接腳為輸出
  pinMode(RledPin, OUTPUT);
  pinMode(GledPin, OUTPUT);
  pinMode(BledPin, OUTPUT);
  
  // 初始化按鈕接腳為輸入
  pinMode(buttonPin, INPUT);
  
  // 初始化序列埠通訊
  Serial.begin(9600);
}

void loop() {
  unsigned long currentMillis = millis();
  unsigned long currentMillis2 = millis();
  buttonState = digitalRead(buttonPin);

  // ------------------ 狀態 1：遊戲尚未開始 / 待機閃爍 ------------------
  if (Gamestart == false) {
    // 閃爍計時 (每 1 秒切換一次亮滅)
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      if (ledState == LOW) {
        ledState = HIGH;
      } else {
        ledState = LOW;
      }
    }

    // 依據 color 變數控制 RGB LED 閃爍模式
    if (color == 1) {
      if (ledState == LOW) {
        digitalWrite(RledPin, LOW);   // 閃紅燈
        digitalWrite(GledPin, HIGH);
        digitalWrite(BledPin, HIGH);
      } else {
        digitalWrite(RledPin, HIGH);
        digitalWrite(GledPin, HIGH);
        digitalWrite(BledPin, HIGH);
      }
    } else if (color == 2) {
      if (ledState == LOW) {
        digitalWrite(RledPin, HIGH);
        digitalWrite(GledPin, HIGH);
        digitalWrite(BledPin, LOW);   // 閃藍燈
      } else {
        digitalWrite(RledPin, HIGH);
        digitalWrite(GledPin, HIGH);
        digitalWrite(BledPin, HIGH);
      }
    } else if (color == 3) {
      if (ledState == LOW) {
        digitalWrite(RledPin, HIGH);
        digitalWrite(GledPin, LOW);    // 閃綠燈
        digitalWrite(BledPin, HIGH);
      } else {
        digitalWrite(RledPin, HIGH);
        digitalWrite(GledPin, HIGH);
        digitalWrite(BledPin, HIGH);
      }
    } else if (color == 0) {
      if (ledState == LOW) {
        digitalWrite(RledPin, LOW);   // 閃白燈
        digitalWrite(GledPin, LOW);
        digitalWrite(BledPin, LOW);
      } else {
        digitalWrite(RledPin, HIGH);
        digitalWrite(GledPin, HIGH);
        digitalWrite(BledPin, HIGH);
      }
    }

    // 當結果顏色閃爍滿 4 秒後，把顏色重置回 0 (白燈)
    if (color != 0 && (currentMillis - previousMillis2 >= 6200)) {
      color = 0;
    }

    // 按下按鈕觸發開始遊戲 (此處以原本 INPUT 邏輯為例，若使用 INPUT_PULLUP 請維持你現有的按鈕判斷)
    if (buttonState == HIGH && !ButtonPressed) {
      ButtonPressed = true;
    } else if (buttonState == LOW && ButtonPressed) {
      ButtonPressed = false;
      previousMillis = currentMillis; 
      hits = 0;                       
      digitalWrite(RledPin, HIGH);
      digitalWrite(GledPin, HIGH);
      digitalWrite(BledPin, HIGH);
      Gamestart = true;              // 開始遊戲
    }
  } 
  // ------------------ 狀態 2：遊戲進行中 ------------------
  else if (Gamestart == true) {
    // 判斷 5 秒遊戲時間是否結束
    if (currentMillis - previousMillis >= Gametime) {
      // 遊戲結束後的 3 秒顯示白燈時間是否結束
      if (currentMillis2 - previousMillis2 >= result) {
        Gamestart = false;            // 回到待機狀態
        previousMillis2 = millis();   // 紀錄剛回到待機的時間，用來計算結果顏色閃爍的 4 秒
      } else {
        // 5 秒剛到，顯示白燈 (結束標示)
        digitalWrite(RledPin, LOW);
        digitalWrite(GledPin, LOW);
        digitalWrite(BledPin, LOW);

        // 計算成績對應的顏色
        if (hits <= 10) {
          color = 1;      // 紅燈
        } else if (10 < hits && hits <= 20) {
          color = 2;      // 藍燈
        } else if (hits > 20) {
          color = 3;      // 綠燈
        }
      }
    } else {
      // 遊戲進行中：連擊計數
      previousMillis2 = currentMillis2;
      
      if (buttonState == HIGH && ButtonPressed == false) {
        ButtonPressed = true;
        hits = hits + 1;             
        Serial.println(hits);        
        digitalWrite(RledPin, LOW);  // 按下亮紅燈回饋
        digitalWrite(GledPin, HIGH);
        digitalWrite(BledPin, HIGH);
      } else if (buttonState == LOW && ButtonPressed == true) {
        ButtonPressed = false;
        digitalWrite(RledPin, HIGH); 
        digitalWrite(GledPin, HIGH);
        digitalWrite(BledPin, HIGH);
      }
    }
  }
}