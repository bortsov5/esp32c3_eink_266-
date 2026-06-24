#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
#include "imagedata.h"
#include <stdlib.h>
#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "RestfrontMI";
const char* password = "Rf7531711";

const int EPD_W = 152;
const int EPD_H = 296;
const int BMP_SIZE = (EPD_W * EPD_H / 8) * 2; 

uint8_t* bitmap = nullptr;
bool newDrawing = false;

WebServer server(80);

#define EPD_WIDTH  152
#define EPD_HEIGHT 296
#define ARRAY_SIZE ((EPD_WIDTH / 8) * EPD_HEIGHT)
#define SIZE40 ((400 / 8) * 40)

unsigned char black_data[ARRAY_SIZE];
unsigned char red_data[ARRAY_SIZE];
unsigned char img40_data[SIZE40];

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width,initial-scale=1,user-scalable=no">
<title>E-Paper Draw</title>
<style>
*{margin:0;padding:0;box-sizing:border-box}
body{background:#1a1a1a;color:#fff;font-family:system-ui;display:flex;flex-direction:column;align-items:center;height:100dvh;padding:10px}
h2{margin:4px 0;font-size:18px}
#toolbar{display:flex;gap:6px;margin:6px 0;align-items:center;flex-wrap:wrap;justify-content:center}
button{background:#333;color:#fff;border:1px solid #555;border-radius:6px;padding:8px 14px;font-size:14px;cursor:pointer}
button:active{background:#555}
button.active{background:#fff;color:#000}
#btnR.active{background:#ff0000;color:#fff;border-color:#ff0000}
#send{background:#2563eb;border-color:#2563eb}
#pasteBtn{background:#7c3aed;border-color:#7c3aed}
#canvasWrap{border:2px solid #444;border-radius:4px;background:#fff;overflow:hidden;touch-action:none;max-width:100%}
canvas{display:block;max-width:100%;height:auto}
#status{margin-top:6px;font-size:13px;color:#aaa}
#brushSize{width:70px}
label{font-size:13px}
#pasteArea{display:none;position:fixed;top:0;left:0;right:0;bottom:0;background:rgba(0,0,0,0.8);justify-content:center;align-items:center;z-index:1000}
#pasteArea.active{display:flex}
#pasteBox{background:#2a2a2a;padding:20px;border-radius:12px;max-width:90%;max-height:90%;overflow:auto}
#pasteBox textarea{width:100%;height:200px;background:#1a1a1a;color:#fff;border:1px solid #555;border-radius:6px;padding:10px;font-family:monospace;font-size:12px;resize:vertical}
#pasteBox .btnRow{display:flex;gap:8px;margin-top:10px;justify-content:flex-end}
#pasteBox .btnRow button{padding:6px 16px}
#fileInput{display:none}
</style>
</head>
<body>
<h2>E-Paper Draw</h2>
<div id="toolbar">
  <button id="btnB" class="active" onclick="setC(0)">Black</button>
  <button id="btnR" onclick="setC(2)">Red</button>
  <button id="btnW" onclick="setC(1)">White</button>
  <label>Brush</label>
  <input id="brushSize" type="range" min="1" max="20" value="3">
  <button onclick="clr()">Clear</button>
  <button id="pasteBtn" onclick="showPasteDialog()">Paste</button>
  <button id="send" onclick="send()">Send</button>
</div>
<div id="canvasWrap">
  <canvas id="c" width="152" height="296"></canvas>
</div>
<p id="status">Draw something, paste image, or hit Send</p>

<!-- Диалог вставки -->
<div id="pasteArea">
  <div id="pasteBox">
    <h3 style="margin-bottom:10px">Paste Image</h3>
    <p style="font-size:13px;color:#aaa;margin-bottom:10px">
      Paste from clipboard (Ctrl+V) or enter URL / base64 data
    </p>
    <textarea id="pasteInput" placeholder="Paste image data here (URL or base64) or use Ctrl+V"></textarea>
    <div style="margin:10px 0;display:flex;gap:10px;flex-wrap:wrap">
      <button onclick="pasteFromClipboard()">Paste from Clipboard</button>
      <button onclick="document.getElementById('fileInput').click()">Choose File</button>
      <input type="file" id="fileInput" accept="image/*" onchange="loadFile(event)">
    </div>
    <div class="btnRow">
      <button onclick="hidePasteDialog()">Cancel</button>
      <button id="applyPasteBtn" onclick="applyPaste()" style="background:#2563eb;border-color:#2563eb">Apply</button>
    </div>
    <div id="pastePreview" style="margin-top:10px;text-align:center;max-height:200px;overflow:hidden">
      <img id="previewImg" style="max-height:200px;max-width:100%;display:none">
    </div>
  </div>
</div>

<script>
var W=152,H=296,c=document.getElementById('c'),
ctx=c.getContext('2d'),dr=false,col=0;
ctx.fillStyle='#fff';ctx.fillRect(0,0,W,H);
ctx.lineCap='round';ctx.lineJoin='round';

// Переменная для хранения вставляемого изображения
var pasteImageData = null;

function setC(v){
  col=v;
  document.getElementById('btnB').className = (v==0) ? 'active' : '';
  document.getElementById('btnR').className = (v==2) ? 'active' : '';
  document.getElementById('btnW').className = (v==1) ? 'active' : '';
}

function bs(){return document.getElementById('brushSize').value}
function gp(e){var r=c.getBoundingClientRect(),t=e.touches?e.touches[0]:e;
  return[(t.clientX-r.left)*(W/r.width),(t.clientY-r.top)*(H/r.height)]}

function sd(e){
  e.preventDefault();
  dr=true;
  var p=gp(e);
  ctx.beginPath();
  ctx.moveTo(p[0],p[1]);
  
  if(col == 0) ctx.strokeStyle = '#000000';
  else if(col == 1) ctx.strokeStyle = '#ffffff';
  else if(col == 2) ctx.strokeStyle = '#ff0000';
  
  ctx.lineWidth = bs();
}

function mv(e){
  e.preventDefault();
  if(!dr)return;
  var p=gp(e);
  ctx.lineTo(p[0],p[1]);
  ctx.stroke();
  ctx.beginPath();
  ctx.moveTo(p[0],p[1]);
}

function ed(e){
  e.preventDefault();
  dr=false;
}

c.onmousedown=sd;
c.onmousemove=mv;
c.onmouseup=ed;
c.onmouseleave=ed;
c.ontouchstart=sd;
c.ontouchmove=mv;
c.ontouchend=ed;

function clr(){
  ctx.fillStyle='#fff';
  ctx.fillRect(0,0,W,H);
  pasteImageData = null;
  document.getElementById('status').textContent = 'Canvas cleared';
}

// Функции для работы с буфером обмена
function showPasteDialog() {
  document.getElementById('pasteArea').classList.add('active');
  document.getElementById('pasteInput').value = '';
  document.getElementById('previewImg').style.display = 'none';
  document.getElementById('pasteInput').focus();
}

function hidePasteDialog() {
  document.getElementById('pasteArea').classList.remove('active');
  pasteImageData = null;
}

// Вставка из буфера обмена (Ctrl+V)
document.addEventListener('paste', function(e) {
  var pasteArea = document.getElementById('pasteArea');
  if (!pasteArea.classList.contains('active')) return;
  
  var items = e.clipboardData.items;
  for (var i = 0; i < items.length; i++) {
    if (items[i].type.indexOf('image') !== -1) {
      var blob = items[i].getAsFile();
      var reader = new FileReader();
      reader.onload = function(event) {
        document.getElementById('pasteInput').value = event.target.result;
        previewPaste(event.target.result);
      };
      reader.readAsDataURL(blob);
      break;
    }
  }
});

// Вставка из буфера обмена по кнопке
function pasteFromClipboard() {
  navigator.clipboard.read().then(function(items) {
    for (var i = 0; i < items.length; i++) {
      if (items[i].types.indexOf('image/png') !== -1 || items[i].types.indexOf('image/jpeg') !== -1) {
        items[i].getType(items[i].types[0]).then(function(blob) {
          var reader = new FileReader();
          reader.onload = function(event) {
            document.getElementById('pasteInput').value = event.target.result;
            previewPaste(event.target.result);
          };
          reader.readAsDataURL(blob);
        });
        break;
      }
    }
  }).catch(function(err) {
    alert('Cannot read clipboard. Please use Ctrl+V or paste URL manually.');
  });
}

// Загрузка файла
function loadFile(event) {
  var file = event.target.files[0];
  if (!file) return;
  var reader = new FileReader();
  reader.onload = function(e) {
    document.getElementById('pasteInput').value = e.target.result;
    previewPaste(e.target.result);
  };
  reader.readAsDataURL(file);
  event.target.value = '';
}

// Предпросмотр изображения
function previewPaste(data) {
  var img = document.getElementById('previewImg');
  img.onload = function() {
    img.style.display = 'block';
    // Масштабируем для предпросмотра
    var maxHeight = 200;
    if (img.naturalHeight > maxHeight) {
      img.style.height = maxHeight + 'px';
      img.style.width = 'auto';
    } else {
      img.style.height = 'auto';
      img.style.width = 'auto';
    }
    pasteImageData = data;
  };
  img.src = data;
}

// Применить вставленное изображение
function applyPaste() {
  var input = document.getElementById('pasteInput').value.trim();
  if (!input) {
    alert('Please paste an image first');
    return;
  }
  
  // Если это не data URL, пробуем загрузить как URL
  if (!input.startsWith('data:image')) {
    // Пробуем как URL
    var img = new Image();
    img.crossOrigin = 'Anonymous';
    img.onload = function() {
      drawImageOnCanvas(img);
      hidePasteDialog();
      document.getElementById('status').textContent = 'Image pasted successfully!';
    };
    img.onerror = function() {
      alert('Cannot load image from URL. Please use base64 data or file.');
    };
    img.src = input;
    return;
  }
  
  // Это data URL
  var img = new Image();
  img.onload = function() {
    drawImageOnCanvas(img);
    hidePasteDialog();
    document.getElementById('status').textContent = 'Image pasted successfully!';
  };
  img.onerror = function() {
    alert('Invalid image data');
  };
  img.src = input;
}

function drawImageOnCanvas(img) {
  // Очищаем canvas
  ctx.fillStyle = '#ffffff';
  ctx.fillRect(0, 0, W, H);
  
  // Рисуем изображение с сохранением пропорций, заполняя весь canvas
  var imgW = img.naturalWidth || img.width;
  var imgH = img.naturalHeight || img.height;
  
  // Вычисляем масштаб для заполнения всего canvas
  var scaleX = W / imgW;
  var scaleY = H / imgH;
  var scale = Math.max(scaleX, scaleY);
  
  var drawW = imgW * scale;
  var drawH = imgH * scale;
  var drawX = (W - drawW) / 2;
  var drawY = (H - drawH) / 2;
  
  ctx.drawImage(img, drawX, drawY, drawW, drawH);
  
  // Сохраняем данные для возможного повторного использования
  pasteImageData = img.src;
}

// Обработка нажатия Enter в текстовом поле
document.addEventListener('keydown', function(e) {
  if (e.key === 'Enter' && document.getElementById('pasteArea').classList.contains('active')) {
    if (e.target === document.getElementById('pasteInput')) {
      applyPaste();
    }
  }
  if (e.key === 'Escape') {
    hidePasteDialog();
  }
});

// Клик вне диалога для закрытия
document.getElementById('pasteArea').addEventListener('click', function(e) {
  if (e.target === this) {
    hidePasteDialog();
  }
});

// Основная функция отправки
function send() {
  var st = document.getElementById('status');
  st.textContent = 'Converting...';
  var img = ctx.getImageData(0, 0, W, H).data;
  
  var buf = new Uint8Array(11248);
  
  // Кодируем черный цвет
  for(var i = 0; i < W * H; i++){
    var r = img[i*4];
    var g = img[i*4+1];
    var b = img[i*4+2];
    
    if(r < 128 && g < 128 && b < 128) {
      buf[i >> 3] |= (0x80 >> (i & 7));
    }
  }
  
  // Кодируем красный цвет
  var offset = 5624;
  for(var i = 0; i < W * H; i++){
    var r = img[i*4];
    var g = img[i*4+1];
    var b = img[i*4+2];
    
    if(r > g + 30 && r > b + 30 && r > 100) {
      buf[offset + (i >> 3)] |= (0x80 >> (i & 7));
    }
  }
  
  st.textContent = 'Sending 11KB...';
  var fd = new FormData();
  fd.append('img', new Blob([buf]), 'img.bin');
  fetch('/draw', {method:'POST', body:fd})
  .then(function(r){st.textContent = r.ok ? 'Refreshing display...' : 'Error '+r.status})
  .catch(function(e){st.textContent = 'Failed: '+e});
}

// Обработка перетаскивания изображений на canvas
c.addEventListener('dragover', function(e) {
  e.preventDefault();
  this.style.borderColor = '#2563eb';
});

c.addEventListener('dragleave', function(e) {
  e.preventDefault();
  this.style.borderColor = '#444';
});

c.addEventListener('drop', function(e) {
  e.preventDefault();
  this.style.borderColor = '#444';
  var files = e.dataTransfer.files;
  if (files.length > 0 && files[0].type.startsWith('image/')) {
    var reader = new FileReader();
    reader.onload = function(event) {
      var img = new Image();
      img.onload = function() {
        drawImageOnCanvas(img);
        document.getElementById('status').textContent = 'Image dropped successfully!';
      };
      img.src = event.target.result;
    };
    reader.readAsDataURL(files[0]);
  }
});

// Подсказка пользователю
console.log('E-Paper Draw loaded. You can paste images with Ctrl+V in the paste dialog.');
</script>
</body>
</html>
)rawliteral";

// Функция для установки пикселя
void SetPixelR(int x, int y, int color) {
    if (x < 0 || x >= EPD_WIDTH || y < 0 || y >= EPD_HEIGHT) return;
    
    int byte_index = (y * EPD_WIDTH + x) / 8;
    int bit_index = 7 - (x % 8);  // ВАЖНО: биты идут справа налево!
    
    if (color == 0) { // Черный или красный
        red_data[byte_index] &= ~(1 << bit_index);
    } else { // Белый
        red_data[byte_index] |= (1 << bit_index);
    }
}

void SetPixelB(int x, int y, int color) {
    if (x < 0 || x >= EPD_WIDTH || y < 0 || y >= EPD_HEIGHT) return;
    
    int byte_index = (y * EPD_WIDTH + x) / 8;
    int bit_index = 7 - (x % 8);  // ВАЖНО: биты идут справа налево!
    
    if (color == 0) { // Черный или красный
        black_data[byte_index] &= ~(1 << bit_index);
    } else { // Белый
        black_data[byte_index] |= (1 << bit_index);
    }
}

// Функция для рисования прямоугольника
void DrawRect(int x1, int y1, int x2, int y2, int color) {
    for (int y = y1; y <= y2; y++) {
        for (int x = x1; x <= x2; x++) {
            SetPixelB(x, y, color);
        }
    }
}

void DrawHLine(int y, int x_start, int x_end, int color) {
    for (int x = x_start; x <= x_end; x++) {
        SetPixelB(x, y, color);
    }
}

void DrawHLineR(int y, int x_start, int x_end, int color) {
    for (int x = x_start; x <= x_end; x++) {
        SetPixelR(x, y, color);
    }
}

void DrawVLine(int x, int y_start, int y_end, int color) {
    for (int y = y_start; y <= y_end; y++) {
        SetPixelB(x, y, color);
    }
}

void handleRoot() {
  server.send_P(200, "text/html", INDEX_HTML);
}

int bytesReceived = 0;

void handleDrawUpload() {
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    Serial.println("Upload started"); 
    bytesReceived = 0;
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    Serial.println("Uploading...");
    int copyLen = min((int)upload.currentSize, BMP_SIZE - bytesReceived);
    if (copyLen > 0) {
      memcpy(bitmap + bytesReceived, upload.buf, copyLen);
      bytesReceived += copyLen;
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    Serial.println("Upload finished");
    Serial.printf("Upload done: %d bytes\n", bytesReceived);
  }
}

void handleDrawPost() {
  if (bytesReceived == BMP_SIZE) {
    newDrawing = true;
    server.send(200, "text/plain", "OK");
    Serial.println("Drawing received");
  } else {
    server.send(400, "text/plain", "Bad size");
    Serial.printf("Got %d bytes, expected %d\n", bytesReceived, BMP_SIZE);
  }
}

void renderBitmap()
{
  Serial.println("Rendering to e-paper...");
  
  // Очищаем данные дисплея (все пиксели в белый)
  memset(black_data, 0xFF, sizeof(black_data));
  memset(red_data, 0xFF, sizeof(red_data));
  
  // Обработка черного цвета (первые 5624 байта)
  for (int y = 0; y < EPD_H; y++)
  {
    for (int x = 0; x < EPD_W; x++)
    {
      int pixel_idx = y * EPD_W + x;
      int byte_idx = pixel_idx >> 3;
      int bit_idx = 7 - (pixel_idx & 7);
      
      // Проверяем бит в черной части буфера
      if ((bitmap[byte_idx] >> bit_idx) & 1) {
        black_data[byte_idx] &= ~(1 << bit_idx); // Устанавливаем черный
      }
      // else оставляем белый (0xFF)
    }
  }
  
  
  // Обработка красного цвета (вторые 5624 байта)
  int offset_bytes = (BMP_SIZE / 2); // 5624 байта смещения
  for (int y = 0; y < EPD_H; y++)
  {
    for (int x = 0; x < EPD_W; x++)
    {
      int pixel_idx = y * EPD_W + x;
      int byte_idx = pixel_idx >> 3;
      int bit_idx = 7 - (pixel_idx & 7);
      
      if ((bitmap[offset_bytes + byte_idx] >> bit_idx) & 1) {
        red_data[byte_idx] &= ~(1 << bit_idx); // Устанавливаем красный
      }
    }
  }

  EPD_2IN66BSES_Init();
  EPD_2IN66BSES_Clear();
  EPD_2IN66B_ImgBR(black_data, red_data);
  EPD_2IN66BSES_Sleep();
  Serial.println("Done!");
}

//----

void setup()
{
  printf("EPD_2IN66BSES_test Demo\r\n");
  DEV_Module_Init();

  printf("e-Paper Init and Clear...\r\n");
  EPD_2IN66BSES_Init();

  bitmap = (uint8_t *)malloc(BMP_SIZE);
  if (!bitmap)
  {
    Serial.println("FATAL: malloc failed");
    while (1)
      delay(1000);
  }
  memset(bitmap, 0, BMP_SIZE);

  memset(black_data, 0xFF, sizeof(black_data));
  memset(red_data, 0xFF, sizeof(red_data));

  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\nIP: %s\n", WiFi.localIP().toString().c_str());

  server.on("/", HTTP_GET, handleRoot);
  server.on("/draw", HTTP_POST, handleDrawPost, handleDrawUpload);
  server.begin();
  Serial.println("Server ready");

  EPD_2IN66BSES_Sleep();
}

/* The main loop -------------------------------------------------------------*/
void loop()
{
   server.handleClient();
  if (newDrawing) {
    newDrawing = false;
    renderBitmap();
  }
}
