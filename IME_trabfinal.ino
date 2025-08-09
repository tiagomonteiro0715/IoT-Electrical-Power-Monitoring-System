#include <WiFi.h>
#include "secrets.h"
#include "ThingSpeak.h"

char ssid[] = SECRET_SSID;   // your network SSID (name)
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

// Measurement variables
float media_voltageRMS = 0.0;
float media_currentRMS = 0.0;
float media_activePower = 0.0;
float media_apparentPower = 0.0;
float media_powerFactor = 0.0;
float media_frequencyVoltage = 0.0;
float media_frequencyCurrent = 0.0;
int N_amostras = 1000;
int contador_t_voltage = 0;
int contador_t_current = 0;
float freq_voltage;
float freq_current;
int i;
float voltage[1000];
float current[1000];
float power[1000];
float med_voltage = 0;
float med_current = 0;
float valor_tensao = 0;
float valor_corrente = 0;
float valor_potencia = 0;
float apparentpower;
float fator_potencia;
unsigned long timer_out;
unsigned long timer_t_voltage;
unsigned long timer_t_current;
unsigned long aux_timer_2[1000];
float T_voltage = 0;
unsigned long T_current = 0;
float contar_periodo_tensao = 0;
float contar_periodo_corrente = 0;


float leitura_analog(int analog_pin) {
  int x = analogRead(analog_pin);
  float tensao_x = (x / 4096) * 3.3;
  return tensao_x;
}
void sendDataToThingSpeak() {
  ThingSpeak.setField(1, media_voltageRMS);
  ThingSpeak.setField(2, media_frequencyVoltage);
  ThingSpeak.setField(3, media_currentRMS);
  ThingSpeak.setField(4, media_frequencyCurrent );
  ThingSpeak.setField(5, media_activePower);
  ThingSpeak.setField(6, media_apparentPower);
  ThingSpeak.setField(7, media_powerFactor);

  int result = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if (result == 200) {
    Serial.println("Channel update successful.");
  } else {
    Serial.println("Problem updating channel. HTTP error code " + String(result));
  }
}
void setup() {
  Serial.begin(115200);  //Initialize serial
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }

  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {
  unsigned long aux_timer = micros();
  int contador = 0;
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);
    }
  }
  Serial.println("\nConnected.");


  while (micros() - aux_timer < 60000000) {
    unsigned long timer_scan = micros();
    for (i = 0; i < N_amostras; i++) {
      timer_out = micros();
      aux_timer_2[i] = micros();
      voltage[i] = analogRead(34) * (3.3 / 4095);
      med_voltage += voltage[i]; //offset da tensão
      current[i] = analogRead(35) * (3.3 / 4095);
      med_current += current[i]; //offset da corrente
      while (micros() - timer_out < 200) {}
      
    }

    med_voltage = med_voltage / N_amostras;
    med_current = med_current / N_amostras;

    contador_t_voltage=0;
    contador_t_current =0;
    for (i = 0; i < N_amostras ; i++) {
      voltage[i] = (voltage[i] - med_voltage) * 2 * 132; //pegamos no valor que sai do condicionador voltage[index] e descondicionamos com a subtração da media(offset) e multiplicando pela divisão que fizemos e depois multiplicamos pelo valor da constante do transdutor
      current[i] = (current[i] - med_current) * 20; //pegamos no valor que sai do condicionador current[index] e descondicionamos com a subtração da media(offset) e multiplicando pela divisão que fizemos e depois multiplicamos pelo valor da constante do transdutor
     
      if (i > 0) { // Adiciona uma verificação para evitar o acesso ao índice -1
        if (voltage[i] * voltage[i - 1] <= 0) { //verificar zero
          contador_t_voltage++; //incrementar pra contar os zeros
          if (contador_t_voltage > 2 ) { //verificar se e o 3 zero
            T_voltage += (aux_timer_2[i] - timer_t_voltage); //pegar o tempo do periodo
            timer_t_voltage = aux_timer_2[i] ; //pegar o tempo do terceiro zero para o proximo ciclo          
            contador_t_voltage = 1; // por o contador a 1 para nao pegar o 2 zero
            contar_periodo_tensao++ ; //quantos ciclos foram feitos
          }
          else {
            if (contador_t_voltage == 1) { //ver se e o 1 zero
              timer_t_voltage = aux_timer_2[i] ; //pegar o tempo do 1 zero da sinusoide
              Serial.println(aux_timer_2[i]);
            }
          }
        }
      }

      if (i > 0) {
        if (current[i] * current[i - 1] <= 0) { //verificar zero
          contador_t_current++; //incrementar pra contar os zeros
          if (contador_t_current > 2 ) { //verificar se e o 3 zero
            T_current += (aux_timer_2[i]  - timer_t_current); //pegar o tempo do periodo
            timer_t_current = aux_timer_2[i] ; //pegar o tempo do terceiro zero para o proximo ciclo
            contador_t_current = 1; // por o contador a 1 para nao pegar o 2 zero
            contar_periodo_corrente++; //quantos ciclos foram feitos
          }
          else {
            if (contador_t_current == 1) { //ver se e o 1 zero
              timer_t_current = aux_timer_2[i]; //pegar o tempo do 1 zero da sinusoide
            }
          }
        }
      }

      power[i] = voltage[i] * current[i];
      valor_tensao += (voltage[i] * voltage[i]);
      valor_corrente += (current[i] * current[i]);
      valor_potencia += power[i];
      //Serial.println(voltage[i]);
      //Serial.println(current[i]);
    }

      T_voltage = T_voltage / contar_periodo_tensao;  // ver o tempo medio do periodo da tensao
      freq_voltage = 1.0 / (T_voltage / 1000000.0); // calculo da freq da tensao
      T_current = T_current / contar_periodo_corrente; // calculo da freq da corrente
      freq_current = 1.0 / (T_current / 1000000.0); // calculo da freq da corrente
      valor_tensao = sqrt(valor_tensao / N_amostras);
      valor_corrente = sqrt(valor_corrente / N_amostras);
      apparentpower = valor_tensao * valor_corrente;
      valor_potencia = valor_potencia / N_amostras;
      fator_potencia = valor_potencia / apparentpower;
      
      Serial.println(T_voltage);
      Serial.println(contar_periodo_tensao);
      Serial.println(valor_tensao);
      Serial.println(valor_corrente);
      Serial.println(valor_potencia);
      Serial.println(apparentpower);
      Serial.println(fator_potencia);
      Serial.println(freq_voltage);
      Serial.println(freq_current);

      T_voltage = 0;
      contar_periodo_tensao = 0;
      T_current = 0;
      contar_periodo_corrente = 0;

      media_frequencyVoltage += freq_voltage;
      media_frequencyCurrent += freq_current;
      media_voltageRMS +=  valor_tensao;
      media_currentRMS += valor_corrente;
      media_activePower += valor_potencia ;
      media_powerFactor += fator_potencia ;
      media_apparentPower +=  apparentpower ;
      while (micros() - timer_scan < 10000000) {}
      contador++;
  }

  media_frequencyVoltage = media_frequencyVoltage / contador ;
  media_frequencyCurrent = media_frequencyCurrent / contador ;
  media_voltageRMS = media_voltageRMS / contador ;
  media_currentRMS = media_currentRMS / contador ;
  media_activePower = media_activePower / contador ;
  media_powerFactor = media_powerFactor / contador ;
  media_apparentPower = media_apparentPower / contador ;

  sendDataToThingSpeak();

  media_frequencyVoltage = 0;
  media_frequencyCurrent = 0;
  media_voltageRMS = 0;
  media_voltageRMS = 0;
  media_currentRMS = 0;
  media_activePower = 0;
  media_powerFactor = 0;
  media_apparentPower = 0;

}
