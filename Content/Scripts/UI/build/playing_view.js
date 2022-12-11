const _ = require('lodash');
const React = require('react');
const AD = require('animation-driver');
const ReactUMG = require('react-umg');
const Utils = require('../utils');
const EAnchors = require('../anchors');
const { F_Sans, T_Rect } = require('../style');

const PointBar = require('./point_bar');
const EngineToward = require('./engine_toward');
const TargetIndicator = require('./target_indicator');

const T_Keycap = Texture2D.Load('/Game/UI/Icon/T_Keycap64x64');

let ThisWidget = null;

class PlayingView extends React.Component {
    constructor(props) {
        super(props);
        this.helper = new PlayingViewHelper();

        this.state = {
            charBar: null,
            shipBar: null,
            actionPrompt: null,
            showDestroy: false,
            locationInfo: "",
            enginInfo: { max: 0, upDown: 0, frontBack: 0 },
            basicInfo: [],
            speakData: null
        };

        this.timer = setInterval(() => {
            let player = MainLevelScript.GetMainChar();
            if (!player) {
                let char_bar = this.state.charBar;
                let ship_bar = this.state.shipBar;
                if (char_bar) {
                    char_bar.HP.cur = 0;
                } else if (ship_bar) {
                    ship_bar.HP.cur = 0;
                }
                this.setState({
                    charBar: char_bar,
                    ship_bar: ship_bar
                });

                return;
            }

            let bar_obj = {
                charBar: null,
                shipBar: null
            };
            if (player.IsDriving()) {
                player = MainLevelScript.GetSpaceship();
                bar_obj.shipBar = {
                    HP: { cur: player.Body.CurrentHP, max: player.Body.MaximumHP },
                    MP: { cur: player.Engine.CurrentEnergy, max: player.Engine.MaximumEnergy }
                };
            } else {
                bar_obj.charBar = {
                    HP: { cur: player.Body.CurrentHP, max: player.Body.MaximumHP },
                    MP: { cur: player.Engine.CurrentEnergy, max: player.Engine.MaximumEnergy }
                };
            }
            this.setState(bar_obj);

            this.setState({ enginInfo: { max: player.Engine.Power, upDown: player.Engine.UpForce, frontBack: player.Engine.ForwardForce } });

            let loc_info = player.GetLocationInfo();
            this.setState({ locationInfo: `${loc_info.Site} (${(loc_info.Loction.X / 100).toFixed(0)}, ${(loc_info.Loction.Y / 100).toFixed(0)}, ${(loc_info.Loction.Z / 100).toFixed(0)})` });

            this.setState({ basicInfo: [{ icon: "T_Temperature32x32", text: `${Utils.tr("温度")}: ${Utils.num2Txt(player.Body.CurrentTemp)} ℃`, color: player.Body.CurrentTemp > player.Body.ShieldHeat ? Utils.color("#f00") : player.Body.CurrentTemp < player.Body.ShieldCold ? Utils.color("#00f") : Utils.color("#fff") }, { icon: "T_Pressure32x32", text: `${Utils.tr("流体压力")}: ${Utils.num2Txt(player.Body.CurrentPress)} kPa`, color: player.Body.CurrentPress > player.Body.ShieldPress ? Utils.color("#f00") : Utils.color("#fff") }, { icon: "T_Mass32x32", text: `${Utils.tr("质量")}: ${Utils.num2Txt(player.GetMass())} kg`, color: Utils.color("#fff") }, { icon: "T_Gravity32x32", text: `${Utils.tr("重力")}: ${Utils.num2Txt(player.GetGravityAccel() / 100, 1)} m/s²`, color: Utils.color("#fff") }, { icon: "T_Thrust32x32", text: `${Utils.tr("推力MAX")}: ${Utils.num2Txt(player.Engine.Power)} N`, color: Utils.color("#fff") }] });
        }, 50);
        MainLevelScript.Instance().NPCSpeakDelegate.Add((Name, Text, Time) => {
            this.setState({ speakData: { name: Name, text: Text } });
            setTimeout(() => {
                this.setState({ speakData: null });
            }, Time * 1000);
        });
        MainLevelScript.Instance().ActionAddedDelegate.Add((Key, Tag, Interval) => {
            this.actionAnimeVal = { X: 0, Y: 999 };
            this.setState({ actionPrompt: { key: Key, tag: Tag, interval: Interval } });
        });
        MainLevelScript.Instance().ActionRemovedDelegate.Add(() => {
            this.setState({ actionPrompt: null });
        });
        MainLevelScript.Instance().ActionPressedDelegate.Add(() => {
            const prompt = this.state.actionPrompt;
            if (prompt) {
                if (prompt.interval > 0) {
                    const real_height = this.uActionBg.GetCachedGeometry().GetLocalSize().Y;
                    this.actionAnime = AD();
                    this.actionAnime.apply(this.uActionBg, { duration: prompt.interval }, { RenderTranslation: t => {
                            this.actionAnimeVal = { X: 0, Y: (1 - t) * real_height };return this.actionAnimeVal;
                        } }).then(_ => {
                        if (this.actionAnime) {
                            this.actionAnime = null;
                            MainLevelScript.ActionDone();
                        }
                    });
                } else {
                    MainLevelScript.ActionDone();
                }
            }
        });
        MainLevelScript.Instance().ActionReleasedDelegate.Add(() => {
            this.actionAnimeVal = { X: 0, Y: 999 };
            if (this.actionAnime) {
                this.actionAnime.destroy();
                this.actionAnime = null;
            }
        });

        MainLevelScript.Instance().DestroyPressedDelegate.Add(() => {
            this.destroyAnimeVal = { X: 0, Y: 1.0 };
            this.setState({ showDestroy: true });
            this.destroyAnime = AD();
            this.destroyAnime.apply(this.uDestroyBg, { delay: 1, duration: 2 }, { RenderScale: t => {
                    this.destroyAnimeVal = { X: t, Y: 1.0 };return this.destroyAnimeVal;
                } }).then(_ => {
                this.setState({ showDestroy: false });
                if (this.destroyAnime) {
                    this.destroyAnime = null;
                    let player = MainLevelScript.GetMainChar();
                    if (player) {
                        if (player.IsDriving()) {
                            player = MainLevelScript.GetSpaceship();
                        }
                        player.Body.ChangeHP(-player.Body.MaximumHP);
                    }
                }
            });
        });
        MainLevelScript.Instance().DestroyReleasedDelegate.Add(() => {
            this.destroyAnimeVal = { X: 0, Y: 1.0 };
            if (this.destroyAnime) {
                this.destroyAnime.destroy();
                this.destroyAnime = null;
            }
        });

        this.charKeys = [{ keys: this.helper.GetAxisKeys("MoveForward"), desc: Utils.tr("前后移动/推进") }, { keys: this.helper.GetAxisKeys("MoveRight"), desc: Utils.tr("左右移动或(在太空中)左右转动") }, { keys: this.helper.GetAxisKeys("MoveUp"), desc: Utils.tr("向上或向下推进") }, { keys: this.helper.GetActionKeys("Jump"), desc: Utils.tr("(陆地上)跳跃") }, { keys: this.helper.GetAxisKeys("Turn").concat(this.helper.GetAxisKeys("LookUp")), desc: Utils.tr("转动视角") }, { keys: this.helper.GetActionKeys("Lock"), desc: Utils.tr("锁定并显示星球信息") }, { keys: this.helper.GetActionKeys("Destroy"), desc: Utils.tr("长按自毁") }];
        this.shipKeys = [{ keys: this.helper.GetAxisKeys("MoveForward"), desc: Utils.tr("向前或向后推进") }, { keys: this.helper.GetAxisKeys("MoveRight"), desc: Utils.tr("锁定视角时偏转机身") }, { keys: this.helper.GetAxisKeys("MoveUp"), desc: Utils.tr("向上或向下推进") }, { keys: this.helper.GetActionKeys("Hold"), desc: Utils.tr("按住调整机头朝向(锁定视角)") }, { keys: this.helper.GetActionKeys("Lock"), desc: Utils.tr("锁定并显示星球信息") }, { keys: this.helper.GetActionKeys("Drive"), desc: Utils.tr("脱离飞船") }, { keys: this.helper.GetActionKeys("Destroy"), desc: Utils.tr("长按自毁") }];
    }

    componentWillUnmount() {
        ThisWidget = null;
        clearInterval(this.timer);
    }

    render() {
        const on_ship = MainLevelScript.GetMainChar() && MainLevelScript.GetMainChar().IsDriving();
        const char_bar = this.state.charBar;
        const ship_bar = this.state.shipBar;
        return React.createElement(
            'uCanvasPanel',
            null,
            '/* \u5DE6\u4E0A\u89D2\u7684\u8840\u6761\u548C\u84DD\u6761 */',
            char_bar && React.createElement(
                'div',
                { Slot: { LayoutData: { Offsets: Utils.ltrb(50, 50, 400, 50) } } },
                React.createElement(PointBar, {
                    Slot: { Size: { SizeRule: ESlateSizeRule.Fill } },
                    curVal: char_bar.HP.cur,
                    maxVal: char_bar.HP.max
                }),
                React.createElement('uSpacer', { Size: { Y: 4 } }),
                React.createElement(PointBar, {
                    Slot: { Size: { SizeRule: ESlateSizeRule.Fill } },
                    curVal: char_bar.MP.cur,
                    maxVal: char_bar.MP.max
                })
            ),
            ship_bar && React.createElement(
                'div',
                { Slot: { LayoutData: { Offsets: Utils.ltrb(50, 50, 400, 50) } } },
                React.createElement(PointBar, {
                    Slot: { Size: { SizeRule: ESlateSizeRule.Fill } },
                    curVal: ship_bar.HP.cur,
                    maxVal: ship_bar.HP.max
                }),
                React.createElement('uSpacer', { Size: { Y: 4 } }),
                React.createElement(PointBar, {
                    Slot: { Size: { SizeRule: ESlateSizeRule.Fill } },
                    curVal: ship_bar.MP.cur,
                    maxVal: ship_bar.MP.max
                })
            ),
            '/* \u57FA\u672C\u72B6\u6001\u4FE1\u606F */',
            React.createElement(
                'div',
                { Slot: { LayoutData: { Offsets: Utils.ltrb(50, 105, 400, 32) } } },
                _.map(this.state.basicInfo, item => React.createElement(
                    'uBorder',
                    {
                        Slot: {
                            HorizontalAlignment: EHorizontalAlignment.HAlign_Left,
                            Padding: Utils.ltrb(0, 1)
                        },
                        Padding: Utils.ltrb(10, 1),
                        Background: {
                            TintColor: { SpecifiedColor: Utils.rgba(0, 0, 0, 0.2) }
                        }
                    },
                    React.createElement(
                        'span',
                        null,
                        React.createElement('uImage', {
                            Slot: { Padding: Utils.ltrb(0, 0, 10, 0) },
                            ColorAndOpacity: item.color,
                            Brush: {
                                ResourceObject: Utils.icon(item.icon),
                                ImageSize: { X: 32, Y: 32 }
                            }
                        }),
                        React.createElement('uTextBlock', {
                            Slot: { VerticalAlignment: EVerticalAlignment.VAlign_Center },
                            Font: { FontObject: F_Sans, Size: 12 },
                            ColorAndOpacity: {
                                SpecifiedColor: item.color
                            },
                            Text: item.text
                        })
                    )
                ))
            ),
            React.createElement(EngineToward, {
                Slot: { LayoutData: { Offsets: Utils.ltrb(50, 280, 160, 160) } },
                Max: this.state.enginInfo.max,
                UpDown: this.state.enginInfo.upDown,
                FrontBack: this.state.enginInfo.frontBack
            }),
            '/* \u53F3\u4E0A\u89D2\u7684\u6309\u952E\u64CD\u4F5C\u6307\u793A */',
            React.createElement(
                'div',
                {
                    Slot: {
                        LayoutData: {
                            Anchors: EAnchors.TopRight,
                            Alignment: { X: 1.0, Y: 0 },
                            Offsets: Utils.ltrb(-50, 50, 0, 0)
                        },
                        bAutoSize: true
                    }
                },
                _.map(on_ship ? this.shipKeys : this.charKeys, obj => React.createElement(
                    'span',
                    null,
                    _.map(obj.keys, key => {
                        const is_mouse = key.Key_IsButtonAxis() || key.Key_IsMouseButton();
                        return React.createElement(
                            'uBorder',
                            {
                                HorizontalAlignment: EHorizontalAlignment.HAlign_Center,
                                VerticalAlignment: EVerticalAlignment.VAlign_Center,
                                Padding: Utils.ltrb(20, 5)
                                /* 这里有个bug，本来应该是Background属性的，但没有对应的SetBackground方法；
                                 * 而React-umg就是用【Set+属性名】的方式刷新的，所以用Background属性就不能正常刷新了；
                                 * 但是它有SetBrush方法，效果就是设置Background背景，于是偷了个鸡用Brush属性赋值了。
                                */
                                , Brush: {
                                    ResourceObject: is_mouse ? Texture2D.Load(`/Game/UI/Icon/T_${key.KeyName}64x64`) : T_Keycap,
                                    DrawAs: is_mouse ? ESlateBrushDrawType.Image : ESlateBrushDrawType.Box,
                                    ImageSize: { X: 64, Y: 64 },
                                    Margin: Utils.ltrb(0.5)
                                }
                            },
                            React.createElement('uTextBlock', {
                                Font: {
                                    FontObject: F_Sans,
                                    Size: 12
                                },
                                ColorAndOpacity: { SpecifiedColor: Utils.color("#333") },
                                Text: is_mouse ? " " : key.Key_GetDisplayName()
                            })
                        );
                    }),
                    React.createElement('uTextBlock', {
                        Slot: {
                            VerticalAlignment: EVerticalAlignment.VAlign_Center
                        },
                        Font: {
                            FontObject: F_Sans,
                            Size: 14,
                            OutlineSettings: {
                                OutlineSize: 1,
                                OutlineColor: Utils.rgba(0, 0, 0, 0.6)
                            }
                        },
                        Text: obj.desc
                    })
                ))
            ),
            '/* \u4E2D\u95F4\u7684\u51C6\u5FC3 */',
            React.createElement('uTextBlock', {
                Slot: {
                    LayoutData: {
                        Anchors: EAnchors.Center,
                        Alignment: { X: 0.5, Y: 0.5 },
                        Offsets: Utils.ltrb(0)
                    },
                    bAutoSize: true
                },
                Font: {
                    FontObject: F_Sans,
                    Size: 8,
                    OutlineSettings: {
                        OutlineSize: 2,
                        OutlineColor: Utils.rgba(0, 0, 0, 0.3)
                    }
                },
                ColorAndOpacity: { SpecifiedColor: Utils.rgba(1, 1, 1, 0.9) },
                Text: "●"
            }),
            '/* \u81EA\u6BC1\u63D0\u793A */',
            this.state.showDestroy && React.createElement(
                'uBorder',
                {
                    Slot: {
                        LayoutData: {
                            Anchors: EAnchors.Center,
                            Alignment: { X: 0.5, Y: 0.5 },
                            Offsets: Utils.ltrb(0, 0, 400, 80)
                        }
                    },
                    Brush: {
                        ResourceObject: T_Rect,
                        DrawAs: ESlateBrushDrawType.Border,
                        TintColor: { SpecifiedColor: Utils.color("#F33") },
                        Margin: Utils.ltrb(4 / 64)
                    }
                },
                React.createElement(
                    'uCanvasPanel',
                    {
                        Slot: {
                            Padding: Utils.ltrb(0),
                            HorizontalAlignment: EHorizontalAlignment.HAlign_Fill,
                            VerticalAlignment: EVerticalAlignment.VAlign_Fill
                        }
                    },
                    React.createElement('uImage', {
                        ref: elem => {
                            if (elem && this.uDestroyBg !== elem.ueobj) {
                                this.uDestroyBg = elem.ueobj;
                            }
                        },
                        Slot: {
                            LayoutData: {
                                Anchors: EAnchors.FillAll,
                                Offsets: Utils.ltrb(0)
                            }
                        },
                        Brush: {
                            TintColor: { SpecifiedColor: Utils.color("#F33") }
                        },
                        RenderTransform: { Scale: this.destroyAnimeVal }
                    }),
                    React.createElement('uTextBlock', {
                        Slot: {
                            LayoutData: {
                                Anchors: EAnchors.Center,
                                Alignment: { X: 0.5, Y: 0.5 }
                            },
                            bAutoSize: true
                        },
                        Font: {
                            FontObject: F_Sans,
                            TypefaceFontName: "Bold",
                            Size: 18,
                            LetterSpacing: 50,
                            OutlineSettings: {
                                OutlineSize: 1,
                                OutlineColor: Utils.rgba(0, 0, 0, 0.5)
                            }
                        },
                        ColorAndOpacity: { SpecifiedColor: Utils.color("#F22") },
                        Text: Utils.tr("自毁程序")
                    })
                )
            ),
            '/* \u4E2D\u95F4\u7684\u6309\u952E\u63D0\u793A */',
            this.state.actionPrompt && React.createElement(
                'uCanvasPanel',
                {
                    Slot: {
                        LayoutData: {
                            Anchors: EAnchors.Center,
                            Alignment: { X: 0, Y: 0.5 },
                            Offsets: Utils.ltrb(50, 50, 0, 0)
                        },
                        bAutoSize: true
                    }
                },
                React.createElement('uImage', {
                    Slot: {
                        LayoutData: {
                            Anchors: EAnchors.FillAll,
                            Offsets: Utils.ltrb(0)
                        }
                    },
                    Brush: {
                        TintColor: { SpecifiedColor: Utils.rgba(0, 0, 0, 0.1) }
                    }
                }),
                React.createElement(
                    'span',
                    {
                        Slot: {
                            bAutoSize: true
                        }
                    },
                    React.createElement(
                        'uCanvasPanel',
                        {
                            Clipping: EWidgetClipping.ClipToBounds
                        },
                        React.createElement('uImage', {
                            ref: elem => {
                                if (elem && this.uActionBg !== elem.ueobj) {
                                    this.uActionBg = elem.ueobj;
                                }
                            },
                            Slot: {
                                LayoutData: {
                                    Anchors: EAnchors.FillAll,
                                    Offsets: Utils.ltrb(0)
                                }
                            },
                            Brush: {
                                TintColor: { SpecifiedColor: Utils.rgba(1, 1, 1, 0.6) }
                            },
                            RenderTransform: { Translation: this.actionAnimeVal }
                        }),
                        React.createElement(
                            'uBorder',
                            {
                                Slot: {
                                    LayoutData: { Offsets: Utils.ltrb(0) },
                                    bAutoSize: true
                                },
                                Padding: Utils.ltrb(13, 5),
                                HorizontalAlignment: EHorizontalAlignment.HAlign_Center,
                                VerticalAlignment: EVerticalAlignment.VAlign_Center,
                                Brush: {
                                    ResourceObject: T_Rect,
                                    DrawAs: ESlateBrushDrawType.Border,
                                    TintColor: { SpecifiedColor: Utils.color("#FFF") },
                                    Margin: Utils.ltrb(2 / 64)
                                }
                            },
                            React.createElement('uTextBlock', {
                                Font: {
                                    FontObject: F_Sans,
                                    TypefaceFontName: "Bold",
                                    Size: 14
                                },
                                ColorAndOpacity: { SpecifiedColor: Utils.color("#FFF") },
                                Text: this.state.actionPrompt.key.Key_GetDisplayName().replace("键", "")
                            })
                        )
                    ),
                    React.createElement('uTextBlock', {
                        Slot: {
                            VerticalAlignment: EVerticalAlignment.VAlign_Center,
                            Padding: Utils.ltrb(10, 0)
                        },
                        Font: {
                            FontObject: F_Sans,
                            TypefaceFontName: "Bold",
                            Size: 14
                        },
                        Justification: ETextJustify.Center,
                        Text: this.state.actionPrompt.tag
                    })
                )
            ),
            '/* \u4F4D\u7F6E\u4FE1\u606F */',
            React.createElement(
                'uBorder',
                {
                    Slot: {
                        LayoutData: {
                            Anchors: EAnchors.BottomRight,
                            Alignment: { X: 1.0, Y: 1.0 },
                            Offsets: Utils.ltrb(-50, -50, 0, 0)
                        },
                        bAutoSize: true
                    },
                    Padding: Utils.ltrb(10),
                    Background: {
                        TintColor: { SpecifiedColor: Utils.rgba(0, 0, 0, 0.2) }
                    }
                },
                React.createElement(
                    'span',
                    null,
                    React.createElement('uImage', {
                        Slot: { Padding: Utils.ltrb(0, 0, 10, 0) }
                        //ColorAndOpacity={ item.color }
                        , Brush: {
                            ResourceObject: Utils.icon("T_Location32x32"),
                            ImageSize: { X: 32, Y: 32 }
                        }
                    }),
                    React.createElement('uTextBlock', {
                        Slot: { VerticalAlignment: EVerticalAlignment.VAlign_Center },
                        Font: { FontObject: F_Sans, Size: 14 }
                        //ColorAndOpacity={{ SpecifiedColor: item.color }}
                        , Text: this.state.locationInfo
                    })
                )
            ),
            '/* \u661F\u7403\u4FE1\u606F */',
            React.createElement(TargetIndicator, {
                Slot: {
                    LayoutData: {
                        Anchors: EAnchors.FillAll
                    }
                }
            }),
            '/* NPC\u81EA\u53D1\u8A00 */',
            this.state.speakData && React.createElement(
                'uBorder',
                {
                    Slot: {
                        LayoutData: {
                            Anchors: EAnchors.Bottom,
                            Alignment: { X: 0.5, Y: 1.0 },
                            Offsets: Utils.ltrb(0, -150, 0, 0)
                        },
                        bAutoSize: true
                    },
                    Padding: Utils.ltrb(10),
                    Background: {
                        TintColor: { SpecifiedColor: Utils.rgba(0, 0, 0, 0.2) }
                    }
                },
                React.createElement('uTextBlock', {
                    Slot: { VerticalAlignment: EVerticalAlignment.VAlign_Center },
                    Font: {
                        FontObject: F_Sans,
                        TypefaceFontName: "Bold",
                        Size: 16,
                        OutlineSettings: {
                            OutlineSize: 1,
                            OutlineColor: Utils.rgba(0, 0, 0, 0.6)
                        }
                    },
                    WrapTextAt: 1200,
                    ColorAndOpacity: { SpecifiedColor: Utils.color("#fff") },
                    Text: `${this.state.speakData.name}: ${this.state.speakData.text}`
                })
            )
        );
    }
}

module.exports = function () {
    if (!ThisWidget) {
        ThisWidget = ReactUMG.wrap(React.createElement(PlayingView, null));
    }
    return ThisWidget;
};