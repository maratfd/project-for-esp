#define RELAY LOW  

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>




//=====Server=============== 
WiFiServer server(80);


//===WiFI============================== 
const char* ssid = "";  
const char* password = "";


//====Telegram=========================
String BOTtoken = "";
String CHAT_ID = "";
X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);
int botRequestDelay = 1000; //задержка для проверки новых сообщений
unsigned long lastTimeBotRan;
//==================================
void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
 
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

   if (text == "/start") {
      String welcome = "Welcome, " + from_name + ".\n";
      bot.sendMessage(chat_id, welcome, "");
    }
     if (text == "/on") {
      bot.sendMessage(chat_id, "RELAY set to ON", "");
      digitalWrite(RELAY,HIGH);
    }
      if (text == "/off") {
      bot.sendMessage(chat_id, "LED set to OFF", "");
      digitalWrite(RELAY,LOW);
    }
  }
}


void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  configTime(0, 0, "pool.ntp.org");
  client.setTrustAnchors(&cert);
  /*bot.sendMessage(CHAT_ID, "Bot Started", "");
 */
  server.begin();
  Serial.print("Got IP: ");  Serial.print(WiFi.localIP());
  Serial.print("\n");
  
  pinMode(RELAY, OUTPUT);                      // Указываем вывод RELAY как выход
 
  
  
}

void loop() {
//========Messages Telegram=================================
if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
 
 
 
 WiFiClient client = server.available();    // Получаем данные, посылаемые клиентом 
  if (!client)                                
  {
    return;
  }
  Serial.println("new client");               // Отправка "new client"
  while(!client.available())                  // Пока есть соединение с клиентом 
  {
    delay(1);                                 // пауза 1 мс
  }
 
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
 
  int value = LOW;
  if (request.indexOf("/RELAY=ON") != -1)  
  {
    Serial.println("RELAY=ON");
    digitalWrite(RELAY,LOW);
    value = LOW;
  }
  if (request.indexOf("/RELAY=OFF") != -1)  
  {
    Serial.println("RELAY=OFF");
    digitalWrite(RELAY,HIGH);
    value = HIGH;
  }
 
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); 
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<head><title>ESP8266 RELAY Control</title></head>");
  client.print("Relay is now: ");
 
  if(value == HIGH) 
  {
    client.print("OFF");
  } 
  else 
  {
    client.print("ON");
  }
  client.println("<br><br>");
  client.println("Turn <a href=\"/RELAY=OFF\">OFF</a> RELAY<br>");
  client.println("Turn <a href=\"/RELAY=ON\">ON</a> RELAY<br>");
    client.println("</html>");
 
  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");  

}

/*
 Моргание светодиодом с помощью чипа ESP8266
 
 Автор – Саймон Питер (Simon Peter)
 
 Выполняет моргание синим светодиодом, встроенным в модуль ESP-01.
 
 Этот код не защищен авторским правом.

 Синий светодиод на модуле ESP-01 подключен к контакту GPIO1.
 Контакт GPIO1 также является контактом TXD, поэтому мы 
 не сможем одновременно управлять светодиодом
 и использовать функцию Serial.print().

 Обратите внимание, что этот скетч использует LED_BUILDIN,
 чтобы найти контакт, к которому подключен встроенный светодиод.

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);     //  инициализируем контакт 
                                    //  в качестве выходного контакта 
}

// это блок loop(); он будет запускаться снова и снова:
void loop() {
  digitalWrite(LED_BUILTIN, LOW);   //  включаем светодиод; 
                                    //  обратите внимание, что для 
                                    //  включения светодиода 
                                    //  используется значение LOW;
                                    //  это из-за того, что на ESP-01
                                    //  активным состоянием 
                                    //  является LOW

  delay(1000);                      //  ждем одну секунду

  digitalWrite(LED_BUILTIN, HIGH);  //  выключаем светодиод,
                                    //  используя состояние HIGH

  delay(2000);                      //  ждем две секунды, чтобы
                                    //  продемонстрировать, что
                                    //  светодиод активируется
                                    //  при помощи значения LOW
}
*/
