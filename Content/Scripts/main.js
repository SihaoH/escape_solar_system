/// <reference path="typings/ue.d.ts">/>
Context.RunFile('aliases.js');
//Context.RunFile('polyfill/unrealengine.js');
Context.RunFile('polyfill/timers.js');

function JSX(file) {
    return `UI/build/${file}.js`
}

// let p = JavascriptProcess.Create("C:\\Program Files\\nodejs\\npm", "--version")
// p.Wait()
//require('devrequire')(__filename)
MainLevelScriptActor.Instance().MenuOpenedDelegate.Add(()=> {
    let menu_view = require(JSX("menu_view"))()
    if (!menu_view.IsInViewport()) {
        menu_view.AddToViewport()
    }
})
MainLevelScriptActor.Instance().DeathOpenedDelegate.Add(()=> {
    let menu_view = require(JSX("menu_view"))()
    if (menu_view.IsInViewport()) {
        menu_view.RemoveFromViewport()
    }

    let death_view = require(JSX("death_view"))()
    death_view.AddToViewport()
})

let playing_view = require(JSX("playing_view"))
playing_view.AddToViewport()

let prompt_view = require(JSX("prompt_view"))
prompt_view.AddToViewport(999)
