// Board   : Arduino Uno/Nano/Micro
// Author  : Fabian Kung
// Date    : 31 Aug 2022
// Purpose : Driver code for ADF4156 (6.2GHz Fractional-N) PLL Chip.

// Pins declarations
//        Pin Name    Arduino Pin     Remark
//        --------    -----------     ------
#define   PIN_CLK       13           // Clock
#define   PIN_DATA      12           // Serial data
#define   PIN_LE        11           // Latch enable
#define   PIN_MUXOUT    10           // For checking if PLL is phase-locked or not,
                                     // and for debugging.
#define   PIN_LED        9           // Optional, connect to LED via 1k resistor.
#define   PIN_TRIGGER    8           // Optional, to trigger oscilloscope.

// Global variables declarations
#define ASSERT_PIN_LE             LOW
#define DEASSERT_PIN_LE           HIGH

byte   gbytRead;
  
unsigned int unN;          // N divider for frequency synthesizer IC.

void setup()
{
  pinMode(PIN_CLK, OUTPUT);
  pinMode(PIN_DATA, OUTPUT);
  pinMode(PIN_LE, OUTPUT);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_MUXOUT, INPUT);
  pinMode(PIN_TRIGGER, OUTPUT);
  Serial.begin(9600);  // Setup Serial Port.
}

void loop()
{
  static byte bytState = 0;
  static int  nTimer = 1;

  // --- Setup up PLL Chip ---
  // fosc = 16 MHz
  // R = 4
  // N = 3480
  // MOD = 100
  // FRAC = 0
  // fvco = (N + (FRAC/MOD))*(fosc/R) = 870 MHz.
  
  delay(1000);
  // Load register R4, the CLK DIV register.
  // 1. The internal clock divider allows the PLL IC to extract clocking signals from the 
  // reference oscillator.  This is used for the IC internal logic process such as
  // lock detection, modulation, fast-lock assist, cycle-slip reduction etc.  
  // Here it is not important and we set the clock divider to 16.  Thus:
  // Assuming fosc = 16MHz, a 1MHz internal clock is generated.
  // 2. Clock divider mode = off.  We set it to other modes when either fast-lock or
  // cycle-slip reduction functions are activated.  
  nSendData_ADF41XX(0x00000804,32);
  delay(1); // A short delay for the PLL chip to lock.
  //Load register R3, the Function Register.
  // 1. Counter reset = disabled.
  // 2. CP three-state = disabled.
  // 3. Power down = disabled.
  // 4. Phase detector polarity = positive (e.g. rising edge).
  // 5. Lock detect precision = high.
  // 6. Sigma-delta reset = yes whenever registor R0 is written.    
  nSendData_ADF41XX(0x00000043,32);  
  delay(1); // A short delay for the PLL chip to lock.
  //Load register R2, the MOD/R Register.
  // 1. Here we set the reference divider R to 4 by
  //    D=0, R=4, T=0.
  // 2. We also use P=8 (e.g. p/p+1) prescalar. This means minimum INT value allowed is 75.
  // 3. Here we set the modulus (MOD) = 10.
  // 4. Charge pump current setting = 3 (e.g. (3+1)/16 = 1/4 of max current),
  //    or for Rset = 10kOhm, Icp = (1/4)x2.55 mA = 0.6375 mA.
  // 5. Cycle-slip reduction = disabled.
  // 6. Noise mode = Low noise.     
  nSendData_ADF41XX(0x03420322,32); // Low noise mode, M = 100
  //nSendData_ADF41XX(0x63420322,32); // Low spur mode, M = 100
  delay(1); // A short delay for the PLL chip to lock. 
  // Load register R1, the Phase Register.
  // We can change the phase of the RF output in relation to the reference signal here 
  // at a resolution of (360degree)/MOD. Thus the value of Phase Register should always
  // be less than MOD.
  // If not used, set to 1 as per the recommendation of the datasheet.  Note that we
  // adjust this to optimize the RF output spurs.   
  nSendData_ADF41XX(0x00000009,32);  
  delay(1); // A short delay for the PLL chip to lock.
  //Load register R0, the FRAC/INT Register.
  // Here we set:
  // 1. FRAC = 0.
  // 2. INT or N = 870.
  // 3. MUX out = As per application.
  // Subsequently these can be changed as per application.      
  nSendData_ADF41XX(0x306C8180,32);   // N = 217, m = 50 
  //nSendData_ADF41XX(0x306B8000,32);   // N = 215, m = 0
  delay(1); // A short delay for the PLL chip to lock.

  // User routines
  while (1)
  {
    // Uncomment and comment as needed.
    /*
    // --- Small sweep 0.1 MHz ---
    delay(1000);
    CheckMuxOut();
    digitalWrite(PIN_TRIGGER, HIGH);
    nSendData_ADF41XX(0x31B20000,32);   // N + m/M = 868.0 (Make sure M is set properly in R2)
    digitalWrite(PIN_TRIGGER, LOW);
    delay(1000);
    CheckMuxOut();
    //nSendData_ADF41XX(0x31B20008,32);   // N + m/M = 868.1 (Make sure M is set properly in R2)
    //delay(1000);
    //CheckMuxOut();
    //nSendData_ADF41XX(0x31B20010,32);   // N + m/M = 868.2 (Make sure M is set properly in R2)
    //delay(1000);
    //CheckMuxOut();
    digitalWrite(PIN_TRIGGER, HIGH);
    nSendData_ADF41XX(0x31B20018,32);   // N + m/M = 868.3 (Make sure M is set properly in R2)         
    digitalWrite(PIN_TRIGGER, LOW);
    */
    
    
    // --- Large sweep 10 MHz ---
    delay(1000);
    CheckMuxOut();
    digitalWrite(PIN_TRIGGER, HIGH);    // Optional, to trigger oscilloscope.
    nSendData_ADF41XX(0x306C8180,32);   // N = 217, m = 50, M = 100, fref = 4 MHz.     
    digitalWrite(PIN_TRIGGER, LOW);     // fvco = 870 MHz.
    delay(1000);
    CheckMuxOut();
    digitalWrite(PIN_TRIGGER, HIGH);    // Optional, to trigger oscilloscope.
    nSendData_ADF41XX(0x306B8000,32);   // N = 215, m = 0, M =100, fref = 4 MHz.
    digitalWrite(PIN_TRIGGER, LOW);     // fvco = 860 MHz.
    
  }
}

// Function name  : SendData_ADF41XX
// Author         : Fabian Kung
// Last modified  : 22 July 2022
// Description    : To send serial data to ADF41XX family of PLL chip via 
//                  SPI bus. The length of the data is variable length  
//                  (1-32 bits), and data is shifted MSb first.
// Arguments      : ulnData - unsigned 32 bit data (max length) to be transmitted out.
//                  nLength - Length of data in bits, limited to 32 bits.
// Return         : 1 if successful
//                  else 0.
int nSendData_ADF41XX(unsigned long ulnData, int nLength)
{
    int nCount;
    int nTemp;
    unsigned long ulnTemp;
    
    if (nLength > 32)       // Limit the maximum bit length.
    {
        nLength = 32;
    }
    ulnTemp = ulnData;
    ulnTemp = ulnTemp << (32-nLength);          // Left shift the data by (32-nLength)
                                                // bits so that all the data bits to
                                                // be send out occupy left most bit
                                                // positions.
    //digitalWrite(PIN_LE, ASSERT_PIN_LE);  
    for (nCount = 0; nCount < nLength; nCount++)
    {
        digitalWrite(PIN_CLK, LOW);
        if ((ulnTemp & 0x80000000) > 0)       // Check MSb value.
        {
            digitalWrite(PIN_DATA, HIGH);
        }
        else
        {
            digitalWrite(PIN_DATA, LOW);
        }      
        nTemp++;            // Dummy instruction as delay.
        digitalWrite(PIN_CLK, HIGH);
        //nTemp++;            // Dummy instruction as delay.
        ulnTemp = ulnTemp << 1;          // Left shift 1 unit.
    }
    
    digitalWrite(PIN_LE, DEASSERT_PIN_LE);  
    nTemp++;                // Dummy instruction as delay.
    digitalWrite(PIN_LE, ASSERT_PIN_LE);
    nTemp++;            // Dummy instruction as delay.
    digitalWrite(PIN_DATA, LOW);
    digitalWrite(PIN_CLK, LOW);
    return  1;
} 

// Function name  : CheckMuxOut
// Author         : Fabian Kung
// Last modified  : 29 Aug 2022
// Description    : To check the status of FoLD pin of ADF41XX family of PLL 
//                  chip. Lights up LED is pin is High, else turn off LED.
// Arguments      : None.
// Return         : None.
void CheckMuxOut()
{
  if (digitalRead(PIN_MUXOUT) == 1)
  {
    digitalWrite(PIN_LED, HIGH);
  }
  else
  {
    digitalWrite(PIN_LED, LOW);
  }
}
