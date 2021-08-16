const char MAIN_page[] PROGMEM = R"=====(
<html>
    <head>
    <title>Sensor page</title>
    </head>
<body>
    <style>
      .ibutton {
        display: table;
        background-color: #afafaf;
        outline: none;
        text-decoration: none;
        border-radius: 5px;
        padding: 5px;
        margin: 10px 0;
        cursor: pointer;
        color: white;
      }
      .ibutton-off {
        background-color: #e25d5d;
      }
      .ibutton-on {
        background-color: #67d06d;
      }
    </style>
    <script>
      function SendRequest(requestValue) {
        var xhr = new XMLHttpRequest();
        xhr.open('GET', requestValue, false);
        xhr.send();
      }
    </script>
    <h2>Sensor page</h2>
    <h4>Data</h4>
    <a href="/registration" target="_blank"><div class="ibutton ibutton-on" style="display: inline-block;">Registration</div></a>
    <a href="/info" target="_blank"><div class="ibutton ibutton-on" style="display: inline-block;">Info</div></a>
    <a href="/setting" target="_blank"><div class="ibutton ibutton-on" style="display: inline-block;">Setting</div></a>

    <h4>Service</h4>
    <div class="ibutton" onclick="SendRequest('/restart')">Restart</div>
    <h4>Links</h4>
    <a href="/metrics" target="_blank"><div class="ibutton" style="display: inline-block;">Metrics page</div></a>
</body>
</html>
)=====";
