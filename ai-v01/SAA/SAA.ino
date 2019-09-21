/**/
#include <SoftwareSerial.h>

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
SoftwareSerial esp8266 (2, 3); //Arduino TX (ESP8266 RX) connected to Arduino Pin 2, Arduino RX(ESP8266 TX) connected to Arduino Pin 3

String ssid = "ALHN-8E28";
String password = "4650730851";

String path = "/api/arduino/oi";
String server = "192.168.1.70";
String request = "GET " + path + " HTTP/1.1\r\n" + "Host: " + server + "\r\n" + "Connection: keep-alive\r\n\r\n";
String requestLength = String(request.length());

const int timeout = 2000;

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

  esp8266.begin(9600); // Should match ESP's current baudrate
 
  setupESP8266();
  connectToWiFi();

  startTCPConnection();  
  sendGetRequest();
  closeTCPConnection();
  
}

void loop() {

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
String atCommand(String command, int timeout) {
  String response = "";
  esp8266.println(command);

  long int time = millis();

  while( (time+timeout) > millis() ) {
    while(esp8266.available()) {
      char c = esp8266.read();
      response += c;
    }
  }

  Serial.println(response);
  return response;
}

void setupESP8266() {
  atCommand("AT+RST", timeout);
  atCommand("AT+CWMODE=1", timeout);
}

void connectToWiFi() {
  String connect = "AT+CWJAP=\"" +ssid+"\",\"" + password + "\"";
  atCommand(connect, 6000);
  atCommand("AT+CIFSR", timeout);
}

void startTCPConnection() {
  String connect = "AT+CIPSTART=\"TCP\",\"" + server + "\",8090";
  atCommand(connect, timeout);
}

void closeTCPConnection() {
  atCommand("AT+CIPCLOSE", timeout);
}

String sendGetRequest() {
  atCommand("AT+CIPSEND=" + requestLength, timeout);
  String response = atCommand(request, 6000);
  return response;
}





/*
  void cheio(boolean cheio) {
  isCheio = cheio;
  }*/
