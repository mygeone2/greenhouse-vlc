#include <iostream>
#include <string>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include <cstring>
#include <vector>

// Definiciones y declaraciones globales de tu primer código
// ...

// Funciones para la pantalla LCD (las que ya tienes definidas)
// ...

void printToLCDAndSerial(const char* text) {
    // Imprime en la pantalla LCD
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_string(text);

    // Imprime en la terminal serial
    std::cout << text << std::endl;
}

int main() {
    // Inicialización de la terminal serial y la ADC
    stdio_init_all();
    adc_init();
    adc_gpio_init(26);
    adc_select_input(0);

    // Inicialización de la pantalla LCD
    i2c_init(i2c_default, 100 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    lcd_init();

    // ... (El resto de tu inicialización si es necesario)

    while (true) {
        // ... (Tu lógica de lectura ADC y manejo de estados)

        // Supongamos que tienes una variable 'stateChanged' que es true cuando el estado cambia
        bool stateChanged = false; // Debes actualizar esta variable en tu lógica de ADC
        std::string currentStateText = "Estado: X"; // Debes actualizar este texto con el estado actual

        if (stateChanged) {
            // Imprime el estado actual en la LCD y la terminal serial
            printToLCDAndSerial(currentStateText.c_str());
            stateChanged = false; // Recuerda resetear la bandera
        }

        // ... (Cualquier otra lógica que necesites)
    }
}
