#include <WiFi.h>
#include <Luxiot_12v.h>
#include <Max44009.h>

/*Wifi and TCP parameters*/
const char* ssid     = "M&M_Wi-Fi";
const char* password = "BSeX9kAa6K7YmrRRMyM9";
const uint16_t port = 1234;
const char* host = "192.168.2.110"; // ip or dns
uint8_t Online = 0;
/*.................................................*/

/* PWM parameters*/
const uint16_t freq = 1000;
const uint8_t ledChannel = 0;
const uint8_t resolution = 16;
const uint8_t ledPin= 5;
const uint32_t factor = 655;
uint32_t dutyCycle[9];
uint32_t dutyCycle_pos = 4;
/*...............................................................................................................*/

/* Timer interrupt parameters*/
hw_timer_t * timer = NULL;
volatile SemaphoreHandle_t timerSemaphore;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
volatile uint32_t isrCounter = 0;
volatile uint32_t lastIsrAt = 0;
/*.................................................*/

/*Other parameters*/
float lux_value;
String data;
/*.................................................*/

/*Constructor's block*/
Max44009 max44009(0x4A);  // default addr
Luxiot_12v Luxiot_12v;
WiFiClient client;
/*.................................................*/


/* TimerISR function */
void IRAM_ATTR onTimer(){
  // Increment the counter and set the time of ISR
  portENTER_CRITICAL_ISR(&timerMux);
  isrCounter++;
  lastIsrAt = millis();
  portEXIT_CRITICAL_ISR(&timerMux);
  // Give a semaphore that we can check in the loop
  xSemaphoreGiveFromISR(timerSemaphore, NULL);
}
/*...........................................................................*/

 /* Send data to TCP server procedure */
void send_data(String data_send){

  if (Online == 0) {
    Serial.println("The \"LUX IoT System\" is working offline");
    Serial.println(data_send);
    Serial.println(".................");
    delay(1000);
  }
  else
  {
    client.print(data_send);
    Serial.print("Data sended to ");
    Serial.print(host);
    Serial.print(":");
    Serial.println(port);
    Serial.println(data_send);
    Serial.println(".................");
  }
}
/*...........................................................................*/

/* Start Setup Block */
void setup() {
  
  /* Start serial coummication (baud_rate)*/ 
  Serial.begin(115200);

  /* PWM setup */
  dutyCycle[0] = factor*37;
  dutyCycle[1] = factor*43;
  dutyCycle[2] = factor*51;
  dutyCycle[3] = factor*57;
  dutyCycle[4] = factor*63;
  dutyCycle[5] = factor*69;
  dutyCycle[6] = factor*75;
  dutyCycle[7] = factor*81;
  dutyCycle[8] = factor*87;
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(ledPin, ledChannel);
  ledcWrite(ledChannel, dutyCycle[dutyCycle_pos]);
/*...........................................................................*/

  /* Max44009 set configuration */
  max44009.setContinuousMode();
  max44009.setManualMode(0x00, 0x000);
/*...........................................................................*/
  
  /* WiFi and TCP connection procedure */
  int wifi_conn_try =0;
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
    while(WiFi.status() != WL_CONNECTED) {
        if(wifi_conn_try <= 60)
        {
          Serial.print(".");
          delay(500);
          wifi_conn_try = ++wifi_conn_try;
        }
        else
        {
          Serial.println("The \"LUX IoT System\" will work offline");
          Serial.println(".................");
          break;
        }
    }
    if(WiFi.status() == WL_CONNECTED)
    {
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      Serial.println(".................");

          
      // Start TCP as client connection
      Serial.print("connecting to ");
      Serial.println(host);

      if (!client.connect(host, port)) {
        Serial.println("TCP connection failed");
        Online = 0;
      }
      else
      {
        Serial.print("Connected to ");
        Serial.print(host);
        Serial.print(":");
        Serial.println(port);
        Serial.println(".................");
        Online = 1;
      }
    }
/*...........................................................................*/

  /* Timer setup */
  // Create semaphore to inform when the timer has fired
  timerSemaphore = xSemaphoreCreateBinary();
  // Use 1st timer of 4 (counted from zero).
  // Set 80 divider for prescaler.
  timer = timerBegin(0, 80, true);
  // Attach onTimer function to timer.
  timerAttachInterrupt(timer, &onTimer, true);  
  // Set alarm to call onTimer function every 30 seconds (value in microseconds).
  // Repeat the alarm (third parameter)
  timerAlarmWrite(timer, 10000000, true);
  // Start an alarm
  timerAlarmEnable(timer);
}
/*...........................................................................*/
/*......................END OF SETUP BLOCK...................................*/


void loop() {

  // If Timer has fired
  if (xSemaphoreTake(timerSemaphore, 0) == pdTRUE){    
    data = String(lux_value) + ',' + String(dutyCycle_pos);
    send_data(data);         
  }
  
  portENTER_CRITICAL(&timerMux);
  lux_value = max44009.getLux();
  portEXIT_CRITICAL(&timerMux);
  dutyCycle_pos = Luxiot_12v.balance(lux_value,dutyCycle_pos);
  ledcWrite(ledChannel, dutyCycle[dutyCycle_pos]);    
  delay(1000); 
}
