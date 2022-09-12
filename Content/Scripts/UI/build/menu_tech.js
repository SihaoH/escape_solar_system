var _extends = Object.assign || function (target) { for (var i = 1; i < arguments.length; i++) { var source = arguments[i]; for (var key in source) { if (Object.prototype.hasOwnProperty.call(source, key)) { target[key] = source[key]; } } } return target; };

const _ = require('lodash');
const UClass = require('uclass')();
const AD = require('animation-driver');
const React = require('react');
const ReactUMG = require('react-umg');
const Utils = require('../utils');
const EAnchors = require('../anchors');
const TaggedCard = require('tagged_card');
const { F_Sans } = require('../style');

let helper = null;
const DecoratorClass = Blueprint.Load('/Game/UI/BP_RichTextBlockImageDecorator').GeneratedClass;
let upgradeAnime = null;

function info_row(k, v, u = "") {
    return { key: k, val: v, unit: u };
}

function tech_btn(p, is_ship = false) {
    return { name: helper.GetTechName(p), desc: helper.GetTechDesc(p), props: p, isShip: is_ship };
}

function engine_btn(p, ship_engine) {
    return _extends({}, tech_btn(p, true), { shipEngine: ship_engine });
}

function energy_btn(p, ship_energy) {
    return _extends({}, tech_btn(p, true), { shipEnergy: ship_energy });
}

function checkBase(btn_val) {
    const base = MainLevelScript.GetEarthBase();
    return btn_val.isShip ? base.FindMainChar() && base.FindSpaceship() : base.FindMainChar();
}

class SplitLine extends React.Component {
    constructor(props) {
        super(props);
    }

    render() {
        return React.createElement(
            'span',
            _extends({}, this.props, {
                Slot: { Padding: Utils.ltrb(0, 5) }
            }),
            React.createElement('uTextBlock', {
                Font: {
                    FontObject: F_Sans,
                    TypefaceFontName: "Bold",
                    Size: 14
                },
                ColorAndOpacity: { SpecifiedColor: Utils.color("#333") },
                Text: this.props.Text
            }),
            React.createElement('uImage', {
                Slot: {
                    Padding: Utils.ltrb(10, 0, 0, 0),
                    Size: { SizeRule: ESlateSizeRule.Fill, Value: 1.0 },
                    VerticalAlignment: EVerticalAlignment.VAlign_Center
                },
                Brush: { ImageSize: { X: 32, Y: 1 } },
                ColorAndOpacity: Utils.color("#888")
            })
        );
    }
}

class TechButton extends React.Component {
    constructor(props) {
        super(props);
    }

    render() {
        const { btnVal } = this.props;
        let isEnabled = true;
        const in_base = checkBase(btnVal);
        if (btnVal.shipEngine !== undefined) {
            isEnabled = MainLevelScript.GetSpaceship().EngineType <= btnVal.shipEngine;
        } else if (btnVal.shipEnergy !== undefined) {
            isEnabled = MainLevelScript.GetSpaceship().EngineType === btnVal.shipEnergy;
        }
        if (this.btn) this.btn.SetIsEnabled(isEnabled);
        return React.createElement(
            'uSizeBox',
            _extends({}, this.props, {
                WidthOverride: 80,
                HeightOverride: 80
            }),
            React.createElement(
                'uButton',
                {
                    ref: elem => {
                        if (elem && !this.btn) {
                            this.btn = elem.ueobj;
                            // ToolTipWidgetDelegate重新赋值，就会失效
                            // 而任意祖父元素或自身的state改变，都会触发更新
                            // 所以这里限制只在最开始赋值一次
                            this.btn.ToolTipWidgetDelegate = () => {
                                helper.SelectTech(btnVal.props);
                                const cur_lv = helper.GetCurLv(btnVal.props);
                                const max_lv = helper.GetMaxLv(btnVal.props);
                                const in_base = checkBase(btnVal);
                                return ReactUMG.wrap(React.createElement(
                                    'uBorder',
                                    {
                                        Slot: { Padding: Utils.ltrb(0) },
                                        Padding: Utils.ltrb(20),
                                        Background: {
                                            TintColor: { SpecifiedColor: Utils.rgba(1, 1, 1, 0.6) }
                                        }
                                    },
                                    React.createElement(
                                        'div',
                                        null,
                                        React.createElement('uTextBlock', {
                                            Font: {
                                                FontObject: F_Sans,
                                                TypefaceFontName: "Bold",
                                                Size: 16
                                            },
                                            ColorAndOpacity: {
                                                SpecifiedColor: Utils.color("#111")
                                            },
                                            Text: btnVal.name
                                        }),
                                        React.createElement('uTextBlock', {
                                            Font: {
                                                FontObject: F_Sans,
                                                TypefaceFontName: "Bold",
                                                Size: 12
                                            },
                                            ColorAndOpacity: {
                                                SpecifiedColor: Utils.color("#333")
                                            },
                                            Text: `Lv.${cur_lv}`
                                        }),
                                        React.createElement('uImage', {
                                            Slot: { Padding: Utils.ltrb(0, 5) },
                                            Brush: { ImageSize: { X: 32, Y: 2 } },
                                            ColorAndOpacity: Utils.rgba(0, 0, 0, 0.1)
                                        }),
                                        React.createElement('uTextBlock', {
                                            Font: {
                                                FontObject: F_Sans,
                                                TypefaceFontName: "Bold",
                                                Size: 14
                                            },
                                            WrapTextAt: 300,
                                            ColorAndOpacity: {
                                                SpecifiedColor: Utils.color("#222")
                                            },
                                            Text: btnVal.desc
                                        }),
                                        React.createElement(SplitLine, { Text: "当前" }),
                                        React.createElement('uTextBlock', {
                                            Font: {
                                                FontObject: F_Sans,
                                                TypefaceFontName: "Bold",
                                                Size: 14
                                            },
                                            WrapTextAt: 300,
                                            ColorAndOpacity: {
                                                SpecifiedColor: Utils.color("#222")
                                            },
                                            Text: helper.CurTechInfo
                                        }),
                                        cur_lv < max_lv && React.createElement(
                                            'div',
                                            null,
                                            React.createElement(SplitLine, { Text: "下一级" }),
                                            React.createElement('uTextBlock', {
                                                Font: {
                                                    FontObject: F_Sans,
                                                    TypefaceFontName: "Bold",
                                                    Size: 14
                                                },
                                                WrapTextAt: 300,
                                                ColorAndOpacity: {
                                                    SpecifiedColor: Utils.color("#222")
                                                },
                                                Text: helper.NextTechInfo
                                            }),
                                            React.createElement(SplitLine, { Text: "升级所需" }),
                                            React.createElement('uTextBlock', {
                                                Font: {
                                                    FontObject: F_Sans,
                                                    TypefaceFontName: "Bold",
                                                    Size: 14
                                                },
                                                WrapTextAt: 300,
                                                ColorAndOpacity: {
                                                    SpecifiedColor: Utils.color("#222")
                                                },
                                                Text: helper.DemandPoints
                                            }),
                                            React.createElement('uRichTextBlock', {
                                                // 估计是react-umg实现逻辑的问题
                                                // 这里给TextStyleSet赋值会先调用URichTextBlock::SetTextStyleSet()
                                                // 导致URichTextBlock::UpdateStyleData()在之后执行就不能正常添加装饰类了（StyleInstance已创建，跳出了分支）
                                                // 所以这里强行延后了SetTextStyleSet的调用
                                                ref: elem => {
                                                    if (elem) process.nextTick(() => {
                                                        if (elem.ueobj) elem.ueobj.SetTextStyleSet(DataTable.Load('/Game/UI/RichTextTable'));
                                                    });
                                                },
                                                DecoratorClasses: [DecoratorClass],
                                                Text: helper.DemandItems
                                            })
                                        ),
                                        React.createElement('uTextBlock', {
                                            Slot: { Padding: Utils.ltrb(0, 5, 0, 0) },
                                            Font: {
                                                FontObject: F_Sans,
                                                Size: 12
                                            },
                                            WrapTextAt: 300,
                                            ColorAndOpacity: {
                                                SpecifiedColor: Utils.color(in_base && helper.CanUpgrade ? "#5F5" : "#F55")
                                            },
                                            Text: in_base ? helper.CanUpgrade ? "长按升级" : "无法升级" : "需在基地升级"
                                        })
                                    )
                                ));
                            };
                        }
                    },
                    WidgetStyle: {
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
                    },
                    IsFocusable: false,
                    OnPressed: () => {
                        if (!in_base || !helper.CanUpgrade) return;

                        upgradeAnime = AD();
                        this.btn.SetVisibility(ESlateVisibility.HitTestInvisible);
                        upgradeAnime.apply(this.uBtnBg, { duration: 0.5 }, { RenderTranslation: t => {
                                return { X: 0, Y: (1 - t) * 80 };
                            } }).then(_ => {
                            if (upgradeAnime) {
                                helper.UpgradETech(btnVal.props);
                                this.uBtnBg.SetRenderTranslation({ X: 0, Y: 80 });
                                this.btn.SetVisibility(ESlateVisibility.Visible);
                                upgradeAnime = null;

                                this.props.parent.forceUpdate();
                            }
                        });
                    },
                    OnReleased: () => {
                        if (upgradeAnime) {
                            upgradeAnime.destroy();
                            upgradeAnime = null;
                        }
                        this.btn.SetVisibility(ESlateVisibility.Visible);
                        this.uBtnBg.SetRenderTranslation({ X: 0, Y: 80 });
                    }
                },
                React.createElement(
                    'uCanvasPanel',
                    {
                        Slot: {
                            Padding: Utils.ltrb(0),
                            HorizontalAlignment: EHorizontalAlignment.HAlign_Fill,
                            VerticalAlignment: EVerticalAlignment.VAlign_Fill
                        },
                        Clipping: EWidgetClipping.ClipToBounds
                    },
                    React.createElement('uImage', {
                        ref: elem => {
                            if (elem) {
                                this.uBtnBg = elem.ueobj;
                            }
                        },
                        Slot: {
                            LayoutData: {
                                Anchors: EAnchors.FillAll,
                                Offsets: Utils.ltrb(0)
                            }
                        },
                        RenderTransform: { Translation: { X: 0, Y: 80 } }
                    }),
                    React.createElement('uImage', {
                        Slot: {
                            LayoutData: {
                                Anchors: EAnchors.Center,
                                Alignment: { X: 0.5, Y: 0.5 },
                                Offsets: Utils.ltrb(0, 0, 64, 64)
                            }
                        },
                        Brush: {
                            ResourceObject: Texture2D.Load("/Game/UI/Icon/T_TechTemp"),
                            ImageSize: { X: 64, Y: 64 }
                        }
                    }),
                    React.createElement('uTextBlock', {
                        Slot: {
                            LayoutData: {
                                Anchors: EAnchors.BottomRight,
                                Alignment: { X: 1, Y: 1 }
                            },
                            bAutoSize: true
                        },
                        Font: {
                            FontObject: F_Sans,
                            TypefaceFontName: "Bold",
                            Size: 14,
                            OutlineSettings: {
                                OutlineSize: 2,
                                OutlineColor: Utils.rgba(0, 0, 0, 0.6)
                            }
                        },
                        Text: `${helper.GetCurLv(btnVal.props)}/${helper.GetMaxLv(btnVal.props)}`
                    })
                )
            )
        );
    }
}

class InfoCard extends React.Component {
    constructor(props) {
        super(props);
    }
    render() {
        return React.createElement(
            'uSizeBox',
            _extends({}, this.props, {
                WidthOverride: 380,
                HeightOverride: 420
            }),
            React.createElement(
                TaggedCard,
                { title: this.props.title },
                _.map(this.props.info, val => {
                    return Array.isArray(val) ? [..._.map(val, c_val => React.createElement(
                        'span',
                        null,
                        React.createElement('uTextBlock', {
                            Slot: {
                                HorizontalAlignment: EHorizontalAlignment.HAlign_Left,
                                Padding: Utils.ltrb(20, 0, 0, 0)
                            },
                            Font: {
                                FontObject: F_Sans,
                                Size: 12
                            },
                            Text: c_val.key
                        }),
                        React.createElement('uTextBlock', {
                            Slot: {
                                HorizontalAlignment: EHorizontalAlignment.HAlign_Right,
                                Size: { SizeRule: ESlateSizeRule.Fill }
                            },
                            Font: {
                                FontObject: F_Sans,
                                Size: 12
                            },
                            Text: typeof c_val.val === "string" ? c_val.val : `${Utils.num2Txt(c_val.val, 2)} ${c_val.unit}`
                        })
                    ))] : React.createElement(
                        'span',
                        {
                            Slot: {
                                Padding: Utils.ltrb(0, 10, 0, 0)
                            }
                        },
                        React.createElement('uTextBlock', {
                            Slot: {
                                HorizontalAlignment: EHorizontalAlignment.HAlign_Left
                            },
                            Font: {
                                FontObject: F_Sans,
                                Size: 16
                            },
                            Text: val.key
                        }),
                        React.createElement('uTextBlock', {
                            Slot: {
                                HorizontalAlignment: EHorizontalAlignment.HAlign_Right,
                                Size: { SizeRule: ESlateSizeRule.Fill }
                            },
                            Font: {
                                FontObject: F_Sans,
                                Size: 16
                            },
                            Text: typeof val.val === "string" ? val.val : `${Utils.num2Txt(val.val, 2)} ${val.unit}`
                        })
                    );
                })
            )
        );
    }
}

class TechView extends React.Component {
    constructor(props) {
        super(props);
    }
    render() {
        return React.createElement(
            'div',
            this.props,
            _.map(this.props.techList, val => React.createElement(
                'span',
                {
                    Slot: { Padding: Utils.ltrb(0, 5) }
                },
                React.createElement(
                    'uSizeBox',
                    {
                        Slot: { VerticalAlignment: EVerticalAlignment.VAlign_Center },
                        WidthOverride: 100
                    },
                    React.createElement('uTextBlock', {
                        Font: {
                            FontObject: F_Sans,
                            TypefaceFontName: "Bold",
                            Size: 14
                        },
                        Justification: ETextJustify.Right,
                        text: val.tag
                    })
                ),
                _.map(val.btn, b_val => React.createElement(TechButton, {
                    Slot: {
                        Padding: Utils.ltrb(20, 0, 0, 0)
                    },
                    parent: this,
                    btnVal: b_val
                }))
            ))
        );
    }
}

class MenuTech extends React.Component {
    constructor(props) {
        super(props);

        helper = new MenuTechHelper();
        this.charTech = [{
            tag: "机身",
            btn: [tech_btn([ETech.CharHP, ETech.CharMass]), tech_btn([ETech.CharBackpack])]
        }, {
            tag: "护盾",
            btn: [tech_btn([ETech.CharShieldCold]), tech_btn([ETech.CharShieldHeat]), tech_btn([ETech.CharShieldPress])]
        }, {
            tag: "引擎",
            btn: [tech_btn([ETech.CharEnginePower, ETech.CharEngineMass, ETech.CharEngineEPR, ETech.CharEngineEMR])]
        }, {
            tag: "燃料仓",
            btn: [tech_btn([ETech.CharEnergy])]
        }];
        this.shipTech = [{
            tag: "机身",
            btn: [tech_btn([ETech.ShipHP, ETech.ShipMass], true), tech_btn([ETech.ShipBackpack], true)]
        }, {
            tag: "护盾",
            btn: [tech_btn([ETech.ShipShieldCold], true), tech_btn([ETech.ShipShieldHeat], true), tech_btn([ETech.ShipShieldPress], true)]
        }, {
            tag: "引擎",
            btn: [engine_btn([ETech.ShipEngine0Power, ETech.ShipEngine0Mass, ETech.ShipEngine0EPR, ETech.ShipEngine0EMR], 0), engine_btn([ETech.ShipEngine1Power, ETech.ShipEngine1Mass, ETech.ShipEngine1EPR, ETech.ShipEngine1EMR], 1), engine_btn([ETech.ShipEngine2Power, ETech.ShipEngine2Mass, ETech.ShipEngine2EPR, ETech.ShipEngine2EMR], 2)]
        }, {
            tag: "燃料仓",
            btn: [energy_btn([ETech.ShipEnergy0], 0), energy_btn([ETech.ShipEnergy1], 1), energy_btn([ETech.ShipEnergy2], 2)]
        }];
        this.state = {
            charInfo: [],
            shipInfo: []
        };

        this.timer = setInterval(() => {
            this.updateInfo();
        }, 300);
    }

    componentWillUnmount() {
        clearInterval(this.timer);
        helper = null;
    }

    updateInfo() {
        if (upgradeAnime) return;

        const char = MainLevelScript.GetMainChar();
        const ship = MainLevelScript.GetSpaceship();
        if (char) {
            this.setState({
                charInfo: [info_row("HP", `${Utils.num2Txt(char.Body.CurrentHP)}/${Utils.num2Txt(char.Body.MaximumHP)}`), info_row("质量", char.GetMass(), "kg"), [info_row("躯体", char.Body.Mass, "kg"), info_row("背包", char.Backpack.Mass, "kg"), info_row("引擎", char.Engine.Mass, "kg"), info_row("燃料", char.Engine.GetEnergyMass(), "kg")], info_row("引擎类型", "化学动力"), info_row("引擎推力", char.Engine.Power, "N"), info_row("燃料能量", `${Utils.num2Txt(char.Engine.CurrentEnergy)}/${Utils.num2Txt(char.Engine.MaximumEnergy)}`), info_row("耐冷", char.Body.ShieldCold, "℃"), info_row("耐热", char.Body.ShieldHeat, "℃"), info_row("耐压", char.Body.ShieldPress, "kPa")]
            });
        }
        if (ship) {
            this.setState({
                shipInfo: [info_row("HP", `${Utils.num2Txt(ship.Body.CurrentHP)}/${Utils.num2Txt(ship.Body.MaximumHP)}`), info_row("质量", ship.GetMass(), "kg"), [info_row("躯体", ship.Body.Mass, "kg"), info_row("背包", ship.Backpack.Mass, "kg"), info_row("引擎", ship.Engine.Mass, "kg"), info_row("燃料", ship.Engine.GetEnergyMass(), "kg")], info_row("引擎类型", ["化学动力", "核裂变动力", "核聚变动力"][ship.EngineType]), info_row("引擎推力", ship.Engine.Power, "N"), info_row("燃料能量", `${Utils.num2Txt(ship.Engine.CurrentEnergy)}/${Utils.num2Txt(ship.Engine.MaximumEnergy)}`), info_row("耐冷", ship.Body.ShieldCold, "℃"), info_row("耐热", ship.Body.ShieldHeat, "℃"), info_row("耐压", ship.Body.ShieldPress, "kPa")]
            });
        }
    }

    render() {
        const has_ship = !!MainLevelScript.GetSpaceship();
        return React.createElement(
            'uCanvasPanel',
            null,
            React.createElement(
                'span',
                {
                    Slot: {
                        LayoutData: {
                            Anchors: EAnchors.Center,
                            Alignment: { X: 0.5, Y: 0.5 },
                            Offsets: Utils.ltrb(0, -100, 0, 0)
                        },
                        bAutoSize: true
                    }
                },
                React.createElement(InfoCard, {
                    title: "躯体",
                    info: this.state.charInfo
                }),
                React.createElement(TechView, {
                    Slot: {
                        Padding: Utils.ltrb(20, 70, 120, 0)
                    },
                    techList: this.charTech
                }),
                has_ship && React.createElement(InfoCard, {
                    title: "飞船",
                    info: this.state.shipInfo
                }),
                has_ship && React.createElement(TechView, {
                    Slot: {
                        Padding: Utils.ltrb(20, 70, 0, 0)
                    },
                    techList: this.shipTech
                })
            )
        );
    }
}

module.exports = MenuTech;