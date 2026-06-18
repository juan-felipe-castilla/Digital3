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
Se adjunta evidencia del proyecto funcionando, junto con una fotografía del hardware ensamblado:
<img width="960" height="1280" alt="placa" src="https://github.com/user-attachments/assets/2cce1d20-7199-4e7f-b99a-0e402adb9e8e" />

<img width="960" height="1280" alt="WhatsApp Image 2026-06-17 at 11 47 51 PM" src="https://github.com/user-attachments/assets/c6c89046-2b1a-4e93-84b2-6c3fd52ccb49" />

Prueba funcional: A continuación se presentan imagenes de las siguientes pruebas:

Prueba 1: Inyectamos con el generador de funciones una señal cuya forma y frecuencia varía, cambiando lo que se muestra en el osciloscopio digital (acorde a la señal inyectada). Son 3 formas de onda: sinusoidal, cuadrada y triangular.
<img width="322" height="217" alt="image" src="https://github.com/user-attachments/assets/5ae56e9a-f61e-4db1-8f7b-cdce538e0aa9" />
<img width="344" height="181" alt="image" src="https://github.com/user-attachments/assets/08590ed2-883f-4e69-adc7-9ca325e6bdb6" />
<img width="362" height="216" alt="image" src="https://github.com/user-attachments/assets/678543ce-7c64-4e0f-940d-0f0b7952f5d2" />
<img width="359" height="222" alt="image" src="https://github.com/user-attachments/assets/5e8fe741-0c95-4ad2-988c-9239b39bfba7" />
En la proxima imagen se evidencia como responde al cambio de frecuencia (cambia el ancho de banda de la señal).
<img width="370" height="207" alt="image" src="https://github.com/user-attachments/assets/d963a5fb-0068-4a80-bcb7-c691cccc682b" />

Prueba 2: DAC y el generador de señales para calibrar el osciloscopio. Se activa DAC con EINT0 y encendemos un LED indicador (LED VERDE).
<img width="279" height="244" alt="image" src="https://github.com/user-attachments/assets/6d58fafd-35fa-412b-9f0b-658c31f80a74" />
A medida que se presiona EINT1, se encienden determinados LEDS ROJOS a modo de indicador de que señal se está generando.
<img width="175" height="159" alt="image" src="https://github.com/user-attachments/assets/120c7326-386d-4d1b-94c5-46662db00f6b" />
Por último, imagenes de las señales generadas (triangular, sinusoidal y cuadrada).
<img width="310" height="196" alt="image" src="https://github.com/user-attachments/assets/4b5c6fab-c168-4321-b034-fd52aab20554" />
<img width="288" height="170" alt="image" src="https://github.com/user-attachments/assets/b24529ac-369a-4710-8b2d-edef43cbcbe5" />
<img width="304" height="188" alt="image" src="https://github.com/user-attachments/assets/6618ef6f-3fd6-468a-8f4f-80989e583ba3" />

Prueba 3: Captura de la freuencia de la señal inyectada por el generador mediante CAPTURE. Se muestra la frecuencia en pantalla tras la pulsación de EINT2.
880Hz de señal inyectada.
<img width="242" height="96" alt="image" src="https://github.com/user-attachments/assets/eda63a84-79fc-468d-9bf6-457f5d6a1be8" />
Mensaje en pantalla.
<img width="161" height="34" alt="image" src="https://github.com/user-attachments/assets/58e9bd44-5c59-4bbb-9460-bcf19f197385" />

---

## 📂 6. Estructura del Repositorio
```text
├── firmware/          # Código fuente del proyecto (MPLABX / MCUXpresso / STM32Cube)
│   ├── src/           # Archivos de código (.c)
│   └── inc/           # Archivos de cabecera (.h)
├── hardware/          # Archivos de diseño (KiCad/Altium), esquemáticos en PDF/Imagen y BOM
├── docs/              # Datasheets clave, imágenes del README, notas de aplicación
└── README.md          # Este archivo de presentación
