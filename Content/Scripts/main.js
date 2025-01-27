/// <reference path="typings/ue.d.ts">/>
Context.RunFile('aliases.js');
//Context.RunFile('polyfill/unrealengine.js');
Context.RunFile('polyfill/timers.js');

function JSX(file) {
    return `UI/build/${file}.js`
}

function removeViews(list) {
    for (let i in list) {
        let view = require(JSX(list[i]))(false)
        if (view && view.IsInViewport()) {
            view.RemoveFromViewport()
        }
    }
}

// 画面淡入效果
process.nextTick(() => GWorld.GetPlayerCameraManager(0).StartCameraFade(1.0, 0.0, 3.0))

// let p = JavascriptProcess.Create("C:\\Program Files\\nodejs\\npm", "--version")
// p.Wait()
//require('devrequire')(__filename)
MainLevelScript.Instance().PausedDelegate.Add(()=> {
    let pause_view = require(JSX("pause_view"))()
    pause_view.AddToViewport(998)
})

MainLevelScript.Instance().MenuOpenedDelegate.Add(()=> {
    let menu_view = require(JSX("menu_view"))()
    if (!menu_view.IsInViewport()) {
        menu_view.AddToViewport()
    }
})

MainLevelScript.Instance().DeathOpenedDelegate.Add(()=> {
    removeViews(["menu_view", "talk_view"])

    let death_view = require(JSX("death_view"))()
    death_view.AddToViewport(996)
})

MainLevelScript.Instance().TalkOpenedDelegate.Add(()=> {
    let talk_view = require(JSX("talk_view"))()
    talk_view.AddToViewport(996)
})

MainLevelScript.Instance().PlayingDelegate.Add(()=> {
    let playing_view = require(JSX("playing_view"))()
    if (!playing_view.IsInViewport()) {
        playing_view.AddToViewport()
    }
})

let cinematics_view = null
MainLevelScript.Instance().CinematicsDelegate.Add((File)=> {
    if (File) {
        cinematics_view = require(JSX(File))
        cinematics_view.AddToViewport()
    } else {
        cinematics_view.RemoveFromViewport()
    }
})

let prompt_view = require(JSX("prompt_view"))
prompt_view.AddToViewport(997)

// 存档
setInterval(() => {
    MainSaveGame.SaveAr()
}, 60*1000)
