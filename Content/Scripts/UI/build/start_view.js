const _ = require('lodash');
const React = require('react');
const ReactUMG = require('react-umg');
const Utils = require('../utils');
const EAnchors = require('../anchors');
const { F_Sans, ButtonStyle } = require('../style');

const ConfirmDialog = require('confirm_dialog');

let ThisWidget = null;

class StartView extends React.Component {
    constructor(props) {
        super(props);

        this.options = [Utils.tr("新游戏"), Utils.tr("设置"), Utils.tr("退出游戏")];
        if (MainSaveGame.HasAr()) {
            this.options.unshift(Utils.tr("继续"));
        }

        this.openMainLv = need_load => {
            MainSaveGame.SetNeedLoad(need_load);
            ThisWidget.RemoveFromViewport();
            // 第二参数直接使用string不行，要这样先加载关卡对象
            setTimeout(() => {
                GameplayStatics.OpenLevel(GWorld, World.Load("/Game/MainBP/Maps/SolarSystemMap"));
            }, 2000);
        };

        this.handleAction = idx => {
            if (idx === 0) {
                // 继续
                this.openMainLv(true);
            } else if (idx === 1) {
                // 新游戏
                if (MainSaveGame.HasAr()) {
                    ConfirmDialog.open(Utils.tr("提示"), Utils.tr("开始新游戏会覆盖已有存档"), () => {
                        this.openMainLv(false);
                    });
                } else {
                    this.openMainLv(false);
                }
            } else if (idx === 2) {// 设置
                // TODO
            } else if (idx === 3) {
                // 退出游戏
                ThisWidget.RemoveFromViewport();
                process.nextTick(() => GWorld.QuitGame());
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
        let CM = GWorld ? GWorld.GetPlayerCameraManager(0) : null;
        if (CM) {
            CM.StartCameraFade(0.0, 1.0, 2.0, Utils.rgba(0, 0, 0), false, true);
        }
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
                    LetterSpacing: 100,
                    OutlineSettings: {
                        OutlineSize: 4,
                        OutlineColor: Utils.rgba(0, 0, 0, 0.6)
                    }
                },
                ColorAndOpacity: { SpecifiedColor: Utils.color("#CCF") },
                Text: Utils.tr("逃离：太阳系")
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
                _.map(this.options, (val, idx) => React.createElement(
                    'uButton',
                    {
                        Slot: { Padding: Utils.ltrb(0, 5) },
                        WidgetStyle: ButtonStyle,
                        IsFocusable: false,
                        OnReleased: () => {
                            this.handleAction(idx + (this.options.length > 3 ? 0 : 1));
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