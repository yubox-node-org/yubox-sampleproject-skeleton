#include <WiFi.h>
#include "SPIFFS.h"
#include <ESPAsyncWebServer.h>

#include <TaskScheduler.h>

#define ARDUINOJSON_USE_LONG_LONG 1

#include "AsyncJson.h"
#include "ArduinoJson.h"

#include "YuboxWiFiClass.h"
#include "YuboxNTPConfigClass.h"
#include "YuboxOTAClass.h"

AsyncWebServer server(80);
Scheduler yuboxScheduler;

void notFound(AsyncWebServerRequest *);
void setupAsyncServerHTTP(void);

void yuboxUpdateNTP(void);
Task task_yuboxUpdateNTP( TASK_SECOND * 3, TASK_FOREVER, &yuboxUpdateNTP );

void setup()
{
  // La siguiente demora es sólo para comodidad de desarrollo para enchufar el USB
  // y verlo en gtkterm. No es en lo absoluto necesaria como algoritmo requerido.
  //delay(3000);
  Serial.begin(115200);

  //Serial.println("DEBUG: inicializando SPIFFS...");
  if (!SPIFFS.begin(true)) {
    Serial.println("ERR: ha ocurrido un error al montar SPIFFS");
    while (true) delay(1000);
  }

  // Limpiar archivos que queden de actualización fallida
  YuboxOTA.cleanupFailedUpdateFiles();

  setupAsyncServerHTTP();

  YuboxWiFi.beginServerOnWiFiReady(&server);

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
  // Activar y agregar todas las rutas que requieren autenticación
  YuboxWebAuth.setEnabled(true);	// <-- activar explícitamente la autenticación

  YuboxWiFi.begin(server);
  YuboxWebAuth.begin(server);
  YuboxNTPConf.begin(server);
  YuboxOTA.begin(server);
  server.onNotFound(notFound);

  AsyncWebHandler &h = server.serveStatic("/", SPIFFS, "/");
  YuboxWebAuth.addManagedHandler(&h);
}

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "application/json", "{\"success\":false,\"msg\":\"El recurso indicado no existe o no ha sido implementado\"}");
}
