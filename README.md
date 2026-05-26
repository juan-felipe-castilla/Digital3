📟 Proyecto Trabajo Final Electrónica Digital 3: Osciloscopio Digital
Este proyecto consiste en el diseño y desarrollo de un Osciloscopio Digital basado en el microcontrolador LPC1769 (arquitectura ARM Cortex-M3).

El objetivo principal de este sistema es la adquisición precisa de señales analógicas, su procesamiento eficiente mediante técnicas de hardware avanzado, y su posterior transmisión a una computadora para ser visualizadas en tiempo real. Además, el proyecto cuenta con un modo de "Auto-Calibración" que genera sus propias ondas de prueba.

👨‍💻 Autores
Viberti, Benjamín
Castilla, Felipe
Dalmazzo, Agustín

🛠️ Arquitectura y Módulos del Sistema
Para garantizar que el osciloscopio funcione a la máxima frecuencia posible sin saturar el procesador, el código fue estructurado de manera modular, delegando el trabajo pesado a los periféricos de hardware. A continuación, se detalla el funcionamiento de cada bloque:

🎛️ ADC (Convertidor Analógico a Digital)
Es el corazón del osciloscopio en su modo de funcionamiento normal.

Función: Se encarga de digitalizar la señal analógica proveniente del mundo exterior.

Implementación: Trabaja a una resolución de 12 bits, garantizando una excelente fidelidad en la lectura de las tensiones. Está configurado para muestrear a alta velocidad y sincronizado de forma estricta para asegurar que el tiempo entre muestras (Sample Rate) sea constante, evitando deformaciones visuales en la señal capturada.

⚡ DMA (Acceso Directo a Memoria)
Es la columna vertebral del movimiento de datos.

Función: Permite que los datos viajen entre la memoria RAM y los periféricos (ADC y DAC) de forma 100% autónoma, sin interrupciones y sin usar ciclos de reloj de la CPU.

Implementación: Se configuraron canales en modo P2M (Periférico a Memoria) para guardar las lecturas del ADC, y en modo M2P (Memoria a Periférico) para inyectar datos al DAC. Además, se hace uso intensivo de Linked Lists (LLI) para lograr un flujo de datos continuo e infinito. Gracias al DMA, el microcontrolador queda libre para atender las interrupciones externas de los botones y gestionar la comunicación.

🌊 DAC (Convertidor Digital a Analógico)
Este módulo se utiliza exclusivamente para el modo especial de Calibración.

Función: Genera señales de referencia conocidas que se realimentan directamente a la entrada del osciloscopio (bucle cerrado) para verificar el correcto funcionamiento del equipo.

Implementación: A través de interrupciones externas (botones físicos), el usuario puede alternar el equipo al modo de calibración. En este estado, el DAC (alimentado ininterrumpidamente por el DMA) es capaz de sintetizar dos tipos de formas de onda: Cuadrada y Triangular. El sistema conmuta entre ambas señales bajo demanda, operando en el modo de alta velocidad del periférico para mantener la integridad de los flancos.

💻 Visualización de Datos (Interfaz PC)
Para que un osciloscopio sea útil, la señal debe poder verse de forma clara. Toda la información recolectada por la LPC1769 es empaquetada y enviada a la PC.

Script en Python 🐍: En lugar de usar una pantalla integrada, desarrollamos un script en Python dedicado a la recepción y graficación de los datos. Este script lee el puerto serie, procesa la trama de datos enviada por el microcontrolador y plotea la señal en tiempo real en la pantalla de la computadora, brindando una experiencia de usuario fluida y una visualización nítida de las ondas analizadas o generadas.



🚀 Proyecto desarrollado para la cátedra de Electrónica Digital 3.
