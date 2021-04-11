/*
Este programa se encarga de cancelar todo el sistema de procesos, 
enviando mensajes de finalización a cada consumidor vivo usando 
el buzón diseñado para este proyecto, e indicándo- le a los 
productores que cesen actividades con alguna bandera global en 
memoria compartida. Una vez que la cantidad de productores y 
consumidores llega a cero, el buffer compartido es liberado. 
El finalizador deberá dar mensajes y todas las estadı́sticas 
posibles de su gestión.
*/