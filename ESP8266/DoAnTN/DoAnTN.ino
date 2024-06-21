#include <Adafruit_Fingerprint.h>
#include <ESP8266WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <FirebaseESP8266.h>
#define WIFI_SSID "Duy Khôg Nến"
#define WIFI_PASSWORD "12345678"
#define API_KEY "jkAlgUNIdZ8r6QL96927m4NZuzDLfPHD9BqSL5T9"
#define DATABASE_URL "https://datn-trunghieu-default-rtdb.firebaseio.com/"
FirebaseData firebaseData;
LiquidCrystal_I2C lcd(0x27, 16, 2);  //SCL D1 SDA D2
#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
SoftwareSerial mySerial(12, 14);  // Chân D5 D6
#else
#define mySerial Serial1
#endif

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

uint8_t choice;
int button = 0;
uint8_t idadd;
int checkchamcong = 1;
int coi = 13;
int checkbutton = 0;


void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;
  delay(100);
  finger.begin(57600);
  if (!finger.verifyPassword()) {
    Serial.println("Không thể xác minh cảm biến.");
    while (true)
      ;
  } else {
    Serial.println("Tim thay cam bien.");
  }
  pinMode(coi, OUTPUT);
  pinMode(button, INPUT);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(3, 0);
  lcd.print("Connecting");
  lcd.setCursor(4, 1);
  lcd.print("to WiFi!");

  Serial.println("\nFingerprint Sensor Menu:");
  Serial.println("1. Them Van Tay");
  Serial.println("2. Cham Cong");
  Serial.println("3. Xoa van Tay");
  Serial.println("4. Xem nhan vien");


  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Firebase.begin(DATABASE_URL, API_KEY);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connected!");
  delay(2000);
  lcd.clear();
}

uint8_t readnumber(void) {
  uint8_t num = 0;
  while (num == 0) {
    while (!Serial.available())
      ;
    num = Serial.parseInt();
  }
  return num;
}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print("Dang cho...");
  Firebase.getInt(firebaseData, "/idprepareadd");
  choice = firebaseData.intData();
  if (choice != 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Them van tay");
    addFingerprint();
  }
  if (digitalRead(button) == LOW) {
    if (checkbutton == 0) {
      checkbutton = 1;
    }
  }
  if (checkbutton == 1) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Cham cong...");
    verifyFingerprint();
  }
}


void printMenu() {
  Serial.println("\nFingerprint Sensor Menu:");
  Serial.println("1. Them Van Tay");
  Serial.println("2. Cham Cong");
}


void viewstaff() {
  finger.getTemplateCount();
  int soluongvantay = finger.templateCount;
  Serial.print("\nSo luong van tay:");
  Serial.println(soluongvantay);
  delay(200);
}


// thêm vân tay mới
void addFingerprint() {
  Serial.println("Chuan bi lay dau van tay!!");
  Firebase.getInt(firebaseData, "/idprepareadd");
  idadd = firebaseData.intData();
  if (idadd == 0) {
    return;
  }
  Serial.print("So ID dang ky:");
  Serial.println(idadd);
  while (!getFingerprintEnroll())
    ;
}


// chấm công
void verifyFingerprint() {
  finger.getTemplateCount();
  int soluongvantay = finger.templateCount;
  Serial.print("\nSo luong van tay:");
  Serial.println(soluongvantay);
  while (checkchamcong > 0) {
    getFingerprintID();
  }
  checkchamcong = 1;
}


// xóa vân tay
void deleteFingerprint() {
  Serial.println("\nChuan bi xoa dau van tay");
  Serial.println("Nhap van tay can xoa");
  uint8_t iddelete = readnumber();
  if (iddelete == 0) {
    return;
  }
  Serial.print("Dang xoa ID:");
  Serial.println(iddelete);

  deleteFingerprint(iddelete);
}

//thêm vân tay
uint8_t getFingerprintEnroll() {

  int p = -1;
  Serial.print("Đang chờ ngón tay hợp lệ để đăng ký với tên:");
  Serial.println(idadd);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Lưu dấu vân tay thành công!!!");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.print("Lỗi giao tiếp");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.print("Lỗi hình ảnh");
        break;
      default:
        Serial.print("Lỗi không rõ");
        break;
    }
  }


  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Vây tay được xác nhận!!!");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Khong the tim thay tinh nang van tay");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Loi");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  digitalWrite(coi, HIGH);
  delay(500);
  digitalWrite(coi, LOW);
  lcd.setCursor(0, 1);
  lcd.print("Them lan 2");
  Serial.println("Bỏ ngón tay");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID: ");
  Serial.println(idadd);
  p = -1;
  Serial.println("Đặt lại cùng một ngón tay");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Vân tay được lưu");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Vây tay được xác nhận!!!");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Không thể tìm thấy tính năng vân tay");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Không thể tìm thấy tính năng vân tay");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  Serial.print("Tạo mô hình cho: ");
  Serial.println(idadd);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Vân tay đã khớp!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Dấu vân tay không khớp");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  Serial.print("ID: ");
  Serial.println(idadd);
  p = finger.storeModel(idadd);
  if (p == FINGERPRINT_OK) {
    Serial.println("Đã lưu!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Không thể lưu trữ ở vị trí đó!");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Lỗi lưu vào Flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Them van tay");
  lcd.setCursor(0, 1);
  lcd.print("Thanh cong!");
  digitalWrite(coi, HIGH);
  delay(500);
  digitalWrite(coi, LOW);
  delay(3000);
  lcd.clear();
  luufirebase();
  return true;
}

void luufirebase() {
  String path = "/dsnhanvien/" + String(idadd) + "/vantay";
  Firebase.setInt(firebaseData, path.c_str(), 1);
  Firebase.setInt(firebaseData, "idprepareadd", 0);
}

// chấm công
uint8_t getFingerprintID() {


  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }


  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
    checkchamcong = 0;
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  // found a match!
  Serial.print("Found ID #");
  Serial.print(finger.fingerID);
  Serial.print(" with confidence of ");
  Serial.println(finger.confidence);
  statuschamcong(finger.fingerID);
  lcd.setCursor(0, 1);
  lcd.print(finger.fingerID);
  lcd.setCursor(5, 1);
  lcd.print("Thanh cong!");
  checkbutton = 0;
  digitalWrite(coi, HIGH);
  delay(500);
  digitalWrite(coi, LOW);
  lcd.clear();
  return finger.fingerID;
}


int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) return -1;

  // found a match!
  Serial.print("Found ID #");
  Serial.print(finger.fingerID);
  Serial.print(" with confidence of ");
  Serial.println(finger.confidence);
  return finger.fingerID;
}


void statuschamcong(int findID) {
  int foundID = findID;
  String path = "/dsnhanvien/" + String(foundID) + "/statuschamcong";
  Firebase.setInt(firebaseData, path.c_str(), foundID);
}

uint8_t deleteFingerprint(uint8_t iddelete) {
  uint8_t p = -1;
  p = finger.deleteModel(iddelete);
  if (p == FINGERPRINT_OK) {
    Serial.println("Xoa Thanh Cong!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not delete in that location");
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
  } else {
    Serial.print("Unknown error: 0x");
    Serial.println(p, HEX);
  }

  return p;
}
