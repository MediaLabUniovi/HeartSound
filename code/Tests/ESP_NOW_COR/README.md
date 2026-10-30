# Código de test para comunicación ESP-NOW entre los dos ESPs.
Se ha llamado Corazón al ESP que actúa de mando, encargado de seleccionar que tipo de corzón será utilizado con MAC: 3C:8A:1F:99:4C:C0 y Tórax al ESP encargado de recibir el tipo de corazon y gestionar los relés, con MAC: 3C:8A:1F:99:FB:F0.

Los nombres pueden ser cambiados modificando el valor de las variables string nombrePlacaMando y nombrePlacaCuerpo respectivamente.

En caso de querer cambiar los ESPs habría qu ecmabiar las direcciones MAC a mano, ya que estas son necesarias para comunicarse con ESP-NOW.

## Función de cada ESP
El código de ejemplo está hecho para mandar instrucciones por serial de la siguiente manera:
1. El ESP Corazón recibe un numero del 1 al 6 y responde con error si el valor sale de este rango y manda por ESP-NOW este valor si se produce un cambio en el tipo de corazón seleccionado. También manda si recibe un mensaje "ASK" por parte del ESP Tórax.
2. El ESP Tórax recibe del Corazón un tipo de corazón del 1 al 6 y puede pedir el tipo de corazón mandando un mensaje "ASK" escrito por serie por parte del usuario.

## Uso
Las funciones de ESP_NOW_functions.cpp son funciones para inicializar la comunicación ESP-NOW y gestionar el flujo de datos. Debería de importarse este archivo con su cabecera ESP_NOW_functions.h y modificar el código del main para recibir las instrucciones y los tipos de corazóin por flujo de código interno en vez de por parte del usuario via Serial.

Para comunicarse será necesario simplemente utilizar por parte del corazón la función sendHeartType(unit8_t heartType) con el tipo de corazón que se quiera mandar y por parte del tórax la función de askHeartType() para pedir el tipo de corazón para actualizarlo.

El resto de funciones tipo callback ya se utilizan para gestionar internamente el flujo de datos.
