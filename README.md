# Osciloscopio Digital
> **Asignatura:** Electrónica Digital III - Universidad Nacional de Córdoba
> 
> **Integrantes:** 
>  - **Viberti, Benjamín**
>  - **Castilla, Felipe**
>  - **Dalmazzo, Agustín**
>    
> -**Profesor:**
 >- **Blasco Marcos**

---

## 🚀 1. Descripción General del Proyecto
Este proyecto consiste en el diseño y desarrollo de un **Osciloscopio Digital** basado en el microcontrolador **LPC1769** (arquitectura ARM Cortex-M3).

El objetivo principal es la **adquisición de señales analógicas**, su procesamiento  mediante técnicas de software y hardware  y la transmisión a una computadora para su visualización, mediante UART, y tambien posee un generador de funciones interno, para así poder realizar ensayos a diferentes dispositivos.


### 🎯 Alcances del Proyecto 

* **El sistema SÍ es capaz de:**
* 
* Muestrear y procesar señales de frecuencias medias, bajas.
* Una precisión en su medidas válidas para realizar pruebas en casa
* Atenuar señales de entrada, para así ampliar su rango dinamico
* Generar señales de frecuencia fija
* Mostrar tanto niveles de amplitud y tiempo, junto a un botón que muestra la frecuencia de entrada
* 
* **El sistema NO incluye:**
* 
* Muestreo de alta frecuencia
* Una precisión comparable a uno comercial
* Un trigger ajustable

### ⏩ Posibles Etapas Siguientes
Cómo escalar el proyecto:
* Utilizar otro protocolo de comunicación serie, más eficiente
* Utilizar un módulo de adc más exacto, externo
* Agregar una punta compensada
* Poder hacerlo portatil, es decir, que no se requiera una computadora completa para su funcionamiento

---

## 📐 2. Arquitectura del Sistema: Hardware y Software

### 🔌 Hardware & Interconexión
* **Diagrama de Bloques:** <img width="1436" height="833" alt="Diagrama_Circuito" src="Diagrama_Hardware.pdf" />
* **Esquemático del Circuito:** 
  <img width="1436" height="833" alt="image" src="https://github.com/user-attachments/assets/d00f7812-289d-4fc9-b759-fd9c6c0528b7" />

* **Descripción del Circuito y Consideraciones de Diseño:** En la entrada se encuentra un filtro pasa-bajos de 5kHz que cumple la funcion de filtro anti Aliasing, siguiente a este se puede encontrar un DIP Switch que nos permitira seleccionar entre 2 Atenuadores de -6dB y -20dB o una entrada sin atenuacion. Siguiente a los Atenuadores se encuentra la entrada al ADC y al pin de Capture simultaneamente.
  
* Los botones de interrupcion poseen filtros anti rebote fisicos.
  
* Cada LED cuenta con resistencias limitadoras, se recomienda el uso de Leds rojos.

### 💻 Arquitectura de Software
* **Diagrama de Flujo:** <img width="1436" height="833" alt="Diagrama_Codigo" src="Diagrama_Code.pdf" />

---

## ⚡ 3. Especificaciones Eléctricas, Alimentación y Entorno

### 🔌 Parámetros de Alimentación y Consumo
* **Tensión de operación del sistema:** 3.3V
* **Método de alimentación:**  Fuente de alimentación externa
* **Consumo estimado o medido:** * En modo activo, con todos sus modulos operativos 100mA, debido a que el unico componente activo es la lpc
* En modo pasivo(sin el generador de funciones) el consumo baja a los 67mA


### 📌 Entorno:
* **IDE y SDK:** MCUXpresso IDE v11.8 con LPCOpen v2.10
* **Microcontrolador Principal:** NXP LPC1769 
* **Bibliotecas de Terceros y Versiones:** --.
* **Periféricos Avanzados Utilizados:** GPDMA, TIMER, ADC, DAC, UART, GPIO, NVIC ...
* **Estrategia de Concurrencia:** Nuestra estrategia es descargar a la CPU delegando todo el trabajo al hardware (DMA). El main se queda libre esperando comandos de Python por UART, mientras que de fondo los dos canales de DMA hacen todo en paralelo: el Canal 0 (RAM al DAC) envia datos para generar las señales (si está previamente activado el modo), y el Canal 1 (del ADC a la RAM) toma las muestras del generador de funciones. Así, la placa procesa, genera y captura todo al mismo tiempo sin colgarse nunca.

---

## 🔄 4. Proceso de Integración y Desarrollo

* **Etapa 1 (Pruebas UART):** El primer paso fue implementar una UART funcional .
* **Etapa 2 (Plotter):** Desarrollo del plotter en python.
* **Etapa 3 (Adquisición):** Configuración y pruebas de ADC , vía UART.
* **Etapa 4 (Filtrados):** Agreagmos un filtro Anti-Alias(Hardware) y uno de mediana(Software) ..
* **Etapa 5(Generación de señales)** Implementación del DA.
* **Etapa 6(Calibración y Ajuste)** Se ajustaron tiempos de adquisición y comunicación.

---

## 📊 5. Ensayos, Pruebas y Resultados

Se adjunta evidencia del proyecto funcionando, junto con una fotografía del hardware ensamblado y los resultados de las pruebas funcionales.

### 🛠️ Hardware Ensamblado y Banco de Pruebas

Esta imagen integra la fotografía de la placa terminada y el equipo de medición utilizado para las validaciones.

<p align="center">
  <img src="URL_IMAGEN_1_HARDWARE.png" alt="Hardware Ensamblado y Banco de Pruebas" width="80%">
</p>

### Prueba Funcional 1: Verificación de Formas de Onda

A continuación se presentan imágenes de las siguientes pruebas. Inyectamos con el generador de funciones una señal cuya forma y frecuencia varía, cambiando lo que se muestra en el osciloscopio digital (acorde a la señal inyectada). Son 3 formas de onda: sinusoidal, cuadrada y triangular.

En la segunda parte de la imagen se evidencia cómo responde al cambio de frecuencia (cambia el ancho de banda de la señal).

<p align="center">
  <img src="URL_IMAGEN_2_PRUEBA1.png" alt="Prueba Funcional 1 - Verificación de Ondas" width="80%">
</p>

### Prueba Funcional 2: DAC y Calibración de Señales

Prueba del DAC y el generador de señales para calibrar el osciloscopio. Se activa DAC con EINT0 y encendemos un LED indicador (LED VERDE).

A medida que se presiona EINT1, se encienden determinados LEDS ROJOS a modo de indicador de qué señal se está generando.

<p align="center">
  <img src="URL_IMAGEN_3_PRUEBA2.png" alt="Prueba Funcional 2 - DAC y Calibración" width="80%">
</p>

### Prueba Funcional 3: Captura de Frecuencia (CAPTURE)

Captura de la frecuencia de la señal inyectada por el generador mediante CAPTURE. Se muestra la frecuencia en pantalla tras la pulsación de EINT2.

Para una señal inyectada de 880Hz, se muestra el mensaje correspondiente en la pantalla del dispositivo.

<p align="center">
  <img src="URL_IMAGEN_4_PRUEBA3.png" alt="Prueba Funcional 3 - Captura de Frecuencia" width="60%">
</p>
---

## 📂 6. Estructura del Repositorio
```text
├── firmware/          # Código fuente del proyecto (MPLABX / MCUXpresso / STM32Cube)
│   ├── src/           # Archivos de código (.c)
│   └── inc/           # Archivos de cabecera (.h)
├── hardware/          # Archivos de diseño (KiCad/Altium), esquemáticos en PDF/Imagen y BOM
├── docs/              # Datasheets clave, imágenes del README, notas de aplicación
└── README.md          # Este archivo de presentación
