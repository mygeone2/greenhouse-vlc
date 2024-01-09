/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <iostream>
#include <string>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include <vector>




uint32_t DEBOUNCE_TIME = 10000;
uint32_t DEBOUNCE_TIME_ON = 10000;
uint32_t start_time_crossed = 0;
uint16_t THRESHOLD = 2000; // Initial threshold
uint32_t last_crossed_time = 0;
uint32_t start_timer_on = 0;
uint32_t temp = 0;
uint32_t end_timer_on = 0;
uint32_t current_combination_state = 0;
uint32_t prev_combination_state = -1;
uint32_t value_error_threshold = 1000;



bool debug = false;
bool threshold_crossed = false;
bool print_value = false; 
bool print_time = true; 
bool identified_change_on_to_off = false;
bool print_vector = false;
bool stop_at_first_below_threshold = false;
bool error_threshold = false;



// dict with the sensor state and the time range in which it is valid

    std::vector<std::pair<uint32_t, uint32_t>> sensor_ranges_measures = {
        {23000, 30000},
        {48000, 55000},
        {70000, 80000},
        {98000, 105000},
        {123000, 126000},
        {148000, 155000},
        {173000, 180000},
        {198000, 205000}
    };

int main() {
    stdio_init_all();
   
    adc_init();

    adc_gpio_init(28);
    adc_select_input(0);

    gpio_init(2);
    gpio_init(3);
    gpio_init(4);
    gpio_init(5);
    gpio_init(13);

    gpio_set_dir(2, GPIO_OUT);
    gpio_set_dir(3, GPIO_OUT);
    gpio_set_dir(4, GPIO_OUT);
    gpio_set_dir(5, GPIO_OUT);

    //buzzer
    gpio_set_dir(13, GPIO_OUT);

    //print all key variables and their values to the console
    std::cout << "Umbral: " << THRESHOLD << std::endl;
    std::cout << "Debounce: " << DEBOUNCE_TIME << std::endl;
    std::cout << "Debounce ON: " << DEBOUNCE_TIME_ON << std::endl;




    std::string input_buffer;
    
    while (true) {
        uint16_t result = adc_read();

        if(print_value){
            std::cout << result << std::endl;
        }


        if(result < value_error_threshold){
            error_threshold = true;
            gpio_put(13, 1);

            gpio_put(2, 1);
            gpio_put(3, 1);
            gpio_put(4, 1);

        }else{
            error_threshold = false;
            gpio_put(13, 0);
        }

        if (result > THRESHOLD) {
            uint32_t crossTime = time_us_32();
            uint32_t difference = crossTime - last_crossed_time;
            
            
            if (print_time && difference > DEBOUNCE_TIME) {
                start_timer_on = crossTime;
                stop_at_first_below_threshold = false;
            }

            last_crossed_time = crossTime;
        }else{
            uint32_t measure_below = time_us_32();
            uint32_t difference = measure_below - last_crossed_time;
            

            if ((print_time ) && difference > DEBOUNCE_TIME_ON && !stop_at_first_below_threshold) {
                stop_at_first_below_threshold = true;
                end_timer_on = measure_below;

                uint32_t time_signal_stayed_on = end_timer_on - start_timer_on - 10000;
                if (debug && time_signal_stayed_on > DEBOUNCE_TIME_ON) {
                    std::cout << "status: "<< current_combination_state<< " - "<<time_signal_stayed_on << std::endl;
                }

                if(time_signal_stayed_on > sensor_ranges_measures[0].first && time_signal_stayed_on < sensor_ranges_measures[0].second){
                    current_combination_state = 1;
                }else if(time_signal_stayed_on > sensor_ranges_measures[1].first && time_signal_stayed_on < sensor_ranges_measures[1].second){
                    current_combination_state = 2;
                }else if(time_signal_stayed_on > sensor_ranges_measures[2].first && time_signal_stayed_on < sensor_ranges_measures[2].second){
                    current_combination_state = 3;
                }else if(time_signal_stayed_on > sensor_ranges_measures[3].first && time_signal_stayed_on < sensor_ranges_measures[3].second){
                    current_combination_state = 4;
                }else if(time_signal_stayed_on > sensor_ranges_measures[4].first && time_signal_stayed_on < sensor_ranges_measures[4].second){
                    current_combination_state = 5;
                }else if(time_signal_stayed_on > sensor_ranges_measures[5].first && time_signal_stayed_on < sensor_ranges_measures[5].second){
                    current_combination_state = 6;
                }else if(time_signal_stayed_on > sensor_ranges_measures[6].first && time_signal_stayed_on < sensor_ranges_measures[6].second){
                    current_combination_state = 7;
                }else if(time_signal_stayed_on > sensor_ranges_measures[7].first && time_signal_stayed_on < sensor_ranges_measures[7].second){
                    current_combination_state = 8;
                }else{
                    current_combination_state = -1;
                }

                //if (prev_combination_state != current_combination_state) {

                    // if(current_combination_state != -1){
                    //     std::cout << "Senal recuperada correspondiente a configuración: " << current_combination_state << " de " << time_signal_stayed_on << "ms" <<std::endl;
                    //     prev_combination_state = current_combination_state;  // Update the previous state
                    // }else{
                        
                    //     prev_combination_state = current_combination_state;
                    // }
                    


                    // pin 2 -  relay 2 = humidificador
                    // pin 3 -  relay 3 = bomba de agua
                    // pin 4 -  relay 4 = fan
                    

                    // activan con 0 y apagan con 1

                    if (current_combination_state == 1) {
                        gpio_put(2, 1);
                        gpio_put(3, 1);
                        gpio_put(4, 1);
                        
                    }else if (current_combination_state == 2) {
                        gpio_put(2, 1);
                        gpio_put(3, 1);
                        gpio_put(4, 0);
                                            
                    }else if (current_combination_state == 3) {
                        gpio_put(2, 0);
                        gpio_put(3, 1);
                        gpio_put(4, 1);
  
                    }else if (current_combination_state == 4) {
                        gpio_put(2, 0);
                        gpio_put(3, 1);
                        gpio_put(4, 0);
                        
                    }else if (current_combination_state == 5) {
                        gpio_put(2, 1);
                        gpio_put(3, 1);
                        gpio_put(4, 1);
                    }else if (current_combination_state == 6) {
                        gpio_put(2, 1);
                        gpio_put(3, 1);
                        gpio_put(4, 0);
                        
                    }else if (current_combination_state == 7) {
                        gpio_put(2, 0);
                        gpio_put(3, 1);
                        gpio_put(4, 1);
                    }else if (current_combination_state == 8) {
                        gpio_put(2, 0);
                        gpio_put(3, 1);
                        gpio_put(4, 0);                        
                    }
                //}

            }
        }

       

        int c = getchar_timeout_us(0);
        if (c != PICO_ERROR_TIMEOUT) {
            switch (c) {
                case 's':
                    print_value = true;
                    print_time = false;
                    print_vector = false;
                    debug = false;
                    break;
                case 'm':
                    std::cout << "MONITORING" << std::endl;
                    print_time = true;
                    print_value = false;
                    print_vector = false;
                    debug = false;
                    break;
                case 'g':
                    std::cout << "DEBUG" << std::endl;
                    debug = true;
                    print_time = true;
                    print_value = false;
                    print_vector = false;
                    break;   
                case 't':
                    getline(std::cin, input_buffer);
                    if (input_buffer[0] == ' ') {
                        uint16_t new_threshold = std::stoi(input_buffer.substr(1));
                        THRESHOLD = new_threshold;
                        std::cout << "Umbral ajustado a: " << THRESHOLD << std::endl;
                    }
                    break;
                case 'd':
                    getline(std::cin, input_buffer);
                    if (input_buffer[0] == ' ') {
                        uint16_t new_debounce = std::stoi(input_buffer.substr(1));
                        DEBOUNCE_TIME = new_debounce * 1000;
                        std::cout << "Debounce ajustado a: " << DEBOUNCE_TIME << std::endl;
                    }
                    break;
                     case 'e':
                    getline(std::cin, input_buffer);
                    if (input_buffer[0] == ' ') {
                        value_error_threshold = std::stoi(input_buffer.substr(1));

                        std::cout << "error de thrshold ajustado a: " << DEBOUNCE_TIME << std::endl;
                    }
                    break;
                case 'o':
                    getline(std::cin, input_buffer);
                    if (input_buffer[0] == ' ') {
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
}

