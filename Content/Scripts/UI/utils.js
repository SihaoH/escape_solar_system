function ltrb(l, t, r, b) {
    if (t === undefined) { t = l }
    if (r === undefined) { r = l }
    if (b === undefined) { b = t }
    return { Left: l, Top: t, Right: r, Bottom: b }
}

function rgba(r, g, b, a = 1.0) {
    return { R: r, G: g, B: b, A: a }
}

function color(str) {
    str = str.toLowerCase();

    let reg = /^#([0-9a-fA-f]{3}|[0-9a-fA-f]{6}|[0-9a-fA-f]{8})$/;
    if (reg.test(str)) {
        let argb = ""
        if (str.length === 4) {
            argb = "ff"
            for (var i = 0; i < 3; i ++) {
                argb += (str[i+1] + str[i+1])
            }
        } else if (str.length === 7) {
            argb = "ff" + str.slice(1)
        } else {
            argb = str.slice(1)
        }

        return {
            A: parseInt(argb.slice(0, 2), 16) / 0xff,
            R: parseInt(argb.slice(2, 4), 16) / 0xff,
            G: parseInt(argb.slice(4, 6), 16) / 0xff,
            B: parseInt(argb.slice(6, 8), 16) / 0xff
        }
    }

    return { R: 1, G: 1, B: 1, A: 1}
}

function icon(file) {
    return Texture2D.Load(`/Game/UI/Icon/${file}`)
}

function num2Txt(num, x = 0) {
    if (num > 10000000) {
        return Math.round((num / 1000000) * 100) / 100 + " M"
    } else if (num > 10000) {
        return Math.round((num / 1000) * 100) / 100 + " K"
    } else {
        let multiple = Math.pow(10, x)
        return Math.round(num * multiple) / multiple + ""
    }
}

function isDev() {
    return MainLibrary.IsDev()
}

function setInputMode(cursor) {
    let mainController = GameplayStatics.GetPlayerController(GWorld, 0)
    if (!mainController) return
    
    mainController.bShowMouseCursor = cursor
    if (cursor) {
        WidgetBlueprintLibrary.SetInputMode_UIOnlyEx(mainController)
    } else {
        WidgetBlueprintLibrary.SetInputMode_GameOnly(mainController)
    }
}

function tr(str) {
    return MainLibrary.Translate(str)
}

function debug(obj, tag) {
    console.error(tag ? `${tag}: ${JSON.stringify(obj, null, 2)}` : JSON.stringify(obj, null, 2))
}

module.exports = {
    ltrb: ltrb,
    rgba: rgba,
    color: color,
    icon: icon,
    num2Txt: num2Txt,
    isDev: isDev,
    setInputMode: setInputMode,
    tr: tr,
    debug: debug
}