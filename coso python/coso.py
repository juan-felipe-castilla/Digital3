import serial
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from matplotlib.widgets import Button, Slider

# Configuración del puerto serie
SERIAL_PORT = 'COM9' 
BAUD_RATE = 115200

MAX_SAMPLES = 400  
TRIGGER_LEVEL = 1.65  

# Búferes inicializados con flotantes
raw_buffer = []
plot_data = [0.0] * MAX_SAMPLES
is_running = True  

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
        
        # Lógica de Trigger
        if len(raw_buffer) >= MAX_SAMPLES + 100:
            trigger_idx = -1
            
            for i in range(len(raw_buffer) - MAX_SAMPLES):
                if raw_buffer[i] < TRIGGER_LEVEL and raw_buffer[i+1] >= TRIGGER_LEVEL:
                    trigger_idx = i
                    break
            
            if trigger_idx != -1:
                plot_data = raw_buffer[trigger_idx : trigger_idx + MAX_SAMPLES]
                raw_buffer = raw_buffer[trigger_idx + int(MAX_SAMPLES/2):]
            else:
                if len(raw_buffer) > MAX_SAMPLES * 3:
                    raw_buffer = raw_buffer[-MAX_SAMPLES:]

        # Aplicamos el offset a los datos antes de dibujarlos
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
plt.subplots_adjust(left=0.15, bottom=0.2) 

line_plot, = ax.plot(range(MAX_SAMPLES), plot_data, color='#00ff00') 

ax.set_title("Osciloscopio Serie - Señal Filtrada", fontsize=14)
ax.set_xlabel("Muestras (Tiempo)")
ax.set_ylabel("Amplitud (Voltios)")

# --- CUADRÍCULA FIJA ---
# Dejamos un margen fijo amplio de -1V a +4.5V para poder mover la onda cómodamente
ax.set_ylim(-1.0, 4.5) 
# Forzamos a que haya una línea de cuadrícula exactamente cada 0.5V
ax.set_yticks([x * 0.5 for x in range(-2, 10)]) 
ax.axhline(TRIGGER_LEVEL, color='red', linestyle='--', alpha=0.5, label='Nivel de Trigger')
ax.legend(loc="upper right")

# Estética
ax.set_facecolor('black')
# Hacemos la cuadrícula un poco más visible para poder alinear bien
ax.grid(color='#005500', linestyle='-', linewidth=1.0)
fig.patch.set_facecolor('#1e1e1e')
ax.xaxis.label.set_color('white')
ax.yaxis.label.set_color('white')
ax.title.set_color('white')
ax.tick_params(colors='white')

# --- CONFIGURACIÓN DEL SLIDER Y-OFFSET ---
ax_slider = plt.axes([0.05, 0.2, 0.03, 0.6], facecolor='#1e1e1e')
y_pos_slider = Slider(
    ax=ax_slider,
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
    # Permite que la onda se mueva en la pantalla incluso cuando está en Stop
    offset_data = [v + val for v in plot_data]
    line_plot.set_ydata(offset_data)
    fig.canvas.draw_idle()

y_pos_slider.on_changed(update_y_pos)

# --- CONFIGURACIÓN DEL BOTÓN RUN/STOP ---
ax_button = plt.axes([0.8, 0.05, 0.1, 0.075])
btn_run_stop = Button(ax_button, 'Stop', color='#ff4c4c', hovercolor='#ff7f7f')

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
