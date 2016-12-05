const electron = require('electron')
const fs = require('fs')
const path = require('path')
const { desktopCapturer, ipcRenderer: ipc, screen } = electron

function getMainSource(desktopCapturer, screen, done) {
  console.log(screen.getPrimaryDisplay().workAreaSize);

	const options = {types: ['screen'], thumbnailSize: screen.getPrimaryDisplay().workAreaSize}
	desktopCapturer.getSources(options, (err, sources) => {
		if (err) return console.log("cant capture screen:", err)
      console.log(sources);

		const isMainSource = source => source.name === 'Entire screen' || source.name === 'Screen 1'
		done(sources.filter(isMainSource)[0])
		})
}

function onCapture(evt, targetPath){
	//console.log("capture!")
	getMainSource(desktopCapturer, screen, source => {
		const png = source.thumbnail.toPng()
		const filePath = path.join(targetPath, 'screens', Math.floor((Math.random() * 10000) + 1) + 'printscreen.png')
		writeScreenShot(png, filePath)
	})
}

function writeScreenShot(png, filePath) {
	fs.writeFile(filePath, png, err => {
		if (err) return console.log("failed to write to screen:", err)
	})
}

ipc.on('capture', onCapture)
