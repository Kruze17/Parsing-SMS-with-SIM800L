/* Code Written and tested by:
 *  Youtube- Hyper_makes(https://www.youtube.com/channel/UCrCxVyN2idCxPNOwCwK6qtQ)
 *  Github:https://github.com/Kruze17/Parsing-SMS-with-SIM800L
 */



#include <SoftwareSerial.h>

//sender phone number with country code
//const String PHONE = "+ZZxxxxxxxxxx";//For only 1 default number user
String PHONE = ""; //For Multiple user
String msg;

//GSM Module RX pin to Arduino 3
//GSM Module TX pin to Arduino 2
#define rxPin 2
#define txPin 3
SoftwareSerial sim800(txPin, rxPin);

#define LED_PIN 13

void setup() {
  pinMode(LED_PIN, OUTPUT); //Setting Pin 13 as output
  digitalWrite(LED_PIN, LOW);
  delay(1000);

  Serial.begin(115200);
  Serial.println("Initializing Serial... ");

  sim800.begin(9600);
  Serial.println("Initializing GSM module...");

  sim800.print("AT+CMGF=1\r"); //SMS text mode
  delay(1000);
}

void loop()
{
  while (sim800.available())
  {
    parseData(sim800.readString());//Calls the parseData function to parse SMS
  }
  doAction();//Does necessary action according to SMS message
  while (Serial.available())
  {
    sim800.println(Serial.readString());
  }

}

void parseData(String buff) {
  Serial.println(buff);

  unsigned int index;

  //Remove sent "AT Command" from the response string.
  index = buff.indexOf("\r");
  buff.remove(0, index + 2);
  buff.trim();

  if (buff != "OK") {
    index = buff.indexOf(":");
    String cmd = buff.substring(0, index);
    cmd.trim();

    buff.remove(0, index + 2);

    //Parse necessary message from SIM800L Serial buffer string
    if (cmd == "+CMT") {
      //get newly arrived memory location and store it in temp
      index = buff.lastIndexOf(0x0D);//Looking for position of CR(Carriage Return)
      msg = buff.substring(index + 2, buff.length());//Writes message to variable "msg"
      msg.toLowerCase();//Whole message gets changed to lower case
      Serial.println(msg);

      index = buff.indexOf(0x22);//Looking for position of first double quotes-> "
      PHONE = buff.substring(index + 1, index + 14); //Writes phone number to variable "PHONE"
      Serial.println(PHONE);

    }
  }
}

void doAction()
{

  if (msg == "led on")
  {
    digitalWrite(LED_PIN, HIGH);
    Reply("LED is ON");
  }
  else if (msg == "led off")
  {
    digitalWrite(LED_PIN, LOW);
    Reply("LED is OFF");
  }

  PHONE = "";//Clears phone string
  msg = "";//Clears message string
}

void Reply(String text)
{
  sim800.print("AT+CMGF=1\r");
  delay(1000);
  sim800.print("AT+CMGS=\"" + PHONE + "\"\r");
  delay(1000);
  sim800.print(text);
  delay(100);
  sim800.write(0x1A); //ascii code for ctrl+z, DEC->26, HEX->0x1A
  delay(1000);
  Serial.println("SMS Sent Successfully.");
}
