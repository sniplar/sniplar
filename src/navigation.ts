'use strict';

const ipcRenderer = require('electron').ipcRenderer;

function windowMinimize() {
    ipcRenderer.send('window-minimize');
}

function windowMaximize() {
    ipcRenderer.send('window-maximize');
}

function windowClose() {
    ipcRenderer.send('quit');
}

function restartApp() {
    ipcRenderer.send('restart');
}

document.addEventListener('dragover', event => event.preventDefault());
document.addEventListener('drop', event => event.preventDefault());