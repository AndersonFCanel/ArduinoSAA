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

/*VARIAVEI DO CONTRA SECO*/
#define pinContraSeco 2


//variavel que contem todos os pinos usados para pinMode
int pinosUsados[10] = {ledVerde, ledAmarelo, ledAzul, ledVermelho, nivel0, nivel1, nivel2, nivel3, pinRele, pinContraSeco};
//variavel que term todos os tipos de cada pinMode
int tipoPinos[10] = {OUTPUT, OUTPUT, OUTPUT, OUTPUT, INPUT, INPUT, INPUT, INPUT, OUTPUT, INPUT};

/* CODIGO ANTIGO
  //valor real do sensor
  int valorSensorReservatorio = 0;
  //valor em porcentagem do sensor
  int valorSaidaReservatorio = 0;
*/
void setup()
{
  Serial.begin(9600); // Inicializa o Monitor Serial porta 9600
  //inicializa todos os pinModes
  for (int i = 0; i < sizeof(pinosUsados); i++) {
    pinMode(pinosUsados[i], tipoPinos[i]);
  }
}

void loop() {

  //Se circuito contra seco FECHADO então...(ÁGUA NO CANO)
  Serial.print(digitalRead(pinContraSeco));

  //CHECAR NÍVEL 0
  // se o circuito de nivel0 estiver ABERTO (BOIA 0 ABAIXADA)
  if (digitalRead(nivel0) == HIGH) {
    //nivel baixo
    //liga o led vermelho
    digitalWrite(ledVermelho, HIGH);

    //aqui ligaria a bomba
    if (digitalRead(pinContraSeco) == HIGH) {
      digitalWrite(pinRele, HIGH);
    };

    //***********************************
    //Avisa WS que LIGOU bomba
    //Envia hora e nivel O
    //Avisa WS que Tem água na tubulação
    //Envia hora
    //***********************************

    //por precaução , desliga o verde e o amarelo
    digitalWrite(ledAzul, LOW);
    digitalWrite(ledVerde, LOW);
    digitalWrite(ledAmarelo, LOW);


    // se o circuito de nivel0 estiver FECHADO (BOIA 0 LEVANTADA)
  }  else if (digitalRead(nivel0) == LOW) {

    //CHECAR NÍVEL 1
    // se o circuito de nivel1 estiver ABERTO (BOIA 1 ABAIXADA)
    if (digitalRead(nivel1) == HIGH) {
      //nivel baixo
      //liga o led vermelho
      digitalWrite(ledVermelho, HIGH);
      //liga o led vermelho
      digitalWrite(ledAzul, HIGH);

      //aqui ligaria a bomba
      if (digitalRead(pinContraSeco) == HIGH) {
        digitalWrite(pinRele, HIGH);
      };

      //***********************************
      //Avisa WS que LIGOU bomba
      //Envia hora e nivel O
      //Avisa WS que Tem água na tubulação
      //Envia hora
      //***********************************

      //por precaução , desliga o verde e o amarelo
      digitalWrite(ledVerde, LOW);
      digitalWrite(ledAmarelo, LOW);

      // se o circuito de nivel1 estiver FECHADO  (BOIA 1 LEVANTADA)
    }
  } else  if (digitalRead(nivel1) == LOW) {
    //CHECAR NÍVEL 2

    // se o circuito de nivel2 estiver ABERTO (BOIA 2 ABAIXADA)
    if (digitalRead(nivel2) == HIGH) {
      //nivel baixo
      //liga o led vermelho
      digitalWrite(ledVermelho, HIGH);
      digitalWrite(ledAzul, HIGH);
      digitalWrite(ledAmarelo, HIGH);

      //aqui ligaria a bomba
      if (digitalRead(pinContraSeco) == HIGH) {
        digitalWrite(pinRele, HIGH);
      }

      //***********************************
      //Avisa WS que LIGOU bomba
      //Envia hora e nivel O
      //Avisa WS que Tem água na tubulação
      //Envia hora
      //***********************************

      //por precaução , desliga o verde e o amarelo

      digitalWrite(ledAmarelo, LOW);


      // se o circuito de nivel2 estiver FECHADO (BOIA 2 LEVANTADA)
    }

  }
  // espera 1 segundo para o proximo loop
  delay(10);

}
