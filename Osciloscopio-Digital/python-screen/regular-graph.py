import serial
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import collections

# Configuración del puerto serie
# Ajusta 'COM3' al puerto que use tu placa (ej: 'COM4', 'COM5', etc.)
# En Linux suele ser algo como '/dev/ttyACM0' o '/dev/ttyUSB0'
SERIAL_PORT = 'COM20' 
BAUD_RATE = 115200
MAX_SAMPLES = 1024  # Número de muestras a mostrar en pantalla

# Búfer circular para almacenar las muestras recibidas
data = collections.deque([0] * MAX_SAMPLES, maxlen=MAX_SAMPLES)

def update_plot(frame):
    """Función que se ejecuta periódicamente para actualizar la gráfica."""
    try:
        # Lee todas las líneas disponibles en el búfer serie
        while ser.in_waiting > 0:
            line = ser.readline().decode('utf-8').strip()
            if line.isdigit():
                val = int(line)
                data.append(val)
        
        # Actualiza los datos de la línea en la gráfica
        line_plot.set_ydata(data)
        return line_plot,
    except Exception as e:
        print(f"Error: {e}")
        return line_plot,

# Inicialización de la comunicación serie
try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    print(f"Conectado a {SERIAL_PORT} a {BAUD_RATE} baudios.")
except Exception as e:
    print(f"No se pudo abrir el puerto serie: {e}")
    exit()

# Configuración de la gráfica con Matplotlib
fig, ax = plt.subplots()
line_plot, = ax.plot(range(MAX_SAMPLES), data, color='blue')

ax.set_title("Visualización de Señal Triangular (8-bit)")
ax.set_xlabel("Muestras")
ax.set_ylabel("Amplitud (0-255)")
ax.set_ylim(-10, 265)  # Margen para ver bien los límites 0 y 255
ax.grid(True)

# Animación en tiempo real (se actualiza cada 20ms)
ani = FuncAnimation(fig, update_plot, interval=20, blit=False, cache_frame_data=False)

try:
    plt.show()
except KeyboardInterrupt:
    print("Cerrando aplicación...")
finally:
    ser.close()
    print("Puerto serie cerrado.")