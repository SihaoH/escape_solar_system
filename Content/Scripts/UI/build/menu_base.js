var _extends = Object.assign || function (target) { for (var i = 1; i < arguments.length; i++) { var source = arguments[i]; for (var key in source) { if (Object.prototype.hasOwnProperty.call(source, key)) { target[key] = source[key]; } } } return target; };

const _ = require('lodash');
const Events = require('events');
const AD = require('animation-driver');
const UClass = require('uclass')();
const React = require('react');
const ReactUMG = require('react-umg');
const Utils = require('../utils');
const EAnchors = require('../anchors');
const TaggedCard = require('tagged_card');
const ItemToolTip = require('item_tooltip');
const { F_Sans, ButtonStyle, SpinBoxProps } = require('../style');

let Event = null;

class MakeableEntry extends JavascriptWidget {
    properties() {
        // 不知道为啥，有的会自动改变首字母大小写
        this.index; /*int*/
    }
    OnMouseEnter(MyGeometry, MouseEvent) {
        Event.emit("MouseEnter", this.index);
    }
    OnMouseLeave(MouseEvent) {
        Event.emit("MouseLeave", -1);
    }
    OnMouseButtonDown(MyGeometry, MouseEvent) {
        Event.emit("MouseClick", this.index);
    }
}
ReactUMG.Register("uMakeableEntry", UClass(global, MakeableEntry));

class Menu extends React.Component {
    constructor(props) {
        super(props);

        this.helper = new MenuBaseHelper();

        this.state = {
            gameDate: this.helper.GetGameDate(),
            planetDist: this.helper.GetPlanetDistance().List,
            shipDist: this.helper.GetShipDistance(),
            hoveredIndex: -1,
            selectedIndex: -1,
            makeCount: 1
        };

        this.updateCountBtn = () => {
            if (this.uCountBox && this.state.selectedIndex > -1) {
                const sel_item = this.uMakeableList.GetItemAt(this.state.selectedIndex);
                const max_make = this.helper.GetMaxMakeableCount(sel_item.RowName);
                this.uCountBox.SetMaxValue(max_make || 1);
                this.uCountBox.SetMaxSliderValue(max_make || 1);
                this.uCountBox.SetValue(1);
            }
        };

        this.earthBase = MainLevelScript.GetEarthBase();
        this.earthBase.Backpack.ChangedDelegate.Add(() => {
            this.forceUpdate();
            this.updateCountBtn();
        });

        this.timer = setInterval(() => {
            if (this.makedAnime || this.destroyAnime) return;

            this.setState({
                gameDate: this.helper.GetGameDate(),
                planetDist: this.helper.GetPlanetDistance().List,
                shipDist: this.helper.GetShipDistance()
            });
        }, 500);
    }

    componentDidMount() {
        Event = new Events.EventEmitter();
        Event.on("MouseEnter", idx => {
            if (this.state.hoveredIndex !== idx) {
                this.uMakeableList.RegenerateAllEntries();
                this.setState({ hoveredIndex: idx });
            }
        });
        Event.on("MouseLeave", idx => {
            if (this.state.hoveredIndex !== -1) {
                this.uMakeableList.RegenerateAllEntries();
                this.setState({ hoveredIndex: -1 });
            }
        });
        Event.on("MouseClick", idx => {
            if (this.state.selectedIndex !== idx) {
                this.uMakeableList.RegenerateAllEntries();
                this.setState({ selectedIndex: idx, makeCount: 1 });
                this.updateCountBtn();
            }
        });
    }

    componentWillUnmount() {
        clearInterval(this.timer);
        Event = null;
        this.earthBase.Backpack.ChangedDelegate = null;
    }

    render() {
        const show_demand = this.state.selectedIndex > -1;
        const sel_item = this.state.selectedIndex > -1 ? this.uMakeableList.GetItemAt(this.state.selectedIndex) : { RowName: "未知" };
        const sel_data = MainLibrary.GetItemData(sel_item.RowName);
        const max_make = this.helper.GetMaxMakeableCount(sel_item.RowName);
        const btn_color = Utils.color(max_make > 0 ? "#FFF" : "#F22");
        return React.createElement(
            'uCanvasPanel',
            null,
            React.createElement(
                TaggedCard,
                {
                    Slot: {
                        LayoutData: {
                            Anchors: EAnchors.Left,
                            Alignment: { X: 0, Y: 0.5 },
                            Offsets: Utils.ltrb(0, 0, 460, 400)
                        }
                    },
                    title: Utils.tr("状态")
                },
                React.createElement(
                    'span',
                    null,
                    React.createElement('uTextBlock', {
                        Slot: {
                            HorizontalAlignment: EHorizontalAlignment.HAlign_Left
                        },
                        Font: {
                            FontObject: F_Sans,
                            Size: 16
                        },
                        Text: Utils.tr("时间")
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
                        Text: this.state.gameDate
                    })
                ),
                React.createElement('uImage', {
                    Slot: { Padding: Utils.ltrb(0, 5) },
                    Brush: { ImageSize: { X: 32, Y: 2 } },
                    ColorAndOpacity: Utils.rgba(1, 1, 1, 0.5)
                }),
                '/* \u5929\u4F53\u79BB\u57FA\u5730\u7684\u8DDD\u79BB */',
                _.map(this.state.planetDist, (v, k) => React.createElement(
                    'span',
                    null,
                    React.createElement('uTextBlock', {
                        Slot: {
                            HorizontalAlignment: EHorizontalAlignment.HAlign_Left
                        },
                        Font: {
                            FontObject: F_Sans,
                            Size: 16
                        },
                        Text: k
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
                        Text: `${Utils.num2Txt(v / 100)}m`
                    })
                )),
                React.createElement('uImage', {
                    Slot: { Padding: Utils.ltrb(0, 5) },
                    Brush: { ImageSize: { X: 32, Y: 2 } },
                    ColorAndOpacity: Utils.rgba(1, 1, 1, 0.5)
                }),
                this.state.shipDist !== -1 && React.createElement(
                    'span',
                    null,
                    React.createElement('uTextBlock', {
                        Slot: {
                            Padding: Utils.ltrb(0, 0, 10, 0),
                            VerticalAlignment: EVerticalAlignment.VAlign_Center
                        },
                        Font: {
                            FontObject: F_Sans,
                            Size: 16
                        },
                        Text: Utils.tr("飞船")
                    }),
                    this.state.shipDist > 10 && React.createElement(
                        'uSizeBox',
                        {
                            WidthOverride: 160,
                            HeightOverride: 50
                        },
                        React.createElement(
                            'uButton',
                            {
                                WidgetStyle: {
                                    Normal: {
                                        DrawAs: ESlateBrushDrawType.Image,
                                        TintColor: { SpecifiedColor: Utils.color("#FFF") }
                                    },
                                    Hovered: {
                                        DrawAs: ESlateBrushDrawType.Image,
                                        TintColor: { SpecifiedColor: Utils.color("#EEE") }
                                    },
                                    Pressed: {
                                        DrawAs: ESlateBrushDrawType.Image,
                                        TintColor: { SpecifiedColor: Utils.color("#CCC") }
                                    },
                                    NormalForeground: { SpecifiedColor: Utils.color("#333") },
                                    HoveredForeground: { SpecifiedColor: Utils.color("#F55") },
                                    PressedForeground: { SpecifiedColor: Utils.color("#333") },
                                    NormalPadding: Utils.ltrb(0),
                                    PressedPadding: Utils.ltrb(0)
                                },
                                IsFocusable: false,
                                OnPressed: () => {
                                    this.destroyAnime = AD();
                                    this.destroyAnime.apply(this.uBtnBg, { duration: 2 }, { RenderTranslation: t => {
                                            return { X: (t - 1) * 160, Y: 0 };
                                        } }).then(() => {
                                        if (this.destroyAnime) {
                                            this.uBtnBg.SetRenderTranslation({ X: -160, Y: 0 });
                                            this.destroyAnime = null;

                                            MainLevelScript.GetSpaceship().Destroy();
                                            this.setState({ shipDist: this.helper.GetShipDistance() });
                                        }
                                    });
                                },
                                OnReleased: () => {
                                    if (this.destroyAnime) {
                                        this.destroyAnime.destroy();
                                        this.destroyAnime = null;
                                    }
                                    this.uBtnBg.SetRenderTranslation({ X: -160, Y: 0 });
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
                                    ColorAndOpacity: Utils.color("#F22"),
                                    RenderTransform: { Translation: { X: -160, Y: 0 } }
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
                                        Size: 16
                                    },
                                    ColorAndOpacity: {
                                        ColorUseRule: ESlateColorStylingMode.UseColor_Foreground
                                    },
                                    Text: Utils.tr("自毁指令")
                                })
                            )
                        )
                    ),
                    React.createElement('uSpacer', { Slot: { Size: { SizeRule: ESlateSizeRule.Fill } } }),
                    React.createElement('uTextBlock', {
                        Slot: {
                            VerticalAlignment: EVerticalAlignment.VAlign_Center
                        },
                        Font: {
                            FontObject: F_Sans,
                            Size: 16
                        },
                        Text: `${Utils.num2Txt(this.state.shipDist)}m`
                    })
                )
            ),
            React.createElement(
                'uBorder',
                {
                    Slot: {
                        LayoutData: {
                            Anchors: EAnchors.Center,
                            Alignment: { X: 0.5, Y: 0.5 },
                            Offsets: Utils.ltrb(0, 0, 512, 512)
                        }
                    },
                    HorizontalAlignment: EHorizontalAlignment.HAlign_Center,
                    VerticalAlignment: EVerticalAlignment.VAlign_Center,
                    Background: {
                        ResourceObject: Texture2D.Load('/Game/UI/Icon/T_Base512x512'),
                        TintColor: { SpecifiedColor: Utils.rgba(0, 0, 0, 0.3) }
                    }
                },
                show_demand ? React.createElement(
                    'div',
                    {
                        Slot: {
                            HorizontalAlignment: EHorizontalAlignment.HAlign_Fill,
                            VerticalAlignment: EVerticalAlignment.VAlign_Fill
                        }
                    },
                    React.createElement(
                        'span',
                        null,
                        React.createElement('uTextBlock', {
                            Slot: {
                                VerticalAlignment: EVerticalAlignment.VAlign_Center
                            },
                            Font: {
                                FontObject: F_Sans,
                                TypefaceFontName: "Bold",
                                Size: 18
                            },
                            Text: Utils.tr("制作")
                        }),
                        React.createElement('uImage', {
                            Slot: {
                                Padding: Utils.ltrb(10, 0, 0, 0),
                                HorizontalAlignment: EHorizontalAlignment.HAlign_Center,
                                VerticalAlignment: EVerticalAlignment.VAlign_Center
                            },
                            Brush: {
                                ResourceObject: Texture2D.Load(sel_data.Icon),
                                ImageSize: { X: 32, Y: 32 }
                            }
                        }),
                        React.createElement('uTextBlock', {
                            Slot: {
                                Padding: Utils.ltrb(10, 0),
                                Size: { SizeRule: ESlateSizeRule.Fill },
                                VerticalAlignment: EVerticalAlignment.VAlign_Center
                            },
                            Font: {
                                FontObject: F_Sans,
                                Size: 18
                            },
                            ColorAndOpacity: { SpecifiedColor: Utils.color("#CCF") },
                            Text: sel_data.Name
                        }),
                        React.createElement(
                            'uSizeBox',
                            {
                                WidthOverride: 120,
                                HeightOverride: 50
                            },
                            React.createElement('uSpinBox', _extends({
                                ref: elem => {
                                    if (elem && !this.uCountBox) {
                                        this.uCountBox = elem.ueobj;
                                    }
                                },
                                Slot: {
                                    Size: {
                                        SizeRule: ESlateSizeRule.Fill,
                                        Value: 0.5
                                    }
                                }
                            }, SpinBoxProps, {
                                MaxValue: 1,
                                OnValueChanged: InValue => {
                                    this.setState({ makeCount: InValue });
                                }
                            }))
                        )
                    ),
                    React.createElement(
                        'span',
                        {
                            Slot: {
                                Padding: Utils.ltrb(0, 10)
                            }
                        },
                        React.createElement('uTextBlock', {
                            Font: {
                                FontObject: F_Sans,
                                TypefaceFontName: "Bold",
                                Size: 14
                            },
                            ColorAndOpacity: { SpecifiedColor: Utils.color("#888") },
                            Text: Utils.tr("所需资源")
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
                    ),
                    React.createElement(
                        'div',
                        {
                            Slot: {
                                Size: { SizeRule: ESlateSizeRule.Fill, Value: 1.0 }
                            }
                        },
                        _.map(sel_data.DemandList, (val, key) => {
                            const item_data = MainLibrary.GetItemData(key);
                            const hold_count = this.helper.GetHoldCount(key);
                            const need_count = val * this.state.makeCount;
                            return React.createElement(
                                'span',
                                {
                                    Slot: {
                                        Padding: Utils.ltrb(10, 5)
                                    }
                                },
                                React.createElement('uImage', {
                                    Slot: {
                                        HorizontalAlignment: EHorizontalAlignment.HAlign_Center,
                                        VerticalAlignment: EVerticalAlignment.VAlign_Center
                                    },
                                    Brush: {
                                        ResourceObject: Texture2D.Load(item_data.Icon),
                                        ImageSize: { X: 32, Y: 32 }
                                    }
                                }),
                                React.createElement('uTextBlock', {
                                    Slot: {
                                        Padding: Utils.ltrb(5, 0),
                                        Size: { SizeRule: ESlateSizeRule.Fill, Value: 1.0 },
                                        VerticalAlignment: EVerticalAlignment.VAlign_Center
                                    },
                                    Font: {
                                        FontObject: F_Sans,
                                        Size: 16
                                    },
                                    ColorAndOpacity: {
                                        SpecifiedColor: Utils.color("#CFF")
                                    },
                                    Text: item_data.Name
                                }),
                                React.createElement(
                                    'span',
                                    {
                                        Slot: {
                                            Size: { SizeRule: ESlateSizeRule.Fill, Value: 0.2 }
                                        }
                                    },
                                    React.createElement('uTextBlock', {
                                        Slot: {
                                            VerticalAlignment: EVerticalAlignment.VAlign_Center
                                        },
                                        Font: {
                                            FontObject: F_Sans,
                                            Size: 12
                                        },
                                        ColorAndOpacity: { SpecifiedColor: Utils.color("#555") },
                                        Text: Utils.tr("库存")
                                    }),
                                    React.createElement('uTextBlock', {
                                        Slot: {
                                            VerticalAlignment: EVerticalAlignment.VAlign_Center
                                        },
                                        Font: {
                                            FontObject: F_Sans,
                                            Size: 12
                                        },
                                        ColorAndOpacity: { SpecifiedColor: hold_count >= need_count ? Utils.color("#5F5") : Utils.color("#F55") },
                                        Text: ` ${hold_count}`
                                    })
                                ),
                                React.createElement('uTextBlock', {
                                    Slot: {
                                        Size: { SizeRule: ESlateSizeRule.Fill, Value: 0.2 },
                                        HorizontalAlignment: EHorizontalAlignment.HAlign_Right,
                                        VerticalAlignment: EVerticalAlignment.VAlign_Center
                                    },
                                    Font: {
                                        FontObject: F_Sans,
                                        Size: 16
                                    },
                                    ColorAndOpacity: {
                                        SpecifiedColor: Utils.color("#AAA")
                                    },
                                    Text: `x${need_count}`
                                })
                            );
                        })
                    ),
                    React.createElement('uSpacer', { Slot: { Size: { SizeRule: ESlateSizeRule.Fill, Value: 1.0 } } }),
                    React.createElement(
                        'uSizeBox',
                        {
                            Slot: {
                                HorizontalAlignment: EHorizontalAlignment.HAlign_Center
                            },
                            WidthOverride: 300,
                            HeightOverride: 50
                        },
                        React.createElement(
                            'uButton',
                            {
                                Visibility: max_make > 0 ? ESlateVisibility.Visible : ESlateVisibility.HitTestInvisible,
                                WidgetStyle: {
                                    Normal: {
                                        DrawAs: ESlateBrushDrawType.Border,
                                        TintColor: { SpecifiedColor: btn_color },
                                        Margin: Utils.ltrb(1)
                                    },
                                    Hovered: {
                                        DrawAs: ESlateBrushDrawType.Border,
                                        TintColor: { SpecifiedColor: btn_color },
                                        Margin: Utils.ltrb(1)
                                    },
                                    Pressed: {
                                        DrawAs: ESlateBrushDrawType.Border,
                                        TintColor: { SpecifiedColor: btn_color },
                                        Margin: Utils.ltrb(1)
                                    },
                                    NormalPadding: Utils.ltrb(0),
                                    PressedPadding: Utils.ltrb(0)
                                },
                                IsFocusable: false,
                                OnPressed: () => {
                                    this.makedAnime = AD();
                                    this.makedAnime.apply(this.uBtnBg, { duration: 0.5 }, { RenderScale: t => {
                                            return { X: t, Y: 1.0 };
                                        } }).then(_ => {
                                        if (this.makedAnime) {
                                            const cur = this.uCountBox.GetValue();
                                            this.helper.MakeItem(sel_item, cur);
                                            this.updateCountBtn();
                                            this.uBtnBg.SetRenderScale({ X: 0, Y: 1.0 });
                                            this.setState({ makeCount: 1 });
                                        }
                                    });
                                },
                                OnReleased: () => {
                                    this.makedAnime.destroy();
                                    this.makedAnime = null;
                                    this.uBtnBg.SetRenderScale({ X: 0, Y: 1.0 });
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
                                    RenderTransform: { Scale: { X: 0, Y: 1.0 } }
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
                                        OutlineSettings: {
                                            OutlineSize: 2,
                                            OutlineColor: Utils.rgba(0, 0, 0, 0.6)
                                        }
                                    },
                                    ColorAndOpacity: { SpecifiedColor: btn_color },
                                    Text: max_make ? Utils.tr("制作") : Utils.tr("资源不足")
                                })
                            )
                        )
                    )
                ) : React.createElement(
                    'uBorder',
                    {
                        Background: {
                            DrawAs: ESlateBrushDrawType.Border,
                            TintColor: { SpecifiedColor: Utils.color("#8F8") },
                            Margin: Utils.ltrb(0, 1, 0, 1)
                        },
                        ContentColorAndOpacity: Utils.color("#5F5")
                    },
                    React.createElement('uTextBlock', {
                        Font: {
                            FontObject: F_Sans,
                            Size: 18
                        },
                        Text: Utils.tr("已连接基地")
                    })
                )
            ),
            React.createElement(
                TaggedCard,
                {
                    Slot: {
                        LayoutData: {
                            Anchors: EAnchors.Right,
                            Alignment: { X: 1.0, Y: 0.5 },
                            Offsets: Utils.ltrb(0, 0, 460, 400)
                        }
                    },
                    title: Utils.tr("可加工物品")
                },
                React.createElement('uJavascriptListView', {
                    ref: elem => {
                        if (elem && !this.uMakeableList) {
                            this.uMakeableList = elem.ueobj;
                            this.uMakeableList.SetItems(this.helper.GetMakeableList().OutItems);
                            process.nextTick(_ => this.uMakeableList.SetScrollbarVisibility(ESlateVisibility.Visible));
                        }
                    },
                    Slot: {
                        Size: { SizeRule: ESlateSizeRule.Fill }
                    },
                    SelectionMode: ESelectionMode.Single,
                    OnGenerateRow: (item, view) => {
                        const index = view.GetIndexForItem(item);
                        const item_data = MainLibrary.GetItemData(item.RowName);
                        const is_hovered = this.state.hoveredIndex === index;
                        const is_selected = this.state.selectedIndex === index;
                        return ReactUMG.wrap(React.createElement(
                            'uSizeBox',
                            {
                                Slot: {
                                    Padding: Utils.ltrb(0, 2)
                                },
                                HeightOverride: 64,
                                ToolTipWidgetDelegate: () => ReactUMG.wrap(React.createElement(ItemToolTip, { itemData: item_data }))
                            },
                            React.createElement(
                                'uMakeableEntry',
                                { index: index },
                                React.createElement(
                                    'uCanvasPanel',
                                    null,
                                    React.createElement(
                                        'span',
                                        {
                                            Slot: {
                                                LayoutData: {
                                                    Anchors: EAnchors.FillAll,
                                                    Offsets: Utils.ltrb(0)
                                                }
                                            }
                                        },
                                        React.createElement(
                                            'uBorder',
                                            {
                                                Slot: {
                                                    HorizontalAlignment: EHorizontalAlignment.HAlign_Left
                                                },
                                                Background: {
                                                    TintColor: { SpecifiedColor: is_hovered ? Utils.rgba(0.7, 0.7, 0.7, 0.4) : Utils.rgba(0.4, 0.4, 0.4, 0.4) }
                                                }
                                            },
                                            React.createElement('uImage', {
                                                Slot: {
                                                    Padding: Utils.ltrb(0)
                                                },
                                                Brush: {
                                                    // Icon变成了string类型
                                                    ResourceObject: Texture2D.Load(item_data.Icon),
                                                    ImageSize: { X: 64, Y: 64 }
                                                }
                                            })
                                        ),
                                        React.createElement(
                                            'uBorder',
                                            {
                                                Slot: {
                                                    Size: { SizeRule: ESlateSizeRule.Fill },
                                                    HorizontalAlignment: EHorizontalAlignment.HAlign_Fill,
                                                    VerticalAlignment: EVerticalAlignment.VAlign_Fill
                                                },
                                                Background: {
                                                    TintColor: { SpecifiedColor: is_hovered ? Utils.rgba(0.8, 0.8, 0.8, 0.4) : Utils.rgba(0.5, 0.5, 0.5, 0.4) }
                                                },
                                                VerticalAlignment: EVerticalAlignment.VAlign_Center
                                            },
                                            React.createElement('uTextBlock', {
                                                Slot: {
                                                    Padding: Utils.ltrb(20, 0)
                                                },
                                                Font: {
                                                    FontObject: F_Sans,
                                                    TypefaceFontName: "Bold",
                                                    Size: 14
                                                },
                                                ColorAndOpacity: {
                                                    SpecifiedColor: Utils.color("#FFF")
                                                },
                                                Text: item_data.Name
                                            })
                                        )
                                    ),
                                    React.createElement('uImage', {
                                        Slot: {
                                            LayoutData: {
                                                Anchors: EAnchors.FillBottom,
                                                Offsets: Utils.ltrb(0, 0, 0, is_selected ? 4 : 0),
                                                Alignment: { X: 0, Y: 1.0 }
                                            }
                                        }
                                    })
                                )
                            )
                        ));
                    }
                })
            )
        );
    }
}

module.exports = Menu;