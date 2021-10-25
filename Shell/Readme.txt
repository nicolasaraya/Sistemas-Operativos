Para la ejecucion de la consola primero debe compilar archivo .cpp

g++ shell.cpp

Luego ejecutar el archivo resultante

./a.out

Una vez ejecutado, la consola se limpiara, mostrando unicamente el prompt junto con la ruta actual a la espera de que se ingresen comandos.

Se pueden ejecutar comandos sin limite de argumentos.
Tampoco hay limite de comandos que utilicen pipes.

Puede crear el comando que monitorea usando cmdmonset newCom x z
siendo newCom el nombre del comando a crear, 'x' el tiempo cada cuantos segundosse monitoreara el sistema, y 'z' el tiempo en segundos del monitoreo


Escriba "exit" para abandonar la consola sin confirmacion
Si pulsa Control+C se pedira confirmacion

Si escribe "clear" se limpiara el contenido de la shell en pantalla.


Integrantes: Martina Cadiz, Nicolas Araya
