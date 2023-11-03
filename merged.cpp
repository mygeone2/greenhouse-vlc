#include <iostream>
#include <string>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include <cstring>
#include <vector>

        // Supongamos que tienes una variable 'stateChanged' que es true cuando el estado cambia
        bool stateChanged = false; // Debes actualizar esta variable en tu lógica de ADC
        std::string currentStateText = "Estado: X"; // Debes actualizar este texto con el estado actual

    uint32_t DEBOUNCE_TIME = 10000;
    uint32_t DEBOUNCE_TIME_ON = 500;
    bool threshold_crossed = false;
    uint32_t start_time_crossed = 0;
    bool print_value = false;
    bool print_time = false;
    uint16_t THRESHOLD = 2750;
    uint32_t last_crossed_time = 0;
    uint32_t start_timer_on = 0;
    uint32_t temp = 0;
    bool identified_change_on_to_off = false;
    std::vector<uint32_t> v;
    bool print_vector = false;
    bool stop_at_first_below_threshold = false;
    uint32_t end_timer_on = 0;
    uint32_t current_combination_state = 0;
    uint32_t prev_combination_state = 0;
    std::vector<std::pair<uint32_t, uint32_t>> 
    sensor_ranges_measures = {{25000, 30000}, {50000, 55000}, {70000, 80000}, {100000, 105000}, {126000, 129000}, {150000, 155000}, {175000, 180000}, {200000, 205000}};


void i2c_write_byte(uint8_t val) {
#ifdef i2c_default
    i2c_write_blocking(i2c_default, addr, &val, 1, false);
#endif
}

void lcd_toggle_enable(uint8_t val) {
    const int DELAY_US = 600;
    sleep_us(DELAY_US);
    i2c_write_byte(val | LCD_ENABLE_BIT);
    sleep_us(DELAY_US);
    i2c_write_byte(val & ~LCD_ENABLE_BIT);
    sleep_us(DELAY_US);
}

void lcd_send_byte(uint8_t val, int mode) {
    uint8_t high = mode | (val & 0xF0) | LCD_BACKLIGHT;
    uint8_t low = mode | ((val << 4) & 0xF0) | LCD_BACKLIGHT;

    i2c_write_byte(high);
    lcd_toggle_enable(high);
    i2c_write_byte(low);
    lcd_toggle_enable(low);
}

void lcd_clear(void) {
    lcd_send_byte(LCD_CLEARDISPLAY, LCD_COMMAND);
}

void lcd_set_cursor(int line, int position) {
    int val = (line == 0) ? 0x80 + position : 0xC0 + position;
    lcd_send_byte(val, LCD_COMMAND);
}

static void inline lcd_char(char val) {
    lcd_send_byte(val, LCD_CHARACTER);
}

void lcd_string(const char *s) {
    while (*s) {
        lcd_char(*s++);
    }
}

void lcd_init() {
    lcd_send_byte(0x03, LCD_COMMAND);
    lcd_send_byte(0x03, LCD_COMMAND);
    lcd_send_byte(0x03, LCD_COMMAND);
    lcd_send_byte(0x02, LCD_COMMAND);

    lcd_send_byte(LCD_ENTRYMODESET | LCD_ENTRYLEFT, LCD_COMMAND);
    lcd_send_byte(LCD_FUNCTIONSET | LCD_2LINE, LCD_COMMAND);
    lcd_send_byte(LCD_DISPLAYCONTROL | LCD_DISPLAYON, LCD_COMMAND);
    lcd_clear();
}


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
    std::string input_buffer;

    // Inicialización de la pantalla LCD
    i2c_init(i2c_default, 100 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    lcd_init();

    

    while (true) {
       {
            uint16_t result = adc_read();
            if (print_value)
            {
                std::cout << result << std::endl;
            }
            if (result > THRESHOLD)
            {
                uint32_t crossTime = time_us_32();
                uint32_t difference = crossTime - last_crossed_time;
                if (print_time && difference > DEBOUNCE_TIME)
                {
                    start_timer_on = crossTime;
                    stop_at_first_below_threshold = false;
                }
                last_crossed_time = crossTime;
            }
            else
            {
                uint32_t measure_below = time_us_32();
                uint32_t difference = measure_below - last_crossed_time;
            
                if (print_time && difference > DEBOUNCE_TIME_ON && !stop_at_first_below_threshold){
                    stop_at_first_below_threshold = true;
                    end_timer_on = measure_below;
                    uint32_t time_signal_stayed_on = end_timer_on - start_timer_on - 15000;
                    //if(print_time && time_signal_stayed_on > DEBOUNCE_TIME_ON){
                    //std::cout << time_signal_stayed_on - 15000 << std::endl;
                    //}   
                    
                    if (time_signal_stayed_on > sensor_ranges_measures[0].first && time_signal_stayed_on < sensor_ranges_measures[0].second)
                    {
                        current_combination_state = 1;
                        currentStateText = "Estado: 1";
                    }
                    else if (time_signal_stayed_on > sensor_ranges_measures[1].first && time_signal_stayed_on < sensor_ranges_measures[1].second)
                    {
                        current_combination_state = 2;
                         currentStateText = "Estado: 2";
                    }
                    else if (time_signal_stayed_on > sensor_ranges_measures[2].first && time_signal_stayed_on < sensor_ranges_measures[2].second)
                    {
                        current_combination_state = 3;
                         currentStateText = "Estado: 3";
                    }
                    else if (time_signal_stayed_on > sensor_ranges_measures[3].first && time_signal_stayed_on < sensor_ranges_measures[3].second)
                    {
                        current_combination_state = 4;
                         currentStateText = "Estado: 4";
                    }
                    else if (time_signal_stayed_on > sensor_ranges_measures[4].first && time_signal_stayed_on < sensor_ranges_measures[4].second)
                    {
                        current_combination_state = 5;
                         currentStateText = "Estado: 5";
                    }
                    else if (time_signal_stayed_on > sensor_ranges_measures[5].first && time_signal_stayed_on < sensor_ranges_measures[5].second)
                    {
                        current_combination_state = 6;
                         currentStateText = "Estado: 6";
                    }
                    else if (time_signal_stayed_on > sensor_ranges_measures[6].first && time_signal_stayed_on < sensor_ranges_measures[6].second)
                    {
                        current_combination_state = 7;
                         currentStateText = "Estado: 7";
                    }
                    else if (time_signal_stayed_on > sensor_ranges_measures[7].first && time_signal_stayed_on < sensor_ranges_measures[7].second)
                    {
                        current_combination_state = 8;
                         currentStateText = "Estado: 8";
                    }
                    if (prev_combination_state != current_combination_state)
                    {
                        std::cout << "Current combination state: " << current_combination_state << std::endl;
                        prev_combination_state = current_combination_state;
                        stateChanged = true;

                    }

                    
                    
                }
            
                
            }
            int c = getchar_timeout_us(0);
            if (c != PICO_ERROR_TIMEOUT)
                {
                    switch (c)
                    {
                    case 's':
                        print_value = true;
                        print_time = false;
                        print_vector = false;
                        break;
                    case 'v':
                        print_vector = true;
                        print_time = false;
                        print_value = false;
                        break;
                    case 'm':
                        print_time = true;
                        print_value = false;
                        print_vector = false;
                        std::cout << "Modo print" << THRESHOLD << std::endl;
                        break;
                    case 't':
                        getline(std::cin, input_buffer);
                        if (input_buffer[0] == ' ')
                        {
                            uint16_t new_threshold = std::stoi(input_buffer.substr(1));
                            THRESHOLD = new_threshold;
                            std::cout << "Umbral ajustado a: " << THRESHOLD << std::endl;
                        }
                        break;
                    case 'd':
                        getline(std::cin, input_buffer);
                        if (input_buffer[0] == ' ')
                        {
                            uint16_t new_debounce = std::stoi(input_buffer.substr(1));
                            DEBOUNCE_TIME = new_debounce * 1000;
                            std::cout << "Debounce ajustado a: " << DEBOUNCE_TIME << std::endl;
                        }
                        break;
                    case 'o':
                        getline(std::cin, input_buffer);
                        if (input_buffer[0] == ' ')
                        {
                            uint16_t new_debounce = std::stoi(input_buffer.substr(1));
                            DEBOUNCE_TIME_ON = new_debounce * 1000;
                            std::cout << "Debounce ajustado ON a: " << DEBOUNCE_TIME_ON << std::endl;
                        }
                        break;
                    default:
                        break;
                    }
                }
            
        }



        if (stateChanged) {
            // Imprime el estado actual en la LCD y la terminal serial
            printToLCDAndSerial(currentStateText.c_str());
            stateChanged = false; // Recuerda resetear la bandera
        }

        // ... (Cualquier otra lógica que necesites)
    }
}
