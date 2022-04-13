/// <reference path="typings/ue.d.ts">/>
Context.RunFile('aliases.js');
//Context.RunFile('polyfill/unrealengine.js');
Context.RunFile('polyfill/timers.js');

let widget = require('UI/build/playing_view.js');
widget.AddToViewport();
