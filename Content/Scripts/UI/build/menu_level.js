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

function level_btn(p) {
    return { name: helper.GetLevelName(p), desc: helper.GetLevelDesc(p), props: p };
}

function engine_btn(p, ship_engine) {
    return _extends({}, level_btn(p), { shipEngine: ship_engine });
}

function energy_btn(p, ship_energy) {
    return _extends({}, level_btn(p), { shipEnergy: ship_energy });
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

class LevelButton extends React.Component {
    constructor(props) {
        super(props);
    }

    render() {
        const { btnVal } = this.props;
        let isEnabled = true;
        const in_base = !!MainLevelScriptActor.GetEarthBase();
        if (btnVal.shipEngine !== undefined) {
            isEnabled = MainLevelScriptActor.GetSpaceship().EngineType <= btnVal.shipEngine;
        } else if (btnVal.shipEnergy !== undefined) {
            isEnabled = MainLevelScriptActor.GetSpaceship().EngineType === btnVal.shipEnergy;
        }
        this.btn && this.btn.SetIsEnabled(isEnabled);
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
                                helper.SelectLevel(btnVal.props);
                                const cur_lv = helper.GetCurVal(btnVal.props);
                                const max_lv = helper.GetMaxVal(btnVal.props);
                                const in_base = !!MainLevelScriptActor.GetEarthBase();
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
                                            Text: helper.CurLevelInfo
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
                                                Text: helper.NextLevelInfo
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
                                helper.UpgradeLevel(btnVal.props);
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
                            ResourceObject: Texture2D.Load("/Game/UI/Icon/T_LevelTemp"),
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
                        Text: `${helper.GetCurVal(btnVal.props)}/${helper.GetMaxVal(btnVal.props)}`
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

class LevelView extends React.Component {
    constructor(props) {
        super(props);
    }
    render() {
        return React.createElement(
            'div',
            this.props,
            _.map(this.props.levelList, val => React.createElement(
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
                _.map(val.btn, b_val => React.createElement(LevelButton, {
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

class MenuLevel extends React.Component {
    constructor(props) {
        super(props);

        helper = new MenuLevelHelper();
        this.charLevel = [{
            tag: "机身",
            btn: [level_btn([ELevel.CharHP, ELevel.CharMass]), level_btn([ELevel.CharBackpack])]
        }, {
            tag: "护盾",
            btn: [level_btn([ELevel.CharShieldCold]), level_btn([ELevel.CharShieldHeat]), level_btn([ELevel.CharShieldPress])]
        }, {
            tag: "引擎",
            btn: [level_btn([ELevel.CharEnginePower, ELevel.CharEngineMass, ELevel.CharEngineEPR, ELevel.CharEngineEMR])]
        }, {
            tag: "燃料仓",
            btn: [level_btn([ELevel.CharEnergy])]
        }];
        this.shipLevel = [{
            tag: "机身",
            btn: [level_btn([ELevel.ShipHP, ELevel.ShipMass]), level_btn([ELevel.ShipBackpack])]
        }, {
            tag: "护盾",
            btn: [level_btn([ELevel.ShipShieldCold]), level_btn([ELevel.ShipShieldHeat]), level_btn([ELevel.ShipShieldPress])]
        }, {
            tag: "引擎",
            btn: [engine_btn([ELevel.ShipEngine0Power, ELevel.ShipEngine0Mass, ELevel.ShipEngine0EPR, ELevel.ShipEngine0EMR], 0), engine_btn([ELevel.ShipEngine1Power, ELevel.ShipEngine1Mass, ELevel.ShipEngine1EPR, ELevel.ShipEngine1EMR], 1), engine_btn([ELevel.ShipEngine2Power, ELevel.ShipEngine2Mass, ELevel.ShipEngine2EPR, ELevel.ShipEngine2EMR], 2)]
        }, {
            tag: "燃料仓",
            btn: [energy_btn([ELevel.ShipEnergy0], 0), energy_btn([ELevel.ShipEnergy1], 1), energy_btn([ELevel.ShipEnergy2], 2)]
        }];
        this.state = {
            charInfo: [],
            shipInfo: []
        };

        setInterval(() => {
            this.updateInfo();
        }, 300);
    }

    componentWillUnmount() {
        helper = null;
    }

    updateInfo() {
        if (upgradeAnime) return;

        const char = MainLevelScriptActor.GetMainChar();
        const ship = MainLevelScriptActor.GetSpaceship();
        this.setState({
            charInfo: [info_row("HP", `${Utils.num2Txt(char.Body.CurrentHP)}/${char.Body.MaximumHP}`), info_row("质量", char.GetMass(), "kg"), [info_row("躯体", char.Body.Mass, "kg"), info_row("背包", char.Backpack.Mass, "kg"), info_row("引擎", char.Engine.Mass, "kg"), info_row("燃料", char.Engine.GetEnergyMass(), "kg")], info_row("引擎类型", "化学动力"), info_row("引擎推力", char.Engine.Power, "N"), info_row("燃料能量", `${Utils.num2Txt(char.Engine.CurrentEnergy)}/${char.Engine.MaximumEnergy}`), info_row("耐冷", char.Body.ShieldCold, "℃"), info_row("耐热", char.Body.ShieldHeat, "℃"), info_row("耐压", char.Body.ShieldPress, "kPa")]
        });
        if (ship) {
            this.setState({
                shipInfo: [info_row("HP", `${Utils.num2Txt(ship.Body.CurrentHP)}/${ship.Body.MaximumHP}`), info_row("质量", ship.GetMass(), "kg"), [info_row("躯体", ship.Body.Mass, "kg"), info_row("背包", ship.Backpack.Mass, "kg"), info_row("引擎", ship.Engine.Mass, "kg"), info_row("燃料", ship.Engine.GetEnergyMass(), "kg")], info_row("引擎类型", ["化学动力", "核裂变动力", "核聚变动力"][ship.EngineType]), info_row("引擎推力", ship.Engine.Power, "N"), info_row("燃料能量", `${ship.Engine.CurrentEnergy}/${ship.Engine.MaximumEnergy}`), info_row("耐冷", ship.Body.ShieldCold, "℃"), info_row("耐热", ship.Body.ShieldHeat, "℃"), info_row("耐压", ship.Body.ShieldPress, "kPa")]
            });
        }
    }

    render() {
        const hasShip = !!MainLevelScriptActor.GetSpaceship();
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
                React.createElement(LevelView, {
                    Slot: {
                        Padding: Utils.ltrb(20, 70, 120, 0)
                    },
                    levelList: this.charLevel
                }),
                hasShip && React.createElement(InfoCard, {
                    title: "飞船",
                    info: this.state.shipInfo
                }),
                hasShip && React.createElement(LevelView, {
                    Slot: {
                        Padding: Utils.ltrb(20, 70, 0, 0)
                    },
                    levelList: this.shipLevel
                })
            )
        );
    }
}

module.exports = MenuLevel;