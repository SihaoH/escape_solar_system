const _ = require('lodash')
const Events = require('events')
const UClass = require('uclass')()
const React = require('react')
const ReactUMG = require('react-umg')
const Utils = require('../utils')
const EAnchors = require('../anchors')
const {F_Sans} = require('../style')

const PointBar = require('./point_bar')
const ContextMenu = require('context_menu')
const NumberDialog = require('number_dialog')
const TaggedCard = require('tagged_card')
const ItemToolTip = require('item_tooltip')

const ETitle = {
    Char: Utils.tr("躯体"),
    Base: Utils.tr("基地"),
    Ship: Utils.tr("飞船")
}
let MenuDisplay = null

const BpGetter = title => {
    const char = MainLevelScript.GetMainChar()
    const ship = MainLevelScript.GetSpaceship()
    const base = MainLevelScript.GetEarthBase()
    switch (title) {
    case ETitle.Char:
        // 主角的背包何时都在
        return char.Backpack
    case ETitle.Base:
        // 需要主角在基地才能有基地的存储
        return base.FindMainChar() && base.Backpack
    case ETitle.Ship:
        // 主角在飞船附近 或者 主角和飞船都在基地
        return char.FindSpaceship() || (base.FindMainChar() && base.FindSpaceship()) ? ship.Backpack : null
    }
}

let helper = null
let event = null

class BpEntry extends JavascriptWidget {
    properties() {
        // 不知道为啥，有的会自动改变首字母大小写
        this.index/*int*/
        this.Owner/*string*/
        
        // 由于无法传递object属性，所以只能用一个个基本类型来传
        this.RowName/*string*/
        this.Name/*string*/
        this.Icon/*string*/
        this.Count/*int*/
    }
    OnMouseEnter(MyGeometry, MouseEvent) {
        event.emit("MouseEnter", this.Owner, this.index)
    }
    OnMouseLeave(MouseEvent) {
        event.emit("MouseLeave", -1)
    }
    OnMouseButtonDown(MyGeometry, MouseEvent) {
        const key_name = MouseEvent.PointerEvent_GetEffectingButton().KeyName
        event.emit("MouseClick", this.Owner, this.index)
        if (key_name === "LeftMouseButton") { // 左键拖动
            return MouseEvent.DetectDragIfPressed(this, { KeyName: "LeftMouseButton" })
        } else if (key_name === "RightMouseButton") { // 右键菜单选项
            const options = helper.GetMenuOptions().OutList
            const display = _.map(options, val => MenuDisplay[val])
            ContextMenu.open(MouseEvent.PointerEvent_GetScreenSpacePosition(), display, idx => {
                switch (options[idx]) {
                case EItemOptions.Use:
                    break;
                case EItemOptions.ConsumeToChar:
                    NumberDialog.open(helper.SelectedItem, display[idx], count => {
                        helper.ConsumeItem(count, EPawnType.MainChar)
                    })
                    break;
                case EItemOptions.ConsumeToShip:
                    NumberDialog.open(helper.SelectedItem, display[idx], count => {
                        helper.ConsumeItem(count, EPawnType.SpaceShip)
                    })
                    break;
                case EItemOptions.Discard:
                    NumberDialog.open(helper.SelectedItem, display[idx], count => {
                        helper.DiscardItem(count)
                    })
                    break;
                }
            })
            return EventReply.Handled()
        }
        return EventReply.Unhandled()
    }
    OnDragDetected(Geometry, PointerEvent) {
        let oper = WidgetBlueprintLibrary.CreateDragDropOperation(DragDropOperation)
        const widget = ReactUMG.wrap(
            <uSizeBox
                WidthOverride={400}
                HeightOverride={64}
            >
                <BpEntryContent
                    name={this.Name}
                    icon={this.Icon}
                    count={this.Count}
                />
            </uSizeBox>
        )
        oper.Payload = this
        oper.DefaultDragVisual = widget
        return { Operation: oper }
    }
}
ReactUMG.Register("uBpEntry", UClass(global, BpEntry))

class BpEntryContent extends React.Component {
    constructor(props) {
        super(props)
    }

    render() {
        let {name, icon, count, isHovered} = this.props
        isHovered = isHovered || false
        return (
            <uCanvasPanel {...this.props}>
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
                            TintColor: { SpecifiedColor: isHovered ? Utils.rgba(0.7, 0.7, 0.7, 0.4) : Utils.rgba(0.4, 0.4, 0.4, 0.4) }
                        }}
                    >
                        <uImage
                            Slot={{
                                Padding: Utils.ltrb(0)
                            }}
                            Brush={{
                                // Icon变成了string类型
                                ResourceObject: Texture2D.Load(icon),
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
                            TintColor: { SpecifiedColor: isHovered ? Utils.rgba(0.8, 0.8, 0.8, 0.4) : Utils.rgba(0.5, 0.5, 0.5, 0.4) }
                        }}
                        VerticalAlignment={EVerticalAlignment.VAlign_Center}
                    >
                        <span>
                            <uTextBlock
                                Slot={{
                                    Padding: Utils.ltrb(20, 0),
                                    Size: { SizeRule: ESlateSizeRule.Fill }
                                }}
                                Font={{
                                    FontObject: F_Sans,
                                    TypefaceFontName: "Bold",
                                    Size: 14,
                                }}
                                ColorAndOpacity={{
                                    SpecifiedColor: Utils.color("#FFF")
                                }}
                                Text={name} 
                            />
                            <uTextBlock
                                Slot={{
                                    Padding: Utils.ltrb(20, 0),
                                }}
                                Font={{
                                    FontObject: F_Sans,
                                    TypefaceFontName: "Bold",
                                    Size: 14,
                                }}
                                ColorAndOpacity={{
                                    SpecifiedColor: Utils.color("#FFF")
                                }}
                                Text={count}
                            />
                        </span>
                    </uBorder>
                </span>
            </uCanvasPanel>
        )
    }
}

class BpView extends JavascriptWidget {
    properties() {
        this.Title/*string*/
    }
    OnDrop(Geometry, UPointerEvent, Operation) {
        const payload= Operation.Payload
        if (payload.Owner !== this.Title) {
            event.emit("ItemDrop", payload, this.Title)
        }
        return true
    }
}
ReactUMG.Register("uBpView", UClass(global, BpView))

class BackpackView extends React.Component {
    constructor(props) {
        super(props)
    }

    componentDidMount() {
        event.on("ItemDrop", (payload, target) => {
            if (target === this.props.title) {
                const ret = helper.TryDropItem(BpGetter(target), BpGetter(payload.Owner), payload.Count)
                if (ret.NeedHint) {
                    NumberDialog.open(helper.SelectedItem, Utils.tr("放入"), count => {
                        helper.DropItem(count)
                    }, ret.MaxAdd)
                } else {
                    MainLibrary.SendPrompt(ret.Reason, EPromptType.Error)
                }
            }
        })
        event.on("MouseClick", (owner, idx) => {
            if (owner === this.props.title) {
                const item = this.uListView.GetItemAt(idx)
                helper.SelectItem(item)
            }
        })
    }

    componentWillUnmount() {
        this.props.bp.ChangedDelegate = null
    }

    render() {
        const {title, hoveredIndex} = this.props
        return (
            <uBpView
                {...this.props}
                Slot={{
                    Padding: Utils.ltrb(10, 0)
                }}
                Title={title}
            >
            <uSizeBox
                WidthOverride={420}
                HeightOverride={720}
            >
            <TaggedCard title={title} >
                <uSizeBox
                    Slot={{
                        Padding: Utils.ltrb(0, 0, 16, 10)
                    }}
                    HeightOverride={10}
                >
                    <PointBar
                        fontStyle={{
                            FontObject: F_Sans,
                            Size: 12,
                            OutlineSettings: {
                                OutlineSize: 1,
                                OutlineColor: Utils.rgba(0, 0, 0, 0.6)
                            }
                        }}
                        colorWhenAdded={Utils.color("#F00")}
                        colorWhenReduced={Utils.color("#0F0")}
                        curVal={this.props.bp.Mass}
                        maxVal={this.props.bp.MaxLoad}
                        fractionDigits={1}
                    />
                </uSizeBox>
                <uJavascriptListView
                    ref={elem => {
                        if (elem) {
                            if (this.uListView) {
                                this.uListView.RegenerateAllEntries()
                            } else {
                                this.uListView = elem.ueobj
                                let reflush = () => {
                                    const items = helper.GetListViewItems(this.props.bp).OutItems
                                    this.uListView.SetItems(items)
                                    this.forceUpdate()
                                }
                                this.props.bp.ChangedDelegate.Add(reflush)
                                reflush()
                                
                                process.nextTick(_ => this.uListView.SetScrollbarVisibility(ESlateVisibility.Visible))
                            }
                        }
                    }}
                    Slot={{
                        Size: {SizeRule: ESlateSizeRule.Fill}
                    }}
                    SelectionMode={ESelectionMode.Single}
                    OnGenerateRow={(item, view) => {
                        const index = view.GetIndexForItem(item)
                        const item_data = MainLibrary.GetItemData(item.RowName)
                        const is_hovered = (hoveredIndex === index)
                        return ReactUMG.wrap(
                            <uSizeBox
                                Slot={{
                                    Padding: Utils.ltrb(0, 2),
                                }}
                                HeightOverride={64}
                                ToolTipWidgetDelegate={() => ReactUMG.wrap(<ItemToolTip itemData={item_data} />)}
                            >
                                <uBpEntry
                                    index={index}
                                    Owner={title}
                                    RowName={item.RowName}
                                    Name={item_data.Name}
                                    Icon={item_data.Icon}
                                    Count={item.Count}
                                >
                                    <BpEntryContent
                                        name={item_data.Name}
                                        icon={item_data.Icon}
                                        count={item.Count}
                                        isHovered={is_hovered}
                                    />
                                </uBpEntry>
                            </uSizeBox>
                        )
                    }}
                />
            </TaggedCard>
            </uSizeBox>
            </uBpView>
        )
    }
}
class Menu extends React.Component {
    constructor(props) {
        super(props)

        helper = new MenuBackpackHelper()
        event = new Events.EventEmitter()
        MenuDisplay = helper.GetMenuDisplay().OutList
        this.charBp = BpGetter(ETitle.Char)
        this.baseBp = BpGetter(ETitle.Base)

        this.char = MainLevelScript.GetMainChar()
        this.char.Body.HpChangedDelegate = Delta => this.setState({ charHP: {cur: this.char.Body.CurrentHP, max: this.char.Body.MaximumHP} })
        this.char.Engine.EnergyChangedDelegate = Delta => this.setState({ charHP: {cur: this.char.Engine.CurrentEnergy, max: this.char.Engine.MaximumEnergy} })

        this.handleShip = () => {
            this.shipBp = BpGetter(ETitle.Ship)
            this.ship = MainLevelScript.GetEarthBase().FindSpaceship()
            if (this.ship) {
                this.ship.Body.HpChangedDelegate = Delta => this.setState({ shipHP: {cur: this.ship.Body.CurrentHP, max: this.ship.Body.MaximumHP} })
                this.ship.Engine.EnergyChangedDelegate = Delta => this.setState({ shipMP: {cur: this.ship.Engine.CurrentEnergy, max: this.ship.Engine.MaximumEnergy} })
            }
        }
        this.handleShip()
        MainLevelScript.Instance().ShipChangedDelegate.Add(this.handleShip)

        this.state = {
            hoveredView: "",
            hoveredIndex: -1,
            charHP: {cur: this.char.Body.CurrentHP, max: this.char.Body.MaximumHP},
            charMP: {cur: this.char.Engine.CurrentEnergy, max: this.char.Engine.MaximumEnergy},
            shipHP: {cur: this.ship ? this.ship.Body.CurrentHP : 0, max: this.ship ? this.ship.Body.MaximumHP : 0},
            shipMP: {cur: this.ship ? this.ship.Engine.CurrentEnergy : 0, max: this.ship ? this.ship.Engine.MaximumEnergy : 1},
        }
    }

    componentDidMount() {
        event.on("MouseEnter", (owner, idx) => {
            if (this.state.hoveredView !== owner && this.state.hoveredIndex !== idx) {
                this.setState({ hoveredView: owner, hoveredIndex: idx })
            }
        })
        event.on("MouseLeave", (owner, idx) => {
            if (this.state.hoveredIndex !== -1) {
                this.setState({ hoveredView: "", hoveredIndex: -1 })
            }
        })
    }

    componentWillUnmount() {
        helper = null
        event = null
        const char = MainLevelScript.GetMainChar()
        const ship = MainLevelScript.GetSpaceship()
        if (char) {
            char.Body.HpChangedDelegate = null
            char.Engine.EnergyChangedDelegate = null
        }
        if (ship) {
            ship.Body.HpChangedDelegate = null
            ship.Engine.EnergyChangedDelegate = null
        }
    }

    render() {
        return (
            <uCanvasPanel>
                <span
                    Slot={{
                        LayoutData: {
                            Anchors: EAnchors.Left,
                            Alignment: { X: 0, Y: 0.5 },
                            Offsets: Utils.ltrb(0, 0, 0, 720)
                        }
                    }}
                >
                    <BackpackView
                        title={ETitle.Char}
                        bp={this.charBp}
                        hoveredIndex={ this.state.hoveredView === ETitle.Char ? this.state.hoveredIndex : -1 }
                    />
                    {!!this.baseBp &&
                    <BackpackView
                        title={ETitle.Base}
                        bp={this.baseBp}
                        hoveredIndex={ this.state.hoveredView === ETitle.Base ? this.state.hoveredIndex : -1 }
                    />}
                    {!!this.shipBp &&
                    <BackpackView
                        title={ETitle.Ship}
                        bp={this.shipBp}
                        hoveredIndex={ this.state.hoveredView === ETitle.Ship ? this.state.hoveredIndex : -1 }
                    />}
                </span>
                <div
                    Slot={{
                        LayoutData: {
                            Anchors: EAnchors.Right,
                            Alignment: { X: 1, Y: 0.5 },
                            Offsets: Utils.ltrb(0, 0, 0, 0)
                        },
                        bAutoSize: true
                    }}
                >
                    <TaggedCard
                        title={Utils.tr("躯体状态")}
                    >
                        <uSizeBox
                            Slot={{
                                Padding: Utils.ltrb(0, 5)
                            }}
                            WidthOverride={420}
                            HeightOverride={20}
                        >
                            <PointBar
                                curVal={this.state.charHP.cur}
                                maxVal={this.state.charHP.max}
                            />
                        </uSizeBox>
                        <uSizeBox
                            Slot={{
                                Padding: Utils.ltrb(0, 5)
                            }}
                            HeightOverride={20}
                        >
                            <PointBar
                                curVal={this.state.charMP.cur}
                                maxVal={this.state.charMP.max}
                            />
                        </uSizeBox>
                    </TaggedCard>
                    {!!this.shipBp &&
                    <TaggedCard
                        Slot={{
                            Padding: Utils.ltrb(0, 30)
                        }}
                        title={Utils.tr("飞船状态")}
                    >
                        <uSizeBox
                            Slot={{
                                Padding: Utils.ltrb(0, 5)
                            }}
                            WidthOverride={420}
                            HeightOverride={20}
                        >
                            <PointBar
                                curVal={this.state.shipHP.cur}
                                maxVal={this.state.shipHP.max}
                            />
                        </uSizeBox>
                        <uSizeBox
                            Slot={{
                                Padding: Utils.ltrb(0, 5)
                            }}
                            HeightOverride={20}
                        >
                            <PointBar
                                curVal={this.state.shipMP.cur}
                                maxVal={this.state.shipMP.max}
                            />
                        </uSizeBox>
                    </TaggedCard>
                    }
                </div>
            </uCanvasPanel>
        )
    }
}


module.exports = Menu