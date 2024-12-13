#ifndef WEB_INTERFACE_H
#define WEB_INTERFACE_H

const char WEBINTERFACE[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>NeoPixel Controller</title>
  <style>
    body {
      background: #1a1a1a;
      color: #fff;
      font-family: Arial, sans-serif;
      text-align: center;
      margin: 0;
      padding: 0;
    }
    h1 {
      margin-top: 20px;
      font-size: 2em;
    }
    .container {
      max-width: 800px;
      margin: 20px auto;
      padding: 20px;
      background-color: #333;
      border-radius: 8px;
      box-shadow: 0 0 10px rgba(0, 0, 0, 0.5);
    }
    .button {
      display: inline-block;
      margin: 10px;
      padding: 10px 20px;
      font-size: 16px;
      color: #fff;
      background-color: #007bff;
      border: none;
      border-radius: 5px;
      text-decoration: none;
      cursor: pointer;
    }
    .button:hover {
      background-color: #0056b3;
    }
    canvas {
      display: block;
      margin: 20px auto;
      border-radius: 50%;
    }
    .slider {
      width: 80%;
      margin: 20px auto;
    }
    .scenes-grid {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
      gap: 10px;
      justify-items: center;
      margin-top: 20px;
    }
    .color-preview {
      width: 100px;
      height: 100px;
      margin: 20px auto;
      border-radius: 10px;
      border: 2px solid #fff;
      background-color: #000;
    }
  </style>
</head>
<body>
  <h1>NeoPixel Controller</h1>
  <div class="container">
    <h2>Color Picker</h2>
    <canvas id="colorWheel" width="300" height="300"></canvas>
    <div id="colorPreview" class="color-preview"></div>
    <input type="range" id="brightness" class="slider" min="0" max="255" value="128">
    <p><button class="button" onclick="applyColor()">Farbe Übernehmen</button></p>
    <h2>Szenen</h2>
    <div class="scenes-grid">
      <button class="button" onclick="triggerEffect('lightning')">Blitz</button>
      <button class="button" onclick="triggerEffect('rainbow')">Regenbogen</button>
      <button class="button" onclick="triggerEffect('pulse')">Puls</button>
      <button class="button" onclick="triggerEffect('breathing')">Atmen</button>
      <button class="button" onclick="triggerEffect('candle')">Kerzenflackern</button>
      <button class="button" onclick="triggerEffect('chase')">Jagd</button>
      <button class="button" onclick="triggerEffect('twinkle')">Funkeln</button>
      <button class="button" onclick="triggerEffect('fire')">Feuer</button>
      <button class="button" onclick="triggerEffect('wave')">Welle</button>
      <button class="button" onclick="triggerEffect('meteor')">Meteor</button>
      <button class="button" onclick="triggerEffect('strobe')">Stroboskop</button>
      <button class="button" onclick="triggerEffect('aurora')">Aurora</button>
      <button class="button" onclick="triggerEffect('colorwipe')">Farbwischer</button>
      <button class="button" onclick="triggerEffect('sparkle')">Glitzern</button>
      <button class="button" onclick="triggerEffect('gradient')">Verlauf</button>
      <button class="button" onclick="triggerEffect('alarm')">Alarm</button>
      <button class="button" onclick="triggerEffect('sparkletrail')">Glitzer Spur</button>
      <button class="button" onclick="triggerEffect('runninglight')">Lauflicht</button>
    </div>
  </div>
  <script>
    const canvas = document.getElementById('colorWheel');
    const ctx = canvas.getContext('2d');
    const radius = canvas.width / 2;
    const brightnessSlider = document.getElementById('brightness');
    const colorPreview = document.getElementById('colorPreview');

    // Draw color wheel
    function drawColorWheel() {
      const imageData = ctx.createImageData(canvas.width, canvas.height);
      for (let y = 0; y < canvas.height; y++) {
        for (let x = 0; x < canvas.width; x++) {
          const dx = x - radius;
          const dy = y - radius;
          const distance = Math.sqrt(dx * dx + dy * dy);
          const angle = Math.atan2(dy, dx) + Math.PI;

          if (distance <= radius) {
            const hue = (angle / (2 * Math.PI)) * 360;
            const saturation = distance / radius;
            const [r, g, b] = hsvToRgb(hue, saturation, 1);

            const index = (y * canvas.width + x) * 4;
            imageData.data[index] = r;
            imageData.data[index + 1] = g;
            imageData.data[index + 2] = b;
            imageData.data[index + 3] = 255;
          }
        }
      }
      ctx.putImageData(imageData, 0, 0);
    }

    function hsvToRgb(h, s, v) {
      let r, g, b;
      const i = Math.floor(h / 60);
      const f = h / 60 - i;
      const p = v * (1 - s);
      const q = v * (1 - f * s);
      const t = v * (1 - (1 - f) * s);

      switch (i % 6) {
        case 0: r = v, g = t, b = p; break;
        case 1: r = q, g = v, b = p; break;
        case 2: r = p, g = v, b = t; break;
        case 3: r = p, g = q, b = v; break;
        case 4: r = t, g = p, b = v; break;
        case 5: r = v, g = p, b = q; break;
      }

      return [Math.round(r * 255), Math.round(g * 255), Math.round(b * 255)];
    }

    function getColorFromWheel(event) {
      const rect = canvas.getBoundingClientRect();
      const x = event.clientX - rect.left - radius;
      const y = event.clientY - rect.top - radius;
      const distance = Math.sqrt(x * x + y * y);

      if (distance > radius) return null;

      const angle = Math.atan2(y, x) + Math.PI;
      const hue = (angle / (2 * Math.PI)) * 360;
      const saturation = distance / radius;
      const value = brightnessSlider.value / 255;

      return hsvToRgb(hue, saturation, value);
    }

    canvas.addEventListener('click', (event) => {
      const color = getColorFromWheel(event);
      if (color) {
        canvas.style.borderColor = `rgb(${color[0]}, ${color[1]}, ${color[2]})`;
        colorPreview.style.backgroundColor = `rgb(${color[0]}, ${color[1]}, ${color[2]})`;
      }
    });

    brightnessSlider.addEventListener('input', () => {
      const color = canvas.style.borderColor.replace(/[^\d,]/g, '').split(',');
      if (color.length === 3) {
        const brightness = brightnessSlider.value;
        fetch(`/api/v1/state`, {
          method: 'POST',
          headers: {
            'Content-Type': 'application/json',
          },
          body: JSON.stringify({
            state: 'ON',
            brightness: parseInt(brightness),
            color: {
              r: parseInt(color[0]),
              g: parseInt(color[1]),
              b: parseInt(color[2])
            }
          }),
        });
      }
    });

    function applyColor() {
      const brightness = brightnessSlider.value;
      const color = canvas.style.borderColor.replace(/[^\d,]/g, '').split(',');
      fetch(`/api/v1/state`, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({
          state: 'ON',
          brightness: parseInt(brightness),
          color: {
            r: parseInt(color[0]),
            g: parseInt(color[1]),
            b: parseInt(color[2])
          }
        }),
      }).then(response => {
        if (response.ok) {
          alert('Farbe angewendet!');
        } else {
          alert('Fehler beim Anwenden der Farbe.');
        }
      });
    }

    function triggerEffect(effect) {
      fetch(`/api/v1/effect/${effect}`, { method: 'GET' })
        .then(response => {
          if (response.ok) {
            alert(`${effect} Effekt ausgelöst`);
          } else {
            alert(`Fehler beim Auslösen des Effekts: ${effect}`);
          }
        });
    }

    drawColorWheel();
  </script>
</body>
</html>
)=====";

#endif // WEB_INTERFACE_H
