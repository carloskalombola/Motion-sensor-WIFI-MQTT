// Libs
#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>


///########## configuration MQTT
const char* mqtt_server = "test.mosquitto.org";// MQTT broker
char* my_topic_SUB = "MCK/SUB";// the chosen topic
char* my_topic_PUB = "MCK/PUB";// the chosen topic


/////##### configuration wifi et MQTT-wifi and MQTT configuration
WiFiClient espClient;
PubSubClient client(espClient);



///########### configuration Wi-Fi-wifi configuration
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""
// Définir le canal WiFi accélère la connexion:
// Defining the WiFi channel speeds up the connection
#define WIFI_CHANNEL 6
int status = WL_IDLE_STATUS;  // l'état de la radio Wifi-the Wifi radio's status


//Broche du capteur PIR (analogique)-PIR sensor pin (analog)
int PIR_SENSOR_PIN = 14;
// Variable pour stocker l'état du capteur-Variable to store sensor status
int PIR_SENSOR_STATUS = 0; 
int  valoranterior=-1;
int movimento = 0;
int LED = 2;


/////Réglage du temps de lecture-reading time setting
int contador = 1;
char mensagem[30];
char mensagem1[]="Sem movimento";
char mensagem2[]="Movimento detectado";
 unsigned long lastMillis = 0;




void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");
  pinMode(PIR_SENSOR_PIN, INPUT);
  pinMode(LED, OUTPUT);
  conectawifi();
  //Définir le serveur et le port du courtier MQTT:
  //Set the MQTT broker server and port
  client.setServer(mqtt_server, 1883);
  // Définit la fonction de rappel pour les messages MQTT entrants:
  // Set the callback function for incoming MQTT messages
  client.setCallback(callback);




}


void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  //Ajoutez un délai pour éviter d'inonder le courtier MQTT de messages
  //Add a delay to avoid flooding the MQTT broker with messages
 delay(100); 
 


  movimento = sensor_movimento();
  if(movimento == 1)
  {
    digitalWrite(LED, HIGH);
  }
  else{
    digitalWrite(LED,LOW);
  }




  //Attendez 15 secondes pour envoyer un nouveau message:
  //Wait 15 seconds to send a new message
   if (millis() - lastMillis > 15000) {
    lastMillis = millis();
    sprintf(mensagem, "%d", movimento);
    //sprintf(mensagem, "MQTT ESP32 - Mensagem no. %d", contador);  
  Serial.print("Mensagem enviada: ");
  Serial.println(mensagem);
 //Envoyer le message au courtier-Send the message to the broker
  //client.publish(my_topic_PUB, mensagem);
   if(movimento == 1)
  {
    client.publish(my_topic_PUB, mensagem2);
     Serial.println(mensagem2);
  }
  else{
    client.publish(my_topic_PUB, mensagem1);
     Serial.println(mensagem1);
  }
  Serial.print(contador);
  Serial.println("-Mensagem enviada com sucesso...");
 
  //Incrémenter le compteur-Increment the counter
  contador++;
 
    }


 
}



int sensor_movimento()
{
   PIR_SENSOR_STATUS = digitalRead(PIR_SENSOR_PIN);
   if(PIR_SENSOR_STATUS != valoranterior){
      Serial.println(PIR_SENSOR_STATUS);
      valoranterior = PIR_SENSOR_STATUS;
      if(PIR_SENSOR_STATUS == 1)
      {
        Serial.println("Movimento detectado");
      }
      else{
        Serial.println("Sem movimento");
      }
   }
   return PIR_SENSOR_STATUS;
}




void conectawifi()
  {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
  Serial.print("Conectando ao WiFi ");
  Serial.print(WIFI_SSID);
  //Attendre la connexion
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println(" Conectado!");


  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  }


  // Traiter le message MQTT entrant et contrôler le servomoteur
  // Process incoming MQTT message and control the servo motor
void callback(char* topic, byte* payload, unsigned int length) {
  String string;
  Serial.print("Chegou a mensagem [");
  Serial.print(my_topic_SUB);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
     string+=((char)payload[i]);  
  }
  Serial.print(string);
 
}
//Essayez de vous reconnecter au courtier MQTT si la connexion est perdue:
// Attempt to reconnect to the MQTT broker if the connection is lost
void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conectar ao MQTT ...");
    if (client.connect("ESPClient")) {
      Serial.println("Conectado");
      client.subscribe(my_topic_SUB);
    } else {
      Serial.print("falhou, rc=");
      Serial.print(client.state());
      Serial.println(" Tente novamente em 5 segundos");
     
     
  }
  }
}
