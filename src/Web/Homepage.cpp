#include "Homepage.h"

void WebHompage::setup(AsyncWebServer* server) {

  server->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    const char* out = R"HTML(
<html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.1.1/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-F3w7mX95PdgyTmZZMECAngseQB83DfGTowi0iMjiWaeVhAn4FJkqJByhZMI3AhiU" crossorigin="anonymous">
    <link href="https://cdn.jsdelivr.net/npm/bootswatch@4.5.2/dist/darkly/bootstrap.min.css" rel="stylesheet"/>
  </head>
  <body>
    <div class="container-fluid">
        <h1>Sequence Animator</h1>
        
        <hr/>
        <h2>Control</h2>
        <div class="d-grid gap-2">
          <button class="btn btn-primary" onclick="play(1);">Sequence 1</button>
          <button class="btn btn-primary" onclick="play(2);">Sequence 2</button>
          <button class="btn btn-primary" onclick="play(3);">Sequence 3</button>
          <button class="btn btn-primary" onclick="play(4);">Sequence 4</button>
          <button class="btn btn-primary" onclick="play(5);">Sequence 5</button>
          <button class="btn btn-primary" onclick="play(6);">Sequence 6</button>
          <button class="btn btn-primary" onclick="play(7);">Sequence 7</button>
          <button class="btn btn-primary" onclick="play(8);">Sequence 8</button>
        </div>

        <hr/>
        <h2>API Endpoints</h2>
        <pre style=\"background: #eee;\">

    GET  /api/device
        Show information about the device

    POST /api/sequence?id={number}
        Set a sequence on the device

    GET  /api/sequence?id={number}
        Retrieve a sequence from the device

    POST /api/play?id={number}
        Play the sequence

        </pre>

        <hr/>
        <h2>OTA Update</h2>
        <p>Over The Air Update is available on port 8622</p>
    </div>
</body>
<script>
function play(id) {
  const xhttp = new XMLHttpRequest();
  xhttp.open("POST", `/api/play?id=${id}`, true);
  xhttp.send();
}
</script>
</html>
      )HTML";

    request->send(200, "text/html", out);      
  });
}
