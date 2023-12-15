#include <Arduino.h>


const int pinButton = 16; //pino do botão para simular um travamento
const int LED = 2;

hw_timer_t *timer = NULL; //faz o controle do temporizador (interrupção por tempo)

//função que o temporizador irá chamar, para reiniciar o ESP32
void IRAM_ATTR resetModule(){
  ets_printf("(watchdog) reiniciar\n"); //imprime no log
  esp_restart(); //reinicia o chip
  //esp_restart_noos(); //não funcionou foi substituída pela linha de cima.
}

void setup() {
    Serial.begin(115200);
    Serial.println("running setup");
    
    pinMode(pinButton, INPUT_PULLDOWN);
    pinMode(LED,OUTPUT);
 
    //hw_timer_t * timerBegin(uint8_t num, uint16_t divider, bool countUp)
    /*
       num: é a ordem do temporizador. Podemos ter quatro temporizadores, então a ordem pode ser [0,1,2,3].
      divider: É um prescaler (reduz a frequencia por fator). Para fazer um agendador de um segundo, 
      usaremos o divider como 80 (clock principal do ESP32 é 80MHz). Cada instante será T = 1/(80) = 1us
      countUp: True o contador será progressivo
    */
    timer = timerBegin(0, 80, true); //timerID 0, div 80
    //timer, callback, interrupção de borda
    timerAttachInterrupt(timer, &resetModule, true);
    //timer, tempo (us), repetição
    timerAlarmWrite(timer, 5000000, true); //reinicia após 5s
    timerAlarmEnable(timer); //habilita a interrupção 
    digitalWrite(LED, HIGH);
    delay(3000);
    digitalWrite(LED, LOW);
}

void loop() {
    timerWrite(timer, 0); //reseta o temporizador (alimenta o watchdog) 
    long tme = millis(); //tempo inicial do loop
    //fica preso no loop enquanto o botão estiver pressionado
    while(digitalRead(pinButton))
    {
        Serial.println("botão pressionado: "+String(millis() - tme));
        delay(500);
//        timerWrite(timer, 0); //se fizer esse reset no temporizador, o watchdog não será acionado
    }

    delay(1000);
    Serial.print("tempo passado dentro do loop (ms) = ");
    tme = millis() - tme; //calcula o tempo (atual - inicial)
    Serial.println(tme);
}
