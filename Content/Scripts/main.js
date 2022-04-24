/// <reference path="typings/ue.d.ts">/>
Context.RunFile('aliases.js');
//Context.RunFile('polyfill/unrealengine.js');
Context.RunFile('polyfill/timers.js');

// let p = JavascriptProcess.Create("C:\\Program Files\\nodejs\\npm", "--version")
// p.Wait()
//require('devrequire')(__filename)
MainLevelScriptActor.GetInstance().MenuOpenedDelegate.Add(()=> {
    console.error("TODO: 从JS打开菜单界面")
})

let widget = require('UI/build/playing_view.js');
widget.AddToViewport();
