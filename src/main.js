'use strict';

const path = require('path');
const url = require('url');
const electron = require('electron');
const {app, globalShortcut, ipcMain, BrowserWindow} = electron;
const addon = require('./../build/Release/addon.node')

let win;

app.on('ready', () => {
  globalShortcut.register('CmdOrCtrl+Insert', () => {
    let displays = electron.screen.getCursorScreenPoint();

    console.log('X:' + displays.x + ' Y:' + displays.y);
    win.minimize();
    console.log(`screencall: ${addon.CaptureScreen()}`);
  });

  ipcMain.on('quit', function() {
      app.quit();
  });

  ipcMain.on('restart', function() {
      app.relaunch();
      app.quit();
  });

  win = new BrowserWindow({
    width: 800,
    height: 600,
    icon: path.join(__dirname, '..', 'public', 'images', 'icon.png')
  });
  win.loadURL('file://' + path.join(__dirname, '..', 'view', 'index.html'));
  win.on('closed', () => {
    win = null
  });
  win.once('ready-to-show', () => {
    win.show()
  });
});
