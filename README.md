
# Documentação do Projeto de Temporizador de LEDs

## Objetivo
Este projeto tem como objetivo controlar três LEDs RGB usando um botão. Quando o botão é pressionado, os LEDs acendem e, após um atraso de 3 segundos entre cada um, eles se apagam de forma sequencial. O código é implementado para rodar no microcontrolador Raspberry Pi Pico W, com o uso de interrupções e temporizadores.

## Componentes Utilizados
- **Microcontrolador:** Raspberry Pi Pico W
- **LEDs RGB:** Conectados aos pinos GPIO 13, 12 e 11
- **Resistores:** 3 resistores de 330Ω para os LEDs
- **Botão (Pushbutton):** Conectado ao pino GPIO 5

## Funcionamento do Código

### 1. **Configuração dos Pinos**
No início, configuramos os pinos necessários:

```c
// Configuração dos pinos dos LEDs como saída
for(int i = 0; i < turns; i++) ConfigPins(PINS_RGB[i], GPIO_OUT, false);

// Configuração do pino do botão como entrada com pull-up
ConfigPins(PIN_BUTTON, GPIO_IN, true);
```
- Os pinos dos LEDs (GPIOs 13, 12, 11) são configurados como **saídas**.
- O pino do botão (GPIO 5) é configurado como **entrada** com resistor de **pull-up**.

### 2. **Função Principal (`main`)**
A função principal inicializa os pinos e configura a interrupção do botão para detectar uma **queda de borda** (transição de nível alto para baixo).

```c
// Configuração da interrupção para o botão (queda de borda)
gpio_set_irq_enabled_with_callback(PIN_BUTTON, GPIO_IRQ_EDGE_FALL, true, &button_callback);
```
Em seguida, o código entra em um **loop infinito** monitorando o estado do botão.

### 3. **Interrupção do Botão (`button_callback`)**
Quando o botão é pressionado, a interrupção é acionada e o estado do botão é alterado:

```c
void button_callback(uint gpio, uint32_t events) {
    if(BUTTON_STATE == false && LED_STATE == false) {
        BUTTON_STATE = true;  // Estado do botão é alterado para pressionado
    }
}
```
Se o sistema não está executando nenhuma sequência de apagamento, ele inicia o acendimento dos LEDs.

### 4. **Acendimento dos LEDs**
Após a pressão do botão, todos os LEDs são acesos simultaneamente:

```c
// Acende todos os LEDs
for (int i = 0; i < turns; i++) gpio_put(PINS_RGB[i], LED_STATE);
```

### 5. **Temporização e Sequência de Apagamento dos LEDs**
O código utiliza a função **`add_alarm_in_ms()`** para criar alarmes temporizados de 3 segundos, acionando a função **`Off_RGB_callback()`** para apagar os LEDs sequencialmente.

```c
// Adiciona o alarme para apagar o primeiro LED
add_alarm_in_ms(TIMER_ALARM_MS, Off_RGB_callback, NULL, false);
```

Cada vez que a função **`Off_RGB_callback()`** é chamada, um LED é apagado:

```c
// Função de callback para apagar os LEDs
int64_t Off_RGB_callback(alarm_id_t id, void *user_data) {
    printf("apagando LED %d
", (ALARM_COUNT+1));  
    gpio_put(PINS_RGB[ALARM_COUNT], false);  // Apaga o LED
    ALARM_COUNT++;  // Incrementa a contagem de LEDs apagados

    if(ALARM_COUNT == turns) {
        LED_STATE = false;  // Todos os LEDs apagados
        ALARM_COUNT = 0;  // Reseta o contador
        return 0;  // Parar o alarme
    }

    // Adiciona o próximo alarme
    add_alarm_in_ms(TIMER_ALARM_MS, Off_RGB_callback, NULL, false);  
    return 0;
}
```

### 6. **Controle de Estado**
- **`LED_STATE`** controla se os LEDs estão acesos ou apagados.
- **`BUTTON_STATE`** mantém o estado do botão, garantindo que o ciclo de LEDs não seja interrompido enquanto está em execução.

```c
// Controle do estado dos LEDs e do botão
if(BUTTON_STATE == true && LED_STATE == false) {
    BUTTON_STATE = false;
    LED_STATE = true;
    printf("Botão Acionado...
");
}
```

### 7. **Debouncing do Botão (Opcional)**
Embora o debouncing não esteja explicitamente implementado no código fornecido, o efeito de **bouncing** do botão pode ser mitigado programaticamente. O código garante que, enquanto os LEDs estão sendo apagados, o botão não altere o comportamento do sistema.

### 8. **Fluxo de Execução**
1. O sistema começa com os LEDs apagados.
2. O usuário pressiona o botão, acionando os LEDs para acender.
3. Após 3 segundos, o primeiro LED é apagado.
4. O segundo LED é apagado após mais 3 segundos.
5. O último LED é apagado após mais 3 segundos.
6. O contador de LEDs apagados é resetado, e o sistema aguarda novamente o pressionamento do botão.

### 9. **Simulação no Wokwi**
O código foi projetado para ser simulado no **Wokwi**, uma ferramenta de simulação online. O Wokwi permite testar o código junto com o diagrama do circuito (definido no arquivo `diagram.json`), incluindo os LEDs e o botão, sem a necessidade de hardware físico.

---

## Conclusão
Este projeto exemplifica o uso de temporizadores e interrupções para criar uma sequência controlada de LEDs acionados por um botão. Utilizando o Raspberry Pi Pico W, a simulação no Wokwi permite testar o código sem a necessidade de hardware físico, facilitando o desenvolvimento e a depuração. O controle sequencial dos LEDs e a temporização de 3 segundos entre cada LED fornecem um exemplo prático de como controlar dispositivos eletrônicos de maneira eficiente e simples.
