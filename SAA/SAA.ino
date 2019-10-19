
//Carrega as bibliotecas ESP8266 e SoftwareSerial"
#include <SoftwareSerial.h>
#include "ESP8266.h"

/*VARIAVEIS DOS LEDS*/
#define ledVerde 13
#define ledAmarelo 12
#define ledAzul 11
#define ledVermelho 10

/*VARIAVEIS DOS NIVEIS*/
#define nivel0 8
#define nivel1 7
#define nivel2 6
#define nivel3 5
/*VARIAVEI DO RELÉ*/
#define pinRele 9

/*VARIAVEI DO CS*/
#define pinContraSeco 4

/*Global set nivel*/
int Nivel_Minimo_Para_Acionamento;
int Nivel_Atual = 0;
int NIVEL_MAXIMO = 3;
boolean isCheio = false;

//variavel que contem todos os pinos usados para pinMode
int pinosUsados[10] = {ledVerde, ledAmarelo, ledAzul, ledVermelho, nivel0, nivel1, nivel2, nivel3, pinRele, pinContraSeco};
//variavel que term todos os tipos de cada pinMode
int tipoPinos[10] = {OUTPUT, OUTPUT, OUTPUT, OUTPUT, INPUT, INPUT, INPUT, INPUT, OUTPUT, INPUT};


/*CONFIGURAÇÃO WIFI*/
SoftwareSerial esp8266(2, 3); //Arduino TX (ESP8266 RX) connected to Arduino Pin 2, Arduino RX(ESP8266 TX) connected to Arduino Pin 3

String ssid = "Romero";
String password = "35475442";

//String ssid = "TheHell";
//String password = "ahp15161522";

//Ip do WebServer
String server = "192.168.0.16";

//Porta do Servidor
String port  = "8090";

const int timeout = 3000;

void setup()
{
  // Inicializa o Monitor Serial porta 9600
  Serial.begin(9600);

  //inicializa todos os pinModes
  for (int i = 0; i < sizeof(pinosUsados); i++) {
    pinMode(pinosUsados[i], tipoPinos[i]);
  }

  desligaTudo();
  Nivel_Minimo_Para_Acionamento = NivelDeAcionamentoMinimo(1);


  //ESP8266 CONFIG
  esp8266.begin(9600); // Should match ESP's current baudrate
  String MAC =  getMacAdress( );
  //setupESP8266();
  //connectToWiFi();
  wifisetup();  //wifi setup
  connectWifi();
  //brindo conexão TCP
  startTCPConnection();
  sendPostRequest( "{\"MAC\":\""+MAC+"\"}", "/api/arduino/deviceConect" );
  closeTCPConnection();

}


void loop() {
  if (0) {
    //Toda a lógica de funcionamento deve ocorrer no interior desse if
    //Se circuito CS FECHADO então...(ÁGUA NO CANO)

    boolean  contSeco = temAgua();
    boolean n0 = checaN0();
    boolean n1 = checaN1();
    boolean n2 = checaN2();
    boolean n3 = checaN3();

    //CHECAR NÍVEL 0
    if (contSeco ) {
      // se o circuito de nivel0 estiver FECHADO (BOIA 0 LEVANTADA)
      if (n0 ) {
        Serial.println( "NIVEL-0");
        rotinaN0Levantado();
        defineNivel_Atual(0);
        //CHECAR NÍVEL 1
        if (n1 && n0 ) {
          Serial.println( "NIVEL-1");
          rotinaN1Levantado();
          defineNivel_Atual(1);
          //CHECAR NÍVEL 2
          if (n2 && n1 && n0 ) {
            Serial.println( "NIVEL-N2");
            rotinaN2Levantado();
            defineNivel_Atual(2);
            //CHECAR NÍVEL 3
            if (n3 && n2 && n1 && n0 ) {
              Serial.println( "NIVEL-N3");
              rotinaN3Levantado();
              defineNivel_Atual(3);
              desligaBomba();
            } else {
              rotinaN3Abaixado();
              ligaBomba();
              Serial.println( "falseN3");
            }

            //desligaBomba();
          } else {
            rotinaN2Abaixado();
            ligaBomba();
            Serial.println( "falseN2");
          }

          //desligaBomba();
        } else {
          rotinaN1Abaixado();
          ligaBomba();
          Serial.println( "falseN1");
        }
        //desligaBomba();
      } else {
        Serial.println( "falseN0");
        rotinaN0Abaixado();
        ligaBomba();
      }
    }

    //Se circuito CS ABERTO então...(SECO NO CANO)
    else {
      // se o circuito de nivel0 estiver FECHADO (BOIA 0 LEVANTADA)
      if (n0) {
        Serial.println( "trueN0");
        rotinaN0Levantado();

        //CHECAR NÍVEL 1
        if (n1) {
          Serial.println( "trueN1");
          rotinaN1Levantado();
          //CHECAR NÍVEL 2
          if (n2) {
            Serial.println( "trueN2");
            rotinaN2Levantado();
            //CHECAR NÍVEL 3
            if (n3) {
              Serial.println( "trueN3");
              rotinaN3Levantado();
              desligaBomba();
            } else {
              rotinaN3Abaixado();
              desligaBomba();
            }

            //desligaBomba();
          } else {
            rotinaN2Abaixado();
            desligaBomba();
          }

          //desligaBomba();
        } else {
          rotinaN1Abaixado();
          desligaBomba();
        }
        //desligaBomba();
      } else {
        Serial.println( "falseN0");
        rotinaN0Abaixado();
        desligaBomba();
      }

      //configurar um delay de 5 minutos
      delay(4000);
      desligaBomba();
    }

    // espera 1 segundo para o proximo loop
    delay(1000);
    Serial.println();
    Serial.println();

  }
}

//Checa contra seco
boolean temAgua() {

  //Se circuito CS FECHADO então...(ÁGUA NO CANO)
  if (digitalRead( pinContraSeco ) == HIGH) {

    //*****************************************
    //Avisa WS que Tem água na tubulação
    //****************************************
    Serial.println("CS EST HIGH C/Agua: " );
    return true;
  } else if (digitalRead( pinContraSeco ) == LOW) {
    //***********************************
    //Avisa WS que ESTÁ SECO
    //***********************************

    Serial.println("CS EST LOW S/Agua: " );
    return false;
  }

}

void ligaBomba () {
  //aqui liga bomba
  if (( Nivel_Atual <= Nivel_Minimo_Para_Acionamento )) {
    digitalWrite(pinRele, LOW);
    Serial.println("pRele LOW ON Pump: " );
  } else if ( isCheio ) {
    digitalWrite(pinRele, HIGH);
    Serial.println("pRele HIGH N_MIN OFF Pump: " );
  }
  //***********************************
  //Avisa WS que LIGOU bomba
  //***********************************
}

void desligaBomba () {
  //aqui desliga a bomba
  digitalWrite(pinRele, HIGH);
  Serial.println("pRele HIGH OFF Pump: " );
  //***********************************
  //Avisa WS que DESLIGOU bomba
  //***********************************
}


void desligaTudo() {
  //liga o led vermelho
  digitalWrite(ledVermelho, HIGH);
  digitalWrite(pinRele, HIGH);
  digitalWrite(ledAzul, HIGH);
  digitalWrite(ledVerde, HIGH);
  digitalWrite(ledAmarelo, HIGH);
}


/*
   Metodos nivel 0
*/
//se boia abaixada false
boolean checaN0() {
  //Boia Abaixada
  if (digitalRead(nivel0) == LOW) {
    Serial.println("N-0 LOW Boia A: "  );
    //***********************************
    //Envia hora e estado nivel 0 para WS
    //***********************************
    return false;
    //Boia Levantada
  } else if (digitalRead(nivel0) == HIGH) {
    Serial.println("N-0 HIGH Boia L: " );
    //***********************************
    //Envia hora e estado nivel 0 para WS
    //***********************************
    return true;
  }

}


//
void rotinaN0Abaixado() {
  //desliga o led vermelho
  isCheio = false;
  digitalWrite(ledVermelho, HIGH);
  //por precaução , desliga o verde e o amarelo
  digitalWrite(ledAzul, HIGH);
  digitalWrite(ledVerde, HIGH);
  digitalWrite(ledAmarelo, HIGH);
}


void rotinaN0Levantado() {
  //liga o led vermelho
  isCheio = false;
  digitalWrite(ledVermelho, LOW);
  //por precaução , desliga o verde e o amarelo
  digitalWrite(ledAzul, HIGH);
  digitalWrite(ledVerde, HIGH);
  digitalWrite(ledAmarelo, HIGH);
}

/*
   Metodos nivel 1
*/
//se boia abaixada false
boolean checaN1() {
  //Boia Abaixada
  if (digitalRead(nivel1) == LOW) {
    Serial.println("N-1 LOW Boia A: "  );
    //***********************************
    //Envia hora e estado nivel 1 para WS
    //***********************************
    return false;
    //Boia Levantada
  } else if (digitalRead(nivel1) == HIGH) {
    Serial.println("N-1 HIGH Boia L: " );
    //***********************************
    //Envia hora e estado nivel 1 para WS
    //***********************************
    return true;
  }

}


//
void rotinaN1Abaixado() {
  //desliga o led vermelho
  //digitalWrite(ledVermelho, LOW);
  isCheio = false;
  digitalWrite(ledAzul, HIGH);
  digitalWrite(ledVerde, HIGH);
  digitalWrite(ledAmarelo, HIGH);
}


void rotinaN1Levantado() {
  //liga o led vermelho
  //digitalWrite(ledVermelho, LOW);
  isCheio = false;
  digitalWrite(ledAzul, LOW);
  digitalWrite(ledVerde, HIGH);
  digitalWrite(ledAmarelo, HIGH);
}


/*
    Metodos nivel 2
*/
//se boia abaixada false
boolean checaN2() {
  //Boia Abaixada
  if (digitalRead(nivel2) == LOW) {
    Serial.println("N-2 LOW Boia A: "  );
    //***********************************
    //Envia hora e estado nivel 2 para WS
    //***********************************
    return false;
    //Boia Levantada
  } else if (digitalRead(nivel2) == HIGH) {
    Serial.println("N-2 HIGH Boia L: " );
    //***********************************
    //Envia hora e estado nivel 2 para WS
    //***********************************
    return true;
  }

}

void rotinaN2Abaixado() {
  //desliga o led vermelho
  //digitalWrite(ledVermelho, LOW);
  //digitalWrite(ledAzul, HIGH);
  isCheio = false;
  digitalWrite(ledVerde, HIGH);
  digitalWrite(ledAmarelo, HIGH);
}


void rotinaN2Levantado() {
  //liga o led vermelho
  //digitalWrite(ledVermelho, LOW);
  //(ledAzul, LOW);
  isCheio = false;
  digitalWrite(ledVerde, LOW);
  digitalWrite(ledAmarelo, HIGH);
}


/*
   Metodos nivel 3
*/
//se boia abaixada false
boolean checaN3() {
  //Boia Abaixada
  if (digitalRead(nivel3) == LOW) {
    Serial.println("N-3 LOW Boia A: "  );
    //***********************************
    //Envia hora e estado nivel 3 para WS
    //***********************************
    return false;
    //Boia Levantada
  } else if (digitalRead(nivel3) == HIGH) {
    Serial.println("N-3 HIGH Boia L: " );
    //***********************************
    //Envia hora e estado nivel 3 para WS
    //***********************************
    return true;
  }

}

void rotinaN3Abaixado() {
  //desliga o led vermelho
  //digitalWrite(ledVermelho, LOW);
  //digitalWrite(ledAzul, HIGH);
  //digitalWrite(ledVerde, HIGH);
  isCheio = false;
  digitalWrite(ledAmarelo, HIGH);
}


void rotinaN3Levantado() {
  //liga o led vermelho
  //digitalWrite(ledVermelho, LOW);
  //(ledAzul, LOW);
  //digitalWrite(ledVerde, LOW);
  isCheio = true;
  digitalWrite(ledAmarelo, LOW);
}


int NivelDeAcionamentoMinimo(int nivel) {
  return nivel;
}

int defineNivel_Atual( int nivel) {
  Nivel_Atual = nivel;
}




/*WIFI CONFIG*/

//Metodo para enviar comandos AT para ESP8266
String atCommand(String command, int timeout) {
  String response = "";
  esp8266.println(command);

  long int time = millis();

  while ( (time + timeout) > millis() ) {
    while (esp8266.available()) {
      char c = esp8266.read();
      response += c;
    }
  }

  Serial.println(response);
  return response;
}



//Médodo para enviar rquisisão Post
String sendPostRequest( String msg, String uri ) {
  //Post- EXEMPLO
  //String pathPost = "/api/arduino/oiPost"; //MODELO URI
  //String msg = "{\"o\":\"o\"}";//MODELO MSG

  String requestPost =     "POST " + uri + " HTTP/1.1\r\n"
                           + "Host: " + server + ":" + port + "\r\n"
                           + "Accept: *" + "/" + "*\r\n"
                           + "Content-Length: " + msg.length() + "\r\n"
                           + "Connection: keep-alive\r\n"
                           + "Content-Type: application/json\r\n\r\n"
                           + msg + "\r\n";

  String requestLengthPost = String(requestPost.length());
  atCommand("AT+CIPSEND=" + requestLengthPost, timeout);
  String response = atCommand(requestPost, 6000);
  //Serial.println(response);
  return response;
}


//Médodo para enviar rquisisão get
String sendGetRequest(  String uri )
{
  //GET - EXEMPLO
  //String path = "/api/arduino/oi?oi=123";
  //String path = "/api/arduino/oi";

  String requestGet = "GET " + uri + " HTTP/1.1\r\n"
                      + "Host: " + server + "\r\n"
                      + "Connection: keep-alive\r\n\r\n";

  String requestLengthGet = String(requestGet.length());

  atCommand("AT+CIPSEND=" + requestLengthGet, timeout);
  String response = atCommand(requestGet, 6000);
  //Serial.println(response);
  return response;
}


//Metodo para Conexão wifi - Com Tentativas recursivas
void connectWifi()//wificonnection
{

  String cmd = "AT+CWJAP=\"" + ssid + "\",\"" + password + "\"";
  esp8266.println(cmd);
  delay(3000);

  if (esp8266.find("OK")) {
    Serial.println("WIFI Connected!");
    delay(1000);
  }
  else {

    Serial.println("Cannot connect to wifi");
    connectWifi();
  }
}

//Metodo para resetar o esp8266
void reset() {
  esp8266.println("AT+RST");
  delay(1000);
  if (esp8266.find("OK") ) Serial.println("Module Reset");
}

//Metodo para configurar esp8266  e velocidade
void esp_buad_rate_change()
{
  esp8266.println("AT+CIOBAUD=9600");                  //change buad rate
  Serial.println("AT+CIOBAUD=9600");
}


//Metodo para configurar wifi
void wifisetup()
{
  esp8266.println("AT");
  esp8266.println("AT+CWMODE=3");//Comando AT :  3: SoftAP+Station mode
  Serial.println("Wifi ready");
}



//Metodo para abrir TCP
void startTCPConnection() {
  String connect = "AT+CIPSTART=\"TCP\",\"" + server + "\","+port;
  atCommand(connect, timeout);
}


//Metodo para fchar TCP
void closeTCPConnection() {
  atCommand("AT+CIPCLOSE", timeout);
}


//Metodo para pegar MAC
String getMacAdress() {
  return atCommand("AT+CIPAPMAC?", timeout);
}


/*void setupESP8266() {
  atCommand("AT+RST", timeout);
  atCommand("AT+CWMODE=1", timeout);
  }

  void connectToWiFi() {
  String connect = "AT+CWJAP=\"" +ssid+"\",\"" + password + "\"";
  atCommand(connect, 6000);
  atCommand("AT+CIFSR", timeout);
  }*/

/*
  void cheio(boolean cheio) {
  isCheio = cheio;
  }*/
