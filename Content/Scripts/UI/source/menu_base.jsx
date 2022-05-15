const _ = require('lodash')
const Events = require('events')
const AD = require('animation-driver')
const UClass = require('uclass')()
const React = require('react')
const ReactUMG = require('react-umg')
const Utils = require('../utils')
const EAnchors = require('../anchors')

let event = null

class MyCard extends React.Component {
    constructor(props) {
        super(props)
    }
    render() {
        let { title } = this.props
        title = title || "Title"
        return (
            <div {...this.props} >
                <uTextBlock
                    Slot={{
                        Padding: Utils.ltrb(0, 10)
                    }}
                    Font={{
                        FontObject: Font.Load('/Game/UI/Font/SourceHanSansSC'),
                        TypefaceFontName: "Bold",
                        Size: 18,
                    }}
                    Text={title}
                />
                {this.props.children}
            </div>
        )
    }
}

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
let MakeableEntry_C = UClass(global, MakeableEntry)
ReactUMG.Register("uMakeableEntry", MakeableEntry_C)

class MenuBase extends React.Component {
    constructor(props) {
        super(props)

        this.inBase = !!MainLevelScriptActor.GetEarthBase()
        this.helper = new MenuBaseHelper()
        this.state = {
            status: this.getStatus(),
            hoveredIndex: -1,
            selectedIndex: -1,
            makeCount: 1
        }
        setInterval(() => {
            let status = this.getStatus()
            if (JSON.stringify(this.state.status) !== JSON.stringify(status)) {
                this.setState({status: status})
            }
        }, 500)

        //let bp = MainLevelScriptActor.GetEarthBase().Backpack
        //bp.AddItem("0000", 100)
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
            }
        })
    }

    componentWillUnmount() {
        event = null
    }

    render() {
        const show_demand = this.inBase && this.state.selectedIndex > -1
        const sel_item = this.state.selectedIndex > -1 ? this.uMakeableList.GetItemAt(this.state.selectedIndex) : {RowName: "1000"}
        const sel_data = MainFunctionLibrary.GetItemData(sel_item.RowName)
        const max_make = this.helper.GetMaxMakeableCount(sel_item.RowName)
        return (
            <uCanvasPanel>
                {this.inBase &&
                <MyCard
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
                </MyCard>}

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
                                WidthOverride={100}
                            >
                            <uSpinBox
                                ref={elem => {
                                    if (elem) {
                                        this.uCountBox = elem.ueobj
                                    }
                                }}
                                Slot={{
                                    Size: {
                                        SizeRule: ESlateSizeRule.Fill,
                                        Value: 0.5
                                    }
                                }}
                                MinValue={1}
                                MaxValue={max_make > 0 ? max_make : 1}
                                MinFractionalDigits={0}
                                MaxFractionalDigits={0}
                                Delta={1}
                                ClearKeyboardFocusOnCommit={true}
                                bAlwaysUsesDeltaSnap={true}
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
                                    Size: { SizeRule: ESlateSizeRule.Fill, Value: 1.0 },
                                    VerticalAlignment: EVerticalAlignment.VAlign_Center
                                }}
                                Brush={{ ImageSize: {X: 32, Y: 2} }}
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
                                    TintColor: { SpecifiedColor: Utils.color("#FFF") },
                                    Margin: Utils.ltrb(1)
                                },
                                Hovered: {
                                    DrawAs: ESlateBrushDrawType.Border,
                                    TintColor: { SpecifiedColor: Utils.color("#FFF") },
                                    Margin: Utils.ltrb(1)
                                },
                                Pressed: {
                                    DrawAs: ESlateBrushDrawType.Border,
                                    TintColor: { SpecifiedColor: Utils.color("#FFF") },
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
                                        const max = this.helper.GetMaxMakeableCount(sel_item.RowName)
                                        if (cur > max) {
                                            this.uCountBox.SetValue(max > 0 ? max : 1)
                                        }
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
                                    ColorAndOpacity={{ SpecifiedColor: Utils.color(max_make > 0 ? "#55F" : "#F55") }}
                                    Text={ max_make ? "制作" : "资源不足" }
                                />
                            </uCanvasPanel>
                        </uButton>
                        </uSizeBox>
                    </div> :
                    <uBorder
                        Background={{
                            DrawAs: ESlateBrushDrawType.Border,
                            TintColor: { SpecifiedColor: this.inBase ? Utils.color("#8F8") : Utils.color("#F88") },
                            Margin: Utils.ltrb(0, 1, 0, 1)
                        }}
                        ContentColorAndOpacity={ this.inBase ? Utils.color("#5F5") : Utils.color("#F55") }
                    >
                        <text
                            Font={{
                                FontObject: Font.Load('/Game/UI/Font/SourceHanSansSC'),
                                Size: 18,
                            }}
                            Text={ this.inBase ? "已连接基地" : "未连接基地" }
                        />
                    </uBorder>
                }
                </uBorder>

                {this.inBase &&
                <MyCard
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
                                this.uMakeableList.SetItems(this.helper.GetuMakeableList().OutItems)
                            }
                        }}
                        Slot={{
                            Size: {SizeRule: ESlateSizeRule.Fill}
                        }}
                        SelectionMode={ESelectionMode.Single}
                        OnGenerateRow={(item, view) => {
                            let index = view.GetIndexForItem(item)
                            let item_data = MainFunctionLibrary.GetItemData(item.RowName)
                            let is_hovered = this.state.hoveredIndex === index
                            let is_selected = this.state.selectedIndex === index
                            return ReactUMG.wrap(
                                <uSizeBox
                                    Slot={{
                                        Padding: Utils.ltrb(0, 2),
                                    }}
                                    HeightOverride={64}
                                    ToolTipWidgetDelegate={() => {
                                        return ReactUMG.wrap(
                                            <uBorder
                                                Slot={{ Padding: Utils.ltrb(0) }}
                                                Padding={Utils.ltrb(20)}
                                                Background={{
                                                    TintColor: { SpecifiedColor: Utils.rgba(1, 1, 1, 0.3) }
                                                }}
                                            >
                                                <div>
                                                    <span>
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
                                                        <div>
                                                            <text
                                                                Slot={{
                                                                    Padding: Utils.ltrb(20, 0),
                                                                }}
                                                                Font={{
                                                                    FontObject: Font.Load('/Game/UI/Font/SourceHanSansSC'),
                                                                    TypefaceFontName: "Bold",
                                                                    Size: 16,
                                                                }}
                                                                ColorAndOpacity={{
                                                                    SpecifiedColor: Utils.color("#111")
                                                                }}
                                                                Text={item_data.Name} 
                                                            />
                                                            <text
                                                                Slot={{
                                                                    Padding: Utils.ltrb(20, 5),
                                                                }}
                                                                Font={{
                                                                    FontObject: Font.Load('/Game/UI/Font/SourceHanSansSC'),
                                                                    TypefaceFontName: "Bold",
                                                                    Size: 12,
                                                                }}
                                                                ColorAndOpacity={{
                                                                    SpecifiedColor: Utils.color("#333")
                                                                }}
                                                                Text={`${Math.round(item_data.Mass*100)/100}kg`} 
                                                            />
                                                        </div>
                                                    </span>
                                                    <uImage
                                                        Slot={{ Padding: Utils.ltrb(0, 10) }}
                                                        Brush={{ ImageSize: {X: 32, Y: 2} }}
                                                        ColorAndOpacity={Utils.rgba(0, 0, 0, 0.1)}
                                                    />
                                                    <text
                                                        Font={{
                                                            FontObject: Font.Load('/Game/UI/Font/SourceHanSansSC'),
                                                            TypefaceFontName: "Bold",
                                                            Size: 14,
                                                        }}
                                                        WrapTextAt={300}
                                                        ColorAndOpacity={{
                                                            SpecifiedColor: Utils.color("#222")
                                                        }}
                                                        Text={item_data.Desc} 
                                                    />
                                                </div>
                                            </uBorder>
                                        )
                                    }}
                                >
                                <uMakeableEntry index={index} >
                                <uCanvasPanel>
                                    <span
                                        Slot={{
                                            LayoutData: {
                                                Anchors: EAnchors.FillAll,
                                                Offsets: Utils.ltrb(0, 0, 0, 0),
                                            }
                                        }}
                                    >
                                        <uBorder
                                            Slot={{
                                                HorizontalAlignment: EHorizontalAlignment.HAlign_Left
                                            }}
                                            Background={{
                                                TintColor: { SpecifiedColor: Utils.rgba(1, 1, 1, is_hovered ? 0.6 : 0.5) }
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
                                                TintColor: { SpecifiedColor: Utils.rgba(1, 1, 1, is_hovered ? 0.5 : 0.3) }
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
                </MyCard>}
            </uCanvasPanel>
        )
    }
}


module.exports = MenuBase