const _ = require('lodash')
const Events = require('events')
const AD = require('animation-driver')
const UClass = require('uclass')()
const React = require('react')
const ReactUMG = require('react-umg')
const Utils = require('../utils')
const EAnchors = require('../anchors')
const TaggedCard = require('tagged_card')
const ItemToolTip = require('item_tooltip')
const {SpinBoxProps} = require('../style')

let event = null

class MakeableEntry extends JavascriptWidget {
    properties() {
        // 不知道为啥，有的会自动改变首字母大小写
        this.index/*int*/
    }
    OnMouseEnter(MyGeometry, MouseEvent) {
        event.emit("MouseEnter", this.index)
    }
    OnMouseLeave(MouseEvent) {
        event.emit("MouseLeave", -1)
    }
    OnMouseButtonDown(MyGeometry, MouseEvent) {
        event.emit("MouseClick", this.index)
    }
}
ReactUMG.Register("uMakeableEntry", UClass(global, MakeableEntry))

class Menu extends React.Component {
    constructor(props) {
        super(props)
        this.state = {
            status: this.getStatus(),
            hoveredIndex: -1,
            selectedIndex: -1,
            makeCount: 1
        }

        this.updateCountBtn = () => {
            if (this.uCountBox && this.state.selectedIndex > -1) {
                const sel_item = this.uMakeableList.GetItemAt(this.state.selectedIndex)
                const max_make = this.helper.GetMaxMakeableCount(sel_item.RowName)
                this.uCountBox.SetMaxValue(max_make || 1)
                this.uCountBox.SetMaxSliderValue(max_make || 1)
                this.uCountBox.SetValue(1)
            }
        }
        this.helper = new MenuBaseHelper()
        this.earthBase = MainLevelScriptActor.GetEarthBase()
        if (this.earthBase) {
            this.earthBase.Backpack.ChangedDelegate.Add(() => {
                this.forceUpdate()
                this.updateCountBtn()
            })
        }

        setInterval(() => {
            let status = this.getStatus()
            if (JSON.stringify(this.state.status) !== JSON.stringify(status)) {
                this.setState({status: status})
            }
        }, 500)
    }

    getStatus() {
        return [
            {
                "时间": new Date().toLocaleDateString(),
                "探索点": 100
            },
            {
                "地球": `${Utils.num2Txt(100)}m`,
                "火星": `${Utils.num2Txt(30000)}m`,
            }
        ]
    }

    componentDidMount() {
        event = new Events.EventEmitter()
        event.on("MouseEnter", (idx) => {
            if (this.state.hoveredIndex !== idx) {
                this.uMakeableList.RegenerateAllEntries()
                this.setState({ hoveredIndex: idx })
            }
        })
        event.on("MouseLeave", (idx) => {
            if (this.state.hoveredIndex !== -1) {
                this.uMakeableList.RegenerateAllEntries()
                this.setState({ hoveredIndex: -1 })
            }
        })
        event.on("MouseClick", (idx) => {
            if (this.state.selectedIndex !== idx) {
                this.uMakeableList.RegenerateAllEntries()
                this.setState({ selectedIndex: idx, makeCount: 1 })
                this.updateCountBtn()
            }
        })
    }

    componentWillUnmount() {
        event = null
        if (this.earthBase) {
            this.earthBase.Backpack.ChangedDelegate = null
        }
    }

    render() {
        const show_demand = this.earthBase && this.state.selectedIndex > -1
        const sel_item = this.state.selectedIndex > -1 ? this.uMakeableList.GetItemAt(this.state.selectedIndex) : {RowName: "1000"}
        const sel_data = MainFunctionLibrary.GetItemData(sel_item.RowName)
        const max_make = this.helper.GetMaxMakeableCount(sel_item.RowName)
        const btn_color = Utils.color(max_make > 0 ? "#FFF" : "#F55")
        return (
            <uCanvasPanel>
                {this.earthBase &&
                <TaggedCard
                    Slot={{
                        LayoutData: {
                            Anchors: EAnchors.Left,
                            Alignment: { X: 0, Y: 0.5 },
                            Offsets: Utils.ltrb(0, 0, 460, 400)
                        }
                    }}
                    title={"状态"}
                >
                    {_.map(this.state.status, (val) => [
                        ..._.map(val, (v, k) => (
                            <span>
                                <uTextBlock
                                    Slot={{
                                        HorizontalAlignment: EHorizontalAlignment.HAlign_Left
                                    }}
                                    Font={{
                                        FontObject: Font.Load('/Game/UI/Font/SourceHanSansSC'),
                                        Size: 16,
                                    }}
                                    Text={k}
                                />
                                <uTextBlock
                                    Slot={{
                                        HorizontalAlignment: EHorizontalAlignment.HAlign_Right,
                                        Size: { SizeRule: ESlateSizeRule.Fill }
                                    }}
                                    Font={{
                                        FontObject: Font.Load('/Game/UI/Font/SourceHanSansSC'),
                                        Size: 16,
                                    }}
                                    Text={v}
                                />
                            </span>)
                        ),
                            <uImage
                                Slot={{ Padding: Utils.ltrb(0, 5) }}
                                Brush={{ ImageSize: {X: 32, Y: 2} }}
                                ColorAndOpacity={Utils.rgba(1, 1, 1, 0.5)}
                            />
                        ]
                    )}
                </TaggedCard>}

                <uBorder
                    Slot={{
                        LayoutData: {
                            Anchors: EAnchors.Center,
                            Alignment: { X: 0.5, Y: 0.5 },
                            Offsets: Utils.ltrb(0, 0, 512, 512)
                        }
                    }}
                    HorizontalAlignment={ EHorizontalAlignment.HAlign_Center }
                    VerticalAlignment={ EVerticalAlignment.VAlign_Center }
                    Background={{
                        ResourceObject: Texture2D.Load('/Game/UI/Icon/T_Base512x512'),
                        TintColor: { SpecifiedColor: Utils.rgba(0, 0, 0, 0.3) }
                    }}
                >
                { show_demand ? 
                    <div
                        Slot={{
                            HorizontalAlignment: EHorizontalAlignment.HAlign_Fill,
                            VerticalAlignment: EVerticalAlignment.VAlign_Fill
                        }}
                    >
                        <span>
                            <text
                                Slot={{
                                    VerticalAlignment: EVerticalAlignment.VAlign_Center
                                }}
                                Font={{
                                    FontObject: Font.Load('/Game/UI/Font/SourceHanSansSC'),
                                    TypefaceFontName: "Bold",
                                    Size: 18,
                                }}
                                Text={"制作"}
                            />
                            <uImage
                                Slot={{
                                    Padding: Utils.ltrb(10, 0, 0, 0),
                                    HorizontalAlignment: EHorizontalAlignment.HAlign_Center,
                                    VerticalAlignment: EVerticalAlignment.VAlign_Center
                                }}
                                Brush={{
                                    ResourceObject: Texture2D.Load(sel_data.Icon),
                                    ImageSize: {X: 32, Y: 32}
                                }}
                            />
                            <text
                                Slot={{
                                    Padding: Utils.ltrb(10, 0),
                                    Size: { SizeRule: ESlateSizeRule.Fill },
                                    VerticalAlignment: EVerticalAlignment.VAlign_Center
                                }}
                                Font={{
                                    FontObject: Font.Load('/Game/UI/Font/SourceHanSansSC'),
                                    Size: 18,
                                }}
                                ColorAndOpacity={{ SpecifiedColor: Utils.color("#CCF") }}
                                Text={sel_data.Name}
                            />
                            <uSizeBox
                                WidthOverride={120}
                                HeightOverride={50}
                            >
                            <uSpinBox
                                ref={elem => {
                                    if (elem && !this.uCountBox) {
                                        this.uCountBox = elem.ueobj
                                    }
                                }}
                                Slot={{
                                    Size: {
                                        SizeRule: ESlateSizeRule.Fill,
                                        Value: 0.5
                                    }
                                }}
                                {...SpinBoxProps}
                                MaxValue={1}
                                OnValueChanged={ InValue => {
                                    this.setState({ makeCount: InValue })
                                }}
                            />
                            </uSizeBox>
                        </span>
                        <span
                            Slot={{
                                Padding: Utils.ltrb(0, 10)
                            }}
                        >
                            <text
                                Font={{
                                    FontObject: Font.Load('/Game/UI/Font/SourceHanSansSC'),
                                    TypefaceFontName: "Bold",
                                    Size: 14,
                                }}
                                ColorAndOpacity={{ SpecifiedColor: Utils.color("#888") }}
                                Text={"所需资源"}
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
                    {_.map(sel_data.DemandList, (val, key) => {
                        const item_data = MainFunctionLibrary.GetItemData(key)
                        const hold_count = this.helper.GetHoldCount(key)
                        const need_count = val * this.state.makeCount
                        return (
                            <span
                                Slot={{
                                    Padding: Utils.ltrb(10, 5)
                                }}
                            >
                                <uImage
                                    Slot={{
                                        HorizontalAlignment: EHorizontalAlignment.HAlign_Center,
                                        VerticalAlignment: EVerticalAlignment.VAlign_Center
                                    }}
                                    Brush={{
                                        ResourceObject: Texture2D.Load(item_data.Icon),
                                        ImageSize: {X: 32, Y: 32}
                                    }}
                                />
                                <text
                                    Slot={{
                                        Padding: Utils.ltrb(5, 0),
                                        Size: { SizeRule: ESlateSizeRule.Fill, Value: 1.0 },
                                        VerticalAlignment: EVerticalAlignment.VAlign_Center
                                    }}
                                    Font={{
                                        FontObject: Font.Load('/Game/UI/Font/SourceHanSansSC'),
                                        Size: 16,
                                    }}
                                    ColorAndOpacity={{
                                        SpecifiedColor: Utils.color("#CFF")
                                    }}
                                    Text={item_data.Name} 
                                />
                                <span
                                    Slot={{
                                        Size: { SizeRule: ESlateSizeRule.Fill, Value: 0.2 }
                                    }}
                                >
                                    <uImage
                                        Slot={{
                                            HorizontalAlignment: EHorizontalAlignment.HAlign_Center,
                                            VerticalAlignment: EVerticalAlignment.VAlign_Center
                                        }}
                                        Brush={{
                                            ResourceObject: Texture2D.Load('/Game/UI/Icon/T_Storehouse32x32'),
                                            ImageSize: {X: 32, Y: 32}
                                        }}
                                    />
                                    <text
                                        Slot={{
                                            VerticalAlignment: EVerticalAlignment.VAlign_Center
                                        }}
                                        Font={{
                                            FontObject: Font.Load('/Game/UI/Font/SourceHanSansSC'),
                                            Size: 16,
                                        }}
                                        ColorAndOpacity={{ SpecifiedColor: hold_count >= need_count ? Utils.color("#5F5") : Utils.color("#F55") }}
                                        Text={` ${hold_count}`}    
                                    />
                                </span>
                                <text
                                    Slot={{
                                        Size: { SizeRule: ESlateSizeRule.Fill, Value: 0.2 },
                                        HorizontalAlignment: EHorizontalAlignment.HAlign_Right,
                                        VerticalAlignment: EVerticalAlignment.VAlign_Center
                                    }}
                                    Font={{
                                        FontObject: Font.Load('/Game/UI/Font/SourceHanSansSC'),
                                        Size: 16,
                                    }}
                                    ColorAndOpacity={{
                                        SpecifiedColor: Utils.color("#AAA")
                                    }}
                                    Text={`×${need_count}`}    
                                />
                            </span>
                        )
                    })}
                        <uSpacer Slot={{ Size: { SizeRule: ESlateSizeRule.Fill, Value: 1.0 }}} />
                        <uSizeBox
                            Slot={{
                                HorizontalAlignment: EHorizontalAlignment.HAlign_Center
                            }}
                            WidthOverride={300}
                            HeightOverride={50}
                        >
                        <uButton
                            Visibility={max_make > 0 ? ESlateVisibility.Visible : ESlateVisibility.HitTestInvisible}
                            WidgetStyle={{
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
                            }}
                            IsFocusable={false}
                            OnPressed={() => {
                                this.makedAnime = AD()
                                let completed = () => {
                                    if (this.makedAnime) {
                                        const cur = this.uCountBox.GetValue()
                                        this.helper.MakeItem(sel_item, cur)
                                        this.updateCountBtn()
                                        this.uBtnBg.SetRenderScale({X: 0, Y: 1.0})
                                        this.setState({makeCount: 1})
                                    }
                                }
                                this.makedAnime.apply(this.uBtnBg, 
                                    { duration: 0.5, completed: completed }, 
                                    { RenderScale: t => { return {X: t, Y: 1.0} } }
                                )
                            }}
                            OnReleased={() => {
                                this.makedAnime.destroy()
                                this.makedAnime = null
                                this.uBtnBg.SetRenderScale({X: 0, Y: 1.0})
                            }}
                        >
                            <uCanvasPanel
                                Slot={{
                                    Padding: Utils.ltrb(0),
                                    HorizontalAlignment: EHorizontalAlignment.HAlign_Fill,
                                    VerticalAlignment: EVerticalAlignment.VAlign_Fill
                                }}
                            >
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
                                    RenderTransform={{Scale: {X: 0, Y: 1.0}}}
                                />
                                <uTextBlock
                                    Slot={{
                                        LayoutData: {
                                            Anchors: EAnchors.Center,
                                            Alignment: { X: 0.5, Y: 0.5 },
                                        },
                                        bAutoSize: true
                                    }}
                                    Font={{
                                        FontObject: Font.Load('/Game/UI/Font/SourceHanSansSC'),
                                        TypefaceFontName: "Bold",
                                        Size: 18,
                                    }}
                                    ColorAndOpacity={{ SpecifiedColor: btn_color }}
                                    Text={ max_make ? "制作" : "资源不足" }
                                />
                            </uCanvasPanel>
                        </uButton>
                        </uSizeBox>
                    </div> :
                    <uBorder
                        Background={{
                            DrawAs: ESlateBrushDrawType.Border,
                            TintColor: { SpecifiedColor: this.earthBase ? Utils.color("#8F8") : Utils.color("#F88") },
                            Margin: Utils.ltrb(0, 1, 0, 1)
                        }}
                        ContentColorAndOpacity={ this.earthBase ? Utils.color("#5F5") : Utils.color("#F55") }
                    >
                        <text
                            Font={{
                                FontObject: Font.Load('/Game/UI/Font/SourceHanSansSC'),
                                Size: 18,
                            }}
                            Text={ this.earthBase ? "已连接基地" : "未连接基地" }
                        />
                    </uBorder>
                }
                </uBorder>

                {this.earthBase &&
                <TaggedCard
                    Slot={{
                        LayoutData: {
                            Anchors: EAnchors.Right,
                            Alignment: { X: 1.0, Y: 0.5 },
                            Offsets: Utils.ltrb(0, 0, 460, 400)
                        }
                    }}
                    title={"可加工物品"}
                >
                    <uJavascriptListView
                        ref={elem => {
                            if (elem && !this.uMakeableList) {
                                this.uMakeableList = elem.ueobj
                                this.uMakeableList.SetItems(this.helper.GetMakeableList().OutItems)
                                process.nextTick(_ => this.uMakeableList.SetScrollbarVisibility(ESlateVisibility.Visible))
                            }
                        }}
                        Slot={{
                            Size: {SizeRule: ESlateSizeRule.Fill}
                        }}
                        SelectionMode={ESelectionMode.Single}
                        OnGenerateRow={(item, view) => {
                            const index = view.GetIndexForItem(item)
                            const item_data = MainFunctionLibrary.GetItemData(item.RowName)
                            const is_hovered = this.state.hoveredIndex === index
                            const is_selected = this.state.selectedIndex === index
                            return ReactUMG.wrap(
                                <uSizeBox
                                    Slot={{
                                        Padding: Utils.ltrb(0, 2),
                                    }}
                                    HeightOverride={64}
                                    ToolTipWidgetDelegate={() => ReactUMG.wrap(<ItemToolTip itemData={item_data} />)}
                                >
                                <uMakeableEntry index={index} >
                                <uCanvasPanel>
                                    <span
                                        Slot={{
                                            LayoutData: {
                                                Anchors: EAnchors.FillAll,
                                                Offsets: Utils.ltrb(0),
                                            }
                                        }}
                                    >
                                        <uBorder
                                            Slot={{
                                                HorizontalAlignment: EHorizontalAlignment.HAlign_Left
                                            }}
                                            Background={{
                                                TintColor: { SpecifiedColor: is_hovered ? Utils.rgba(0.7, 0.7, 0.7, 0.4) : Utils.rgba(0.4, 0.4, 0.4, 0.4) }
                                            }}
                                        >
                                            <uImage
                                                Slot={{
                                                    Padding: Utils.ltrb(0)
                                                }}
                                                Brush={{
                                                    // Icon变成了string类型
                                                    ResourceObject: Texture2D.Load(item_data.Icon),
                                                    ImageSize: {X: 64, Y: 64}
                                                }}
                                            />
                                        </uBorder>
                                        <uBorder
                                            Slot={{
                                                Size: { SizeRule: ESlateSizeRule.Fill },
                                                HorizontalAlignment: EHorizontalAlignment.HAlign_Fill,
                                                VerticalAlignment: EVerticalAlignment.VAlign_Fill,
                                            }}
                                            Background={{
                                                TintColor: { SpecifiedColor: is_hovered ? Utils.rgba(0.8, 0.8, 0.8, 0.4) : Utils.rgba(0.5, 0.5, 0.5, 0.4) }
                                            }}
                                            VerticalAlignment={EVerticalAlignment.VAlign_Center}
                                        >
                                            <text
                                                Slot={{
                                                    Padding: Utils.ltrb(20, 0),
                                                }}
                                                Font={{
                                                    FontObject: Font.Load('/Game/UI/Font/SourceHanSansSC'),
                                                    TypefaceFontName: "Bold",
                                                    Size: 14,
                                                }}
                                                ColorAndOpacity={{
                                                    SpecifiedColor: Utils.color("#FFF")
                                                }}
                                                Text={item_data.Name} 
                                            />
                                        </uBorder>
                                    </span>
                                    <uImage
                                        Slot={{
                                            LayoutData: {
                                                Anchors: EAnchors.FillBottom,
                                                Offsets: Utils.ltrb(0, 0, 0, is_selected ? 4 : 0),
                                                Alignment: { X: 0, Y: 1.0 }
                                            }
                                        }}
                                    />
                                </uCanvasPanel>
                                </uMakeableEntry>
                                </uSizeBox>
                            )
                        }}
                    />
                </TaggedCard>}
            </uCanvasPanel>
        )
    }
}


module.exports = Menu