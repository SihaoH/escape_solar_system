const _ = require('lodash')
const UClass = require('UClass')()
const Events = require('events')
const React = require('react')
const AD = require('animation-driver')
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
        process.nextTick(() => {
            if (this.GetOwningPlayerPawn()) {
                this.SetFocus()
            }
        })
        this.close = () => { ThisWidget.RemoveFromViewport() }
    }
    OnKeyDown(MyGeometry, InKeyEvent) {
        if (InKeyEvent.GetKey().KeyName === "Escape") {
            this.close()
            return EventReply.Handled()
        }
        return EventReply.Unhandled()
    }
    OnMouseButtonDown(MyGeometry, MouseEvent) {
        Event.emit("MouseClick")
    }
}
let MyOverlayer_C = UClass(global, MyOverlayer)
ReactUMG.Register('uMyOverlayer', MyOverlayer_C)

function demoFunc() {
    MainLibrary.SendMessage("获取demo大礼包，请回基地查收")
    MainLevelScript.GetEarthBase().Backpack.AddItem("飞船动力装置", 1)
    MainLevelScript.GetEarthBase().Backpack.AddItem("飞船电子系统", 1)
    MainLevelScript.GetEarthBase().Backpack.AddItem("飞船基础外壳", 1)
    MainLevelScript.GetEarthBase().Backpack.AddItem("飞船结构支架", 1)
    ThisWidget.RemoveFromViewport()
}

class TalkView extends React.Component {
    constructor(props) {
        super(props)

        this.state = { currentIndex: "0000" }

        this.NPC = MainLevelScript.GetMainChar().GetTalkableNPC()
    }

    componentDidMount() {
        // 如果菜单打开，按流程自动关闭时，那边触发的设置会比这里要晚
        process.nextTick(() => Utils.setInputMode(true))
        Event = new Events.EventEmitter()
        Event.on("MouseClick", () => {
            // 点击背景，只响应只有下一句或结束对话的情况，更多选项需要用户点击按钮选择
            const options = this.NPC.GetOptions(this.state.currentIndex).OutOptions
            const count = options.length
            if (count === 1) {  // 默认下一句
                const next_opt = options[0]
                if (next_opt.Row === "测试") {
                    demoFunc()
                } else if (!next_opt.text) {
                    this.setState({ currentIndex: next_opt.Row })
                }
            } else if (count < 1) {  // 没有下一句，结束对话
                ThisWidget.RemoveFromViewport()
            }
        })
    }

    componentWillUnmount() {
        Utils.setInputMode(false)
        ThisWidget = null
        Event = null
    }

    render() {
        const options = this.NPC.GetOptions(this.state.currentIndex).OutOptions
        const show_opt = options.length > 1 || (options.length === 1 && options[0].text)
        return (
            <uMyOverlayer>
            <uCanvasPanel>
                <div
                    Slot={{
                        LayoutData: {
                            Anchors: EAnchors.Right,
                            Alignment: { X: 1.0, Y: 0.5 },
                            Offsets: Utils.ltrb(-80, 0, 0, 0)
                        },
                        bAutoSize: true
                    }}
                >
                    {show_opt && _.map(options, (val) => (
                    <uButton
                        Slot={{ Padding: Utils.ltrb(0, 5) }}
                        WidgetStyle={{...ButtonStyle, Normal: {
                            DrawAs: ESlateBrushDrawType.Image,
                            TintColor: { SpecifiedColor: Utils.rgba(0, 0, 0, 0.3) },
                        },}}
                        IsFocusable={false}
                        OnPressed={() => {
                            this.setState({ currentIndex: val.Row })
                        }}
                    >
                        <span
                            Slot={{
                                HorizontalAlignment: EHorizontalAlignment.HAlign_Left
                            }}
                        >
                            <uImage
                                Slot={{
                                    Padding: Utils.ltrb(20, 0, 0, 0),
                                    VerticalAlignment: EVerticalAlignment.VAlign_Center
                                }}
                                Brush={{
                                    ResourceObject: Utils.icon("T_Talk64x64"),
                                    ImageSize: {X: 32, Y: 32},
                                    TintColor: { ColorUseRule: ESlateColorStylingMode.UseColor_Foreground }
                                }}
                            />
                            <uTextBlock
                                Slot={{ Padding: Utils.ltrb(20, 5) }}
                                Font={{
                                    FontObject: F_Sans,
                                    Size: 16,
                                }}
                                ColorAndOpacity={{
                                    ColorUseRule: ESlateColorStylingMode.UseColor_Foreground
                                }}
                                Text={ val.DispText }
                            />
                        </span>
                    </uButton>
                    ))}
                </div>

                <uBorder
                    Slot={{
                        LayoutData: {
                            Anchors: EAnchors.FillBottom,
                            Alignment: { X: 0.5, Y: 1.0 },
                            Offsets: Utils.ltrb(0, 0, 0, 300)
                        }
                    }}
                    Background={{
                        TintColor: { SpecifiedColor: Utils.rgba(0, 0, 0, 0.3) }
                    }}
                    Padding={ Utils.ltrb(0) }
                >
                    <uCanvasPanel>
                        <uTextBlock
                            Slot={{
                                LayoutData: {
                                    Anchors: EAnchors.Top,
                                    Alignment: { X: 0.5, Y: 0 },
                                    Offsets: Utils.ltrb(20)
                                },
                                bAutoSize: true
                            }}
                            Font={{
                                FontObject: F_Sans,
                                TypefaceFontName: "Bold",
                                Size: 18,
                            }}
                            Text={ this.NPC.Name }
                        />
                        <uTextBlock
                            Slot={{
                                LayoutData: {
                                    Anchors: EAnchors.FillAll,
                                    Offsets: Utils.ltrb(50, 80, 50, 50)
                                },
                            }}
                            AutoWrapText={true}
                            Font={{
                                FontObject: F_Sans,
                                Size: 16,
                            }}
                            Text={ this.NPC.GetText(this.state.currentIndex) }
                        />
                    </uCanvasPanel>
                </uBorder>
            </uCanvasPanel>
            </uMyOverlayer>
        )
    }
}

module.exports = function(need_wrap = true) {
    if (!ThisWidget && need_wrap) {
        ThisWidget = ReactUMG.wrap(<TalkView/>)
    }
    return ThisWidget
}