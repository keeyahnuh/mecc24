// SENIOR DESIGN PROJECT: BMS PROTOTYPE

// variables
#define voltageSensor1 A1
#define voltageSensor2 A2
#define voltageSensor3 A3
#define voltageSensor4 A4
#define voltageSensor5 A5
#define current A0

#define discharge1 2
#define discharge2 3
#define discharge3 4
#define discharge4 5
#define discharge5 6

#define charger 8

int amountOfPacks = 2;  // This is the amount of variables in the array above.  This is calculated in setup.
int BV[] = { discharge1, discharge2 };
const int BVsensors[] = { voltageSensor1, voltageSensor2 };
const int BVoffset[] = { 0, 0 };
float BVnum[2];

float vout1 = 0.0;
float vout2 = 0.0;
float vout3 = 0.0;
float vout4 = 0.0;
float vout5 = 0.0;

float vin1 = 0.0;
float vin2 = 0.0;
float vin3 = 0.0;
float vin4 = 0.0;
float vin5 = 0.0;

const float R1 = 30000.0;  // stock arduino resistors for voltage divider
const float R2 = 7500.0;

float val1 = 0;
float val2 = 0;
float val3 = 0;
float val4 = 0;
float val5 = 0;

float BV1 = 0;
float BV2 = 0;
float BV3 = 0;
float BV4 = 0;
float BV5 = 0;

float BV1offset = -0.3;  // offsets for circuit variation, verify with voltmeter on cells
float BV2offset = 0.0;
float BV3offset = 0.0;
float BV4offset = 0.0;
float BV5offset = 0.0;

float delayfactor = 0;

void setup() {
  // relays connected to digital pin + one, ex: relay 1 on pin 2
  amountOfPacks = sizeof(BV) / sizeof(int);
  for (int i = 0; i < amountOfPacks; i++) {
    pinMode(BV[i], OUTPUT);
  }

  pinMode(charger, OUTPUT);

  Serial.begin(9600);
}

// FIXME: CURRENT SENSOR
// int Sens = 66;  // Sensitivity in mV/A for the 30A version

// const int analogIn = A0;  // Analog input pin

// int OffsetVoltage = 2500;  // 0 Current offset Voltage in mV @ 0 amps

// int RawValue = 0;    // Init result variables
// double Voltage = 0;  
// double Amps = 0;     

// void currentSensors() {
//   RawValue = analogRead(analogIn);            // Read voltage from ASC712
//   Voltage = (RawValue / 1024.0) * 5000;       // Convert to mV
//   Amps = ((Voltage - OffsetVoltage) / Sens);  // Convert to amps


//   Serial.print("A/D Read Value = ");  // A/D read value
//   Serial.print(RawValue);             //

//   Serial.print("\t mV = ");  // ACS712 Output voltage
//   Serial.print(Voltage, 3);  //

//   Serial.print("\t Amps = ");  // Current measured
//   Serial.println(Amps, 3);     //

//   delay(2500);  // Wait a bit then do it again
// }

void read_voltages() {
  for (int i = 0; i < amountOfPacks; i++) {
    // read all voltages
    float val = analogRead(BVsensors[i]);

    // do math
    float vout = (val * 5.0) / 1024.0;  // convert from digital to analog
    float vin = vout1 * (R1 + R2) / R2;

    // Set battery voltage to vin plus offest
    BVnum[i] = vin + BVoffset[i];

    // If the current index isn't 0, subtract the previous value from the current value
    if (i != 0) {
      BVnum[i] -= BVnum[i - 1];
    }
  }
}

void print_voltages() {
  Serial.print("BV1: ");
  Serial.print(BV1);
  Serial.print("   BV2: ");
  Serial.print(BV2);
  // Serial.print("BV3: ");
  // Serial.print(BV3);
  // Serial.print("   BV4: ");
  // Serial.print(BV4);
  // Serial.print("BV5: ");
  // Serial.print(BV5);

  Serial.print("      vin1: ");
  Serial.print(vin1);
  Serial.print("   vin2: ");
  Serial.print(vin2);
  // Serial.print("      vin3: ");
  // Serial.print(vin3);
  // Serial.print("   vin4: ");
  // Serial.print(vin4);
  // Serial.print("      vin5: ");
  // Serial.print(vin5);

  // debugging: see voltage sensor raw values
  Serial.print("      val1: ");
  Serial.print(val1);
  Serial.print("   val2: ");
  Serial.println(val2);
  // Serial.print("      val3: ");
  // Serial.print(val3);
  // Serial.print("   val4: ");
  // Serial.println(val4);
  // Serial.print("      val5: ");
  // Serial.print(val5);
}

// void digitalWriteAllDischargers(PinStatus init) {
//   for (int i = 0; i < amountOfPacks; i++) {
//     digitalWrite(BV[i], init);
//   }
// }

int findLargestCell() {
  int largestIndex = 0;
  for (int i = 1; i < amountOfPacks; i++) {
    // Check if the current index is bigger then the last recorded idnex
    if (BVnum[i] > BVnum[largestIndex]) {
      largestIndex = i;  // Update the biggest index if it is
    }
  }

  return largestIndex;
}

bool isACellLargerThan(float input) {
  for (int i = 0; i < amountOfPacks; i++) {
    if (BVnum[i] > input)
      return true;
  }
  return false;
}

bool isACellSmallerThan(float input) {
  for (int i = 0; i < amountOfPacks; i++) {
    if (BVnum[i] < input)
      return true;
  }
  return false;
}

void loop() {
  digitalWrite(charger, LOW);
  // digitalWriteAllDischargers(LOW);
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  read_voltages();
  print_voltages();

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

  // DETERMINE IF CELLS NEED CHARGING OR DISCHARGING
  if (BV1 < 4.08 || BV1 > 4.12 || BV2 < 4.08 || BV2 > 4.12) {
    if (isACellLargerThan(4.12) || isACellSmallerThan(4.08)) {
      Serial.println("Charging/Balancing in progress now, not all cells in acceptable range");
      read_voltages();              // update voltage readings
      digitalWrite(charger, HIGH);  // close charging relay

      // DISCHARGE
      if (BV1 > 4.10 || BV2 > 4.10) {
      float largestValue = 4.10;
        if (isACellLargerThan(largestValue)) {
          digitalWrite(charger, HIGH);  // close charging relay


          int largestCellIndex = findLargestCell();  // Find which cell is the largest

          // Is the largest cell larger than 4.10?
          if (BVnum[largestCellIndex] > largestValue) {
            Serial.print("Discharging Cell ");
            Serial.println(largestCellIndex);
            digitalWrite(BV[largestCellIndex], HIGH);  // open discharging relay
            digitalWrite(charger, HIGH);               // close charging relay
          }
          delay(6000 * delayfactor);  // discharge for 6 seconds

          // close discharging relays once done discharging
          // digitalWriteAllDischargers(LOW);
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
      }

      // check if cells need charging and charge if so
      if (BV1 <= 4.08 || BV2 <= 4.08) {
        if (isACellSmallerThan(4.09)) {  // This was increased to 4.09 to handle getting rid of <= and instead using <
          Serial.println("Charging now...");
          read_voltages();
          print_voltages();
          delay(500);

          // connect charger to circuit via relay #8 (charger)
          // digitalWriteAllDischargers(LOW);
          digitalWrite(2, LOW);
          digitalWrite(3, LOW);
          digitalWrite(charger, LOW);
          delay(15000 * delayfactor);  // charge for this amount of time based on delay factor
          digitalWrite(charger, HIGH);
          Serial.println("Charging stopped. Letting cells stabilize...");
          delay(6000);  // allow delay for cells to stabilize

          read_voltages();
          Serial.println("Updated readings: ");
          print_voltages();
        }

        else {
          // keep everything closed
          digitalWrite(charger, HIGH);
          // digitalWriteAllDischargers(LOW);
          digitalWrite(2, LOW);
          digitalWrite(3, LOW);
        }
      }
    }
  }
}