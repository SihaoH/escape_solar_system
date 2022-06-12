var _extends = Object.assign || function (target) { for (var i = 1; i < arguments.length; i++) { var source = arguments[i]; for (var key in source) { if (Object.prototype.hasOwnProperty.call(source, key)) { target[key] = source[key]; } } } return target; };

const _ = require('lodash');
const Events = require('events');
const UClass = require('uclass')();
const React = require('react');
const ReactUMG = require('react-umg');
const Utils = require('../utils');
const EAnchors = require('../anchors');

const PointBar = require('./point_bar');
const ContextMenu = require('context_menu');
const NumberDialog = require('number_dialog');
const TaggedCard = require('tagged_card');
const ItemToolTip = require('item_tooltip');

const ETitle = {
    Char: "躯体",
    Base: "基地",
    Ship: "飞船"
};
let MenuDisplay = null;

const BpGetter = title => {
    switch (title) {
        case ETitle.Char:
            return MainLevelScriptActor.GetMainChar().Backpack;
        case ETitle.Base:
            return MainLevelScriptActor.GetEarthBase() && MainLevelScriptActor.GetEarthBase().Backpack;
        case ETitle.Ship:
            return MainLevelScriptActor.GetSpaceship() && MainLevelScriptActor.GetSpaceship().Backpack;
    }
};

let helper = null;
let event = null;

class BpEntry extends JavascriptWidget {
    properties() {
        // 不知道为啥，有的会自动改变首字母大小写
        this.index; /*int*/
        this.Owner; /*string*/

        // 由于无法传递object属性，所以只能用一个个基本类型来传
        this.RowName; /*string*/
        this.Name; /*string*/
        this.Icon; /*string*/
        this.Count; /*int*/
    }
    OnMouseEnter(MyGeometry, MouseEvent) {
        event.emit("MouseEnter", this.Owner, this.index);
    }
    OnMouseLeave(MouseEvent) {
        event.emit("MouseLeave", -1);
    }
    OnMouseButtonDown(MyGeometry, MouseEvent) {
        const key_name = MouseEvent.PointerEvent_GetEffectingButton().KeyName;
        event.emit("MouseClick", this.Owner, this.index);
        if (key_name === "LeftMouseButton") {
            // 左键拖动
            return MouseEvent.DetectDragIfPressed(this, { KeyName: "LeftMouseButton" });
        } else if (key_name === "RightMouseButton") {
            // 右键菜单选项
            const options = helper.GetMenuOptions().OutList;
            const display = _.map(options, val => MenuDisplay[val]);
            ContextMenu.open(MouseEvent.PointerEvent_GetScreenSpacePosition(), display, idx => {
                switch (options[idx]) {
                    case EItemOptions.Use:
                        break;
                    case EItemOptions.ConsumeToChar:
                        NumberDialog.open(helper.SelectedItem, "补充给躯体", count => {
                            helper.ConsumeItem(count, EPawnType.MainChar);
                        });
                        break;
                    case EItemOptions.ConsumeToShip:
                        NumberDialog.open(helper.SelectedItem, "补充给飞船", count => {
                            helper.ConsumeItem(count, EPawnType.SpaceShip);
                        });
                        break;
                    case EItemOptions.Discard:
                        NumberDialog.open(helper.SelectedItem, "丢弃", count => {
                            helper.DiscardItem(count);
                        });
                        break;
                }
            });
            return EventReply.Handled();
        }
        return EventReply.Unhandled();
    }
    OnDragDetected(Geometry, PointerEvent) {
        let oper = WidgetBlueprintLibrary.CreateDragDropOperation(DragDropOperation);
        const widget = ReactUMG.wrap(React.createElement(
            'uSizeBox',
            {
                WidthOverride: 400,
                HeightOverride: 64
            },
            React.createElement(BpEntryContent, {
                name: this.Name,
                icon: this.Icon,
                count: this.Count
            })
        ));
        oper.Payload = this;
        oper.DefaultDragVisual = widget;
        return { Operation: oper };
    }
}
ReactUMG.Register("uBpEntry", UClass(global, BpEntry));

class BpEntryContent extends React.Component {
    constructor(props) {
        super(props);
    }

    render() {
        let { name, icon, count, isHovered } = this.props;
        isHovered = isHovered || false;
        return React.createElement(
            'uCanvasPanel',
            this.props,
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
                            TintColor: { SpecifiedColor: isHovered ? Utils.rgba(0.7, 0.7, 0.7, 0.4) : Utils.rgba(0.4, 0.4, 0.4, 0.4) }
                        }
                    },
                    React.createElement('uImage', {
                        Slot: {
                            Padding: Utils.ltrb(0)
                        },
                        Brush: {
                            // Icon变成了string类型
                            ResourceObject: Texture2D.Load(icon),
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
                            TintColor: { SpecifiedColor: isHovered ? Utils.rgba(0.8, 0.8, 0.8, 0.4) : Utils.rgba(0.5, 0.5, 0.5, 0.4) }
                        },
                        VerticalAlignment: EVerticalAlignment.VAlign_Center
                    },
                    React.createElement(
                        'span',
                        null,
                        React.createElement('text', {
                            Slot: {
                                Padding: Utils.ltrb(20, 0),
                                Size: { SizeRule: ESlateSizeRule.Fill }
                            },
                            Font: {
                                FontObject: Font.Load('/Game/UI/Font/SourceHanSansSC'),
                                TypefaceFontName: "Bold",
                                Size: 14
                            },
                            ColorAndOpacity: {
                                SpecifiedColor: Utils.color("#FFF")
                            },
                            Text: name
                        }),
                        React.createElement('text', {
                            Slot: {
                                Padding: Utils.ltrb(20, 0)
                            },
                            Font: {
                                FontObject: Font.Load('/Game/UI/Font/SourceHanSansSC'),
                                TypefaceFontName: "Bold",
                                Size: 14
                            },
                            ColorAndOpacity: {
                                SpecifiedColor: Utils.color("#FFF")
                            },
                            Text: count
                        })
                    )
                )
            )
        );
    }
}

class BpView extends JavascriptWidget {
    properties() {
        this.Title; /*string*/
    }
    OnDrop(Geometry, UPointerEvent, Operation) {
        const payload = Operation.Payload;
        if (payload.Owner !== this.Title) {
            event.emit("ItemDrop", payload, this.Title);
        }
        return true;
    }
}
ReactUMG.Register("uBpView", UClass(global, BpView));

class BackpackView extends React.Component {
    constructor(props) {
        super(props);
    }

    componentDidMount() {
        event.on("ItemDrop", (payload, target) => {
            if (target === this.props.title) {
                const ret = helper.TryDropItem(BpGetter(target), BpGetter(payload.Owner), payload.Count);
                if (ret.NeedHint) {
                    NumberDialog.open(helper.SelectedItem, "放入", count => {
                        helper.DropItem(count);
                    }, ret.MaxAdd);
                } else {
                    // TODO 提示
                    console.error(ret.Reason);
                }
            }
        });
        event.on("MouseClick", (owner, idx) => {
            if (owner === this.props.title) {
                const item = this.uListView.GetItemAt(idx);
                helper.SelectItem(item);
            }
        });
    }

    componentWillUnmount() {
        this.props.bp.ChangedDelegate = null;
    }

    render() {
        const { title, hoveredIndex } = this.props;
        return React.createElement(
            'uBpView',
            _extends({}, this.props, {
                Slot: {
                    Padding: Utils.ltrb(10, 0)
                },
                Title: title
            }),
            React.createElement(
                'uSizeBox',
                {
                    WidthOverride: 420,
                    HeightOverride: 720
                },
                React.createElement(
                    TaggedCard,
                    { title: title },
                    React.createElement(
                        'uSizeBox',
                        {
                            Slot: {
                                Padding: Utils.ltrb(0, 0, 16, 10)
                            },
                            HeightOverride: 10
                        },
                        React.createElement(PointBar, {
                            fontStyle: {
                                FontObject: Font.Load('/Game/UI/Font/SourceHanSansSC'),
                                Size: 12,
                                OutlineSettings: {
                                    OutlineSize: 1,
                                    OutlineColor: Utils.rgba(0, 0, 0, 0.6)
                                }
                            },
                            colorWhenAdded: Utils.color("#F00"),
                            colorWhenReduced: Utils.color("#0F0"),
                            curVal: this.props.bp.Mass,
                            maxVal: this.props.bp.MaxLoad
                        })
                    ),
                    React.createElement('uJavascriptListView', {
                        ref: elem => {
                            if (elem) {
                                if (this.uListView) {
                                    this.uListView.RegenerateAllEntries();
                                } else {
                                    this.uListView = elem.ueobj;
                                    let reflush = () => {
                                        const items = helper.GetListViewItems(this.props.bp).OutItems;
                                        this.uListView.SetItems(items);
                                        this.forceUpdate();
                                    };
                                    this.props.bp.ChangedDelegate.Add(reflush);
                                    reflush();

                                    process.nextTick(_ => this.uListView.SetScrollbarVisibility(ESlateVisibility.Visible));
                                }
                            }
                        },
                        Slot: {
                            Size: { SizeRule: ESlateSizeRule.Fill }
                        },
                        SelectionMode: ESelectionMode.Single,
                        OnGenerateRow: (item, view) => {
                            const index = view.GetIndexForItem(item);
                            const item_data = MainFunctionLibrary.GetItemData(item.RowName);
                            const is_hovered = hoveredIndex === index;
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
                                    'uBpEntry',
                                    {
                                        index: index,
                                        Owner: title,
                                        RowName: item.RowName,
                                        Name: item_data.Name,
                                        Icon: item_data.Icon,
                                        Count: item.Count
                                    },
                                    React.createElement(BpEntryContent, {
                                        name: item_data.Name,
                                        icon: item_data.Icon,
                                        count: item.Count,
                                        isHovered: is_hovered
                                    })
                                )
                            ));
                        }
                    })
                )
            )
        );
    }
}
class Menu extends React.Component {
    constructor(props) {
        super(props);

        helper = new MenuBackpackHelper();
        event = new Events.EventEmitter();
        MenuDisplay = helper.GetMenuDisplay().OutList;
        this.charBp = BpGetter(ETitle.Char);
        this.baseBp = BpGetter(ETitle.Base);
        this.shipBp = BpGetter(ETitle.Ship);

        this.char = MainLevelScriptActor.GetMainChar();
        this.ship = MainLevelScriptActor.GetSpaceship();
        this.char.Body.HpChangedDelegate = Delta => this.setState({ charHP: { cur: this.char.Body.CurrentHP, max: this.char.Body.MaximumHP } });
        this.char.Engine.EnergyChangedDelegate = Delta => this.setState({ charHP: { cur: this.char.Engine.CurrentEnergy, max: this.char.Engine.MaximumEnergy } });
        if (this.ship) {
            this.ship.Body.HpChangedDelegate = Delta => this.setState({ shipHP: { cur: this.ship.Body.CurrentHP, max: this.ship.Body.MaximumHP } });
            this.ship.Engine.EnergyChangedDelegate = Delta => this.setState({ shipMP: { cur: this.ship.Engine.CurrentEnergy, max: this.ship.Engine.MaximumEnergy } });
        }

        this.state = {
            hoveredView: "",
            hoveredIndex: -1,
            charHP: { cur: this.char.Body.CurrentHP, max: this.char.Body.MaximumHP },
            charMP: { cur: this.char.Engine.CurrentEnergy, max: this.char.Engine.MaximumEnergy },
            shipHP: { cur: this.ship ? this.ship.Body.CurrentHP : 0, max: this.ship ? this.ship.Body.MaximumHP : 0 },
            shipMP: { cur: this.ship ? this.ship.Engine.CurrentEnergy : 0, max: this.ship ? this.ship.Engine.MaximumEnergy : 1 }
        };
    }

    componentDidMount() {
        event.on("MouseEnter", (owner, idx) => {
            if (this.state.hoveredView !== owner && this.state.hoveredIndex !== idx) {
                this.setState({ hoveredView: owner, hoveredIndex: idx });
            }
        });
        event.on("MouseLeave", (owner, idx) => {
            if (this.state.hoveredIndex !== -1) {
                this.setState({ hoveredView: "", hoveredIndex: -1 });
            }
        });
    }

    componentWillUnmount() {
        helper = null;
        event = null;
        this.char.Body.HpChangedDelegate = null;
        if (this.ship) {
            this.ship.Body.HpChangedDelegate = null;
        }
    }

    render() {
        return React.createElement(
            'uCanvasPanel',
            null,
            React.createElement(
                'span',
                {
                    Slot: {
                        LayoutData: {
                            Anchors: EAnchors.Left,
                            Alignment: { X: 0, Y: 0.5 },
                            Offsets: Utils.ltrb(0, 0, 0, 720)
                        }
                    }
                },
                React.createElement(BackpackView, {
                    title: ETitle.Char,
                    bp: this.charBp,
                    hoveredIndex: this.state.hoveredView === ETitle.Char ? this.state.hoveredIndex : -1
                }),
                !!this.baseBp && React.createElement(BackpackView, {
                    title: ETitle.Base,
                    bp: this.baseBp,
                    hoveredIndex: this.state.hoveredView === ETitle.Base ? this.state.hoveredIndex : -1
                }),
                !!this.shipBp && React.createElement(BackpackView, {
                    title: ETitle.Ship,
                    bp: this.shipBp,
                    hoveredIndex: this.state.hoveredView === ETitle.Ship ? this.state.hoveredIndex : -1
                })
            ),
            React.createElement(
                'div',
                {
                    Slot: {
                        LayoutData: {
                            Anchors: EAnchors.Right,
                            Alignment: { X: 1, Y: 0.5 },
                            Offsets: Utils.ltrb(0, 0, 0, 0)
                        },
                        bAutoSize: true
                    }
                },
                React.createElement(
                    TaggedCard,
                    {
                        title: "躯体状态"
                    },
                    React.createElement(
                        'uSizeBox',
                        {
                            Slot: {
                                Padding: Utils.ltrb(0, 5)
                            },
                            WidthOverride: 420,
                            HeightOverride: 20
                        },
                        React.createElement(PointBar, {
                            curVal: this.state.charHP.cur,
                            maxVal: this.state.charHP.max
                        })
                    ),
                    React.createElement(
                        'uSizeBox',
                        {
                            Slot: {
                                Padding: Utils.ltrb(0, 5)
                            },
                            HeightOverride: 20
                        },
                        React.createElement(PointBar, {
                            curVal: this.state.charMP.cur,
                            maxVal: this.state.charMP.max
                        })
                    )
                ),
                !!this.shipBp && React.createElement(
                    TaggedCard,
                    {
                        Slot: {
                            Padding: Utils.ltrb(0, 30)
                        },
                        title: "飞船状态"
                    },
                    React.createElement(
                        'uSizeBox',
                        {
                            Slot: {
                                Padding: Utils.ltrb(0, 5)
                            },
                            WidthOverride: 420,
                            HeightOverride: 20
                        },
                        React.createElement(PointBar, {
                            curVal: this.state.shipHP.cur,
                            maxVal: this.state.shipHP.max
                        })
                    ),
                    React.createElement(
                        'uSizeBox',
                        {
                            Slot: {
                                Padding: Utils.ltrb(0, 5)
                            },
                            HeightOverride: 20
                        },
                        React.createElement(PointBar, {
                            curVal: this.state.shipMP.cur,
                            maxVal: this.state.shipMP.max
                        })
                    )
                )
            )
        );
    }
}

module.exports = Menu;