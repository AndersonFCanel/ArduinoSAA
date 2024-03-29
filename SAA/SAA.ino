
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

boolean bomba_on_offAutomatico = true;

boolean falha = false;
boolean falhaN0 = false;
boolean falhaN1 = false;
boolean falhaN2 = false;
boolean falhaN3 = false;

String  nomeFalha0 = String("N0-OK");
String  nomeFalha1 = String("N1-OK");
String  nomeFalha2 = String("N2-OK");
String  nomeFalha3  = String("N3-OK");

boolean teste = false;
boolean isCheio = false;

int delayTime = 1000;

//String ssid = "ALHN-8E28";
//String password = "4650730851" ;

//String ssid = "-";
//String password = "Canel123";

String ssid = "Romero";
String password = "35475442";

//String ssid = "TheHell";
//String password = "ahp151622";

//Ip do WebServer
String server = "192.168.0.16";

//Porta do Servidor
String port  = "8090";

const int timeout = 1000;

boolean  contSeco;
boolean n0 ;
boolean n1 ;
boolean n2 ;
boolean n3 ;

int contaLoop = 0;
int contaLoopGlobal = 0;

//variavel que contem todos os pinos usados para pinMode
int pinosUsados[10] = {ledVerde, ledAmarelo, ledAzul, ledVermelho, nivel0, nivel1, nivel2, nivel3, pinRele, pinContraSeco};
//variavel que term todos os tipos de cada pinMode
int tipoPinos[10] = {OUTPUT, OUTPUT, OUTPUT, OUTPUT, INPUT, INPUT, INPUT, INPUT, OUTPUT, INPUT};

/*CONFIGURAÇÃO WIFI - portas do ESP*/
SoftwareSerial esp8266(2, 3); //Arduino TX (ESP8266 RX) connected to Arduino Pin 2, Arduino RX(ESP8266 TX) connected to Arduino Pin 3

void setup()
{
  // Inicializa o Monitor Serial porta 9600
  Serial.begin(9600);

  //inicializa todos os pinModes
  for (int i = 0; i < sizeof(pinosUsados); i++) {
    pinMode(pinosUsados[i], tipoPinos[i]);
  }

  desligaTudo();
  Nivel_Minimo_Para_Acionamento = 1;

  //ESP8266 CONFIG
   esp8266.begin(9600); // Should match ESP's current baudrate
  //setupESP8266();
  //connectToWiFi();

  //Conexao
   wifisetup();
   connectWifi();
   sendGetRequest(  "/api/ctrl/mac?mac=de:4f:22:36:99:18", 500);
}


//Prepara MAC para parametro de metodo GET
String MAC = "&m=de:4f:22:36:99:18";

void loop() {
  Serial.println("**INICIO LOOP**");

  Serial.println(n0);
  Serial.println(n1);
  Serial.println(n2);
  Serial.println(n3);
  //delay(1000);

  //Prepara Contador De loop para parametro de metodo GET;
  String loopCont = "&l=" + String(contaLoopGlobal);

  //Variaveis de controle
  contSeco = temAgua();
  n0 = checaN0();
  n1 = checaN1();
  n2 = checaN2();
  n3 = checaN3();

  if (contaLoop >= 14 && teste) {
    contaLoop = 0;
  }

  //Buscar rotinas customizadas no servidor
  sendGetRequestExecutaComandos( "/api/ctrl/cmd?m=de:4f:22:36:99:18" + loopCont );

  //TESTE
  if (rotinaDeTestes( )) {
    if (contaLoop == 0 ) {
      //Serial.println("CA N0-A N1-A N2-A N3-A");
      contSeco = true;
      n0 = false;
      n1 = false;
      n2 = false;
      n3 = false;
    }

    if (contaLoop == 1 ) {
      //Serial.println("CA N0-L N1-A N2-A N3-A");
      contSeco = true;
      n0 = true;
      n1 = false;
      n2 = false;
      n3 = false;
    }

    if (contaLoop == 2 ) {
      //Serial.println("CA N0-L N1-L N2-A N3-A");
      contSeco = true;
      n0 = true;
      n1 = true;
      n2 = false;
      n3 = false;
    }

    if (contaLoop == 3 ) {
      //Serial.println("SA N0-L N1-L N2-A N3-A");
      contSeco = true;
      n0 = true;
      n1 = true;
      n2 = true;
      n3 = false;
    }

    if (contaLoop == 4 ) {
      //Serial.println("SA N0-L N1-A N2-A N3-A");
      contSeco = true;
      n0 = true;
      n1 = true;
      n2 = true;
      n3 = true;
    }

    if (contaLoop == 5 ) {
      //Serial.println("CA N1-L N1-0 N2-A N3-A");
      contSeco = true;
      n0 = true;
      n1 = true;
      n2 = true;
      n3 = false;
    }

    if (contaLoop == 6 ) {
      // Serial.println("CA N0-L N1-L N2-A N3-A");
      contSeco = true;
      n0 = true;
      n1 = true;
      n2 = false;
      n3 = false;
    }

    if (contaLoop == 7 ) {
      //Serial.println("CA N0-L N1-L N2-L N3-A");
      contSeco = true;
      n0 = true;
      n1 = false;
      n2 = false;
      n3 = false;
    }

    if (contaLoop == 8 ) {
      //Serial.println("CA N0-L N1-L N2-L N3-A");
      contSeco = true;
      n0 = true;
      n1 = true;
      n2 = false;
      n3 = false;
    }

    if (contaLoop == 9 ) {
      //Serial.println("CA N0-L N1-L N2-L N3-L");
      contSeco = false;
      n0 = true;
      n1 = true;
      n2 = false;
      n3 = false;
    }

    if (contaLoop == 10 ) {
      contSeco = false;
      n0 = true;
      n1 = false;
      n2 = false;
      n3 = false;
    }


    if (contaLoop == 11 ) {
      contSeco = false;
      n0 = false;
      n1 = false;
      n2 = false;
      n3 = false;
    }

    if (contaLoop == 12) {
      contSeco = true;
      n0 = true;
      n1 = true;
      n2 = false;
      n3 = true;
    }

    if (contaLoop == 13) {
      contSeco = true;
      n0 = true;
      n1 = true;
      n2 = false;
      n3 = true;
    }
  }
  //FIM TESTE
  //Toda a lógica de funcionamento deve ocorrer no interior desse if
  //Se circuito CS FECHADO então...(ÁGUA NO CANO)
  //CHECAR NÍVEL 0
  if (!falha) {
    if (contSeco ) {
      // se o circuito de nivel0 estiver FECHADO (BOIA 0 LEVANTADA)
      sendGetRequest(  "/api/ctrl/cS?agua=1" + MAC + loopCont, 500);

      if (n0 && acionamentoAutomatico( ) ) {
        //Serial.println( "NIVEL-0");
        rotinaN0Levantado();
        defineNivel_Atual(0);
        if (!n3 && !n2 && !n1) {
          sendGetRequest(  "/api/ctrl/nH?n=0" + MAC + loopCont, 500);
        }
        //CHECAR NÍVEL 1
        if (n1 && n0 ) {
          //Serial.println( "NIVEL-1");
          rotinaN1Levantado();
          defineNivel_Atual(1);
          if (!n3 && !n2 ) {
            sendGetRequest(  "/api/ctrl/nH?n=1" + MAC + loopCont, 500);
          }
          //CHECAR NÍVEL 2
          if (n2 && n1 && n0 ) {
            //Serial.println( "NIVEL-N2");
            rotinaN2Levantado();
            defineNivel_Atual(2);
            if (!n3) {
              sendGetRequest(  "/api/ctrl/nH?n=2" + MAC + loopCont, 500);
            }
            //CHECAR NÍVEL 3
            if (n3 && n2 && n1 && n0 ) {
              //Serial.println( "NIVEL-N3");
              rotinaN3Levantado();
              defineNivel_Atual(3);
              bomba_on_offAutomatico = true;
              sendGetRequest(  "/api/ctrl/nH?n=3" + MAC + loopCont, 500);
              desligaBomba();
              sendGetRequest(  "/api/ctrl/b?onOff=0" + MAC + loopCont, 500);

            } else {
              rotinaN3Abaixado();
              ligaBomba();
              sendGetRequest(  "/api/ctrl/b?onOff=1" + MAC + loopCont, 500);
              //Serial.println( "NIVEL-3-V");
            }

            //desligaBomba();
          } else {
            rotinaN2Abaixado();
            ligaBomba();
            sendGetRequest(  "/api/ctrl/b?onOff=1" + MAC + loopCont, 500);
            //Serial.println( "NIVEL-2-V");
          }

          //desligaBomba();
        } else {
          rotinaN1Abaixado();
          ligaBomba();
          sendGetRequest(  "/api/ctrl/b?onOff=1" + MAC + loopCont, 500);
          //Serial.println( "NIVEL-1-V");
        }
        //desligaBomba();
      } else {
        //Serial.println( "NIVEL-0-V");

        if (bomba_on_offAutomatico)
        {
          rotinaN0Abaixado();
        }
        sendGetRequest(  "/ap i/ctrl/b?onOff=1" + MAC + loopCont, 500);
        ligaBomba();
      }
    }

    //Se circuito CS ABERTO então...(SECO NO CANO)
    else {
      sendGetRequest(  "/api/ctrl/cS?agua=0" + MAC + loopCont, 500);
      // se o circuito de nivel0 estiver FECHADO (BOIA 0 LEVANTADA)
      if (n0) {
        //Serial.println( "NIVEL-0-S/A");
        rotinaN0Levantado();
        if (!n3 && !n2 && !n1) {
          sendGetRequest(  "/api/ctrl/nH?n=0" + MAC + loopCont, 500);
        }
        //CHECAR NÍVEL 1
        if (n1) {
          //Serial.println( "NIVEL-1-S/A");
          rotinaN1Levantado();
          if (!n3 && !n2 ) {
            sendGetRequest(  "/api/ctrl/nH?n=1" + MAC + loopCont, 500);
          }

          //CHECAR NÍVEL 2
          if (n2) {
            //Serial.println( "NIVEL-2-S/A");
            rotinaN2Levantado();
            if (!n3 ) {
              sendGetRequest(  "/api/ctrl/nH?n=2" + MAC + loopCont, 500);
            }

            //CHECAR NÍVEL 3
            if (n3) {
              //Serial.println( "NIVEL-3-S/A");
              rotinaN3Levantado();
              desligaBomba();
              bomba_on_offAutomatico = true;
              sendGetRequest(  "/api/ctrl/nH?n=3" + MAC + loopCont, 500);
              //sendGetRequest(  "/api/ctrl/notificaCheio" );
            } else {
              //Serial.println( "NIVEL-3-V-S/A");
              rotinaN3Abaixado();
              desligaBomba();
              sendGetRequest(  "/api/ctrl/b?onOff=0" + MAC + loopCont, 500);
            }

            //desligaBomba();
          } else {
            //Serial.println( "NIVEL-2-V-S/A");
            rotinaN2Abaixado();
            desligaBomba();
            sendGetRequest(  "/api/ctrl/b?onOff=0" + MAC + loopCont, 500);
          }

          //desligaBomba();
        } else {
          //Serial.println( "NIVEL-1-V-S/A");
          rotinaN1Abaixado();
          desligaBomba();
          sendGetRequest(  "/api/ctrl/b?onOff=0" + MAC + loopCont, 500);
        }
        //desligaBomba();
      } else {
        //Serial.println( "NIVEL-0-V-S/A");
        rotinaN0Abaixado();
        desligaBomba();
        sendGetRequest(  "/api/ctrl/b?onOff=0" + MAC + loopCont, 500);
      }

      //configurar um delay de 5 minutos
      //delay(10000);
      desligaBomba();
      //sendGetRequest(  "/api/ctrl/b?onOff=0" + MAC + loopCont);
    }
  } else {
    rotinaFalha( );
  }


  //BEGIN-ALARME
  if (!n0) {
    if ( n1 || n2 || n3) {
      if ( n1 ) {

        if (n1) {
          nomeFalha0 = String(String("_N0A-N1L+"));
          falhaN1 = true;
        } else {
          falhaN1 = false;
        }

      } else if ( n2  ) {

        if (n2) {
          nomeFalha0 = "_N0A-N2L+";
          falhaN2 = true;
        } else {
          falhaN2 = false;
        }

      } else if ( n3 ) {

        if (n3) {
          nomeFalha0 = "_N0A-N3L+";
          falhaN3 = true;
        } else {
          falhaN3 = false;
        }

      }
    }
  } else {
    falhaN0 = false;
  }

  if (!n1) {
    if ( n2 || n3) {
      if ( n2  ) {

        if (n2) {
          nomeFalha1 = "_N1A-N2L+";
          falhaN2 = true;
        } else {
          falhaN2 = false;
        }

      } else if ( n3 ) {

        if (n3) {
          nomeFalha1 = "_N1A-N3L+";
          falhaN3 = true;
        } else {
          falhaN3 = false;
        }
      }
    }
  }

  if (!n2) {
    if ( n3 ) {
      nomeFalha2 = "_N2A-N3L+";
      falhaN3 = true;
    } else {
      falhaN3 = false;
    }
  }

  if (falhaN0 || falhaN1 || falhaN2 || falhaN3) {
    delay(delayTime);
    desligaBomba();
    falha = true;
    String conjuntoDeFalhas =  String(nomeFalha0) +  String(nomeFalha1)  + String(nomeFalha2);
    sendGetRequest( "/api/ctrl/a?a=" + conjuntoDeFalhas  + MAC + loopCont , 600 );
  } else {
    falha = false;
  }
  nomeFalha0 = String("_N0-OK");
  nomeFalha1 = String("_N1-OK");
  nomeFalha2 = String("_N2-OK");
  nomeFalha3 = String("_N3-OK");
  falhaN0 = false;
  falhaN1 = false;
  falhaN2 = false;
  falhaN3 = false;
  //END-ALARME

  contaLoop++;
  contaLoopGlobal++;

  // espera 1 segundo para o proximo loop
  //Serial.println("**FIM LOOP**");
}

//Checa contra seco
boolean temAgua() {

  //Se circuito CS FECHADO então...(ÁGUA NO CANO)
  if (digitalRead( pinContraSeco ) == HIGH) {
    return true;
  } else if (digitalRead( pinContraSeco ) == LOW) {
    //Serial.println("CS EST LOW S/Agua: " );
    return false;
  }
}

void ligaBomba () {
  //aqui liga bomba
  if (( Nivel_Atual <= Nivel_Minimo_Para_Acionamento ) && !falha) {
    digitalWrite(pinRele, LOW);
  } else if ( isCheio ) {
    digitalWrite(pinRele, HIGH);
  }
}

void desligaBomba () {
  //aqui desliga a bomba
  digitalWrite(pinRele, HIGH);
}

void desligaTudo() {
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
    return false;
    //Boia Levantada
  } else if (digitalRead(nivel0) == HIGH) {
    //Serial.println("N-0 HIGH Boia L: " );
    return true;
  }

}


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
    return false;
    //Boia Levantada
  } else if (digitalRead(nivel1) == HIGH) {
    return true;
  }

}

void rotinaN1Abaixado() {
  isCheio = false;
  digitalWrite(ledAzul, HIGH);
  digitalWrite(ledVerde, HIGH);
  digitalWrite(ledAmarelo, HIGH);
}


void rotinaN1Levantado() {
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
    return false;
    //Boia Levantada
  } else if (digitalRead(nivel2) == HIGH) {
    return true;
  }
}

void rotinaN2Abaixado() {
  isCheio = false;
  digitalWrite(ledVerde, HIGH);
  digitalWrite(ledAmarelo, HIGH);
}
void rotinaN2Levantado() {
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
    return false;
    //Boia Levantada
  } else if (digitalRead(nivel3) == HIGH) {
    return true;
  }

}

void rotinaN3Abaixado() {
  isCheio = false;
  digitalWrite(ledAmarelo, HIGH);
}


void rotinaN3Levantado() {
  isCheio = true;
  digitalWrite(ledAmarelo, LOW);
}


int defineNivel_Atual( int nivel) {
  Nivel_Atual = nivel;
}

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
  //Serial.println(response);
  return response;
}


String sendPostRequest( String request, String requestLength ) {
  //Abrindo conexão TCP
  startTCPConnection();
  atCommand("AT+CIPSEND=" + requestLength, timeout);
  String response = atCommand(request, 1000);
  //Serial.println(response);

  //Serial.println(response);
  //fechando conexão TCP
  closeTCPConnection();
  return response;

}


//Médodo para enviar rquisisão get
String sendGetRequest(  String uri, int value )
{
  //Abrindo conexão TCP
  startTCPConnection();
  String requestGet = "GET " + uri + " HTTP/1.1\r\n"
                      + "Host: " + server + "\r\n"
                      + "Connection: keep-alive\r\n\r\n";

  String requestLengthGet = String(requestGet.length());

  atCommand("AT+CIPSEND=" + requestLengthGet, timeout);
  String response = atCommand(requestGet, value);

  //Fechando conexão TCP
  closeTCPConnection();

  //Serial.println("RequestGet: " + requestGet);
  //Serial.println("Response: " + response);

  return response;
}


//
void rotinaFalha() {
  int contAux = 0;

  while (contAux < 10) {
    if (contAux % 2 == 0)



    {
      digitalWrite(ledVermelho, LOW);
      digitalWrite(ledAzul, LOW);
      digitalWrite(ledVerde, LOW);
      digitalWrite(ledAmarelo, LOW);

      delay(300);
    }

    else {
      digitalWrite(ledVermelho, HIGH);
      digitalWrite(ledAzul, HIGH);
      digitalWrite(ledVerde, HIGH);
      digitalWrite(ledAmarelo, HIGH);
      delay(300);
    }
    contAux ++;
  }

}

/*WIFI CONFIG*/
//Metodo para enviar comandos AT para ESP8266
String atCommandCmd(String command, int timeout) {
  esp8266.println(command);
  boolean isCmd = false;
  boolean isCmdNivel = false;
  boolean isCmdBomba = false;
  boolean isCmdTeste = false;

  String respostaCmd = "";
  String respostaCmdBomba = "";
  String respostaCmdNivelMin = "";
  String respostaCmdTeste = "";

  long int time = millis();

  while ( (time + timeout) > millis() ) {
    while (esp8266.available()) {
      char c = esp8266.read();

      if (c == '#' || isCmd) {
        isCmd = true;
        respostaCmd += c;
      }

      //COMANDO START BOMBA
      if (c == '<' || isCmdBomba) {
        isCmdBomba = true;
        respostaCmdBomba += c;

        if (c == '1' ) {
          bomba_on_offAutomatico = true;
        } else if (c == '0' ) {
          bomba_on_offAutomatico = false;
        }
        if (c == '_') {
          isCmdBomba = false;
        }
      }
      //fim cmd bomba

      //Comando Nivel  MInimo
      if (c == '>' || isCmdNivel) {
        isCmdNivel = true;
        respostaCmdNivelMin += c;

        if (c == '0') {
          Nivel_Minimo_Para_Acionamento = 0;
        } else if (c == '1' ) {
          Nivel_Minimo_Para_Acionamento = 1;
        } else if (c == '2' ) {
          Nivel_Minimo_Para_Acionamento = 2;
        } else if (c == '3' ) {
          Nivel_Minimo_Para_Acionamento = 3;
        }
        if (c == '_') {
          isCmdNivel = false;
        }
      }
      //fim cmd nivel minimo

      //cmd testes
      //COMANDO START teste
      if (c == '%' || isCmdTeste) {
        isCmdTeste = true;
        respostaCmdTeste += c;

        if (c == '1' ) {
          teste = true;
        } else if (c == '0' ) {
          teste = false;
        }
        if (c == '_') {
          isCmdTeste = false;
        }
      }
      //fim cmd teste
    }
  }
  /*
    if (respostaCmdBomba.length() > 2) {
    Serial.println("Cmd bomba encontrado: " + respostaCmdBomba + " - valor Estado Setado: " + String(bomba_on_offAutomatico));
    }
    if (respostaCmdNivelMin.length() > 2) {
    Serial.println("Cmd Nivel encontrado: " + respostaCmdNivelMin + " - valor Nivel setado: " + String(Nivel_Minimo_Para_Acionamento));
    }
    if (respostaCmdTeste.length() > 2) {
    Serial.println("Cmd Teste encontrado: " + respostaCmdTeste + " - valor Teste setado: " + String(teste));
    }
  */
  return respostaCmd;
}

String sendGetRequestExecutaComandos(  String uri )
{

  //Abrindo conexão TCP
  startTCPConnection();
  String requestGet = "GET " + uri + " HTTP/1.1\r\n"
                      + "Host: " + server + "\r\n"
                      + "Connection: keep-alive\r\n\r\n";

  String requestLengthGet = String(requestGet.length());

  atCommandCmd("AT+CIPSEND=" + requestLengthGet, timeout);
  String response = atCommandCmd(requestGet, 600);

  //Fechando conexão TCP
  closeTCPConnection();

  //Serial.println("RequestGet: " + requestGet);
  //Serial.println("Response: " + response);

  return response;
}


//Metodo para Conexão wifi - Com Tentativas recursivas
void connectWifi()//wificonnection
{
  int i = 0;
  String cmd = "AT+CWJAP=\"" + ssid + "\",\"" + password + "\"";
  esp8266.println(cmd);
  delay(3000);

  if (esp8266.find("OK") && i <= 10 ) {
    Serial.println("WIFI Connected!");
    delay(1000);
  }
  else {
    Serial.println("Cannot connect to wifi");
    connectWifi();
    i++;
  }
}

//Metodo para resetar o esp8266
void reset()     {
  esp8266.println("AT+RST");
  delay(1000);
  if (esp8266.find("OK") ) Serial.println("Module Reset");
}

//Metodo para configurar esp8266  e velocidade
/*void esp_buad_rate_change()
  {
  esp8266.println("AT+CIOBAUD=9600");                  //change buad rate
  Serial.println("AT+CIOBAUD=9600");
  }*/


//Metodo para configurar wifi
void wifisetup()
{
  esp8266.println("AT");
  esp8266.println("AT+CWMODE=1");//Comando AT :  3: SoftAP+Station mode
  Serial.println("Wifi ready");
}

//Metodo para abrir TCP
void startTCPConnection() {
  String connect = "AT+CIPSTART=\"TCP\",\"" + server + "\"," + port;
  atCommand(connect, timeout);
}


//Metodo para fchar TCP
void closeTCPConnection() {
  atCommand("AT+CIPCLOSE", timeout);
}


//Metodo para pegar MAC
String getMacAdress() {
  return atCommand("AT+CIPAPMAC?",  timeout);
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

boolean acionamentoAutomatico() {
  if (bomba_on_offAutomatico) {
    Serial.println("AUTOMÁTICO");
  } else {
    Serial.println("MANUAL");
  }
  return bomba_on_offAutomatico;
}

boolean rotinaDeTestes() {
  if (teste) {
    Serial.println("Testes ON");
  } else {

    Serial.println("Testes OFF");
  }
  return teste;
}