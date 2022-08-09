const _ = require('lodash');
const UClass = require('UClass')();
const React = require('react');
const ReactUMG = require('react-umg');
const Utils = require('../utils');
const EAnchors = require('../anchors');
const { F_Sans, ButtonStyle } = require('../style');

let ThisWidget = null;

class StartView extends React.Component {
    constructor(props) {
        super(props);

        this.handleAction = idx => {
            if (idx === 0) {
                // TODO
            } else if (idx === 1) {
                ThisWidget.RemoveFromViewport();
                // 第二参数直接使用string不行，要这样先加载关卡对象
                setTimeout(() => GameplayStatics.OpenLevel(GWorld, World.Load("/Game/MainBP/Maps/SolarSystemMap")), 2000);
            } else if (idx === 2) {
                // TODO
            } else if (idx === 3) {
                GWorld.QuitGame();
            }
        };
    }

    componentDidMount() {
        process.nextTick(() => {
            // 画面淡入效果
            GWorld.GetPlayerCameraManager(0).StartCameraFade(1.0, 0.0, 3.0);
            Utils.setInputMode(true);
        });
    }

    componentWillUnmount() {
        ThisWidget = null;
        Utils.setInputMode(false);
        GWorld.GetPlayerCameraManager(0).StartCameraFade(0.0, 1.0, 2.0, Utils.rgba(0, 0, 0), false, true);
    }

    render() {

        return React.createElement(
            'uCanvasPanel',
            null,
            React.createElement('uTextBlock', {
                Slot: {
                    LayoutData: {
                        Anchors: EAnchors.Left,
                        Alignment: { X: 0, Y: 1.0 },
                        Offsets: Utils.ltrb(100, -50, 0, 0)
                    },
                    bAutoSize: true
                },
                Font: {
                    FontObject: F_Sans,
                    TypefaceFontName: "Bold",
                    Size: 96,
                    LetterSpacing: 100
                },
                ColorAndOpacity: { SpecifiedColor: Utils.color("#CCF") },
                Text: "逃离：太阳系"
            }),
            React.createElement(
                'div',
                {
                    Slot: {
                        LayoutData: {
                            Anchors: EAnchors.Left,
                            Alignment: { X: 0, Y: 0 },
                            Offsets: Utils.ltrb(100, 50, 0, 0)
                        },
                        bAutoSize: true
                    }
                },
                _.map(["继续", "新游戏", "设置", "退出游戏"], (val, idx) => React.createElement(
                    'uButton',
                    {
                        Slot: { Padding: Utils.ltrb(0, 5) },
                        WidgetStyle: ButtonStyle,
                        IsFocusable: false,
                        OnReleased: () => {
                            this.handleAction(idx);
                        }
                    },
                    React.createElement('uTextBlock', {
                        Slot: { Padding: Utils.ltrb(80, 20) },
                        Font: {
                            FontObject: F_Sans,
                            TypefaceFontName: "Bold",
                            Size: 24
                        },
                        ColorAndOpacity: {
                            ColorUseRule: ESlateColorStylingMode.UseColor_Foreground
                        },
                        Text: val
                    })
                ))
            )
        );
    }
}

ThisWidget = ReactUMG.wrap(React.createElement(StartView, null));
module.exports = ThisWidget;