const electron = require('electron')
const fs = require('fs')
const path = require('path')
const { desktopCapturer, ipcRenderer: ipc, screen } = electron

function getMainSource(desktopCapturer, screen, done) {
  //console.log(screen.getAllDisplays());
  //console.log(screen.getPrimaryDisplay());
  //console.log(screen.getCursorScreenPoint());

  var options = {types: ['screen'], thumbnailSize: screen.getPrimaryDisplay().size}
  desktopCapturer.getSources(options, (err, sources) => {
  	if (err)  {
      return console.log("cant capture screen:", err)
    }
    for (i = 0; i < sources.length; i++ ) {

      console.log(sources[i]);
    	done(sources[i], i);
    }
	})
}

function onCapture(evt, targetPath) {
	//console.log("capture!")
	getMainSource(desktopCapturer, screen, (source, i = 0) => {
		const png = source.thumbnail.toPng()
		const filePath = path.join(targetPath, 'screens', Math.floor((Math.random() * 10000) + 1) + '_' + i + '_printscreen.png')
		writeScreenShot(png, filePath)
	})
}

function writeScreenShot(png, filePath) {
	fs.writeFile(filePath, png, err => {
		if (err) {
      return console.log("failed to write to screen:", err)
    }
	})
}

ipc.on('capture', onCapture)
