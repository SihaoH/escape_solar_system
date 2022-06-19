/// <reference path="typings/ue.d.ts">/>
Context.RunFile('aliases.js');
//Context.RunFile('polyfill/unrealengine.js');
Context.RunFile('polyfill/timers.js');

// let p = JavascriptProcess.Create("C:\\Program Files\\nodejs\\npm", "--version")
// p.Wait()
//require('devrequire')(__filename)
MainLevelScriptActor.Instance().MenuOpenedDelegate.Add(()=> {
    let menu_view = require('UI/build/menu_view.js')()
    if (!menu_view.IsInViewport()) {
        menu_view.AddToViewport()
    }
})

let playing_view = require('UI/build/playing_view.js');
playing_view.AddToViewport();

let prompt_view = require('UI/build/prompt_view.js');
prompt_view.AddToViewport(999);
