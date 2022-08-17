/*
 * source code to project:
 * https://youtu.be/rEeELsgImek
 */
#include <math.h>

/*
  2x16 LCD display
  
  The circuit (MKR1000):
   LCD RS -> pin 6
   LCD Enable pin to digital pin 7
   LCD D4 pin to digital pin 5
   LCD D5 pin to digital pin 4
   LCD D6 pin to digital pin 3
   LCD D7 pin to digital pin 2
   LCD R/W pin to ground
   LCD VSS pin to ground
   LCD VCC pin to 5V
   10K resistor:
   +5V and ground
   wiper to LCD VO pin (pin 3)
*/
#include <LiquidCrystal.h>
const int rs = 6, en = 7, db4 = 5, db5 = 4, db6 = 3, db7 = 2;
LiquidCrystal lcd1(rs, en, db4, db5, db6, db7);

/*
  2x16 LCD display, I2C
*/
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd2(0x27, 16, 2);

/*
   Temperature sensors:
   (VMA320) temp1 to pin A0
   (VMA320) temp2 to pin A1
   (TMP36) temp3 to pin A2
*/
const int temp1Pin = A0, temp2Pin = A1, temp3Pin = A2;
double temp1 = 0, temp2 = 0, temp3 = 0;

/* get VMA320 sensor value */
double VMA320Value(int RawADC)
{
  double temp;
  temp = log(10000.0 / (1024.0 / RawADC - 1)); // for pull-up configuration
  temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * temp * temp )) * temp );
  temp = temp - 273.15;            // Convert Kelvin to Celcius
  return temp;
}

/* get TMP36 sensor value */
double TMP32Value(int RawADC)
{
  double t = 3300 / 1024;
  double   val =  t * RawADC;
  val = -40 + val / (750 / 65); //tmp36 -40 - 125cels | 750mV = 25cels
  return val;
}

void setup()
{
  Serial.begin(9600);
  // set up the LCD's number of columns and rows:
  lcd1.begin(16, 2);
  lcd2.init();
}

void loop()
{
  temp1 = VMA320Value(analogRead(temp1Pin));
  updateLCDs();
  delay(1000);

  temp2 = VMA320Value(analogRead(temp2Pin));
  updateLCDs();
  delay(1000);

  temp3 =  TMP32Value(analogRead(temp3Pin));
  updateLCDs();
  delay(1000);

  Serial.println("t1: " + String(temp1) + " | t2: " + String(temp2) + " | t3: " + String(temp3));
  
  delay(7000);
}

void updateLCDs()
{
  lcd1.clear();
  lcd1.setCursor(0, 0);
  lcd1.print("Temp 1: " + String(temp1, 1) + " C");
  lcd1.setCursor(0, 1);
  lcd1.print("Temp 2: " + String(temp2, 1) + " C");

  lcd2.backlight();
  lcd2.clear();
  lcd2.setCursor(0, 0);
  lcd2.print("Temp 3: " + String(temp3, 1) + " C");
}
