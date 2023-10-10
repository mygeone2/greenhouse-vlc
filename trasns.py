import machine
import utime
import urandom

# Configuración de pines PWM
output_pin = machine.Pin(0)
pwm = machine.PWM(output_pin)
pwm.freq(40000)
pwm.duty_u16(0)  # Iniciar con PWM apagado

# Función para enviar un bit usando codificación Manchester
def send_manchester(bit):
    half_period = 5  # Tiempo en ms para la mitad del período de un bit
    if bit == 1:
        pwm.duty_u16(0)
        utime.sleep_ms(half_period)
        pwm.duty_u16(32767)
        utime.sleep_ms(half_period)
    else:
        pwm.duty_u16(32767)
        utime.sleep_ms(half_period)
        pwm.duty_u16(0)
        utime.sleep_ms(half_period)

try:
    while True:
        # Generar una trama aleatoria de 4 bits representando el estado de los 4 actuadores
        data = [urandom.randint(0, 1) for _ in range(4)]
        print("Enviando trama:", data)  # Imprimir la trama para depuración
        
        # Pausa Larga (ejemplo: 100ms) para indicar el inicio de una nueva trama
        pwm.duty_u16(0)
        utime.sleep_ms(100)
        
        for bit in data:
            send_manchester(bit)
        
        # Pausa Corta (ejemplo: 10ms) entre tramas
        pwm.duty_u16(0)
        utime.sleep_ms(100)

except KeyboardInterrupt:
    pwm.deinit()

