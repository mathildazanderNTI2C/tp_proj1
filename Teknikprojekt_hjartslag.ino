#define USE_ARDUINO_INTERRUPTS true
#include <PulseSensorPlayground.h> // inkludera PulseSensorPlayground-biblioteket
#include "U8glib.h" // Inkludera U8glib-biblioteket för OLED-skärmen
#include <Wire.h> //Inkludera Wire-biblioteket

U8GLIB_SSD1306_128X64 oled(U8G_I2C_OPT_NO_ACK);//så att skärmen ska fungera  

const int PulseWire = 0; //definerar pin-numret för pulsesensorn och LED
const int LED = LED_BUILTIN; 
int Threshold = 600; // anger ett tröskelvärde för pulssignalen så att ett hjärtslagv ska visas

PulseSensorPlayground pulseSensor; // Här skapar vi ett PulseSensorPlayground-objekt

const int maxDataPoints = 100;  // Max antal datapunkter i arrayen
int pulsdata[maxDataPoints];    // Array för att lagra pulsvärden
int dataCount = 0;               // Räknare för antalet datapunkter

void setup() { //Här gör vi en loop som körs vid varje start
  oled.setFont(u8g_font_5x8); //konfigurera oled-skärmen
  Serial.begin(9600); //Starta seriell kommunikation med en baudhastighet på 9600

//Konfigurera PulseSensor med inställningar
  pulseSensor.analogInput(PulseWire);
  pulseSensor.blinkOnPulse(LED);
  pulseSensor.setThreshold(Threshold);
//starta PulseSensor och visa meddelande
  if (pulseSensor.begin()) {
    Serial.println("Vi skapade ett pulseSensor-objekt!");
  }
}

void loop() { //huvudloop som körs flera gånger
  int pulsvarde = analogRead(A0); //läs av pulsvärdet från A0 pinnen 
  
  pulsdata[dataCount] = pulsvarde;  // Lägg till det nya pulsvärdet i arrayen
  dataCount = (dataCount + 1) % maxDataPoints;  // Runt-robin för att skriva över äldsta datapunkten

  // Skriv ut pulsvarde och beräknat BPM på seriell kommunikation 
  Serial.print("Pulsvarde: ");
  Serial.println(pulsvarde);
  int beraknatBPM = pulseSensor.getBeatsPerMinute();
  Serial.print("Beräknat BPM: ");
  Serial.println(beraknatBPM);

  updateOled("BPM: " + String(beraknatBPM)); //uppdatera OLED-skärmen med det nyaste BMP värdet
}

void updateOled(String text) { //funktion för att uppdatera OLED skärmen med text och pulsvärde
  oled.firstPage();
  do {
    // Rita pulsvärdena som en graf på OLED-skärmen
    for (int i = 0; i < maxDataPoints; ++i) {
      int index = (dataCount + i) % maxDataPoints;  // Hämta rätt index för cirkulär buffert
      int yPos = map(pulsdata[index], 0, 1023, 10, 63);//Mappa pulsvärdena till y position på skärmen 
      oled.drawPixel(i, yPos); //rita en pixel på skärmen för vardera datapunkt där endast ett värde kan befinna sig på en x punkt
    }

    // Skriv BPM-texten
    oled.drawStr(0, 10, text.c_str());
  } while (oled.nextPage());
}
