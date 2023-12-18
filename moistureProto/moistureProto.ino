#define a 2
void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
int moisture= analogRead(a);
int mappedValue = map(moisture, 0, 4095, 0, 255);
mappedValue = constrain(mappedValue, 60, 20);

Serial.println(mappedValue);
delay(1000);
}
