const _ = require('lodash')
const UClass = require('uclass')()
const AD = require('animation-driver')
const React = require('react')
const ReactUMG = require('react-umg')
const Utils = require('../utils')
const EAnchors = require('../anchors')
const TaggedCard = require('tagged_card')
const {F_Sans} = require('../style')

let helper = null
let upgradeAnime = null

function info_row(k, v, u = "") {
    return {key: k, val: v, unit: u}
}

function tech_btn(p, is_ship = false) {
    return {name: helper.GetTechName(p), desc: helper.GetTechDesc(p), props: p, isShip: is_ship}
}

function engine_btn(p, ship_engine) {
    return {...tech_btn(p, true), shipEngine: ship_engine}
}

function energy_btn(p, ship_energy) {
    return {...tech_btn(p, true), shipEnergy: ship_energy}
}

function checkBase(btn_val) {
    const base = MainLevelScript.GetEarthBase()
    return btn_val.isShip ? (base.FindMainChar() && base.FindSpaceship()) : base.FindMainChar()
}

class SplitLine extends React.Component {
    constructor(props) {
        super(props)
    }

    render() {
        return (
            <span
                {...this.props}
                Slot={{ Padding: Utils.ltrb(0, 5) }}
            >
                <uTextBlock
                    Font={{
                        FontObject: F_Sans,
                        Size: 14,
                    }}
                    ColorAndOpacity={{ SpecifiedColor: Utils.color("#333") }}
                    Text={this.props.Text}
                />
                <uImage
                    Slot={{
                        Padding: Utils.ltrb(10, 0, 0, 0),
                        Size: { SizeRule: ESlateSizeRule.Fill, Value: 1.0 },
                        VerticalAlignment: EVerticalAlignment.VAlign_Center
                    }}
                    Brush={{ ImageSize: {X: 32, Y: 1} }}
                    ColorAndOpacity={Utils.color("#888")}
                />
            </span>
        )
    }
}

class TechButton extends React.Component {
    constructor(props) {
        super(props)
    }

    render() {
        const {btnVal} = this.props
        let isEnabled = true
        const in_base = checkBase(btnVal)
        if (btnVal.shipEngine !== undefined) {
            isEnabled = MainLevelScript.GetSpaceship().EngineType <= btnVal.shipEngine
        } else if (btnVal.shipEnergy !== undefined) {
            isEnabled = MainLevelScript.GetSpaceship().EngineType === btnVal.shipEnergy
        }
        if (this.btn) this.btn.SetIsEnabled(isEnabled)
        return (
            <uSizeBox
                {...this.props}
                WidthOverride={80}
                HeightOverride={80}
            >
            <uButton
                ref={elem => {
                    if (elem && !this.btn) {
                        this.btn = elem.ueobj
                        // ToolTipWidgetDelegate重新赋值，就会失效
                        // 而任意祖父元素或自身的state改变，都会触发更新
                        // 所以这里限制只在最开始赋值一次
                        this.btn.ToolTipWidgetDelegate = () => {
                            helper.SelectTech(btnVal.props)
                            const cur_lv = helper.GetCurLv(btnVal.props)
                            const max_lv = helper.GetMaxLv(btnVal.props)
                            const in_base = checkBase(btnVal)
                            let curLevelInfo = ""
                            let nextLevelInfo = ""
                            for (let prop of btnVal.props) {
                                curLevelInfo += `${helper.GetTechValueName(prop)}: ${Utils.num2Txt(helper.GetTechValueVal(prop), 4)}\n`
                            }
                            curLevelInfo = curLevelInfo.slice(0, curLevelInfo.length-1)
                            for (let prop of btnVal.props) {
                                nextLevelInfo += `${helper.GetTechValueName(prop)}: ${Utils.num2Txt(helper.GetTechValueVal(prop, true), 4)}\n`
                            }
                            nextLevelInfo = nextLevelInfo.slice(0, nextLevelInfo.length-1)
                            return ReactUMG.wrap(
                            <uBorder
                                Slot={{ Padding: Utils.ltrb(0) }}
                                Padding={Utils.ltrb(20)}
                                Background={{
                                    TintColor: { SpecifiedColor: Utils.rgba(1, 1, 1, 0.6) }
                                }}
                            >
                                <div>
                                    <uTextBlock
                                        Font={{
                                            FontObject: F_Sans,
                                            TypefaceFontName: "Bold",
                                            Size: 16,
                                        }}
                                        ColorAndOpacity={{
                                            SpecifiedColor: Utils.color("#111")
                                        }}
                                        Text={btnVal.name} 
                                    />
                                    <uTextBlock
                                        Font={{
                                            FontObject: F_Sans,
                                            TypefaceFontName: "Bold",
                                            Size: 12,
                                        }}
                                        ColorAndOpacity={{
                                            SpecifiedColor: Utils.color("#333")
                                        }}
                                        Text={`Lv.${cur_lv}`} 
                                    />
                                    <uImage
                                        Slot={{ Padding: Utils.ltrb(0, 5) }}
                                        Brush={{ ImageSize: {X: 32, Y: 2} }}
                                        ColorAndOpacity={Utils.rgba(0, 0, 0, 0.1)}
                                    />
                                    <uTextBlock
                                        Font={{
                                            FontObject: F_Sans,
                                            Size: 14,
                                        }}
                                        WrapTextAt={300}
                                        ColorAndOpacity={{
                                            SpecifiedColor: Utils.color("#222")
                                        }}
                                        Text={btnVal.desc} 
                                    />
                                    <SplitLine Text={ Utils.tr("当前") } />
                                    <uTextBlock
                                        Font={{
                                            FontObject: F_Sans,
                                            Size: 12,
                                        }}
                                        WrapTextAt={300}
                                        ColorAndOpacity={{
                                            SpecifiedColor: Utils.color("#222")
                                        }}
                                        Text={curLevelInfo} 
                                    />
                                    { cur_lv < max_lv &&
                                    <div>
                                        <SplitLine Text={ Utils.tr( Utils.tr("下一级") ) } />
                                        <uTextBlock
                                            Font={{
                                                FontObject: F_Sans,
                                                Size: 12,
                                            }}
                                            WrapTextAt={300}
                                            ColorAndOpacity={{
                                                SpecifiedColor: Utils.color("#222")
                                            }}
                                            Text={nextLevelInfo} 
                                        />
                                        <SplitLine Text={ Utils.tr("升级所需") } />
                                        <uTextBlock
                                            Font={{
                                                FontObject: F_Sans,
                                                TypefaceFontName: "Bold",
                                                Size: 12,
                                            }}
                                            WrapTextAt={300}
                                            ColorAndOpacity={{
                                                SpecifiedColor: Utils.color("#222")
                                            }}
                                            Text={`${Utils.tr("探索点")}: ${helper.DemandPoints}`} 
                                        />
                                        <div>
                                        {_.map(helper.DemandItems, (val) => (
                                            <span Slot={{ Size: { SizeRule: ESlateSizeRule.Fill} }} >
                                                <uTextBlock
                                                    Slot={{
                                                        Size: { SizeRule: ESlateSizeRule.Fill, Value: 0.5 },
                                                        VerticalAlignment: EVerticalAlignment.VAlign_Center
                                                    }}
                                                    Font={{
                                                        FontObject: F_Sans,
                                                        Size: 12,
                                                    }}
                                                    WrapTextAt={300}
                                                    ColorAndOpacity={{ SpecifiedColor: Utils.color("#222") }}
                                                    Text={val.Name} 
                                                />
                                                <uTextBlock
                                                    Slot={{ VerticalAlignment: EVerticalAlignment.VAlign_Center }}
                                                    Font={{
                                                        FontObject: F_Sans,
                                                        Size: 10,
                                                    }}
                                                    ColorAndOpacity={{ SpecifiedColor: Utils.color("#555") }}
                                                    Text={Utils.tr("库存")} 
                                                />
                                                <uTextBlock
                                                    Slot={{ VerticalAlignment: EVerticalAlignment.VAlign_Center }}
                                                    Font={{
                                                        FontObject: F_Sans,
                                                        Size: 10,
                                                    }}
                                                    ColorAndOpacity={{ SpecifiedColor: val.Has >= val.Need ? Utils.color("#5F5") : Utils.color("#F55") }}
                                                    Text={val.Has > -1 ? val.Has : '??'} 
                                                />
                                                <uTextBlock
                                                    Slot={{
                                                        Size: { SizeRule: ESlateSizeRule.Fill, Value: 0.2 },
                                                        HorizontalAlignment: EHorizontalAlignment.HAlign_Right,
                                                        VerticalAlignment: EVerticalAlignment.VAlign_Center
                                                    }}
                                                    Font={{
                                                        FontObject: F_Sans,
                                                        Size: 12,
                                                    }}
                                                    ColorAndOpacity={{ SpecifiedColor: Utils.color("#222") }}
                                                    Text={`x${val.Need}`} 
                                                />
                                            </span>
                                        ))}
                                        </div>
                                    </div>}
                                    <uTextBlock
                                        Slot={{ Padding: Utils.ltrb(0, 5, 0, 0) }}
                                        Font={{
                                            FontObject: F_Sans,
                                            Size: 12,
                                        }}
                                        WrapTextAt={300}
                                        ColorAndOpacity={{
                                            SpecifiedColor: Utils.color(in_base&&helper.CanUpgrade ? "#2D2" : "#D22")
                                        }}
                                        Text={in_base ? (helper.CanUpgrade ? Utils.tr("长按升级") : Utils.tr("无法升级") ) : Utils.tr("需在基地升级") } 
                                    />
                                </div>
                            </uBorder>
                        )}
                    }
                }}
                WidgetStyle={{
                    Normal: {
                        DrawAs: ESlateBrushDrawType.Border,
                        TintColor: { SpecifiedColor: Utils.color("#FFF") },
                        Margin: Utils.ltrb(2)
                    },
                    Hovered: {
                        DrawAs: ESlateBrushDrawType.Border,
                        TintColor: { SpecifiedColor: Utils.color("#FFF") },
                        Margin: Utils.ltrb(2)
                    },
                    Pressed: {
                        DrawAs: ESlateBrushDrawType.Border,
                        TintColor: { SpecifiedColor: Utils.color("#FFF") },
                        Margin: Utils.ltrb(2)
                    },
                    NormalPadding: Utils.ltrb(0),
                    PressedPadding: Utils.ltrb(0)
                }}
                IsFocusable={false}
                OnPressed={() => {
                    if (!in_base || !helper.CanUpgrade) return

                    upgradeAnime = AD()
                    this.btn.SetVisibility(ESlateVisibility.HitTestInvisible)
                    upgradeAnime.apply(this.uBtnBg, 
                        { duration: 0.5 }, 
                        { RenderTranslation: t => { return {X: 0, Y: (1-t) * 80} } }
                    ).then(_ => {
                        if (upgradeAnime) {
                            helper.UpgradeTech(btnVal.props)
                            this.uBtnBg.SetRenderTranslation({X: 0, Y: 80})
                            this.btn.SetVisibility(ESlateVisibility.Visible)
                            upgradeAnime = null

                            this.props.parent.forceUpdate()
                        }
                    })
                }}
                OnReleased={() => {
                    if (upgradeAnime) {
                        upgradeAnime.destroy()
                        upgradeAnime = null
                    }
                    this.btn.SetVisibility(ESlateVisibility.Visible)
                    this.uBtnBg.SetRenderTranslation({X: 0, Y: 80})
                }}
            >
                <uCanvasPanel
                    Slot={{
                        Padding: Utils.ltrb(0),
                        HorizontalAlignment: EHorizontalAlignment.HAlign_Fill,
                        VerticalAlignment: EVerticalAlignment.VAlign_Fill
                    }}
                    Clipping={ EWidgetClipping.ClipToBounds }
                >
                    <uImage
                        Slot={{
                            LayoutData: {
                                Anchors: EAnchors.FillAll,
                                Offsets: Utils.ltrb(2)
                            }
                        }}
                        Brush={{
                            ImageSize: {X: 80, Y: 80},
                            ResourceObject: Texture2D.Load(helper.GetTechIcon(btnVal.props)),
                        }}
                    />
                    <uImage
                        ref={elem => {
                            if (elem) {
                                this.uBtnBg = elem.ueobj
                            }
                        }}
                        Slot={{
                            LayoutData: {
                                Anchors: EAnchors.FillAll,
                                Offsets: Utils.ltrb(0)
                            }
                        }}
                        Brush={{ TintColor: { SpecifiedColor: Utils.rgba(1, 1, 1, 0.8) }, }}
                        RenderTransform={{ Translation: {X: 0, Y: 80} }}
                    />
                    <uTextBlock
                        Slot={{
                            LayoutData: {
                                Anchors: EAnchors.BottomRight,
                                Alignment: { X: 1, Y: 1 },
                            },
                            bAutoSize: true
                        }}
                        Font={{
                            FontObject: F_Sans,
                            TypefaceFontName: "Bold",
                            Size: 14,
                            OutlineSettings: {
                                OutlineSize: 2,
                                OutlineColor: Utils.rgba(0, 0, 0, 0.6)
                            }
                        }}
                        Text={ `${helper.GetCurLv(btnVal.props)}/${helper.GetMaxLv(btnVal.props)}` }
                    />
                </uCanvasPanel>
            </uButton>
            </uSizeBox>
        )
    }
}

class InfoCard extends React.Component {
    constructor(props) {
        super(props)
    }
    render() {
        return (
            <uSizeBox
                {...this.props}
                WidthOverride={380}
                HeightOverride={600}
            >
            <TaggedCard title={this.props.title} >
                {_.map(this.props.info, (val) => {
                    return Array.isArray(val) ? [
                        ..._.map(val, (c_val) => (
                            <div>
                                <span>
                                    <uTextBlock
                                        Slot={{
                                            HorizontalAlignment: EHorizontalAlignment.HAlign_Left,
                                            Padding: Utils.ltrb(20, 0, 0, 0)
                                        }}
                                        Font={{
                                            FontObject: F_Sans,
                                            Size: 12,
                                        }}
                                        Text={c_val.key}
                                    />
                                    <uTextBlock
                                        Slot={{
                                            HorizontalAlignment: EHorizontalAlignment.HAlign_Right,
                                            Size: { SizeRule: ESlateSizeRule.Fill }
                                        }}
                                        Font={{
                                            FontObject: F_Sans,
                                            Size: 12,
                                        }}
                                        Text={typeof(c_val.val) === "string" ? c_val.val : `${Utils.num2Txt(c_val.val, 2)} ${c_val.unit}`}
                                    />
                                </span>
                                <uImage
                                    Brush={{ ImageSize: {X: 32, Y: 1} }}
                                    ColorAndOpacity={Utils.rgba(1, 1, 1, 0.2)}
                                />
                            </div>)
                    )] : 
                    <div
                        Slot={{
                            Padding: Utils.ltrb(0, 10, 0, 0)
                        }}
                    >
                        <span>
                            <uTextBlock
                                Slot={{
                                    HorizontalAlignment: EHorizontalAlignment.HAlign_Left
                                }}
                                Font={{
                                    FontObject: F_Sans,
                                    Size: 16,
                                }}
                                Text={val.key}
                            />
                            <uTextBlock
                                Slot={{
                                    HorizontalAlignment: EHorizontalAlignment.HAlign_Right,
                                    Size: { SizeRule: ESlateSizeRule.Fill }
                                }}
                                Font={{
                                    FontObject: F_Sans,
                                    Size: 16,
                                }}
                                Text={typeof(val.val) === "string" ? val.val : `${Utils.num2Txt(val.val, 2)} ${val.unit}`}
                            />
                        </span>
                        <uImage
                            Slot={{ Padding: Utils.ltrb(0, 5) }}
                            Brush={{ ImageSize: {X: 32, Y: 2} }}
                            ColorAndOpacity={Utils.rgba(1, 1, 1, 0.2)}
                        />
                    </div>
                }
                )}
            </TaggedCard>
            </uSizeBox>
        )
    }
}

class TechView extends React.Component {
    constructor(props) {
        super(props)
    }
    render() {
        return (
            <div
                {...this.props}
            >
                {_.map(this.props.techList, (val) => (
                <span
                    Slot={{ Padding: Utils.ltrb(0, 10) }}
                >
                    <uSizeBox
                        Slot={{ VerticalAlignment: EVerticalAlignment.VAlign_Center }}
                        WidthOverride={100}
                    >
                        <uTextBlock
                            Font={{
                                FontObject: F_Sans,
                                TypefaceFontName: "Bold",
                                Size: 14,
                            }}
                            Justification={ETextJustify.Right}
                            text={val.tag}
                        />
                    </uSizeBox>
                    {_.map(val.btn, (b_val) => (
                        <TechButton
                            Slot={{
                                Padding: Utils.ltrb(20, 0, 0, 0)
                            }}
                            parent={this}
                            btnVal={b_val}
                        />
                    ))}
                </span>
                ))}
            </div>
        )
    }
}

class MenuTech extends React.Component {
    constructor(props) {
        super(props)

        helper = new MenuTechHelper()
        this.charTech = [
            {
                tag: Utils.tr("机身"),
                btn: [
                    tech_btn([ETech.CharHP, ETech.CharMass]),
                    tech_btn([ETech.CharBackpack]),
                ]
            },
            {
                tag: Utils.tr("护盾"),
                btn: [
                    tech_btn([ETech.CharShieldCold]),
                    tech_btn([ETech.CharShieldHeat]),
                    tech_btn([ETech.CharShieldPress]),
                ]
            },
            {
                tag: Utils.tr("引擎"),
                btn: [
                    tech_btn([ETech.CharEnginePower, ETech.CharEngineMass, ETech.CharEngineEPR, ETech.CharEngineEMR]),
                ]
            },
            {
                tag: Utils.tr("燃料仓"),
                btn: [
                    tech_btn([ETech.CharEnergy]),
                ]
            },
        ]
        this.shipTech = [
            {
                tag: Utils.tr("机身"),
                btn: [
                    tech_btn([ETech.ShipHP, ETech.ShipMass], true),
                    tech_btn([ETech.ShipBackpack], true),
                ]
            },
            {
                tag: Utils.tr("护盾"),
                btn: [
                    tech_btn([ETech.ShipShieldCold], true),
                    tech_btn([ETech.ShipShieldHeat], true),
                    tech_btn([ETech.ShipShieldPress], true),
                ]
            },
            {
                tag: Utils.tr("引擎"),
                btn: [
                    engine_btn([ETech.ShipEngine0Power, ETech.ShipEngine0Mass, ETech.ShipEngine0EPR, ETech.ShipEngine0EMR], 0),
                    engine_btn([ETech.ShipEngine1Power, ETech.ShipEngine1Mass, ETech.ShipEngine1EPR, ETech.ShipEngine1EMR], 1),
                    engine_btn([ETech.ShipEngine2Power, ETech.ShipEngine2Mass, ETech.ShipEngine2EPR, ETech.ShipEngine2EMR], 2),
                ]
            },
            {
                tag: Utils.tr("燃料仓"),
                btn: [
                    energy_btn([ETech.ShipEnergy0], 0),
                    energy_btn([ETech.ShipEnergy1], 1),
                    energy_btn([ETech.ShipEnergy2], 2),
                ]
            },
        ]
        this.state = {
            charInfo: [],
            shipInfo: [],
        }

        this.timer = setInterval(() => {
            this.updateInfo()
        }, 300)
    }

    componentWillUnmount() {
        clearInterval(this.timer)
        helper = null
    }

    updateInfo() {
        if (upgradeAnime) return

        const char = MainLevelScript.GetMainChar()
        const ship = MainLevelScript.GetSpaceship()
        if (char) {
            this.setState({
                charInfo: [
                    info_row(Utils.tr("HP"), `${Utils.num2Txt(char.Body.CurrentHP)}/${Utils.num2Txt(char.Body.MaximumHP)}`),
                    info_row(Utils.tr("质量"), char.GetMass(), "kg"),
                    [
                        info_row(Utils.tr("躯体"), char.Body.Mass, "kg"),
                        info_row(Utils.tr("背包"), char.Backpack.Mass, "kg"),
                        info_row(Utils.tr("引擎"), char.Engine.Mass, "kg"),
                        info_row(Utils.tr("燃料"), char.Engine.GetEnergyMass(), "kg"),
                    ],
                    info_row(Utils.tr("引擎类型"), Utils.tr("化学动力")),
                    info_row(Utils.tr("引擎推力"), char.Engine.Power, "N"),
                    info_row(Utils.tr("燃料能量"), `${Utils.num2Txt(char.Engine.CurrentEnergy)}/${Utils.num2Txt(char.Engine.MaximumEnergy)}`),
                    info_row(Utils.tr("耐冷"), char.Body.ShieldCold, "℃"),
                    info_row(Utils.tr("耐热"), char.Body.ShieldHeat, "℃"),
                    info_row(Utils.tr("耐压"), char.Body.ShieldPress, "kPa"),
                ]
            })
        }
        if (ship) {
            this.setState({
                shipInfo: [
                    info_row(Utils.tr("HP"), `${Utils.num2Txt(ship.Body.CurrentHP)}/${Utils.num2Txt(ship.Body.MaximumHP)}`),
                    info_row(Utils.tr("质量"), ship.GetMass(), "kg"),
                    [
                        info_row(Utils.tr("躯体"), ship.Body.Mass, "kg"),
                        info_row(Utils.tr("背包"), ship.Backpack.Mass, "kg"),
                        info_row(Utils.tr("引擎"), ship.Engine.Mass, "kg"),
                        info_row(Utils.tr("燃料"), ship.Engine.GetEnergyMass(), "kg"),
                    ],
                    info_row(Utils.tr("引擎类型"), [Utils.tr("化学动力"), Utils.tr("核裂变动力"), Utils.tr("核聚变动力")][ship.EngineType]),
                    info_row(Utils.tr("引擎推力"), ship.Engine.Power, "N"),
                    info_row(Utils.tr("燃料能量"), `${Utils.num2Txt(ship.Engine.CurrentEnergy)}/${Utils.num2Txt(ship.Engine.MaximumEnergy)}`),
                    info_row(Utils.tr("耐冷"), ship.Body.ShieldCold, "℃"),
                    info_row(Utils.tr("耐热"), ship.Body.ShieldHeat, "℃"),
                    info_row(Utils.tr("耐压"), ship.Body.ShieldPress, "kPa"),
                ]
            })
        }
    }

    render() {
        const has_ship = !!MainLevelScript.GetSpaceship()
        return (
            <uCanvasPanel>
                <span
                    Slot={{
                        LayoutData: {
                            Anchors: EAnchors.Center,
                            Alignment: { X: 0.5, Y: 0.5 },
                            Offsets: Utils.ltrb(0, 0, 0, 0)
                        },
                        bAutoSize: true
                    }}
                >
                    <InfoCard
                        title={Utils.tr("躯体")}
                        info={this.state.charInfo}
                    />
                    <TechView
                        Slot={{
                            Padding: Utils.ltrb(20, 70, 0, 0)
                        }}
                        techList={this.charTech}
                    />

                    {has_ship &&
                    <uImage
                        Slot={{
                            Padding: Utils.ltrb(60, 0)
                        }}
                        Brush={{ ImageSize: {X: 2, Y: 32} }}
                        ColorAndOpacity={Utils.rgba(1, 1, 1, 0.5)}
                    />}

                    {has_ship &&
                    <InfoCard
                        title={Utils.tr("飞船")}
                        info={this.state.shipInfo}
                    />}
                    {has_ship &&
                    <TechView
                        Slot={{
                            Padding: Utils.ltrb(20, 70, 0, 0)
                        }}
                        techList={this.shipTech}
                    />}
                </span>
            </uCanvasPanel>
        )
    }
}


module.exports = MenuTech