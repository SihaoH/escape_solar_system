/// <reference path="typings/ue.d.ts">/>
Context.RunFile('aliases.js')
Context.RunFile('polyfill/timers.js')

function JSX(file) {
    return `UI/build/${file}.js`
}

// 画面淡入效果
process.nextTick(() => GWorld.GetPlayerCameraManager(0).StartCameraFade(1.0, 0.0, 3.0))

let start_view = require(JSX("start_view"))
start_view.AddToViewport()
