'use strict';

const ipcRenderer = require('electron').ipcRenderer;

function quitWindow() {
    ipcRenderer.send('quit');
}

function restartApp() {
    ipcRenderer.send('restart');
}
