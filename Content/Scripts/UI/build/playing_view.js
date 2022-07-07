const _ = require('lodash');
const UClass = require('uclass')();
const React = require('react');
const AD = require('animation-driver');
const ReactUMG = require('react-umg');
const Utils = require('../utils');
const EAnchors = require('../anchors');
const { F_Sans, T_Rect } = require('../style');

const PointBar = require('./point_bar');
const MessageListView = require('./message_listview');

const T_Keycap = Texture2D.Load('/Game/UI/Icon/T_Keycap64x64');

class PlayingView extends React.Component {
    constructor(props) {
        super(props);
        this.helper = new PlayingViewHelper();

        this.state = {
            HP: { cur: 0, max: 1 },
            MP: { cur: 0, max: 1 },
            actionPrompt: null,
            debugInfo: []
        };

        this.timer = setInterval(() => {
            if (this.actionAnime) return;

            let char = MainLevelScript.GetMainChar();
            if (!char) {
                this.setState({
                    HP: { cur: 0 }
                });
                return;
            }

            if (char.IsDriving()) {
                char = MainLevelScript.GetSpaceship();
            }
            this.setState({
                HP: { cur: char.Body.CurrentHP, max: char.Body.MaximumHP },
                MP: { cur: char.Engine.CurrentEnergy, max: char.Engine.MaximumEnergy }
            });

            if (Utils.isDev()) {
                this.setState({ debugInfo: this.helper.GetDebugInfo().OutList });
            }
        }, 200);
        MainLevelScript.Instance().ActionAddedDelegate.Add((Key, Tag, Interval) => {
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
                            return { X: 0, Y: (1 - t) * real_height };
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
            this.uActionBg.SetRenderTranslation({ X: 0, Y: this.uActionBg.GetCachedGeometry().GetLocalSize().Y });
            if (this.actionAnime) {
                this.actionAnime.destroy();
                this.actionAnime = null;
            }
        });
        MainLevelScript.Instance().MessagedDelegate.Add(Msg => {
            this.msgListView.AppendMsg(Msg);
        });
        MainLevelScript.Instance().EnteredDelegate.Add(() => {
            this.msgListView.toggleReview();
        });

        this.charKeys = [{ keys: this.helper.GetAxisKeys("MoveForward"), desc: this.helper.GetCharOperDesc("MoveForward") }, { keys: this.helper.GetAxisKeys("MoveRight"), desc: this.helper.GetCharOperDesc("MoveRight") }, { keys: this.helper.GetAxisKeys("MoveUp"), desc: this.helper.GetCharOperDesc("MoveUp") }, { keys: this.helper.GetActionKeys("Jump"), desc: this.helper.GetCharOperDesc("Jump") }, { keys: this.helper.GetAxisKeys("Turn"), desc: this.helper.GetCharOperDesc("Turn") }, { keys: this.helper.GetAxisKeys("LookUp"), desc: this.helper.GetCharOperDesc("LookUp") }, { keys: this.helper.GetActionKeys("Lock"), desc: this.helper.GetCharOperDesc("Lock") }];
        this.shipKeys = [{ keys: this.helper.GetAxisKeys("MoveForward"), desc: this.helper.GetShipOperDesc("MoveForward") }, { keys: this.helper.GetAxisKeys("MoveRight"), desc: this.helper.GetShipOperDesc("MoveRight") }, { keys: this.helper.GetAxisKeys("MoveUp"), desc: this.helper.GetShipOperDesc("MoveUp") }, { keys: this.helper.GetActionKeys("Hold"), desc: this.helper.GetShipOperDesc("Hold") }, { keys: this.helper.GetAxisKeys("Turn"), desc: this.helper.GetShipOperDesc("Turn") }, { keys: this.helper.GetAxisKeys("LookUp"), desc: this.helper.GetShipOperDesc("LookUp") }, { keys: this.helper.GetActionKeys("Lock"), desc: this.helper.GetShipOperDesc("Lock") }, { keys: this.helper.GetActionKeys("Drive"), desc: this.helper.GetShipOperDesc("Drive") }];
    }

    componentDidMount() {}

    componentWillUnmount() {
        clearInterval(this.timer);
    }

    render() {
        const on_ship = MainLevelScript.GetMainChar() && MainLevelScript.GetMainChar().IsDriving();
        return React.createElement(
            'uCanvasPanel',
            null,
            '/* \u5DE6\u4E0A\u89D2\u7684\u8840\u6761\u548C\u84DD\u6761 */',
            React.createElement(
                'div',
                { Slot: { LayoutData: { Offsets: Utils.ltrb(50, 50, 400, 50) } } },
                React.createElement(PointBar, {
                    Slot: { Size: { SizeRule: ESlateSizeRule.Fill } },
                    curVal: this.state.HP.cur,
                    maxVal: this.state.HP.max
                }),
                React.createElement('uSpacer', { Size: { Y: 4 } }),
                React.createElement(PointBar, {
                    Slot: { Size: { SizeRule: ESlateSizeRule.Fill } },
                    curVal: this.state.MP.cur,
                    maxVal: this.state.MP.max
                })
            ),
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
                            RenderTransform: {
                                Translation: { X: 0, Y: 999 }
                            }
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
            '/* \u8C03\u8BD5\u4FE1\u606F */',
            React.createElement(
                'div',
                { Slot: { LayoutData: { Offsets: Utils.ltrb(50, 300, 0, 0) } } },
                _.map(this.state.debugInfo, str => React.createElement('uTextBlock', {
                    Font: { FontObject: F_Sans, Size: 14 },
                    Text: str
                }))
            ),
            '/* \u4FE1\u606F\u6846 */',
            React.createElement(MessageListView, {
                ref: elem => {
                    if (elem) {
                        this.msgListView = elem;
                    }
                },
                Slot: {
                    LayoutData: {
                        Anchors: EAnchors.Right,
                        Alignment: { X: 1.0, Y: 0.5 },
                        Offsets: Utils.ltrb(-20, 0, 400, 300)
                    }
                }
            })
        );
    }
}

module.exports = ReactUMG.wrap(React.createElement(PlayingView, null));