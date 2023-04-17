include <SimpleDHT.h>
#define pinDHT11 6 //data pin
SimpleDHT11 dht11;
int value = 0;
float voltage;
float R1 = 63000.0;
float R2 = 33000.0;
const int analogIn = A0;
int mVperAmp = 100;
int RawValue = 0;
int ACSoffset = 2500;
double voltage2 = 0;
double amps2 = 0;
void setup() {
Serial.begin(9600);

}

void loop() {
RawValue = analogRead(analogIn);
voltage2 = (RawValue / 1024.0) 5000;
amps2 = ((voltage2 - ACSoffset) / mVperAmp);

value = analogRead(A1);
voltage = value (5.0/1024)((R1 + R2)/R2);
Serial.print("Voltage = ");
Serial.println(voltage);

Serial.print("Raw Value = "); 
Serial.print(RawValue);
Serial.print("\t mV = "); // formatting endl to next line may change 
Serial.print(voltage2, 3);
Serial.println(amps2, 3);


byte temperature = 0;
byte humidity = 0;
dht11.read(pinDHT11, &temperature, &humidity, NULL);
int temperature2 = (temperature 1.8 + 32);
Serial.print("temperature = ");
Serial.print(temperature);
Serial.println("C");
Serial.print("temperature = ");
Serial.print(temperature2);
Serial.println("F");
Serial.print("humidity = ");
Serial.print(humidity);
Serial.println(" % ");
delay(2000);
}