'use strict';

const path = require('path');
const url = require('url');
const menubar = require('menubar');
const electron = require('electron');
const {app, globalShortcut, ipcMain} = electron;

var opts = {
    dir: __dirname,
    icon: path.join(__dirname, '..', 'public', 'images', 'icon.png'),
    index: 'file://' + path.join(__dirname, '..', 'view', 'index.html'),
    tooltip: 'Sniplar',
    preloadWindow: true,
    alwaysOnTop: true,
    width: 300
};

app.on('ready', () => {
  globalShortcut.register('Esc', () => {
    let displays = electron.screen.getCursorScreenPoint();

    console.log('X:' + displays.x + ' Y:' + displays.y);
		mb.window.webContents.send('capture', app.getPath('desktop'))
  });
});

var mb = menubar(opts);
mb.on('ready', function ready () {
    mb.on('show', function show () {
        mb.window.webContents.send("show");
    });

    mb.on('after-hide', function show () {
        mb.window.webContents.send("after_hide");
    });

    ipcMain.on('quit', function() {
        mb.app.quit();
    });

    ipcMain.on('restart', function() {
        mb.app.relaunch();
        mb.app.quit();
    });
});

mb.on('after-create-window', function() {
    mb.window.openDevTools();
    mb.window.setResizable(false);

});
