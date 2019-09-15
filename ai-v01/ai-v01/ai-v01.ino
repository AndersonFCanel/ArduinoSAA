/*VARIAVEIS DO SISTEMA-- ANTIGO*/
#define sensorReservatorio A0
#define sesorContraSeco A1
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
#define pinContraSeco 3


//variavel que contem todos os pinos usados para pinMode
int pinosUsados[10] = {ledVerde, ledAmarelo, ledAzul, ledVermelho, nivel0, nivel1, nivel2, nivel3, pinRele, pinContraSeco};
//variavel que term todos os tipos de cada pinMode
int tipoPinos[10] = {OUTPUT, OUTPUT, OUTPUT, OUTPUT, INPUT, INPUT, INPUT, INPUT, OUTPUT, INPUT};


void setup()
{
  Serial.begin(9600); // Inicializa o Monitor Serial porta 9600
  //inicializa todos os pinModes
  for (int i = 0; i < sizeof(pinosUsados); i++) {
    pinMode(pinosUsados[i], tipoPinos[i]);
  }
  desligaTudo();
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
            ligaBomba();
          }

          //desligaBomba();
        } else {
          rotinaN2Abaixado();
          ligaBomba();
        }

        //desligaBomba();
      } else {
        rotinaN1Abaixado();
        ligaBomba();
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
    //configurar um delay de 5 minutos
    delay(1000);
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
  digitalWrite(pinRele, LOW);
  Serial.println("pRele LOW ON Pump: " );
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
  digitalWrite(ledVermelho, HIGH);
  //por precaução , desliga o verde e o amarelo
  digitalWrite(ledAzul, HIGH);
  digitalWrite(ledVerde, HIGH);
  digitalWrite(ledAmarelo, HIGH);
}


void rotinaN0Levantado() {
  //liga o led vermelho
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
  digitalWrite(ledAzul, HIGH);
  digitalWrite(ledVerde, HIGH);
  digitalWrite(ledAmarelo, HIGH);
}


void rotinaN1Levantado() {
  //liga o led vermelho
  //digitalWrite(ledVermelho, LOW);
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
  digitalWrite(ledVerde, HIGH);
  digitalWrite(ledAmarelo, HIGH);
}


void rotinaN2Levantado() {
  //liga o led vermelho
  //digitalWrite(ledVermelho, LOW);
  //(ledAzul, LOW);
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
  digitalWrite(ledAmarelo, HIGH);
}


void rotinaN3Levantado() {
  //liga o led vermelho
  //digitalWrite(ledVermelho, LOW);
  //(ledAzul, LOW);
  //digitalWrite(ledVerde, LOW);
  digitalWrite(ledAmarelo, LOW);
}
