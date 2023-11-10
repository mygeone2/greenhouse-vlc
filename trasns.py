from machine import Pin
import utime
import urandom

# Configuración de pines PWM
output_pin = Pin(0, Pin.OUT)

time_cycle = 25   # ms -> 1000 us
time_sleep = 30  # ms -> 25000 us
update_interval = 1000  # ms (5 segundos)
last_update = utime.ticks_ms()
random_value = urandom.randint(1, 8)  # valor inicial

while True:
    # Si han pasado 5 segundos desde la última actualización, genera un nuevo valor random
    if utime.ticks_diff(utime.ticks_ms(), last_update) >= update_interval:
        random_value = urandom.randint(1, 8)
        print(random_value)

        last_update = utime.ticks_ms()

    # Envía el pulso
    output_pin.value(1)
    utime.sleep_ms(time_cycle * random_value)
    output_pin.value(0)
    utime.sleep_ms(time_sleep)

