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
Delimiten claramente los objetivos alcanzados para la entrega final:
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
Planteen cómo escalaría este desarrollo en una versión 2.0 o en un ámbito profesional:
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
* **Estrategia de Concurrencia:** ?

---

## 🔄 4. Proceso de Integración y Desarrollo
Describan cronológicamente cómo fueron sumando y testeando las diferentes partes del proyecto (enfoque modular de ingeniería).

* **Etapa 1 (Pruebas UART):** El primer paso fue implementar una UART funcional .
* **Etapa 2 (Plotter):** Desarrollo del plotter en python.
* **Etapa 3 (Adquisición):** Configuración y pruebas de ADC , vía UART.
* **Etapa 4 (Filtrados):** Agreagmos un filtro Anti-Alias(Hardware) y uno de mediana(Software) ..
* **Etapa 5(Generación de señales)** Implementación del DA.
* **Etapa 6(Calibración y Ajuste)** Se ajustaron tiempos de adquisición y comunicación.

---

## 📊 5. Ensayos, Pruebas y Resultados
Demuestren con datos empíricos que el sistema funciona correctamente. **Es obligatorio incluir registro visual**.

* **Pruebas Funcionales Realizadas:** Detallen los ensayos (Ej: "Se inyectó una señal controlada para medir la precisión del ADC...").
* **Evidencia Fotográfica y Gráficos:** * *Capturas de instrumental:* [Insertar capturas de Osciloscopio, Analizador Lógico o Terminal Serie]
  * *Foto del Prototipo Real:* [Insertar foto del hardware final cableado/armado en funcionamiento]

---

## 📂 6. Estructura del Repositorio
El repositorio debe mantener obligatoriamente la siguiente estructura limpia (¡Recuerden configurar correctamente el `.gitignore` para no subir carpetas temporales como `Debug/`, `Release/` o archivos `.p1` / `.d`!).

```text
├── firmware/          # Código fuente del proyecto (MPLABX / MCUXpresso / STM32Cube)
│   ├── src/           # Archivos de código (.c)
│   └── inc/           # Archivos de cabecera (.h)
├── hardware/          # Archivos de diseño (KiCad/Altium), esquemáticos en PDF/Imagen y BOM
├── docs/              # Datasheets clave, imágenes del README, notas de aplicación
└── README.md          # Este archivo de presentación
