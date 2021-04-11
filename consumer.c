/*
Esta es una familia de procesos, todos usando exactamente el mismo código, 
que se vinculan al buffer reservado por el creador y que con tiempos de 
espera aleatorios consumen mensajes tomados del buffer. 
Cada consumidor recibe como argumentos de la lı́nea de comandos 
el nombre del buffer compartido y un parámetro
que indique la media en segundos de los tiempos aleatorios, 
siguiendo una distribución exponencial, que deben esperar antes de consumir 
el siguiente mensaje del buffer administrado circularmente. 
Por supuesto, este acceso debe darse de manera sincronizada ya que el 
buffer es compartido por múltiples procesos. 
El usuario puede ejecutar cuantas veces lo desee este programa, 
creando un nuevo consumidor que compite por el buffer, 
aunque cada vez podrı́a indicarse una media de espera diferente. 
Cuando se crea un consumidor, lo primero que éste hace es incrementar 
el contador de consumidores activos. Estos procesos repiten un ciclo de 
espera aleatoria y consu- mo de mensajes hasta que lean un mensaje especial 
que indique que el sistema se deba suspender, o cuando al leer un mensaje 
este incluya una llave (número entero entre 0 y 4) que sea igual al 
process id o PID del consumidor módulo 5. 
En cualquiera de estos dos casos, el consumidor termina, decrementa el 
contador de consumidores activos, despliega su identificación y algunas 
estadı́sticas básicas (número de mensajes consumidos, 
acumulado de tiempos esperados, acumulado de tiempo que 
estuvo bloqueado por semáforos, etc.). 
Si no hay mensajes en el buffer, el consumidor queda suspendido hasta 
que aparezca uno. No se puede usar busy waiting. 
Cada vez que un mensaje logra ser leı́do del buffer, se debe desplegar 
un mensaje a la consola describiendo la acción realizada incluyendo el 
ı́ndice de la entrada de adonde se tomó el mensaje y la cantidad de 
productores y consumidores vivos al instante de este evento).
*/