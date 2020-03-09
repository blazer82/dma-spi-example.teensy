#include <Arduino.h>
#include <SPI.h>
#include <DmaSpi.h>

/** buffers to send from and to receive to **/
#define DMASIZE 25600
uint16_t src[DMASIZE];

volatile uint16_t loopIndex = 0;

/** Wait for and consume a keypress over USB **/
void waitForKeyPress()
{
  Serial.println("\nPress a key to continue\n");
  while(!Serial.available());
  while(Serial.available())
  {
    Serial.read();
  }
}

void dumpBuffer(const volatile uint8_t* buf, const char* prefix)
{
  Serial.print(prefix);
  for (size_t i = 0; i < DMASIZE; i++)
  {
    Serial.printf("0x%02x ", buf[i]);
  }
  Serial.print('\n');
}

void setSrc(uint16_t v)
{
  for (size_t i = 0; i < DMASIZE; i++)
  {
    src[i] = v;
  }
}

void setup()
{
  //waitForKeyPress();
  Serial.println("Hi!");
  pinMode(LED_BUILTIN, OUTPUT);

  /** set up SPI **/
  SPISettings spiSettings;
  SPI.begin();
  SPI.setCS(SS);

  SPI.beginTransaction(spiSettings);
  SPI.endTransaction();

  DMASPI0.begin();
  DMASPI0.start();
}

void loop()
{
  //digitalWriteFast(LED_BUILTIN, true);


  /** Prepare source and destination **/
  setSrc(loopIndex++);
  //Serial.println("Buffers are prepared");Serial.flush();


  DmaSpi::Transfer trx(nullptr, 0, nullptr);

  //Serial.println("Testing src -> dest, single transfer");
  //Serial.println("--------------------------------------------------");
  ActiveLowChipSelect cs(0, SPISettings(4000000, MSBFIRST, SPI_MODE0));
  trx = DmaSpi::Transfer(src, DMASIZE, nullptr, 0, &cs);
  DMASPI0.registerTransfer(trx);
  while(trx.busy())
  {
  }
  //Serial.print("Finished DMA transfer ");
  //Serial.println(loopIndex);
  //Serial.println("==================================================\n\n");


  //DMASPI0.end();
  //SPI.endTransaction();
  //SPI.end();


  //delay(50);
  //digitalWriteFast(LED_BUILTIN, false);
  //delay(50);

  //waitForKeyPress();
}
