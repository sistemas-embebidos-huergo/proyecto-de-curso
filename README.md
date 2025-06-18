# Consigna Sistemas embebidos - Estación meteorológica
El trabajo práctico de curso consiste en el desarrollo de una estación meteorológica de inicio a fin, pasando por el diseño y desarrollo del PCB y carcaza hasta su programación y puesta a punto para un correcto funcionamiento.
El TP consiste en aprovechar las características multicore del ESP32, donde en uno de los cores funcionará un servidor web que mediante HTTP, servirá una web la cual permitirá visualizar los datos mediante un navegador, tanto mediante un móvil como una computadora.
En el otro core funcionará la lectura de los datos de los sensores, estas se dividirán en dos partes. La primera será una lectura periódica, cada 2 segundos guardando en memoria la lectura mas reciente y, la segunda parte se ejecutará cada 10 minutos almacenando la lectura mas reciente, para así poder elaborar un histórico diario.


## Método de evaluación
La nota esta compuesta por las siguientes verticales: #documentación #PCB #mecanizado #código #funcionalidades-extra
La nota final estará dada por el promedio (ponderado) entre todas las verticales, teniendo como condición mínima y necesaria la aprobación de cada una de ellas, exceptuando la de #funcionalidades-extra la cuál es, naturalmente, un extra no condicionante.
El desarrollo del proyecto será de forma iterativa incremental, por lo tanto en algunos casos, resultará complejo tener una nota concreta rápidamente, por lo que esta misma irá evolucionando de acuerdo a la dedicación del alumno.

## Componentes
Los componentes necesarios para el desarrollo del proyecto son:
- ESP32
- BMP180
- DHT22
- Sensor de lluvia
- Modulo memoria SD
- Memoria micro SD,  de al menos 32GB

## Documentación
El proyecto deberá estar alojado en un repositorio de Github y se deberá elaborar un archivo `README.md` el cual deberá ir reflejando todos los avances y decisiones de diseño tomadas. Este mismo debe reflejar todas las etapas del proyecto y su lectura, debería poder ser suficiente para comprender todo proceso, evolución y desarrollo a lo largo del cuatrimestre. Se recomienda el uso de herramientas para el formateo de de archivos Markdown, fotos y videos (o .gif)
#### Requerimientos mínimos
Desarrollo de un README.md que contenga toda la información acerca de la evolución y decisiones tomadas a lo largo del cuatrimestre.

## PCB
Para el desarrollo del circuito impreso se deberá tener en cuenta las correctas conexiones de los componentes y sensores necesarios. 
Se recomienda pensar en la conexión para un anemómetro, por más de que no sea un requerimiento funcional obligatorio, para facilitar su posterior instalación en el caso de que llegue con el tiempo o se desee agregar a futuro.
El uso de pines hembra para las conexiones de los sensores y ESP32 es ideal para poder facilitar el reemplazo de alguno de ellos o reutilización a futuro de alguno de ellos.

#### Importante
Hay algunas conexiones que deben ser respetadas debido a ciertas limitaciones de infraestructura y uso de los pines del ESP32.

| Pin Módulo SD | Pin ESP32 |
| ------------- | --------- |
| CS            | GPIO 5    |
| MOSI          | GPIO 23   |
| CLK           | GPIO 18   |
| MISO          | GPIO 19   |

| Pin Sensor lluvia | Pin ESP32 |
| ----------------- | --------- |
| A0                | SVP       |

| Pin BMP180 | Pin ESP32     |
| ---------- | ------------- |
| SCL        | SCL (GPIO 22) |
| SDA        | SDA (GPIO 21) |

#### Requerimientos mínimos
Componentes o pines solados que permitan el correcto funcionamiento y sinergia del sistema.

## Mecanizado
La totalidad del circuito deberá estar dentro de una carcaza diseñada por el alumno, donde la única parte externa deberá ser el sensor de lluvia para que, justamente, pueda detectar las gotas de lluvia. 
Se deberá tener en cuenta la sujeción de todos los componentes, ya que ninguno puede estar suelto. También los agujeros para los cables de alimentación y del sensor de lluvia.
Para la evaluación del mismo se tendrá en cuenta la calidad del diseño y el criterio en la ubicación tanto de piezas clave, como de agujeros, encastres y aislamiento (estanco) del mismo.
#### Requerimientos mínimos
Desarrollo e implementación de carcaza que permita sujetar todos los componentes

## Código
Esta sección probablemente será la mas larga y que va a consumir mas tiempo. Que no panda el cúnico.

El desarrollo del código se hará de forma iterativa incremental, para poder así ir probando variaciones del mismo. Si bien debajo se propone una secuencia de pasos a seguir, cada uno es libre, bajo su riesgo, de planificar y ejecutar el proyecto de la forma que considere adecuada.

A partir de esta linea, se propondrán checkpoints para que el avance sea controlado y se pueda medir en todo momento su progreso.

##### Checkpoint 1
Con el PCB ya listo o una protoboard, se propone probar que todos los sensores funcionen correctamente, por lo tanto en esta iteración se busca:
1. Investigar sobre las bibliotecas a utilizar y su funcionamiento, salvo la del sensor de lluvia.
2. Descargar e instalar las bibliotecas necesarias
3. Inicializar cada una de ellas y medir los valores de cada sensor cada un tiempo prudencial
4. Escribir las lecturas en el puerto serie para validar que efectivamente funcione como se espera.

##### Checkpoint 2
Debido a que no existe una biblioteca que podamos instalar para manejar el sensor de lluvia, vamos a elaborar la nuestra.

Se deberá crear dos archivos, `rain-meter.h` y `rain-meter.cpp`

`rain-meter.h`
```cpp
#ifndef RAIN_METER_H
#define RAIN_METER_H

#include <Arduino.h>

class RainMeter {
private:
  int analogPin;
  int digitalPin;

public:
  RainMeter(int analogPin, int digitalPin);
  void begin();
  bool isRaining();
  int rainIntensity();
};

#endif
```

`rain-meter.cpp`
```cpp
#include "rain-meter.h"
#include <Arduino.h>

RainMeter::RainMeter(int analogPin, int digitalPin)
: analogPin(analogPin), digitalPin(digitalPin) {}

void RainMeter::begin() {
  pinMode(analogPin, INPUT);
  pinMode(digitalPin, INPUT);
}

bool RainMeter::isRaining() {
  bool rawRead = digitalRead(digitalPin);
  return !rawRead;
}

int RainMeter::rainIntensity() {
  int rawRead = analogRead(analogPin);
  return abs(5 - map(rawRead, 0, 5000, 0, 5));
}
```

1. Agregar los archivos que se menciona arriba
2. Importar la clase RainMeter en el archivo principal de la siguiente manera: 
	`#include "rain-meter.h"`
3. Generar una instancia de la clase:
	 `RainMeter rainMeter(<pin-analogico>, <pin-digital>);`
4. En este punto podemos utilizar el objeto `rainMeter` en nuestro código.
5. En el setup ejecutar el método `begin` de `rainMeter`
6. Agregar a las escrituras por puerto serie que actualmente ya tenemos, una nueva que sea la de `rainIntensity` e `isRaining`
##### Checkpoint 3
Ahora que ya tenemos todas las lecturas, va a resultar util generar una estructura que nos agrupe todos los valores de las lecturas, para esto vamos a:
1. Declarar la estructura `WeatherData`, que contenga atributos: `temperature`,`humidity`, `pressure`, `isRaining`, `rainIntensity`, `timestamp`
2. Crear una variable global llamada `latestReadings` del tipo `WeatherData`
3. Modificar el código actual para que todas las lecturas se guarden en `latestReadings` y qué las escrituras en el puerto serie se hagan desde esta misma variable, por ejemplo `Serial.println(latestReadings.temperature);`
De esta manera ya tenemos una variable global, accesible desde cualquier función y/o thread que nos permite guardar la lectura más reciente y eventualmente almacenarla (dump) en la memoria SD.
##### Checkpoint 4
Naturalmente, no nos sirve de nada leer información y no poder accederla, por ende vamos a necesitar una conexión a internet.
Para lograr esto contamos con una abstracción de la librería de `WiFi` nativa del ESP32. La misma es es `wifi-manager.h` y `wifi-manager.cpp`
1. Copiar ambos archivos en el nuestro directorio del proyecto.
2. Procederemos a importarla
3. Ahora hay que hacer la conexión con una red wifi que tenga acceso a internet. Para esto, primero que nada instanciamos la clase `WiFiManager` en el contexto global con SSID y Password correspondientes.
4. En el setup procedemos a iniciar la conexión a internet, para ello es necesario que primero sigamos algunos pasos.
	1. Investigar y documentar los conceptos de IP, Gateway, Subnet y DNS, y DHCP
	2. En el caso de conocer todos los datos de la conexión a internet, podremos evitar el proceso de DHCP, por eso opcionalmente podemos utilizar la función `setStaticIPConfig` pasándole los parámetros que no son opcionales de la siguiente manera: `setStaticIPConfig(IPAddress(192, 168, 1, 1),....)` Notar el uso de la función `IpAddress`.
	>El DHCP en este caso particular nos jugará en contra, ya que cada vez que se reinicie la conexión, obtendremos una IP nueva. Esto nos complicará saber a que IP acceder para poder visualizar los datos a futuro.

	3. Procedemos a setear un tiempo de timeout considerable, mediante el uso de su método homónimo. 15 segundos resulta un tiempo aceptable (hasta incluso elevado).
	4. Finalmente podemos proceder a intentar realizar la conexión con el método `connect`. Este método nos retorna un `bool` indicando si la conexión fue exitosa o no, en caso de que no lo haya sido, debemos seguir intentando mediante el uso del método `ensureConnection` hasta que la misma sea satisfactoria.
##### Checkpoint 5
Debido a que los datos que leemos necesitan estar asociados a una variable temporal, es decir "X temperatura fue leída en la fecha y hora Y" necesitaremos poder sincronizar la fecha y hora de nuestro ESP32. 
1. Investigar y documentar por qué razón no podemos definir fecha y hora inicialmente y olvidarnos a futuro.
2. Investigar y documentar sobre NTP
3. Investigar sobre el funcionamiento de la función que viene incluida en `Arduino.h` llamada `configTime`
4.  Investigar sobre que servidores NTP abiertos podemos usar para sincronizarnos
5. Con toda la información anteriormente relevada, proceder a llamar la función `configTime` con sus parámetros correctos
	> En este punto debería resultar obvia esta aclaración... pero ahí va (por las dudas). Para que funcione correctamente la misma, debemos llamarla en el setup, posterior a haber realizado la conexión a internet correctamente.
##### Checkpoint 6
En este punto ya tenemos una fuente fiable de donde consultar la hora y sabemos que la misma se va a mantener sincronizada. Por ende en un futuro muuuy cercano (creeme por ahora), vamos a necesitar dos funciones tipo "helpers" que nos entreguen, la fecha en formato `año-mes-dia` como un string. Y otra que nos entregue la misma con la hora actual en formato `año-mes-dia hora:minutos:segundos`
1. Investigar como utilizar la función `getLocalTime` que viene incluida en `Arduino.h`
2. En base a las siguientes firmas, implementar ambas funciones para cumplir los requerimientos definidos anteriormente.
```cpp
String getDate(); // ejemplo: "2025-04-03"
String getDateTime(); // ejemplo: "2025-04-03 09:45:15"
```
##### Checkpoint 7
El próximo paso será preparar la memoria SD en conjunto a su módulo para la escritura/lectura (R/W). 
1. Investigar que formato debemos darle a la memoria SD
2. Formatear la memoria SD (quizá necesiten una mano, pueden pedir ayuda en este punto)
3. Ahora copiar en el proyecto el archivo `sd-manager.h` el cual tiene las firmas necesarias para nuestro proyecto. Peeeero, no va a ser tan fácil como los casos anteriores.
4. En este caso deberemos implementar el archivo `sd-manager.cpp` , pueden referir a un ejemplo similar en el `rain-meter.cpp` & `rain-meter.h` y así guiarse como hacerlo.
5. El método `appendFile` deberá hacer los siguientes pasos:
	1. Revisar si el `path` existe, es decir si ya hay un archivo con ese nombre
		1. En caso de que exista, deberá insertar el mensaje provisto. El archivo deberá ser abierto con el flag `FILE_APPEND`
		2. En caso de que no exista, deberá crear el archivo con el flag `FILE_WRITE` e insertar los `headers` provistos. Seguidamente cerrar el archivo y, naturalmente ejecutar el paso 1.
	2. En ambos casos asegurarse que el archivo se cierre, esto es MUY importante.
6. Una vez implementados los métodos, podemos pasar a probarlo!
7. Cada lectura que hagamos de los sensores, procederemos a guardarla en la memoria. Por el momento podemos usar cualquier string como headers, no es muy relevante.
##### Checkpoint 8
El formato que utilizaremos para guardar las mediciones históricas, será en CSV. La elección del mismo es por la facilidad de manejo y agregado de nuevos datos con poca cantidad y complejidad de código.
Se espera que se almacenen los datos con la siguiente estructura:

| timestamp           | temperature | humidity | pressure  | isRaining | rainIntensity |
| ------------------- | ----------- | -------- | --------- | --------- | ------------- |
| 2025-05-03 11:22:27 | 22.70       | 82.20    | 101759.00 | 0         | 1             |
| ...                 |             |          |           |           |               |

1. Investigar y documentar sobre el formato CSV
2. Limpiar la memoria SD (pueden pedir ayuda en este punto)
3.  Modificar los guardados del checkpoint del punto anterior para que los headers sean exactamente los que se mencionan arriba, y los datos sigan el formato esperado (respetando mayúsculas).
4. Chequear que los archivos se guarden correctamente y que el contenido sea el esperado. (pueden pedir ayuda en este punto)
##### Checkpoint 9
Para facilitarnos la interacción con la aplicación web, deberemos darle un nombre que tenga sentido a cada archivo. Para esto nos va a resultar útil la función `getDate` que implementamos algunos checkpoints atrás.
La idea es que el archivo contenga el día en el cuál se hicieron todas las lecturas y dentro del mismo, las lecturas por hora (gracias al uso de `getDateTime`), siendo esto una aproximación de un indice que nos permite encontrar datos de manera mas "simple" o lineal.

1. Modificar el guardado de los archivos para que ahora usen la función `getDate` y que su resultado sea el nombre del archivo sobre el cual estamos guardando datos.
##### Checkpoint 10
Ahora que ya tenemos una lectura cada pocos segundos y contamos con las lecturas históricas guardadas en la memoria SD, nos empezamos a acercar a las iteraciones finales.
La idea es poder "servir" estos datos a un cliente que quiera consultarlos (por el momento es bastante complejo saber que se guardó), por eso vamos a implementar un servidor web.
1. Investigar y documentar sobre:
	1. Servidor web
	2. Protocolo HTTP, cómo está compuesto, que son los `headers`
	3. Web API
	4. Endpoint
	5. Códigos HTTP
2. Copiar los archivos `http-server.h` y `http-server.cpp`
3. Instanciar la clase en el nivel superior mediante 
```cpp
HttpServer httpServer;
```
4. Luego ejecutar el método `begin` para inicializar el mismo.
5. Seguido de esto ya podemos crear un endpoint. Inicialmente crearemos uno llamado `/test` que devuelva simplemente el texto `hello world`.
	1. Para agregar un endpoint podemos utilizar la función `on` que puede ser utilizada de la siguiente manera
```cpp
httpServer.on(<el-endpoint-a-crear>, [](){
	// Acá va la lógica que se ejecutará a la hora de llamar a este endpoint.
	// Esto se lo conoce como `callback`
})
```
5. 
	2. Una vez declarado el endpoint, debemos agregar la lógica. En este primer caso es muy simple, solamente queremos devolver un string. Para esto podemos utilizar el método `send` el cuál puede ser usado como se muestra debajo. El status code que utilizaremos será un código de `ok` (buscar número correspondiente), el tipo de contenido `application/json`.
```cpp
httpServer.send(<status-code>,<content-type>,<la-respuesta-que queremos-enviar>);
```
6. En el loop deberemos llamar al método `handleClient` con un delay mínimo, esto permitirá que cada petición hacia servidor sea efectivamente manejada.
7. Con esto listo, ya estamos en condiciones de probar el funcionamiento. Para esto lo que haremos será abrir un navegador y dirigirse a `http://<ip-del-esp32>/test` y deberíamos ver `hello world` en pantalla.
> Importante: El navegador abierto debe estar en dispositivo que esté en LA MISMA red que el ESP32 está conectado.
##### Checkpoint 11
Ahora que ya sabemos que nuestro servidor web funciona, comenzaremos a agregar complejidad al mismo. 
1. Investigar y documentar sobre el formato JSON
2. Mover la lógica del servidor a una task que corra en el nucleo 1.
3. Corroborar que todo sigue funcionando correctamente
4. Modificar el endpoint actual, para que ahora el mismo devuelva los datos en tiempo real:
	1. El nuevo endpoint será `/weather/now`
	2. Con el struct de `latestReadings` debemos armar un JSON en formato string. El mismo deberemos armarlo a mano, por esto es clave tener en cuenta su formato.
	3. Las keys del mismo deben ser, respetando mayusculas y minúsculas: `temperature, humidity, pressure, isRaining, rainIntensity, timestamp` y los values deben ser sacados de `latestReadings`
	4. En el método `send` ahora modificar el último parámetro y devolver el JSON que se armó previamente.
5. Ahora necesitaremos agregar un nuevo header:
	1. llamar al método `sendHeader` (antes de send) el cual tiene la siguiente firma:  `httpServer.sendHeader(<header>, <value>);`
	2. Enviar el header `"Access-Control-Allow-Origin"` con el valor `"*"`
6. Por ultimo podemos acceder a la ip del ESP32 desde el navegador y cambiar el endpoint a `/weather/now` para visualizar si efectivamente el JSON fue creado correctamente.
##### Checkpoint 12
En este punto vamos a continuar con el armado de endpoints, casualmente el que nos permitirá devolver las mediciones históricas de la memoria SD.
1. Investigar y documentar acerca de Query parameters
2. Declarar un nuevo endpoint, en este caso será `/weather/{}` en este caso diferirá de los anteriores debido a que no será simplemente un string, sino que tendremos que envolverlo con la función `UriBraces` desde quedando: `UriBraces("/weather/{}")` 
	1. La función `UriBraces` tendremos acceso una vez que la importemos de `#include <uri/UriBraces.h>`
3. Extraeremos el query param con el método de httpServer llamado `httpServer.getUrlParam(0)`. Esta llamada nos devolverá un string, que, asumiremos, tendrá el formato de fecha que, casualmente (en realidad no) coincide con el nombre del archivo que contiene las mediciones.
4. Creamos una variable que almacenará un string con el siguiente formato: `"/<fecha>.csv"`
5. Leemos de la memoria el archivo, usando el método `readFile(<nombre-del-archivo>)` de nuestro objeto `sdManager`.
	1. Esta lectura nos devolverá un string con el contenido del archivo. Por eso debemos chequear que el mismo tenga alguna longitud mediante `.length()` 
		1. en caso de que este vacío haremos un `.send()` con el código de eror "NOT FOUND" (buscar el código correspondiente), el tipo `"application/json"` y el siguiente JSON: `"{\"error\":\"No data found for this date\"}"`
	2. En el caso de que el archivo contenga algún dato, procederemos a enviarlo, para esto debemos:
		1. enviar los headers correspondientes:
			1. header: `"Content-Disposition"` // valor: `"attachment; filename=<fecha>.csv"`
			2. header: `Access-Control-Allow-Origin` // valor: `"*"`
		2. Por ultimo haremos un `.send` con el código http "OK", el tipo será `"text/csv"` y el contenido los datos leídos del archivo.
6.  Una vez finalizado este paso, podemos ir a a la IP del ESP32, endpoint `/weather/<una-fecha>` y revisar si el mismo funciona
##### Checkpoint 13
Ojo, no se si te diste cuenta... pero hay un posible problema de concurrencia... chan...
Estamos haciendo escrituras y lecturas en la memoria SD, pero no estamos protegiéndolas. Alguien podría leer mientras estamos escribiendo y eso resultaria en un error o datos inconsistentes.
1. Declarar un semáforo mutex
2. Proteger las lecturas sobre archivos en la SD en el que hicimos anteriormente
3. Proteger las escrituras cada vez que escribimos en la memoria SD
##### Checkpoint 14
De a poco estamos llegando al final, quedan dos endpoints finales, los cuales nos servirán para servir la web de donde tomamos los datos.
1. Investigar y documentar acerca de SPIFFS
2. Investigar sobre como utilizar la libreria en el ESP32
3. Guardaremos la pagina web provista dentro de la memoria interna del ESP32, para así tener un acceso mucho mas veloz que si fuéramos a buscarlo a la SD. (acá podes pedir ayuda).
4. Hacer el `.begin` necesario para inicializar la librería
5. Crear dos endpoints que contendrán la misma lógica (probablemente te convenga hacer una función para reutilizar la lógica). uno será `"/"` y el otro `"/historical"`
6. La lógica de los mismos será:
	1. Ir a leer el archivo `index.html.gz` del SPIFFS
	2. Enviar los siguientes headers:
		1. header: `"Vary"` // valor: `"Accept-Encoding"`
		2. header: `"Cache-Control"` // valor: `"max-age=31536000"`
	3. Utilizar el método `.streamFile` del objeto httpServer para enviar el archivo que leemos, con el tipo `"text/html"`
	4. Cerrar el archivo llamando a `close` de la lib SSPIFFS
Ahora accedemos simplemente a la dirección del ESP32 sin ningún endpoint y Voilà! tenemos la pagina web lista para utilizar!

