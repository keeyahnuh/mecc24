void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Bonjour");
  delay(1000);
  Serial.println("Hello");
  delay(1000);
}
