#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pin tanimlamalari
const int trigPin = 9;
const int echoPin = 10;
const int yesilLed = 2;
const int sariLed = 3;
const int kirmiziLed = 4;
const int buzzer = 5;

// Sadece 3 esik degiskeni (Baslangicta 0)
int guvenli = 0;
int dikkat = 0;
int tehlikeli = 0;

long sure;
int mesafe;

void setup() {
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(yesilLed, OUTPUT);
  pinMode(sariLed, OUTPUT);
  pinMode(kirmiziLed, OUTPUT);
  pinMode(buzzer, OUTPUT);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Ayar Bekleniyor");
  lcd.setCursor(0, 1);
  lcd.print("Seri Porta Girin");

  Serial.println("==========================================");
  Serial.println("Sistem baslatilmadi. Deger bekleniyor...");
  Serial.println("Lutfen buyukten kucuge 3 deger girin:");
  Serial.println("(Guvenli, Dikkat, Tehlikeli) -> Orn: 20,15,10");
  Serial.println("==========================================");

  // Kullanici gecerli 3 deger girene kadar burada bekle
  while (guvenli == 0 || dikkat == 0 || tehlikeli == 0) {
    if (Serial.available() > 0) {
      degerleriOku();
    }
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Degerler Alindi!");
  lcd.setCursor(0, 1);
  lcd.print("Sistem Basliyor");
  delay(1500);
  lcd.clear();
}

void loop() {
  // Calisirken de yeni deger girilirse guncelle
  if (Serial.available() > 0) {
    degerleriOku();
  }

  // Mesafe olcumu
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  sure = pulseIn(echoPin, HIGH, 30000);

  if (sure == 0) {
    mesafe = 999;
  } else {
    mesafe = sure * 0.034 / 2;
  }

  // LCD mesafe bilgisi
  lcd.setCursor(0, 0);
  lcd.print("Mesafe: ");
  if (mesafe == 999) {
    lcd.print("--- cm   ");
  } else {
    lcd.print(mesafe);
    lcd.print(" cm    ");
  }

  lcd.setCursor(0, 1);

  // 1. GUVENLI BOLGE (Girilen 'guvenli' degerin ustu)
  if (mesafe > guvenli) {
    digitalWrite(yesilLed, HIGH);
    digitalWrite(sariLed, LOW);
    digitalWrite(kirmiziLed, LOW);
    digitalWrite(buzzer, LOW);
    lcd.print("Durum: Guvenli  ");
    delay(250);
  }
  // 2. DIKKAT BOLGESI ('dikkat' ile 'guvenli' arasi)
  else if (mesafe > dikkat) {
    digitalWrite(yesilLed, LOW);
    digitalWrite(sariLed, HIGH);
    digitalWrite(kirmiziLed, LOW);
    lcd.print("Durum: Dikkat!  ");
    digitalWrite(buzzer, HIGH);
    delay(100);
    digitalWrite(buzzer, LOW);
    delay(300);
  }
  // 3. TEHLIKELI BOLGE ('dikkat' degeri ve alti - 'tehlikeli' siniri)
  else if (mesafe > 0) {
    digitalWrite(yesilLed, LOW);
    digitalWrite(sariLed, LOW);
    digitalWrite(kirmiziLed, HIGH);
    lcd.print("Durum: TEHLIKE! ");
    digitalWrite(buzzer, HIGH);
    delay(100);
    digitalWrite(buzzer, LOW);
    // Mesafe girilen 'tehlikeli' degerin de altindaysa daha hizli uyar
    if (mesafe <= tehlikeli) {
      delay(50);
    } else {
      delay(150);
    }
  }
  // OLCUM HATASI / KOR NOKTA
  else {
    digitalWrite(yesilLed, LOW);
    digitalWrite(sariLed, LOW);
    digitalWrite(kirmiziLed, LOW);
    digitalWrite(buzzer, LOW);
    lcd.print("Olcum Bekleniyor");
    delay(250);
  }
}

void degerleriOku() {
  int g = Serial.parseInt();
  int d = Serial.parseInt();
  int t = Serial.parseInt();

  // Tampon bellekteki kalan bosluk/enter karakterlerini temizle
  while (Serial.available() > 0) {
    Serial.read();
  }

  // Buyukten kucuge dogru 3 sayi girildigini dogrula
  if (g > d && d > t && t > 0) {
    guvenli = g;
    dikkat = d;
    tehlikeli = t;

    Serial.print("Ayarlandi -> Guvenli: >");
    Serial.print(guvenli);
    Serial.print(" | Dikkat: >");
    Serial.print(dikkat);
    Serial.print(" | Tehlikeli: <=");
    Serial.println(tehlikeli);
  } else {
    Serial.println("HATA! Buyukten kucuge 3 sayi girin (Orn: 20,15,10)");
  }
}
