# Unreal.js插件指定的脚本目录

## 依赖模块
此项目用到react-umg、babel等模块，这些在`package.json`里已经指定

## 关键步骤
- 为了使用npm命令，需要先安装Node.js
- 在当前路径中执行`npm install`，此命令会下载安装`package.json`中指定的模块
- 虽然有jsx文件，但插件(v8引擎)运行时执行的还是js，所以运行前需要先执行`npm run build`，此命令会将jsx编译为js
- 入口脚本为`main.js`，加载过程：MainLevelScriptActor.cpp -> NewObject(JavascriptComponet) -> RegisterComponent -> main.js

## 补充说明
要想使用Unreal.js插件所提供的js功能，游戏开始运行后至少要创建一个V8引擎的实例，再通过v8去执行js脚本。目前本人所知的只有一种方法，创建一个Actor，该Actor包含一个`JavascriptComponet`，同时这个Compoent指定某个js文件。

作为主入口的js脚本，理所当然就应该放在关卡脚本中，因为关卡脚本也是Actor，而且任何关卡都必然会有一个。不过由于关卡脚本不支持在编辑器中直接添加Component，所以只能在蓝图或C++中通过代码动态添加。

如果想要在C++中使用插件的类，需要在`[project_name].Build.cs`中添加模块名，而`JavascriptComponet`属于`V8`模块。