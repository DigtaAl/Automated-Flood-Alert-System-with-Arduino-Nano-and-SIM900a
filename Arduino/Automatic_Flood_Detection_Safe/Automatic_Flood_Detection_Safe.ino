#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Inisialisasi pin untuk sensor ultrasonik
const int trigPin = 2;
const int echoPin = 4;
const int relay1 = 5;
const int relay2 = 6;
int relayON = LOW;
int relayOFF = HIGH;

// Inisialisasi pin untuk modul GSM SIM900A
SoftwareSerial gsmSerial(11, 10); // RX, TX

// Nomor telepon yang akan menerima SMS dan panggilan
const char* phoneNumber = "+6281936087763"; // Ganti dengan nomor yang sesuai

// Tentukan status untuk tampilan LCD
enum DisplayState {
  DEVICE_NAME,
  FLOOD_STATUS
};

// Inisialisasi status tampilan
DisplayState displayState = DEVICE_NAME;

// Tanda untuk melacak tindakan SMS dan panggilan
bool smsSent = false;
bool callMade = false;

void setup() {
  Serial.begin(9600);
  gsmSerial.begin(9600);

  lcd.init();
  lcd.backlight();

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);

  digitalWrite(relay1, relayOFF);
  digitalWrite(relay2, relayOFF);

  // Mengecek koneksi GSM
  if (sendATCommand("AT")) {
    Serial.println("GSM Module is ready.");
  } else {
    Serial.println("Error: Unable to initialize GSM Module. Check connections.");
  }
}

void loop() {
  lcd.clear();
  long duration, distance;
  // Menguji jarak menggunakan sensor ultrasonik
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration * 0.0343) / 2;
  //Serial.print("Distance: ");
  //Serial.print(distance);
  //Serial.println(" cm");
  if (distance <= 30) {
    digitalWrite(relay1, relayON);
    digitalWrite(relay2, relayON);
    lcd.setCursor(0, 0);
    lcd.print("Status Banjir:");
    lcd.setCursor(0, 1);
    lcd.print("AWAS !! ");
    if (!callMade) {
      sendSMS(phoneNumber, "Peringatan Banjir! Ketinggian air adalah 70 cm, kondisi darurat. Waspadai kenaikan air. Segera pindahkan diri ke tempat yang lebih tinggi.");
      makeCall(phoneNumber);
      //Serial.println("TELPON AWAS !!");
      callMade = true;  // Setel tanda menjadi true setelah panggilan dibuat
      delay(5000);
    }
  } else if (distance <= 70 && distance > 30) {
    digitalWrite(relay1, relayOFF);
    digitalWrite(relay2, relayOFF);
    lcd.setCursor(0, 0);
    lcd.print("Status Banjir:");
    lcd.setCursor(0, 1);
    lcd.print("SIAGA !! ");
    if (!smsSent) {
      sendSMS(phoneNumber, "Peringatan Banjir! Ketinggian air adalah 30 cm, kondisi siaga. Air mulai naik, harap tetap waspada");
      //Serial.println("SMS SIAGA !!");
      smsSent = true;  // Setel tanda menjadi true setelah SMS dikirim
      delay(5000);
    }
  } else {
    digitalWrite(relay1, relayOFF);
    digitalWrite(relay2, relayOFF);
    if (smsSent || callMade) {
      smsSent = false;  // Reset tanda SMS saat status banjir berubah
      callMade = false; // Reset tanda panggilan saat status banjir berubah
    }
    lcd.setCursor(0, 0);
    lcd.print("Status Banjir:");
    lcd.setCursor(0, 1);
    lcd.print("AMAN");
  }
  delay(5000);
  /*
    switch (displayState) {
      case DEVICE_NAME:
        // Tampilkan nama perangkat
        lcd.setCursor(0, 0);
        lcd.print("  Fadelnadana");
        lcd.setCursor(0, 1);
        lcd.print(" Flood Detector");
        delay(3000);  // Tunggu 3 detik
        displayState = FLOOD_STATUS;  // Pindah ke status berikutnya
        break;

      case FLOOD_STATUS:
        delay(3000);  // Tunggu 3 detik
        displayState = DEVICE_NAME;  // Pindah kembali ke status nama perangkat
        break;
    }
  */
}

void sendSMS(const char* number, const char* message) {
  gsmSerial.println("AT+CMGF=1"); // Set mode PDU ke mode teks
  delay(1000);
  gsmSerial.print("AT+CMGS=\"");
  gsmSerial.print(number);
  gsmSerial.println("\"");
  delay(1000);
  gsmSerial.print(message);
  delay(1000);
  gsmSerial.write(26); // Karakter Ctrl+Z untuk mengirim SMS
  delay(1000);
}

void makeCall(const char* number) {
  gsmSerial.print("ATD");
  gsmSerial.print(number);
  gsmSerial.println(";");
  delay(15000); // Menunggu 15 detik atau lebih lama sesuai kebutuhan
  gsmSerial.println("ATH"); // Menutup panggilan
}

bool sendATCommand(const char* command) {
  gsmSerial.println(command);
  delay(1000);

  while (gsmSerial.available()) {
    if (gsmSerial.find("OK")) {
      return true;
    } else {
      return false;
    }
  }

  return false;
}
