#include <Arduino.h>
#include <YuboxSimple.h>

#include <TaskScheduler.h>

Scheduler yuboxScheduler;

void setupAsyncServerHTTP(void);

void yuboxUpdateNTP(void);
Task task_yuboxUpdateNTP( TASK_SECOND * 3, TASK_FOREVER, &yuboxUpdateNTP );

void setup()
{
  // La siguiente demora es sólo para comodidad de desarrollo para enchufar el USB
  // y verlo en gtkterm. No es en lo absoluto necesaria como algoritmo requerido.
  //delay(3000);
  Serial.begin(115200);

  setupAsyncServerHTTP();

  // Actualización de NTP dentro de tarea
  yuboxScheduler.addTask(task_yuboxUpdateNTP);
  task_yuboxUpdateNTP.enable();
}

void loop()
{
  yuboxScheduler.execute();
}

void yuboxUpdateNTP(void)
{
  YuboxNTPConf.update();
  if (!YuboxNTPConf.isNTPValid()) {
    if (WiFi.isConnected()) Serial.println("ERR: fallo al obtener hora de red");
  } else {
    // TODO: activar banderas por ser NTP válido
  }
}

void setupAsyncServerHTTP(void)
{
  // TODO: agregar más inicializaciones que usen yubox_HTTPServer como
  // AsyncWebServer objetivo...

  // Inicialización estándar va al final...
  yuboxSimpleSetup();
}
