#include "Homepage.h"

void WebHompage::setup(AsyncWebServer* server) {

  server->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    const char* out = R"HTML(
<html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.1.1/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-F3w7mX95PdgyTmZZMECAngseQB83DfGTowi0iMjiWaeVhAn4FJkqJByhZMI3AhiU" crossorigin="anonymous">
    <link href="https://cdn.jsdelivr.net/npm/bootswatch@4.5.2/dist/darkly/bootstrap.min.css" rel="stylesheet"/>
    <script src="https://code.jquery.com/jquery-3.6.0.min.js" integrity="sha256-/xUj+3OJU5yExlq6GSYGSHk7tPXikynS7ogEvDej/m4=" crossorigin="anonymous"></script>
  </head>
  <body>
    <div class="container-fluid">
        <h1>Sequence Animator</h1>
        
        <hr/>
        <h2>Control</h2>
        <div class="d-grid gap-2" id="sequence-list">
            <div class="d-flex justify-content-center">
                <div class="spinner-border" role="status">
                    <span class="visually-hidden">Loading...</span>
                </div>
            </div>
        </div>

        <hr/>
        <h2>API Endpoints</h2>
        <pre style="background: #444;">

    GET  /api/device
        Show information about the device

    GET  /api/sequence-list
        Retrieve the list of sequences

    POST /api/sequence?id={number}
        Set a sequence on the device

    GET  /api/sequence?id={number}
        Retrieve a sequence from the device

    POST /api/play?id={number}
        Play the chosen sequence

        </pre>

        <hr/>
        <h2>OTA Update</h2>
        <p>Over The Air Update is available on port 8622</p>
    </div>
</body>
<script>
    function play(id) {
        $.ajax({
            url: `/api/play?id=${id}`,
            method: 'post',
        });
    }

    function loadSequenceList(data) {
        $('#sequence-list').empty();

        (data.sequences || []).forEach(sequence => {
            $('#sequence-list')
                .append(`<button class="btn btn-primary" onclick="play(${sequence.id});">${sequence.name}</button>`);
        });
    }

    $(window).on('load', function () {
        $.ajax({
            url: "/api/sequence-list"
        }).done(loadSequenceList);
    });
</script>
</html>
    )HTML";

    request->send(200, "text/html", out);      
  });
}
