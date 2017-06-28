'use strict';

const ipcRenderer = require('electron').ipcRenderer;

document.addEventListener('dragover', event => event.preventDefault());
document.addEventListener('drop', event => event.preventDefault());

class Navigation {

    static windowMinimize() {
        ipcRenderer.send('window-minimize');
    }

    static windowMaximize() {
        ipcRenderer.send('window-maximize');
    }

    static windowClose() {
        ipcRenderer.send('quit');
    }

    static restartApp() {
        ipcRenderer.send('restart');
    }
}