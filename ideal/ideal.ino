// SENIOR DESIGN PROJECT: BMS PROTOTYPE

// variables
#define bot A2
#define top A1
#define currSensor A0

#define discharge1 2
#define discharge2 3
#define charger 8

int amountOfPacks = 0;  // This is the amount of variables in the array above.  This is calculated in setup.
int BV[] = { discharge1, discharge2 };
float BVnum[2];

// initialized variables
float adc_top = 0.0;
float adc_bot = 0.0;
float vin_top = 0.0;
float vin_bot = 0.0;
float vout_top = 0.0;
float vout_bot = 0.0;
float ref_voltage = 5.0;
float BV1 = 0.0;
float BV2 = 0.0;
float wattage = 0.0;

// voltage sensor values
float R1 = 30000.0;  // stock arduino resistors for voltage divider
float R2 = 7500.0;

// current sensor values
int sens = 66;
int offsetVoltage = 2500;
int rawVal = 0;
double voltage = 0;
double amps = 0; 

float BV1offset = 0.0;  // offsets for circuit variation, verify with voltmeter on cells
float BV2offset = 0.0;
float delayfactor = 0.0;

void setup() {
  amountOfPacks = sizeof(BV) / sizeof(int);
  for (int i = 0; i < amountOfPacks; i++) {
    pinMode(BV[i], OUTPUT);
  }

  pinMode(charger, OUTPUT);

  Serial.begin(9600);
}

// functions
void read_voltages() {
  // Read the Analog Input  
  adc_top = analogRead(top);
  adc_bot = analogRead(bot);
  
  // Determine voltage at ADC input
  vout_bot = (adc_bot * ref_voltage) / 1024.0;
  vout_top = (adc_top * ref_voltage) / 1024.0;
  
  // Calculate voltage at divider input
  vin_bot = vout_bot*(R1+R2)/R2;
  vin_top = abs((vout_top*(R1+R2)/R2)-vin_bot);

  vin_bot = vin_bot + 1.5;
  vin_top = vin_top + 2;

  // Print results to Serial Monitor to 2 decimal places
  Serial.print("bot voltage = ");
  Serial.println(vin_bot, 2);

  Serial.print("top voltage = ");
  Serial.println(vin_top, 2);
  
  BV1 = vin_top;
  BV2 = vin_bot;
  BV[0] = BV1;
  BV[1] = BV2;

  rawVal = analogRead(currSensor);
  voltage = (rawVal/1024.0)*5000;
  amps = ((voltage - offsetVoltage)/sens);

  Serial.print("amps = ");
  Serial.println(amps, 3);

  wattage = (BV1 + BV2)*amps;
  Serial.print("wattage = ");
  Serial.println(wattage, 3);

  delay(500);
}

// void print_voltages() {
//   Serial.print("BV1: ");
//   Serial.print(BV1);
//   Serial.print("   BV2: ");
//   Serial.print(BV2);

//   Serial.print("      vin1: ");
//   Serial.print(vin1);
//   Serial.print("   vin2: ");
//   Serial.print(vin2);

//   Serial.print("      vout1: ");
//   Serial.print(vout1);
//   Serial.print("   vout2: ");
//   Serial.print(vout2);

//   Serial.print("      val1: ");
//   Serial.print(val1);
//   Serial.print("   val2: ");
//   Serial.println(val2);
// }

void digitalWriteAllOn() {
  for (int i = 0; i < amountOfPacks; i++) {
    digitalWrite(BV[i], HIGH);
  }
}

void digitalWriteAllOff() {
  for (int i = 0; i < amountOfPacks; i++) {
    digitalWrite(BV[i], LOW);
  }
}

int findLargestCell() {
  int largestIndex = 0;
  for (int i = 1; i < amountOfPacks; i++) {
    if (BV[i] > BV[largestIndex]) {
      largestIndex = i;
    }
  }

  return largestIndex;
}

void loop() {
  digitalWrite(charger, HIGH);
  digitalWriteAllOff();
  read_voltages();
  // print_voltages();

  // change the delay factor depending on the balancing of cells; longer delay for very unbalanced cells
  if (abs(BV1 - BV2) > 1) {
    delayfactor = 6;
  } else if (abs(BV1 - BV2) > 0.5 && abs(BV1 - BV2) < -1) {
    delayfactor = 4;
  } else if (abs(BV1 - BV2) > 0.2 && abs(BV1 - BV2) <= 0.5) {
    delayfactor = 2;
  } else {
    delayfactor = 0.5;
  }
  // delayfactor = 0.5;

  // DETERMINE IF CELLS NEED CHARGING OR DISCHARGING
  if (BV1 < 4.08 || BV1 > 4.12 || BV2 < 4.08 || BV2 > 4.12) {
    Serial.println("Charging/Balancing in progress now, not all cells in acceptable range");
    read_voltages();              // update voltage readings
    digitalWrite(charger, HIGH);  // close charging relay

    // DISCHARGE
    if (BV1 > 4.10 || BV2 > 4.10) {
      digitalWrite(charger, HIGH);  // close charging relay


      int largestCellIndex = findLargestCell();  // Find which cell is the largest

      // Is the largest cell larger than 4.10?
      if (BVnum[largestCellIndex] > 4.10) {
        Serial.print("Discharging Cell ");
        Serial.println(largestCellIndex);
        digitalWrite(BV[largestCellIndex], HIGH);  // open discharging relay
        digitalWrite(charger, HIGH);               // close charging relay
      }
      delay(6000 * delayfactor);  // discharge for 6 seconds

      /*
      // check up on cell 1
      if (BV1 > 4.10) {
        Serial.println("Discharging Cell 1");
        digitalWrite(discharge1, HIGH);  // open discharging relay
        digitalWrite(charger, HIGH);     // close charging relay
      }

      // check up on cell 2
      if (BV2 > 4.10) {
        Serial.println("Discharging Cell 2");
        digitalWrite(discharge2, HIGH);  // open discharging relay
        digitalWrite(charger, HIGH);     // close charging relay
      }
      */

      // close discharging relays once done discharging
      digitalWriteAllOff();
      digitalWrite(charger, HIGH);
      Serial.println("Stabilizing voltage for 6 seconds");
      delay(6000);

      Serial.println("Updated readings: ");
      read_voltages();
      // print_voltages();
      delay(6000);
    }

    // check if cells need charging and charge if so
    if (BV1 <= 4.08 || BV2 <= 4.08) {
      Serial.println("Charging now...");
      read_voltages();
      // print_voltages();
      delay(500);

      // connect charger to circuit via relay #8 (charger)
      digitalWriteAllOff();
      digitalWrite(charger, LOW);
      delay(15000 * delayfactor);  // charge for this amount of time based on delay factor
      digitalWrite(charger, HIGH);
      Serial.println("Charging stopped. Letting cells stabilize...");
      delay(6000);  // allow delay for cells to stabilize

      Serial.println("Updated readings: ");
      read_voltages();
      // print_voltages();
    }

    else {
      // keep everything closed
      digitalWrite(charger, HIGH);
      digitalWriteAllOff();
    }
  }
}