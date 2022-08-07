const _ = require('lodash')
const UClass = require('UClass')()
const React = require('react')
const ReactUMG = require('react-umg')
const Utils = require('../utils')
const EAnchors = require('../anchors')
const {F_Sans, ButtonStyle} = require('../style')

let ThisWidget = null
let Event = null

class MyOverlayer extends JavascriptWidget {
    ctor() {
        // 这里要延时设置焦点，否则接收不到按键事件
        this.bIsFocusable = true
        this.Visibility = ESlateVisibility.Visible
        setTimeout(() => {
            if (this.GetOwningPlayerPawn()) {
                this.SetFocus()
            }
        }, 50);
        this.close = () => {
            ThisWidget.RemoveFromViewport()
        }
    }
    OnKeyDown(MyGeometry, InKeyEvent) {
        if (InKeyEvent.GetKey().KeyName === "Escape") {
            this.close()
            return EventReply.Handled()
        }
        return EventReply.Unhandled()
    }
}
let MyOverlayer_C = UClass(global, MyOverlayer)
ReactUMG.Register('uMyOverlayer', MyOverlayer_C)

class TalkView extends React.Component {
    constructor(props) {
        super(props)

        this.handleAction = (idx) => {
            if (idx === 0) {
                ThisWidget.RemoveFromViewport()
            } else if (idx === 1) {
                // TODO
            } else if (idx === 2) {
                GWorld.QuitGame();
            }
        }
    }

    componentDidMount() {
        // 如果菜单打开，按流程自动关闭时，那边触发的设置会比这里要晚，所以这里加个延时
        setTimeout(() => {
            Utils.setInputMode(true)
            GWorld.SetGlobalTimeDilation(1/200)
        }, 50)
    }

    componentWillUnmount() {
        GWorld.SetGlobalTimeDilation(1.0)
        Utils.setInputMode(false)
        ThisWidget = null
    }

    render() {
        return (
            <uMyOverlayer>
            <uCanvasPanel>
                <uBackgroundBlur
                    Slot={{
                        LayoutData: {
                            Anchors: EAnchors.FillAll,
                            Offsets: Utils.ltrb(0)
                        }
                    }}
                    BlurStrength={5}
                />

                <uTextBlock
                    Slot={{
                        LayoutData: {
                            Anchors: EAnchors.Center,
                            Alignment: { X: 0.5, Y: 1.0 },
                            Offsets: Utils.ltrb(0, -50, 0, 0)
                        },
                        bAutoSize: true
                    }}
                    Font={{
                        FontObject: F_Sans,
                        TypefaceFontName: "Bold",
                        Size: 80,
                        LetterSpacing: 100,
                        OutlineSettings: {
                            OutlineSize: 5,
                            OutlineColor: Utils.rgba(0, 0, 0, 0.6)
                        }
                    }}
                    ColorAndOpacity={{ SpecifiedColor: Utils.color("#CCF") }}
                    Text={ "游戏已暂停" }
                />
                <div
                    Slot={{
                        LayoutData: {
                            Anchors: EAnchors.Center,
                            Alignment: { X: 0.5, Y: 0 },
                            Offsets: Utils.ltrb(0, 50, 0, 0)
                        },
                        bAutoSize: true
                    }}
                >
                    {_.map(["继续游戏", "标题页面", "退出游戏"], (val, idx) => (
                    <uButton
                        Slot={{ Padding: Utils.ltrb(0, 5) }}
                        WidgetStyle={ButtonStyle}
                        IsFocusable={false}
                        OnPressed={() => {
                            this.handleAction(idx)
                        }}
                    >
                        <uTextBlock
                            Slot={{ Padding: Utils.ltrb(80, 20) }}
                            Font={{
                                FontObject: F_Sans,
                                TypefaceFontName: "Bold",
                                Size: 24,
                            }}
                            ColorAndOpacity={{
                                ColorUseRule: ESlateColorStylingMode.UseColor_Foreground
                            }}
                            Text={ val }
                        />
                    </uButton>
                    ))}
                </div>
            </uCanvasPanel>
            </uMyOverlayer>
        )
    }
}

module.exports = function() {
    if (!ThisWidget) {
        ThisWidget = ReactUMG.wrap(<TalkView/>)
    }
    return ThisWidget
}