/*
Esta es una familia de procesos, todos usando exactamente el mismo código, 
que se vinculan al buffer reservado por el creador. Con tiempos de espera 
aleatorios generarán mensajes que colocarán en el buffer (administrado circularmente). 
Cada productor recibe como argumentos de la lı́nea de comandos el nombre del buffer 
compartido y un parámetro que indique la media en segundos de los tiempos aleatorios, 
siguiendo una distribución exponencial, que deben esperar antes de agregar un nuevo 
mensaje en el buffer. Por supuesto, este acceso debe darse de manera sincronizada ya 
que el buffer es compartido por múltiples procesos. El usuario puede ejecutar cuantas 
veces lo desee este programa, creando un nuevo productor que compite por el buffer, 
aunque cada vez podrı́a indicarse una media de espera diferente. 
Al crearse un productor, este incrementa el contador de productores vivos. 
Estos procesos repiten un ciclo de espera aleatoria y fabricación de mensajes hasta 
que algún tipo de bandera global en memoria compartida indique que el sistema se debe suspender. 
En este caso, los productores terminan, decrementan el contador de productores vivos, 
y despliegan su identificación y algunas estadı́sticas básicas (número de mensajes producidos, 
acumulado de tiempos esperados, acumulado de tiempo que estuvo bloqueado por semáforos, etc.). 
Si no hay espacio en el buffer, el productor queda suspen- dido hasta que se libere un espacio. 
No se puede usar busy waiting. El formato especı́fico del mensaje puede ser definido por 
cada grupo de trabajo, pero debe incluir al menos: Identificación del productor Fecha y 
hora de creación Llave aleatoria entre 0 y 4. 
Cada vez que un mensaje logra ser puesto en el buffer, 
se debe desplegar un mensaje a la consola describiendo la acción realizada incluyendo el 
ı́ndice de la entrada donde se dejó el mensaje y la cantidad de productores y consumidores 
vivos al instante de este evento.
*/