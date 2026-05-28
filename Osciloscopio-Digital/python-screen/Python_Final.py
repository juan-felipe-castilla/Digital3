import serial
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from matplotlib.widgets import Button, Slider

# Configuración del puerto serie
SERIAL_PORT = 'COM9' 
BAUD_RATE = 460800

MAX_SAMPLES = 400  
TRIGGER_LEVEL = 1.65

# Base de tiempo calculada: 200kHz / 5 = 40kHz -> 1/40kHz = 25us = 0.025ms por muestra
SAMPLING_PERIOD_MS = 0.025  
time_axis = [i * SAMPLING_PERIOD_MS for i in range(MAX_SAMPLES)]

# Búferes inicializados
raw_buffer = []
plot_data = [0.0] * MAX_SAMPLES
is_running = True  

HOLDOFF_SAMPLES = 200  

def update_plot(frame):
    global raw_buffer, plot_data, is_running
    
    if not is_running:
        if ser.in_waiting > 0:
            ser.reset_input_buffer()
        return line_plot,

    try:
        while ser.in_waiting > 0:
            line = ser.readline().decode('utf-8', errors='ignore').strip()
            if line.isdigit():
                val_bits = int(line)
                val_volts = val_bits * (3.3 / 255.0)
                raw_buffer.append(val_volts)
        
        # Lógica de Trigger con Histéresis y HOLDOFF
        if len(raw_buffer) >= MAX_SAMPLES + HOLDOFF_SAMPLES + 100:
            trigger_idx = -1
            armed = False
            
            for i in range(len(raw_buffer) - (MAX_SAMPLES + HOLDOFF_SAMPLES)):
                if raw_buffer[i] < (TRIGGER_LEVEL - 0.15):
                    armed = True
                
                if armed and raw_buffer[i] >= TRIGGER_LEVEL:
                    trigger_idx = i
                    break
            
            if trigger_idx != -1:
                plot_data = raw_buffer[trigger_idx : trigger_idx + MAX_SAMPLES]
                raw_buffer = raw_buffer[trigger_idx + MAX_SAMPLES + HOLDOFF_SAMPLES:]
            else:
                if len(raw_buffer) > int(MAX_SAMPLES * 1.5):
                    plot_data = raw_buffer[-MAX_SAMPLES:] 
                    raw_buffer = raw_buffer[-int(MAX_SAMPLES/2):]

        # Aplicamos el offset de posición vertical
        offset = y_pos_slider.val
        offset_data = [v + offset for v in plot_data]
        line_plot.set_ydata(offset_data)
        
        return line_plot,
        
    except Exception as e:
        return line_plot,

# Inicialización serie
try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0.1)
    print(f"Conectado a {SERIAL_PORT} a {BAUD_RATE} baudios.")
except Exception as e:
    print(f"No se pudo abrir el puerto serie: {e}")
    exit()

# Configuración de la gráfica
fig, ax = plt.subplots(figsize=(10, 5))
# Dejamos más espacio abajo (bottom=0.25) para acomodar el botón y el nuevo slider horizontal
plt.subplots_adjust(left=0.15, bottom=0.25) 

line_plot, = ax.plot(time_axis, plot_data, color='#00ff00') 

ax.set_title("Osciloscopio Serie - Señal Filtrada", fontsize=14)
ax.set_xlabel("Tiempo (milisegundos)") 
ax.set_ylabel("Amplitud (Voltios)")

# Límites iniciales fijos para Y
ax.set_ylim(-1.0, 4.5) 
ax.set_yticks([x * 0.5 for x in range(-2, 10)]) 
ax.axhline(TRIGGER_LEVEL, color='red', linestyle='--', alpha=0.5, label='Nivel de Trigger')
ax.legend(loc="upper right")

# Estética
ax.set_facecolor('black')
ax.grid(color='#005500', linestyle='-', linewidth=1.0)
fig.patch.set_facecolor('#1e1e1e')
ax.xaxis.label.set_color('white')
ax.yaxis.label.set_color('white')
ax.title.set_color('white')
ax.tick_params(colors='white')

# --- CONFIGURACIÓN DEL SLIDER Y-OFFSET (Vertical) ---
ax_slider_y = plt.axes([0.05, 0.25, 0.03, 0.55], facecolor='#1e1e1e')
y_pos_slider = Slider(
    ax=ax_slider_y,
    label='Pos Y\n[V]',
    valmin=-3.3,
    valmax=3.3,
    valinit=0.0,
    orientation='vertical',
    color='#00ff00'
)
y_pos_slider.label.set_color('white')
y_pos_slider.valtext.set_color('white')

def update_y_pos(val):
    offset_data = [v + val for v in plot_data]
    line_plot.set_ydata(offset_data)
    fig.canvas.draw_idle()

y_pos_slider.on_changed(update_y_pos)


# --- CONFIGURACIÓN DEL SLIDER TIMEBASE (Horizontal) ---
# Ejes: [izquierda, abajo, ancho, alto]
ax_slider_t = plt.axes([0.15, 0.05, 0.4, 0.04], facecolor='#1e1e1e')
t_scale_slider = Slider(
    ax=ax_slider_t,
    label='Escala X [ms] ',
    valmin=1.0,                                 # Zoom máximo: ver solo 1 ms de señal
    valmax=MAX_SAMPLES * SAMPLING_PERIOD_MS,    # Zoom mínimo: ver los 10 ms completos
    valinit=MAX_SAMPLES * SAMPLING_PERIOD_MS,   # Arranca mostrando todo
    color='#00ff00'
)
t_scale_slider.label.set_color('white')
t_scale_slider.valtext.set_color('white')

def update_t_scale(val):
    # Cambia el rango visible del eje X dinámicamente
    ax.set_xlim(0, val)
    
    # Redibujamos las divisiones de la cuadrícula para que se adapten al zoom
    if val <= 3.0:
        ax.set_xticks([x * 0.25 for x in range(0, int(val * 4) + 1)]) # Grilla cada 0.25ms si hay mucho zoom
    else:
        ax.set_xticks([x for x in range(0, int(val) + 1)])            # Grilla cada 1ms estándar
        
    fig.canvas.draw_idle()

t_scale_slider.on_changed(update_t_scale)


# --- CONFIGURACIÓN DEL BOTÓN RUN/STOP ---
ax_button = plt.axes([0.8, 0.05, 0.1, 0.075])
btn_run_stop = Button(ax_button, 'Stop', color="#ff4d4d", hovercolor='#ff7f7f')

def toggle_run(event):
    global is_running, raw_buffer
    is_running = not is_running
    
    if is_running:
        btn_run_stop.label.set_text('Stop')
        btn_run_stop.color = '#ff4c4c'
        ser.reset_input_buffer()
        raw_buffer.clear()
    else:
        btn_run_stop.label.set_text('Run')
        btn_run_stop.color = '#4cff4c'
        
    fig.canvas.draw_idle()

btn_run_stop.on_clicked(toggle_run)

# Inicializamos los límites del eje X por primera vez
update_t_scale(MAX_SAMPLES * SAMPLING_PERIOD_MS)

# Animación
ani = FuncAnimation(fig, update_plot, interval=30, blit=True, cache_frame_data=False)

try:
    plt.show()
except KeyboardInterrupt:
    print("Cerrando aplicación...")
finally:
    if 'ser' in locals() and ser.is_open:
        ser.close()
    print("Puerto serie cerrado.")
