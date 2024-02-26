// SENIOR DESIGN PROJECT: BMS PROTOTYPE

// variables
#define voltageSensor1 A4
#define voltageSensor2 A5

// 3.99
// 2.33 



// 3.98 - 3.98
// 2.66 - 2.36


const int charger = 8;

float vout1 = 0.0;
float vout2 = 0.0;
float vin1 = 0.0;
float vin2 = 0.0;

float R1 = 30000.0;   // stock arduino resistors for voltage divider
float R2 = 7500.0;

float val1 = 0;
float val2 = 0;
float BV1 = 0;
float BV2 = 0;

float BV1offset = -0.3;   // offsets for circuit variation, verify with voltmeter on cells
float BV2offset = 0.0;
float delayfactor = 0;;

void setup() {
  // relays connected to digital pin + one, ex: relay 1 on pin 2
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(charger, OUTPUT);
  
  Serial.begin(9600);
}

// functions
void read_voltages() {
  // read all voltages
  val1 = analogRead(voltageSensor1);
  val2 = analogRead(voltageSensor2);

  // do math
  vout1 = (val1 * 5.0) / 1024.0;  // convert from digital to analog
  vout2 = (val2 * 5.0) / 1024.0;

  vin1 = vout1 * (R1 + R2) / R2;
  vin2 = vout2 * (R1 + R2) / R2;

  if (vin1 > vin2) {
    BV1 = vin1 - vin2;
    BV2 = vin2 + BV2offset;
  } else {
    BV1 = vin1 + BV1offset;
    BV2 = vin2 - vin1;
  }
}

void print_voltages() {
  Serial.print("BV1: ");
  Serial.print(BV1);
  Serial.print("   BV2: ");
  Serial.print(BV2);

  Serial.print("      vin1: ");
  Serial.print(vin1);
  Serial.print("   vin2: ");
  Serial.print(vin2);

  Serial.print("      val1: ");
  Serial.print(val1);
  Serial.print("   val2: ");
  Serial.println(val2);
}

void loop() {
  digitalWrite(charger, HIGH);
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  read_voltages();

  // change the delay factor depending on the balancing of cells; longer delay for very unbalanced cells
  if (abs(BV1-BV2) > 1) {delayfactor = 6;}
  else if (abs(BV1-BV2) > 0.5 && abs(BV1-BV2) <-1) {delayfactor = 4;}
  else if (abs(BV1-BV2) > 0.2 && abs(BV1-BV2) <= 0.5) {delayfactor = 2;}
  else {delayfactor = 0.5;}

  // DETERMINE IF CELLS NEED CHARGING OR DISCHARGING
  if (BV1 < 4.08 || BV1 > 4.12 || BV2 < 4.08 || BV2 > 4.12) {
    Serial.println("Charging/Balancing in progress now, not all cells in acceptable range");
    read_voltages();  // update voltage readings
    digitalWrite(charger, HIGH);           // close charging relay

    // DISCHARGE
    if (BV1 > 4.10 || BV2 > 4.10){
      digitalWrite(charger, HIGH);           // close charging relay

      // check up on cell 1
      if (BV1 > 4.10) {
        Serial.println("Discharging Cell 1");
        digitalWrite(2, HIGH);        // open discharging relay
        digitalWrite(charger, HIGH);   // close charging relay
      }

      // check up on cell 2
      if (BV2 > 4.10) {
        Serial.println("Discharging Cell 2");
        digitalWrite(3, HIGH);        // open discharging relay
        digitalWrite(charger, HIGH);   // close charging relay
      }
      delay(6000*delayfactor);        // discharge for 6 seconds

      // close discharging relays once done discharging
      digitalWrite(2, LOW);
      digitalWrite(3, LOW);
      digitalWrite(charger, HIGH);
      Serial.println("Stabilizing voltage for 6 seconds");
      delay(6000);

      read_voltages();
      Serial.println("Updated readings: ");
      print_voltages();
      delay(6000); 
    }
    
    // check if cells need charging and charge if so
    if (BV1 <= 4.08 || BV2 <= 4.08) {
      Serial.println("Charging now...");
      read_voltages();
      print_voltages();
      delay(500);

      // connect charger to circuit via relay #8 (charger)
      digitalWrite(2, LOW);
      digitalWrite(3, LOW);
      digitalWrite(charger, LOW);
      delay(15000*delayfactor);       // charge for this amount of time based on delay factor
      digitalWrite(charger, HIGH);
      Serial.println("Charging stopped. Letting cells stabilize...");
      delay(6000);                    // allow delay for cells to stabilize

      read_voltages();
      Serial.println("Updated readings: ");
      print_voltages(); 
    }
    
    else {
      // keep everything closed
      digitalWrite(charger, HIGH);
      digitalWrite(2, LOW);
      digitalWrite(3, LOW);
    }
  }
}