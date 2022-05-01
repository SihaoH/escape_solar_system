/// <reference path="typings/ue.d.ts">/>
Context.RunFile('aliases.js');
//Context.RunFile('polyfill/unrealengine.js');
Context.RunFile('polyfill/timers.js');

// let p = JavascriptProcess.Create("C:\\Program Files\\nodejs\\npm", "--version")
// p.Wait()
//require('devrequire')(__filename)
MainLevelScriptActor.GetInstance().MenuOpenedDelegate.Add(()=> {
    let menu_view = require('UI/build/menu_view.js')()
    if (!menu_view.IsInViewport()) {
        menu_view.AddToViewport()
    }
})

let playing_view = require('UI/build/playing_view.js');
playing_view.AddToViewport();
