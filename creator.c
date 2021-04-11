/*
Este programa gráfico será responsable de crear el buffer compartido por todos los procesos, 
de inicializar todas las variables de control asociadas al 
mismo (semáforos, banderas, contador de productores, contador de consumidores, etc.), 
y de monitorear de manera gráfica el estado de todo el sistema. 
El nombre del buffer, el tamaño en entradas para mensajes, y cualquier otro parámetro que 
cada grupo considere conve- niente serán recibidos de la lı́nea de comando al ser ejecutado 
desde consola. 
Usando GTK o algún equivalente, este programa mostrará constantemente el 
nombre y contenido del buffer (incluyendo indicadores del punto de entrada 
y el punto de salida del buffer), la cantidad de productores y consumidores, 
y una bitácora (scrollable) que registre todos los eventos del sistema. 
Inclusive, cuando el finalizador haya terminado al sistema, 
el creador seguirá vivo para poder examinar esta bitácora.
*/