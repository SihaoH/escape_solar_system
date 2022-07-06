const _ = require('lodash');
const React = require('react');
const AD = require('animation-driver');
const ReactUMG = require('react-umg');
const Utils = require('../utils');
const EAnchors = require('../anchors');
const { F_Sans, ButtonStyle } = require('../style');

let ThisWidget = null;

class DeathView extends React.Component {
    constructor(props) {
        super(props);
    }

    componentDidMount() {
        // 如果菜单打开，按流程自动关闭时，那边触发的设置会比这里要晚，所以这里加个延时
        setTimeout(() => Utils.setInputMode(true), 50);
    }

    componentWillUnmount() {
        Utils.setInputMode(false);
        ThisWidget = null;
    }

    render() {
        return React.createElement(
            'uCanvasPanel',
            null,
            React.createElement('uBackgroundBlur', {
                Slot: {
                    LayoutData: {
                        Anchors: EAnchors.FillAll,
                        Offsets: Utils.ltrb(0)
                    }
                },
                BlurStrength: 6
            }),
            React.createElement('uTextBlock', {
                Slot: {
                    LayoutData: {
                        Anchors: EAnchors.Top,
                        Alignment: { X: 0.5, Y: 0 },
                        Offsets: Utils.ltrb(0, 100, 0, 0)
                    },
                    bAutoSize: true
                },
                Font: {
                    FontObject: Font.Load('/Game/UI/Font/ZCOOL_Font'),
                    Size: 128
                },
                ColorAndOpacity: { SpecifiedColor: Utils.color("#F00") },
                Text: "DIED"
            }),
            React.createElement(
                'uButton',
                {
                    Slot: {
                        LayoutData: {
                            Anchors: EAnchors.Center,
                            Alignment: { X: 0.5, Y: 0 },
                            Offsets: Utils.ltrb(0, 0, 200, 60)
                        }
                    },
                    WidgetStyle: ButtonStyle,
                    OnReleased: () => {
                        MainLevelScriptActor.GetEarthBase().CreateMainChar();
                        ThisWidget.RemoveFromViewport();
                    }
                },
                React.createElement('uTextBlock', {
                    ColorAndOpacity: { ColorUseRule: ESlateColorStylingMode.UseColor_Foreground },
                    Text: "重生"
                })
            ),
            React.createElement(
                'uButton',
                {
                    Slot: {
                        LayoutData: {
                            Anchors: EAnchors.Center,
                            Alignment: { X: 0.5, Y: 0 },
                            Offsets: Utils.ltrb(0, 90, 200, 60)
                        }
                    },
                    WidgetStyle: ButtonStyle
                },
                React.createElement('uTextBlock', {
                    ColorAndOpacity: { ColorUseRule: ESlateColorStylingMode.UseColor_Foreground },
                    Text: "标题界面"
                })
            )
        );
    }
}

module.exports = function () {
    if (!ThisWidget) {
        ThisWidget = ReactUMG.wrap(React.createElement(DeathView, null));
    }
    return ThisWidget;
};