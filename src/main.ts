'use strict';

const path = require('path');
const electron = require('electron');
const {app, globalShortcut, ipcMain, BrowserWindow} = electron;
const addon = require('./../build/Release/addon.node');

let win, screenCaptureMode = false;

app.on('ready', () => {
  globalShortcut.register('CmdOrCtrl+Insert', () => {
    if (screenCaptureMode) {
      return;
    }

    /**
    globalShortcut.register('Esc', () => {
      console.log('fdg');
    });
    */

    screenCaptureMode = true;
    //let displays = electron.screen.getCursorScreenPoint();
    //console.log('X:' + displays.x + ' Y:' + displays.y);
    win.minimize();
    console.log(`screencall: ${addon.CaptureScreen()}`);
    screenCaptureMode = false;

    //globalShortcut.unregister('Esc');
  });

  ipcMain.on('restart', function() {
    app.relaunch();
    app.quit();
  });
  ipcMain.on('window-minimize', function() {
    win.minimize();
  });
  ipcMain.on('window-maximize', function() {
    win.maximize();
  });
  ipcMain.on('quit', function() {
    app.quit();
  });

  win = new BrowserWindow({
    height: 500,
    width: 450,
    'minHeight': 300,
    'minWidth': 400,
    titleBarStyle : 'hidden-inset',
    frame: false,
    icon: path.join(__dirname, '..', 'public', 'images', 'logo.png')
  });
  win.setMenuBarVisibility(false);
  win.loadURL('file://' + path.join(__dirname, '..', 'view', 'index.html'));
  win.on('closed', () => {
    win = null
  });
  win.once('ready-to-show', () => {
    win.show()
  });
  win.openDevTools();
});
