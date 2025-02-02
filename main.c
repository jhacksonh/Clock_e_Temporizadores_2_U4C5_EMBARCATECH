#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/timer.h"

#define PIN_BUTTON 5
uint PINS_RGB[3] = {13,12,11};
int turns = sizeof(PINS_RGB)/sizeof(PINS_RGB[0]);
uint32_t TIMER_ALARM_MS = 3000;
int ALARM_COUNT = 0;
bool LED_STATE = false;
bool BUTTON_STATE = false;

int64_t Off_RGB_callback(alarm_id_t id, void *user_data){
    printf("apagando LED %d\n", (ALARM_COUNT+1));
    gpio_put(PINS_RGB[ALARM_COUNT], false);
    ALARM_COUNT++;
    if(ALARM_COUNT == turns){
        LED_STATE = false;
        ALARM_COUNT = 0;
        return 0;
    }
    add_alarm_in_ms(TIMER_ALARM_MS, Off_RGB_callback, NULL, false);
    return 0;
}

void button_callback(uint gpio, uint32_t events) {
    if(BUTTON_STATE == false && LED_STATE == false){
        BUTTON_STATE = true;
    }
}

void ConfigPins(uint pin, bool InorOut, bool pullup){
    gpio_init(pin);
    gpio_set_dir(pin, InorOut);
    if(pullup)gpio_pull_up(pin);
    if(!pullup)gpio_put(pin, 0);
}
int main(){
    stdio_init_all();
    for(int i=0;i<turns;i++)ConfigPins(PINS_RGB[i], GPIO_OUT,false);
    ConfigPins(PIN_BUTTON, GPIO_IN,true);
    gpio_set_irq_enabled_with_callback(PIN_BUTTON, GPIO_IRQ_EDGE_FALL,true,&button_callback);

    while(1) {
        if(BUTTON_STATE == true && LED_STATE == false){
            BUTTON_STATE = false;
            LED_STATE = true;
            printf("Botão Acionado...\n");
            for (int i = 0; i < turns; i++){
                printf("Acendendo LED %d\n",(i+1));
                gpio_put(PINS_RGB[i], LED_STATE);
            }
            printf("Apagando 1 led a cada 3s...\n");
            add_alarm_in_ms(TIMER_ALARM_MS,Off_RGB_callback, NULL,false);
        }
        sleep_ms(100);
    }
    return 0;
}