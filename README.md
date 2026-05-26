# 📟 Proyecto Trabajo Final Electrónica Digital 3: Osciloscopio Digital

---

## 🎯 Descripción General

Este proyecto consiste en el diseño y desarrollo de un **Osciloscopio Digital** basado en el microcontrolador **LPC1769** (arquitectura ARM Cortex-M3).

El objetivo principal es la **adquisición precisa de señales analógicas**, su procesamiento eficiente mediante técnicas de hardware avanzado y la transmisión a una computadora para su visualización.

---

## 👨‍💻 Autores

- **Viberti, Benjamín**
- **Castilla, Felipe**
- **Dalmazzo, Agustín**

---

## 🛠️ Arquitectura y Módulos del Sistema

Para garantizar la máxima frecuencia de trabajo sin saturar el procesador, el código está estructurado de manera modular, delegando tareas pesadas a los periféricos de hardware y usando DMA e interrupciones.

---

### 🎛️ 1. ADC (Convertidor Analógico a Digital)
- **Función:** Digitaliza la señal analógica proveniente del mundo exterior.
- **Implementación:**  
  - Resolución de **12 bits**, brindando excelente fidelidad.
  - Alta velocidad de muestreo, sincronizado eficientemente.

---

### ⚡ 2. DMA (Acceso Directo a Memoria)
- **Función:** Mueve datos entre RAM y periféricos (ADC y DAC) de forma **100% autónoma**—sin interrupciones ni consumo de ciclos de CPU.
- **Implementación:**  
    - Canales en modo **P2M** (Periférico a Memoria): almacenan lecturas ADC.
    - Canales en modo **M2P** (Memoria a Periférico): inyectan datos al DAC.
    - Interconexión inteligente para cadena de datos eficiente.

---

### 🌊 3. DAC (Convertidor Digital a Analógico)
- **Utilizado exclusivamente para el modo de Calibración.**
- **Función:** Genera señales de referencia para verificar el funcionamiento del equipo (bucle cerrado).
- **Implementación:**  
    - Conmutación mediante **interruptores físicos**.
    - Calibración automatizada y supervisada con DMA.

---

### 💻 4. Visualización de Datos y UART (Interfaz en PC)
- Para ofrecer claridad de lectura, los datos recolectados se envían vía **puerto serie** hacia una PC.
- **Script Python 🐍:**  
    - Recibe y grafica los datos en tiempo real.
    - Permite una visualización flexible y portátil.

---

## 🚀 Proyecto desarrollado para la cátedra de Electrónica Digital 3
