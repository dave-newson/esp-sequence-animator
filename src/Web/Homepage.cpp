#include "Homepage.h"

void WebHompage::setup(AsyncWebServer* server) {

    server->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(200, "text/html",
"<html>"
"\n <body>"
"\n   <h1>Sequence Animator</h1>"
"\n   <h2>API Endpoints</h2>"
"\n   <pre style=\"background: #eee;\">"
"\n"
"\n   GET  /api/device"
"\n     Show information about the device"
"\n"
"\n   GET  /api/state"
"\n     Get current device state"
"\n"
"\n   POST /api/state"
"\n     Set state for peripherals on the device"
"\n"
"\n   POST /api/sequence/{slot}"
"\n     Set a sequence on the device"
"\n"
"\n   GET  /api/sequence/{slot}"
"\n     Retrieve a sequence from the device"
"\n"
"\n   POST /api/sequence/{slot}/play"
"\n     Play the sequence"
"\n"
"\n   </pre>"
"\n   <h2>OTA Update</h2>"
"\n   <p>Over The Air Update is available on port 8622</p>"
"\n </body>"
"\n</html>"
      );
    });
   
}