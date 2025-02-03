#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/timer.h"


#define PIN_BUTTON 5  // Define o pino do botão como 5
uint PINS_RGB[3] = {13,12,11};  // Define os pinos RGB (3 LEDs) em 13, 12 e 11
int turns = sizeof(PINS_RGB)/sizeof(PINS_RGB[0]);  // Calcula o número de LEDs (3 LEDs)
uint32_t TIMER_ALARM_MS = 3000;  // Define o tempo de alarme (3 segundos)
int ALARM_COUNT = 0;  // Inicializa a contagem de LEDs apagados como 0
bool LED_STATE = false;  // Inicializa o estado do LED como apagado (false)
bool BUTTON_STATE = false;  // Inicializa o estado do botão como não pressionado (false)

// Função de callback para apagar o LED
int64_t Off_RGB_callback(alarm_id_t id, void *user_data) {  
    // Exibe a mensagem de apagamento do LED
    printf("apagando LED %d\n", (ALARM_COUNT+1));  
    gpio_put(PINS_RGB[ALARM_COUNT], false);  // Desliga o LED no índice ALARM_COUNT
    ALARM_COUNT++;  // Incrementa o contador de LEDs apagados
    if(ALARM_COUNT == turns) {  // Se todos os LEDs foram apagados
        LED_STATE = false;  // Define o estado do LED como apagado
        ALARM_COUNT = 0;  // Reseta o contador de LEDs apagados
        return 0;  // Retorna 0 para parar o alarme
    }
    // Adiciona um novo alarme para apagar o próximo LED
    add_alarm_in_ms(TIMER_ALARM_MS, Off_RGB_callback, NULL, false);  
    return 0;  // Retorna 0
}
// Função de callback do botão
void button_callback(uint gpio, uint32_t events) {  
    if(BUTTON_STATE == false && LED_STATE == false) {  // Verifica se o botão não foi pressionado e os LEDs estão apagados
        BUTTON_STATE = true;  // Define o estado do botão como pressionado
    }
}
// Função para configurar os pinos
void ConfigPins(uint pin, bool InorOut, bool pullup) {  
    gpio_init(pin);  // Inicializa o pino
    gpio_set_dir(pin, InorOut);  // Define a direção do pino (entrada ou saída)
    if(pullup) gpio_pull_up(pin);  // Ativa o resistor de pull-up se necessário
    if(!pullup) gpio_put(pin, 0);  // Se não houver pull-up, define o valor do pino como 0
}
int main() {  // Função principal
    stdio_init_all();  // Inicializa a biblioteca de entrada/saída
    // Configura os pinos dos LEDs como saída
    for(int i=0; i<turns; i++) ConfigPins(PINS_RGB[i], GPIO_OUT, false);  
    // Configura o pino do botão como entrada com pull-up
    ConfigPins(PIN_BUTTON, GPIO_IN, true);  
    // Configura a interrupção para o botão (queda de borda)
    gpio_set_irq_enabled_with_callback(PIN_BUTTON, GPIO_IRQ_EDGE_FALL, true, &button_callback);

    while(1) {  // Loop principal
        if(BUTTON_STATE == true && LED_STATE == false) {  // Se o botão foi pressionado e os LEDs estão apagados
            BUTTON_STATE = false;  // Reseta o estado do botão
            LED_STATE = true;  // Define o estado dos LEDs como aceso
            printf("Botão Acionado...\n");  // Exibe a mensagem indicando que o botão foi pressionado
            // Loop para acender todos os LEDs
            for (int i = 0; i < turns; i++) gpio_put(PINS_RGB[i], LED_STATE);  // Acende o LED no índice i
            printf("Apagando 1 led a cada 3s...\n"); // Exibe a mensagem informando que LEDs serão apagados a cada 3 segundos
            // Adiciona o alarme para apagar o primeiro LED
            add_alarm_in_ms(TIMER_ALARM_MS, Off_RGB_callback, NULL, false);  
        }
        sleep_ms(100);  // Espera por 100 milissegundos antes de verificar novamente
    }
    return 0;  // Retorna 0 (não será alcançado pois o loop é infinito)
}
