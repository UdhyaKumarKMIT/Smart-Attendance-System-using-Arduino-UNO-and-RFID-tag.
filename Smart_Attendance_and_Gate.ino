#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <LiquidCrystal.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN); 

//LCD Configuration
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//Servo motor 
Servo myservo;  
Servo ldrServo; 

int sensorval;
int readsuccess;
byte readcard[4];
char str[32] = "";
String StrUID;

const int ldrPin = A4;  
const int myservoPin= A2;
const int ldrservoPin=A5;
const char studentInfo[][4][50] = {
    {"3988D2CF", "Udhya","2022503051","Computer Science"},
    {"69E7E832", "Mahes", "2022503057", "Mathematics"},
    // Add more rows for additional students
};

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  lcd.begin(16, 2);
  myservo.attach(myservoPin);
  myservo.write(0); 
  ldrServo.attach(ldrservoPin);  

  Serial.println("CLEARDATA");
  Serial.println("LABEL,Date,Time,RFID UID,Name,Register Number,Department"); 
  
  lcd.print("Place your card:");
  delay(1000);

  Serial.println("Scan PICC to see UID...");
  Serial.println("");
}

void loop() {
  lcd.clear();
  lcd.print("Place your card ");

  readsuccess = getid();

  if (readsuccess) {
   
    lcd.clear();
    String dateTime = "DATE,TIME"; 
    int index = getinfo(StrUID);
    if (index != -1) {
       Serial.println((String) "DATA," + dateTime + "," + StrUID + "," + studentInfo[index][1] + "," + studentInfo[index][2] + "," + studentInfo[index][3]);
       lcd.setCursor(0,0);  //Syntax lcd.setCursor(col, row) Parameters lcd
       lcd.print("Welcome     ");
       lcd.setCursor(0,1);
       lcd.print(studentInfo[index][1]);
    
      
    // Rotate the existing servo to 100 degrees
    myservo.write(100);
    delay(4000); // Hold for 3 seconds
    myservo.write(0); 
     lcd.clear();
    }
     else
  { 
      lcd.clear();
      lcd.print("Invalid User");
  }
    delay(1000); // Wait for a second before next scan
  }
 

  sensorval = analogRead(ldrPin); 
  delay(100);
  if (sensorval < 700) {
    digitalWrite(LED_BUILTIN, HIGH); 
    ldrServo.write(90);  
  } else {
    digitalWrite(LED_BUILTIN, LOW);
    ldrServo.write(0);  
  }
}

int getid() {
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return 0;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {
    return 0;
  }
  Serial.println("THE UID OF THE SCANNED CARD IS:");

  for (int i = 0; i < 4; i++) {
    readcard[i] = mfrc522.uid.uidByte[i];
    array_to_string(readcard, 4, str);
    StrUID = str;
  }
  mfrc522.PICC_HaltA();
  return 1;
}

int getinfo(String id) {
    for (int i = 0; i < sizeof(studentInfo) / sizeof(studentInfo[0]); i++) {
        if (id.equals(studentInfo[i][0])) {
            return i;
        }
    }
    return -1; 
}

void array_to_string(byte array[], unsigned int len, char buffer[]) {
  for (unsigned int i = 0; i < len; i++) {
    byte nib1 = (array[i] >> 4) & 0x0F;
    byte nib2 = (array[i] >> 0) & 0x0F;
    buffer[i * 2 + 0] = nib1 < 0xA ? '0' + nib1 : 'A' + nib1 - 0xA;
    buffer[i * 2 + 1] = nib2 < 0xA ? '0' + nib2 : 'A' + nib2 - 0xA;
  }
  buffer[len * 2] = '\0';
}
