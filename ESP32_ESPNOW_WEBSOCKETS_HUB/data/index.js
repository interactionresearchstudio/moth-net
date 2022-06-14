/**
 * ----------------------------------------------------------------------------
 * ESP32 Remote Control with WebSocket
 * ----------------------------------------------------------------------------
 * © 2020 Stéphane Calderoni
 * ----------------------------------------------------------------------------
 */

var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

// ----------------------------------------------------------------------------
// Initialization
// ----------------------------------------------------------------------------

window.addEventListener('load', onLoad);

function onLoad(event) {
    initWebSocket();
    initButton();
}

// ----------------------------------------------------------------------------
// WebSocket handling
// ----------------------------------------------------------------------------

function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage;
}

function onOpen(event) {
    console.log('Connection opened');
}

function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

function onMessage(event) {
    console.log(event.data);
    let data = JSON.parse(event.data);
    //document.getElementById('led').className = data.status;
    document.getElementById('clients').innerHTML = data[0];
   console.log(data);
}

// ----------------------------------------------------------------------------
// Button handling
// ----------------------------------------------------------------------------

function initButton() {
    document.getElementById('toggle').addEventListener('click', onToggle);
    document.getElementById('toggle2').addEventListener('click', onToggle2);
    document.getElementById('toggle3').addEventListener('click', onToggle3);
}

function onToggle(event) {
    websocket.send(JSON.stringify({'action':'toggle','test':99}));
}

function onToggle2(event) {
    websocket.send(JSON.stringify({'action':'toggle2','test':99}));
}

function onToggle3(event) {
    websocket.send(JSON.stringify({'action':'toggle3','test':99}));
}

