#ifndef WEB_UI_H
#define WEB_UI_H

static const char PROGMEM INDEX_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no" />
  <title>ESP32-CAM</title>
  <style>
    :root {
      --primary-color: #2c3e50;
      --secondary-color: #3498db;
      --success-color: #27ae60;
      --danger-color: #e74c3c;
      --warning-color: #f39c12;
      --light-color: #ecf0f1;
      --dark-color: #2c3e50;
      --text-color: #333;
      --text-light: #fff;
    }
    
    * {
      box-sizing: border-box;
      margin: 0;
      padding: 0;
    }
    
    body {
      font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
      line-height: 1.6;
      color: var(--text-color);
      background-color: #f5f7fa;
      padding: 0;
      margin: 0;
    }
    
    .container {
      width: 100%;
      max-width: 1200px;
      margin: 0 auto;
      padding: 15px;
    }
    
    header {
      background-color: var(--primary-color);
      color: var(--text-light);
      padding: 15px 0;
      box-shadow: 0 2px 5px rgba(0,0,0,0.1);
      position: sticky;
      top: 0;
      z-index: 100;
    }
    
    .header-content {
      display: flex;
      justify-content: space-between;
      align-items: center;
      flex-wrap: wrap;
    }
    
    h1 {
      font-size: 1.5rem;
      margin: 0;
    }
    
    #tempDisplay {
      background-color: rgba(255,255,255,0.2);
      padding: 5px 10px;
      border-radius: 20px;
      font-size: 0.9rem;
    }
    
    nav {
      width: 100%;
      margin-top: 10px;
      display: flex;
      justify-content: space-around;
      background-color: rgba(255,255,255,0.1);
      border-radius: 5px;
      padding: 5px;
    }
    
    nav a {
      color: var(--text-light);
      text-decoration: none;
      padding: 5px 10px;
      border-radius: 3px;
      transition: background-color 0.3s;
      font-size: 0.9rem;
    }
    
    nav a:hover {
      background-color: rgba(255,255,255,0.2);
    }
    
    .section {
      background: white;
      border-radius: 8px;
      padding: 20px;
      margin: 15px 0;
      box-shadow: 0 2px 10px rgba(0,0,0,0.05);
    }
    
    .hidden {
      display: none;
    }
    
    h2 {
      color: var(--primary-color);
      margin-bottom: 15px;
      font-size: 1.3rem;
    }
    
    #stream {
      width: 100%;
      background: #000;
      aspect-ratio: 4/3;
      border-radius: 5px;
      display: block;
      margin: 0 auto;
    }
    
    .button-group {
      display: flex;
      gap: 10px;
      margin-top: 15px;
      flex-wrap: wrap;
    }
    
    button {
      padding: 10px 15px;
      border: none;
      border-radius: 5px;
      cursor: pointer;
      font-weight: 500;
      transition: all 0.3s;
      flex: 1 1 0;
      min-width: 100px;
    }
    
    button:active {
      transform: scale(0.98);
    }
    
    .start-btn { 
      background: var(--success-color); 
      color: white; 
    }
    
    .stop-btn { 
      background: var(--danger-color); 
      color: white; 
    }
    
    .snap-btn { 
      background: var(--secondary-color); 
      color: white; 
    }
    
    .flash-btn {
      background: var(--warning-color);
      color: white;
    }
    
    .update-btn { 
      background: var(--warning-color); 
      color: white;
      width: 100%;
    }
    
    .form-group {
      margin-bottom: 15px;
    }
    
    label {
      display: block;
      margin-bottom: 5px;
      font-weight: 500;
    }
    
    input[type="text"],
    input[type="password"],
    input[type="file"],
    input[type="number"],
    input[type="range"],
    select {
      width: 100%;
      padding: 10px;
      border: 1px solid #ddd;
      border-radius: 5px;
      font-size: 1rem;
    }
    
    input[type="range"] {
      padding: 0;
    }
    
    .progress-container {
      margin: 15px 0;
    }
    
    .progress-bar {
      height: 10px;
      background: #eee;
      border-radius: 5px;
      margin: 5px 0;
      overflow: hidden;
    }
    
    .progress {
      height: 100%;
      width: 0%;
      background: var(--success-color);
      transition: width 0.3s;
    }
    
    #otaResult {
      margin-top: 15px;
      padding: 10px;
      border-radius: 5px;
      display: none;
    }
    
    .success {
      background-color: #d4edda;
      color: #155724;
    }
    
    .error {
      background-color: #f8d7da;
      color: #721c24;
    }
    
    @media (min-width: 768px) {
      .header-content {
        flex-wrap: nowrap;
      }
      
      nav {
        width: auto;
        margin-top: 0;
        background: transparent;
      }
      
      nav a {
        margin: 0 5px;
      }
      
      .button-group {
        flex-wrap: nowrap;
      }
      
      .form-row {
        display: flex;
        gap: 15px;
      }
      
      .form-group {
        flex: 1;
      }
    }
  </style>
</head>
<body>
  <header>
    <div class="container">
      <div class="header-content">
        <h1>ESP32-CAM</h1>
        <div id="tempDisplay">Temp: -- °C</div>
        <nav>
          <a href="#live">Live</a>
          <a href="#settings" onclick="toggleSettings()">Settings</a>
          <a href="#ota">OTA Update</a>
        </nav>
      </div>
    </div>
  </header>

  <main class="container">
    <section id="live" class="section">
      <h2>Live Preview</h2>
      <img id="stream" src="" alt="Camera Stream" />
      <div class="button-group">
        <button class="start-btn" onclick="startStream()">Start</button>
        <button class="stop-btn" onclick="stopStream()">Stop</button>
        <button class="snap-btn" onclick="capture()">Snapshot</button>
        <button id="flashBtn" class="flash-btn" onclick="toggleFlash()">Flash: OFF</button>
      </div>
    </section>

    <section id="settings" class="section hidden">
      <h2>Camera Settings</h2>
      
      <div class="form-group">
        <label for="framesize">Resolution</label>
        <select id="framesize" onchange="updateSetting(this)">
          <option value="10">UXGA (1600x1200)</option>
          <option value="9">SXGA (1280x1024)</option>
          <option value="8">XGA (1024x768)</option>
          <option value="7">SVGA (800x600)</option>
          <option value="6">VGA (640x480)</option>
          <option value="5">CIF (400x296)</option>
          <option value="4">QVGA (320x240)</option>
          <option value="3">HQVGA (240x176)</option>
          <option value="0">QQVGA (160x120)</option>
        </select>
      </div>

      <div class="form-group">
        <label for="quality">JPEG Quality</label>
        <input type="number" id="quality" min="10" max="63" value="12" onchange="updateSetting(this)">
      </div>

      <div class="form-group">
        <label for="brightness">Brightness</label>
        <input type="range" id="brightness" min="-2" max="2" value="0" oninput="updateSetting(this)">
      </div>

      <div class="form-group">
        <label for="contrast">Contrast</label>
        <input type="range" id="contrast" min="-2" max="2" value="0" oninput="updateSetting(this)">
      </div>

      <div class="form-group">
        <label for="gainceiling">Gain Ceiling</label>
        <select id="gainceiling" onchange="updateSetting(this)">
          <option value="0">2x</option>
          <option value="1">4x</option>
          <option value="2">8x</option>
          <option value="3">16x</option>
          <option value="4">32x</option>
          <option value="5">64x</option>
          <option value="6">128x</option>
        </select>
      </div>

      <div class="form-group">
        <label for="hmirror">Horizontal Mirror</label>
        <select id="hmirror" onchange="updateSetting(this)">
          <option value="1">On</option>
          <option value="0">Off</option>
        </select>
      </div>

      <div class="form-group">
        <label for="vflip">Vertical Flip</label>
        <select id="vflip" onchange="updateSetting(this)">
          <option value="1">On</option>
          <option value="0">Off</option>
        </select>
      </div>
    </section>

    <section id="ota" class="section">
      <h2>Firmware Update</h2>
      <p>Upload a new firmware binary to update your ESP32-CAM. The device will restart automatically after update.</p>
      
      <div class="form-row">
        <div class="form-group">
          <label for="otaUsername">Username</label>
          <input type="text" id="otaUsername" value="admin">
        </div>
        <div class="form-group">
          <label for="otaPassword">Password</label>
          <input type="password" id="otaPassword" value="admin123">
        </div>
      </div>
      
      <div class="form-group">
        <label for="firmwareFile">Firmware File</label>
        <input type="file" id="firmwareFile" accept=".bin">
      </div>
      
      <button class="update-btn" onclick="startOTAUpdate()">Upload & Update</button>
      
      <div class="progress-container">
        <div id="otaStatus">Ready for update</div>
        <div class="progress-bar">
          <div id="otaProgress" class="progress"></div>
        </div>
        <div id="otaProgressText">0%</div>
      </div>
      
      <div id="otaResult"></div>
    </section>
  </main>

  <script>
    let isStreaming = false;
    let flashLevel = 0;
    const flashMap = [0, 64, 128, 255];
    const flashLabels = ["OFF", "LOW", "MED", "HIGH"];

    function startStream() {
      document.getElementById("stream").src = "/stream";
      isStreaming = true;
    }

    function stopStream() {
      document.getElementById("stream").src = "";
      isStreaming = false;
    }

    function capture() {
      const link = document.createElement("a");
      link.href = "/capture?" + new Date().getTime();
      link.download = "snapshot.jpg";
      document.body.appendChild(link);
      link.click();
      document.body.removeChild(link);
    }

    function toggleFlash() {
      flashLevel = (flashLevel + 1) % 4;
      const btn = document.getElementById("flashBtn");
      fetch(`/flash?state=${flashLevel > 0 ? 1 : 0}&level=${flashMap[flashLevel]}`)
        .then(() => {
          btn.textContent = `Flash: ${flashLabels[flashLevel]}`;
        });
    }

    function toggleSettings() {
      const settings = document.getElementById("settings");
      settings.classList.toggle("hidden");
    }

    async function updateSetting(el) {
      const id = el.id;
      const val = el.value;
      const wasStreaming = isStreaming;
      if (wasStreaming) stopStream();
      await fetch(`/control?var=${id}&val=${val}`);

      if (id === "framesize") {
        const preview = document.getElementById("stream");
        const sizeMap = {
          "10": [1600, 1200], "9": [1280, 1024], "8": [1024, 768],
          "7": [800, 600], "6": [640, 480], "5": [400, 296],
          "4": [320, 240], "3": [240, 176], "0": [160, 120]
        };
        const [w, h] = sizeMap[val] || [640, 480];
        preview.style.width = w + "px";
        preview.style.height = h + "px";
      }

      if (wasStreaming) startStream();
    }

    // OTA Update Functions
    function startOTAUpdate() {
      const fileInput = document.getElementById("firmwareFile");
      const file = fileInput.files[0];
      const username = document.getElementById("otaUsername").value;
      const password = document.getElementById("otaPassword").value;
      
      if (!file) {
        showResult("Please select a firmware file", "error");
        return;
      }
      
      updateOTAStatus("Uploading firmware...");
      
      const formData = new FormData();
      formData.append("update", file);
      
      const xhr = new XMLHttpRequest();
      xhr.open("POST", "http://" + window.location.hostname + ":81/update");
      xhr.setRequestHeader("Authorization", "Basic " + btoa(username + ":" + password));
      
      xhr.upload.onprogress = function(e) {
        if (e.lengthComputable) {
          const percent = Math.round((e.loaded / e.total) * 100);
          document.getElementById("otaProgress").style.width = percent + "%";
          document.getElementById("otaProgressText").textContent = percent + "%";
        }
      };
      
      xhr.onload = function() {
        if (xhr.status === 200) {
          showResult("Update successful! Device will restart...", "success");
          updateOTAStatus("Update complete");
          
          const checkRestart = setInterval(() => {
            fetch("/")
              .then(() => {
                clearInterval(checkRestart);
                window.location.reload();
              })
              .catch(() => {});
          }, 1000);
        } else {
          showResult("Update failed: " + xhr.responseText, "error");
          updateOTAStatus("Update failed");
        }
      };
      
      xhr.onerror = function() {
        showResult("Error during update. Check connection.", "error");
        updateOTAStatus("Update error");
      };
      
      xhr.send(formData);
    }
    
    function updateOTAStatus(text) {
      document.getElementById("otaStatus").textContent = text;
    }
    
    function showResult(message, type) {
      const resultDiv = document.getElementById("otaResult");
      resultDiv.textContent = message;
      resultDiv.className = type;
      resultDiv.style.display = "block";
    }

    // Realtime temperature fetch every 5 seconds
    function fetchTemp() {
      fetch("/temp")
        .then(res => res.json())
        .then(data => {
          const tempSpan = document.getElementById("tempDisplay");
          if (data.temperature !== undefined) {
            tempSpan.textContent = `Temp: ${data.temperature.toFixed(1)} °C`;
          }
        })
        .catch(() => {
          document.getElementById("tempDisplay").textContent = "Temp: -- °C";
        });
    }

    window.onload = () => {
      fetch("/status")
        .then(res => res.json())
        .then(data => {
          ["framesize", "quality", "brightness", "contrast", "gainceiling", "hmirror", "vflip"].forEach(id => {
            const el = document.getElementById(id);
            if (el && data[id] !== undefined) el.value = data[id];
          });
        });

      fetchTemp();
      setInterval(fetchTemp, 5000);
    };
  </script>
</body>
</html>
)rawliteral";

#endif