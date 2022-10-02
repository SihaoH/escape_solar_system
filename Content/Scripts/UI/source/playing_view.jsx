const _ = require('lodash')
const React = require('react')
const AD = require('animation-driver')
const ReactUMG = require('react-umg')
const Utils = require('../utils')
const EAnchors = require('../anchors')
const {F_Sans, T_Rect} = require('../style')

const PointBar = require('./point_bar')
const MessageListView = require('./message_listview')
const EngineToward = require('./engine_toward')
const CelestialIndicator = require('./celestial_indicator')

const T_Keycap = Texture2D.Load('/Game/UI/Icon/T_Keycap64x64')

class PlayingView extends React.Component {
    constructor(props) {
        super(props)
        this.helper = new PlayingViewHelper()

        this.state = {
            HP: { cur: 0, max: 1},
            MP: { cur: 0, max: 1},
            actionPrompt: null,
            locationInfo: "",
            enginInfo: { max: 0, upDown: 0, frontBack: 0 },
            basicInfo: []
        }

        this.timer = setInterval(() => {
            if (this.actionAnime) return

            let player = MainLevelScript.GetMainChar()
            if (!player) {
                this.setState({
                    HP: { cur: 0 }
                })
                return
            }

            if (player.IsDriving()) {
                player = MainLevelScript.GetSpaceship()
            }
            this.setState({
                HP: { cur: player.Body.CurrentHP, max: player.Body.MaximumHP },
                MP: { cur: player.Engine.CurrentEnergy, max: player.Engine.MaximumEnergy},
            })

            this.setState({enginInfo: { max: player.Engine.Power, upDown: player.Engine.UpForce, frontBack: player.Engine.ForwardForce }})

            let loc_info = player.GetLocationInfo()
            this.setState({locationInfo: `${loc_info.Planet} (${(loc_info.Loction.X/100).toFixed(0)}, ${(loc_info.Loction.Y/100).toFixed(0)}, ${(loc_info.Loction.Z/100).toFixed(0)})` })

            this.setState({basicInfo: [
                { icon: "T_Temperature32x32", text: `${Utils.tr("温度")}: ${Utils.num2Txt(player.Body.CurrentTemp)} ℃`, color: player.Body.CurrentTemp > player.Body.ShieldHeat ? Utils.color("#f00") : player.Body.CurrentTemp < player.Body.ShieldCold ? Utils.color("#00f") : Utils.color("#fff") },
                { icon: "T_Pressure32x32", text: `${Utils.tr("流体压力")}: ${Utils.num2Txt(player.Body.CurrentPress)} kPa`, color: player.Body.CurrentPress > player.Body.ShieldPress ? Utils.color("#f00") : Utils.color("#fff") },
                { icon: "T_Mass32x32", text: `${Utils.tr("质量")}: ${Utils.num2Txt(player.GetMass())} kg`, color: Utils.color("#fff") },
                { icon: "T_Gravity32x32", text: `${Utils.tr("重力")}: ${Utils.num2Txt(player.GetGravityAccel() / 100, 1)} m/s²`, color: Utils.color("#fff") },
                { icon: "T_Thrust32x32", text: `${Utils.tr("推力MAX")}: ${Utils.num2Txt(player.Engine.Power)} N`, color: Utils.color("#fff") },
            ]})
        }, 200)
        MainLevelScript.Instance().ActionAddedDelegate.Add((Key, Tag, Interval) => {
            this.setState({ actionPrompt: { key: Key, tag: Tag, interval: Interval } })
        })
        MainLevelScript.Instance().ActionRemovedDelegate.Add(() => {
            this.setState({ actionPrompt: null })
        })
        MainLevelScript.Instance().ActionPressedDelegate.Add(() => {
            const prompt = this.state.actionPrompt
            if (prompt) {
                if (prompt.interval > 0) {
                    const real_height = this.uActionBg.GetCachedGeometry().GetLocalSize().Y
                    this.actionAnime = AD()
                    this.actionAnime.apply(this.uActionBg, 
                        { duration: prompt.interval }, 
                        { RenderTranslation: t => { return {X: 0, Y: (1-t) * real_height} } }
                    ).then(_ => {
                        if (this.actionAnime) {
                            this.actionAnime = null
                            MainLevelScript.ActionDone()
                        }
                    })
                } else {
                    MainLevelScript.ActionDone()
                }
            }
        })
        MainLevelScript.Instance().ActionReleasedDelegate.Add(() => {
            this.uActionBg.SetRenderTranslation({ X: 0, Y: this.uActionBg.GetCachedGeometry().GetLocalSize().Y })
            if (this.actionAnime) {
                this.actionAnime.destroy()
                this.actionAnime = null
            }
        })
        MainLevelScript.Instance().MessagedDelegate.Add((Msg) => {
            this.msgListView.AppendMsg(Msg)
        })
        MainLevelScript.Instance().EnteredDelegate.Add(() => {
            this.msgListView.toggleReview()
        })

        this.charKeys = [
            { keys: this.helper.GetAxisKeys("MoveForward"), desc: this.helper.GetCharOperDesc("MoveForward") },
            { keys: this.helper.GetAxisKeys("MoveRight"), desc: this.helper.GetCharOperDesc("MoveRight") },
            { keys: this.helper.GetAxisKeys("MoveUp"), desc: this.helper.GetCharOperDesc("MoveUp") },
            { keys: this.helper.GetActionKeys("Jump"), desc: this.helper.GetCharOperDesc("Jump") },
            { keys: this.helper.GetAxisKeys("Turn"), desc: this.helper.GetCharOperDesc("Turn") },
            { keys: this.helper.GetAxisKeys("LookUp"), desc: this.helper.GetCharOperDesc("LookUp") },
            { keys: this.helper.GetActionKeys("Lock"), desc: this.helper.GetCharOperDesc("Lock") },
        ]
        this.shipKeys = [
            { keys: this.helper.GetAxisKeys("MoveForward"), desc: this.helper.GetShipOperDesc("MoveForward") },
            { keys: this.helper.GetAxisKeys("MoveRight"), desc: this.helper.GetShipOperDesc("MoveRight") },
            { keys: this.helper.GetAxisKeys("MoveUp"), desc: this.helper.GetShipOperDesc("MoveUp") },
            { keys: this.helper.GetActionKeys("Hold"), desc: this.helper.GetShipOperDesc("Hold") },
            { keys: this.helper.GetAxisKeys("Turn"), desc: this.helper.GetShipOperDesc("Turn") },
            { keys: this.helper.GetAxisKeys("LookUp"), desc: this.helper.GetShipOperDesc("LookUp") },
            { keys: this.helper.GetActionKeys("Lock"), desc: this.helper.GetShipOperDesc("Lock") },
            { keys: this.helper.GetActionKeys("Drive"), desc: this.helper.GetShipOperDesc("Drive") },
        ]
    }

    componentDidMount() {
    }

    componentWillUnmount() {
        clearInterval(this.timer)
    }

    render() {
        const on_ship = MainLevelScript.GetMainChar() && MainLevelScript.GetMainChar().IsDriving()
        return (
            <uCanvasPanel>
                /* 左上角的血条和蓝条 */
                <div Slot={{ LayoutData: { Offsets: Utils.ltrb(50, 50, 400, 50) } }} >
                    <PointBar
                        Slot={{Size: { SizeRule: ESlateSizeRule.Fill }}}
                        curVal={this.state.HP.cur}
                        maxVal={this.state.HP.max}
                    />
                    <uSpacer Size={{ Y: 4 }} />
                    <PointBar
                        Slot={{Size: { SizeRule: ESlateSizeRule.Fill }}}
                        curVal={this.state.MP.cur}
                        maxVal={this.state.MP.max}
                    />
                </div>
                
                /* 基本状态信息 */
                <div Slot={{ LayoutData: { Offsets: Utils.ltrb(50, 105, 400, 32) } }} >
                    {_.map(this.state.basicInfo, item => (
                        <uBorder
                            Slot={{
                                HorizontalAlignment: EHorizontalAlignment.HAlign_Left,
                                Padding: Utils.ltrb(0, 1)
                            }}
                            Padding={ Utils.ltrb(10, 1) }
                            Background={{
                                TintColor: { SpecifiedColor: Utils.rgba(0, 0, 0, 0.2) }
                            }}
                        >
                            <span>
                                <uImage
                                    Slot={{ Padding: Utils.ltrb(0, 0, 10, 0) }}
                                    ColorAndOpacity={ item.color }
                                    Brush={{
                                        ResourceObject: Utils.icon(item.icon),
                                        ImageSize: {X: 32, Y: 32}
                                    }}
                                />
                                <uTextBlock
                                    Slot={{ VerticalAlignment: EVerticalAlignment.VAlign_Center }}
                                    Font={{ FontObject: F_Sans, Size: 12 }}
                                    ColorAndOpacity={{
                                        SpecifiedColor: item.color
                                    }}
                                    Text={item.text}
                                />
                            </span>
                        </uBorder>
                    ))}
                </div>

                <EngineToward
                    Slot={{ LayoutData: { Offsets: Utils.ltrb(50, 280, 160, 160) } }}
                    Max={this.state.enginInfo.max}
                    UpDown={this.state.enginInfo.upDown}
                    FrontBack={this.state.enginInfo.frontBack}
                />

                /* 右上角的按键操作指示 */
                <div
                    Slot={{
                        LayoutData: {
                            Anchors: EAnchors.TopRight,
                            Alignment: { X: 1.0, Y: 0 },
                            Offsets: Utils.ltrb(-50, 50, 0, 0)
                        },
                        bAutoSize: true
                    }}
                >
                    {_.map(on_ship ? this.shipKeys : this.charKeys, obj => (
                        <span>
                            {_.map(obj.keys, key => {
                                const is_mouse = key.Key_IsButtonAxis() || key.Key_IsMouseButton()
                                return <uBorder
                                    HorizontalAlignment={ EHorizontalAlignment.HAlign_Center }
                                    VerticalAlignment={ EVerticalAlignment.VAlign_Center }
                                    Padding={ Utils.ltrb(20, 5) }
                                    /* 这里有个bug，本来应该是Background属性的，但没有对应的SetBackground方法；
                                     * 而React-umg就是用【Set+属性名】的方式刷新的，所以用Background属性就不能正常刷新了；
                                     * 但是它有SetBrush方法，效果就是设置Background背景，于是偷了个鸡用Brush属性赋值了。
                                    */
                                    Brush={{
                                        ResourceObject: is_mouse ? Texture2D.Load(`/Game/UI/Icon/T_${key.KeyName}64x64`) : T_Keycap,
                                        DrawAs: is_mouse ? ESlateBrushDrawType.Image : ESlateBrushDrawType.Box,
                                        ImageSize: {X: 64, Y: 64},
                                        Margin: Utils.ltrb(0.5)
                                    }}
                                >
                                    <uTextBlock
                                        Font={{
                                            FontObject: F_Sans,
                                            Size: 12
                                        }}
                                        ColorAndOpacity={{ SpecifiedColor: Utils.color("#333") }}
                                        Text={ is_mouse ? " " : key.Key_GetDisplayName() }
                                    />
                                </uBorder>
                            })}
                            <uTextBlock
                                Slot={{
                                    VerticalAlignment: EVerticalAlignment.VAlign_Center
                                }}
                                Font={{
                                    FontObject: F_Sans,
                                    Size: 14,
                                    OutlineSettings: {
                                        OutlineSize: 1,
                                        OutlineColor: Utils.rgba(0, 0, 0, 0.6)
                                    }
                                }}
                                Text={obj.desc}
                            />
                        </span>
                    ))}
                </div>

                /* 中间的准心 */
                <uTextBlock
                    Slot={{
                        LayoutData: {
                            Anchors: EAnchors.Center,
                            Alignment: { X: 0.5, Y: 0.5 },
                            Offsets: Utils.ltrb(0)
                        },
                        bAutoSize: true
                    }}
                    Font={{
                        FontObject: F_Sans,
                        Size: 8,
                        OutlineSettings: {
                            OutlineSize: 2,
                            OutlineColor: Utils.rgba(0, 0, 0, 0.3)
                        }
                    }}
                    ColorAndOpacity={{ SpecifiedColor: Utils.rgba(1, 1, 1, 0.9) }}
                    Text={"●"}
                />

                /* 中间的按键提示 */
                {this.state.actionPrompt &&
                <uCanvasPanel
                    Slot={{
                        LayoutData: {
                            Anchors: EAnchors.Center,
                            Alignment: { X: 0, Y: 0.5 },
                            Offsets: Utils.ltrb(50, 50, 0, 0)
                        },
                        bAutoSize: true
                    }}
                >
                    <uImage
                        Slot={{
                            LayoutData: {
                                Anchors: EAnchors.FillAll,
                                Offsets: Utils.ltrb(0)
                            },
                        }}
                        Brush={{
                            TintColor: { SpecifiedColor: Utils.rgba(0, 0, 0, 0.1) },
                        }}
                    />
                    <span
                        Slot={{
                            bAutoSize: true
                        }}
                    >
                        <uCanvasPanel
                            Clipping={ EWidgetClipping.ClipToBounds } 
                        >
                            <uImage
                                ref={ elem => {
                                    if (elem && this.uActionBg !== elem.ueobj) {
                                        this.uActionBg = elem.ueobj
                                    }
                                }}
                                Slot={{
                                    LayoutData: {
                                        Anchors: EAnchors.FillAll,
                                        Offsets: Utils.ltrb(0)
                                    },
                                }}
                                Brush={{
                                    TintColor: { SpecifiedColor: Utils.rgba(1, 1, 1, 0.6) },
                                }}
                                RenderTransform={{
                                    Translation: {X: 0, Y: 999}
                                }}
                            />
                            <uBorder
                                Slot={{
                                    LayoutData: { Offsets: Utils.ltrb(0) },
                                    bAutoSize: true
                                }}
                                Padding={ Utils.ltrb(13, 5) }
                                HorizontalAlignment={ EHorizontalAlignment.HAlign_Center }
                                VerticalAlignment={ EVerticalAlignment.VAlign_Center }
                                Brush={{
                                    ResourceObject: T_Rect,
                                    DrawAs: ESlateBrushDrawType.Border,
                                    TintColor: { SpecifiedColor: Utils.color("#FFF") },
                                    Margin: Utils.ltrb(2/64)
                                }}
                            >
                                <uTextBlock
                                    Font={{
                                        FontObject: F_Sans,
                                        TypefaceFontName: "Bold",
                                        Size: 14
                                    }}
                                    ColorAndOpacity={{ SpecifiedColor: Utils.color("#FFF") }}
                                    Text={this.state.actionPrompt.key.Key_GetDisplayName().replace("键", "")}
                                />
                            </uBorder>
                        </uCanvasPanel>
                        <uTextBlock
                            Slot={{
                                VerticalAlignment: EVerticalAlignment.VAlign_Center,
                                Padding: Utils.ltrb(10, 0)
                            }}
                            Font={{
                                FontObject: F_Sans,
                                TypefaceFontName: "Bold",
                                Size: 14,
                            }}
                            Justification={ ETextJustify.Center }
                            Text={this.state.actionPrompt.tag}
                        />
                    </span>
                </uCanvasPanel>}

                /* 信息框 */
                <MessageListView
                    ref={(elem) => {
                        if (elem) {
                            this.msgListView = elem
                        }
                    }}
                    Slot={{
                        LayoutData: {
                            Anchors: EAnchors.Right,
                            Alignment: { X: 1.0, Y: 0.5 },
                            Offsets: Utils.ltrb(-20, 0, 400, 300)
                        }
                    }}
                />

                /* 位置信息 */
                <uBorder
                    Slot={{
                        LayoutData: {
                            Anchors: EAnchors.BottomRight,
                            Alignment: { X: 1.0, Y: 1.0 },
                            Offsets: Utils.ltrb(-50, -50, 0, 0)
                        },
                        bAutoSize: true
                    }}
                    Padding={ Utils.ltrb(10, 10) }
                    Background={{
                        TintColor: { SpecifiedColor: Utils.rgba(0, 0, 0, 0.2) }
                    }}
                >
                    <span>
                        <uImage
                            Slot={{ Padding: Utils.ltrb(0, 0, 10, 0) }}
                            //ColorAndOpacity={ item.color }
                            Brush={{
                                ResourceObject: Utils.icon("T_Location32x32"),
                                ImageSize: {X: 32, Y: 32}
                            }}
                        />
                        <uTextBlock
                            Slot={{ VerticalAlignment: EVerticalAlignment.VAlign_Center }}
                            Font={{ FontObject: F_Sans, Size: 14 }}
                            //ColorAndOpacity={{ SpecifiedColor: item.color }}
                            Text={this.state.locationInfo}
                        />
                    </span>
                </uBorder>

                /* 星球信息 */
                <CelestialIndicator
                    Slot={{
                        LayoutData: {
                            Anchors: EAnchors.FillAll,
                        }
                    }}
                />
            </uCanvasPanel>
        )
    }
}

module.exports = ReactUMG.wrap(<PlayingView/>)