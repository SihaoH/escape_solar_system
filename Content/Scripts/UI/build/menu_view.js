const _ = require('lodash');
const uclass = require('uclass')();
const React = require('react');
const ReactUMG = require('react-umg');
const Utils = require('../Utils');
const EAnchors = require('../anchors');
const { F_Sans } = require('../style');

const TabButton = require('./tab_button');
const MenuBase = require('./menu_base');
const MenuBackpack = require('./menu_backpack');
const MenuLevel = require('./menu_level');
const MenuInfo = require('./menu_info');

let ThisWidget = null;

class MyOverlayer extends JavascriptWidget {
    ctor() {
        this.bIsFocusable = true;
        this.Visibility = ESlateVisibility.Visible;
        setTimeout(() => {
            if (this.GetOwningPlayerPawn()) {
                this.SetFocus();
            }
        }, 50);

        this.close = () => {
            ThisWidget.RemoveFromViewport();
            Utils.setInputMode(false);
            ThisWidget = null;
        };
    }
    OnKeyDown(MyGeometry, InKeyEvent) {
        if (InKeyEvent.GetKey().KeyName === "Escape") {
            this.close();
            return EventReply.Handled();
        }
        return EventReply.Unhandled();
    }
}
let MyOverlayer_C = uclass(global, MyOverlayer);
ReactUMG.Register('uMyOverlayer', MyOverlayer_C);

class MenuView extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            currentIndex: 0,
            explorePoints: MainPlayerState.Instance().GetExplorePoints()
        };
        let self = this;
        this.updatePoints = () => {
            self.setState({ explorePoints: MainPlayerState.Instance().GetExplorePoints() });
        };
        MainPlayerState.Instance().ExplorePointsChangedDelegate.Add(this.updatePoints);

        let list = ["存储", "科技", "情报"];
        this.menuList = MainLevelScriptActor.GetEarthBase() ? ["基地", ...list] : list;

        Utils.setInputMode(true);
    }

    componentWillUnmount() {
        MainPlayerState.Instance().ExplorePointsChangedDelegate.Remove(this.updatePoints);
    }

    render() {
        return React.createElement(
            'uMyOverlayer',
            null,
            React.createElement(
                'uCanvasPanel',
                null,
                React.createElement(
                    'uBackgroundBlur',
                    {
                        Slot: {
                            LayoutData: {
                                Anchors: EAnchors.FillAll,
                                Offsets: Utils.ltrb(0)
                            }
                        },
                        BlurStrength: 7
                    },
                    React.createElement('uImage', {
                        Slot: { Padding: Utils.ltrb(0) },
                        ColorAndOpacity: Utils.rgba(0, 0, 0, 0.2)
                    })
                ),
                React.createElement(
                    'span',
                    {
                        Slot: {
                            LayoutData: {
                                Anchors: EAnchors.FillTop,
                                Offsets: Utils.ltrb(50, 50, 50, 80)
                            }
                        }
                    },
                    _.map(this.menuList, (v, i) => React.createElement(TabButton, {
                        checked: this.state.currentIndex === i,
                        text: v,
                        onClicked: () => {
                            this.setState({ currentIndex: i });
                        }
                    })),
                    React.createElement('uSpacer', { Slot: { Size: { SizeRule: ESlateSizeRule.Fill, Value: 1.0 } } }),
                    React.createElement(
                        'span',
                        null,
                        React.createElement('uImage', {
                            Slot: {
                                VerticalAlignment: EVerticalAlignment.VAlign_Center
                            },
                            Brush: {
                                ResourceObject: Texture2D.Load("/Game/UI/Icon/T_Points64x64"),
                                ImageSize: { X: 48, Y: 48 }
                            }
                        }),
                        React.createElement('uTextBlock', {
                            Slot: {
                                Padding: Utils.ltrb(10, 0, 0, 0),
                                VerticalAlignment: EVerticalAlignment.VAlign_Center
                            },
                            Font: {
                                FontObject: F_Sans,
                                TypefaceFontName: "Bold",
                                Size: 20
                            },
                            ColorAndOpacity: {
                                SpecifiedColor: Utils.color("#D49D35")
                            },
                            Text: this.state.explorePoints
                        })
                    )
                ),
                React.createElement(
                    'uWidgetSwitcher',
                    {
                        ref: elem => {
                            if (elem && !this.tabView) {
                                this.tabView = elem.ueobj;
                            }
                        },
                        Slot: {
                            LayoutData: {
                                Anchors: EAnchors.FillAll,
                                Offsets: Utils.ltrb(50, 50 + 80, 50, 50)
                            }
                        },
                        ActiveWidgetIndex: this.state.currentIndex
                    },
                    MainLevelScriptActor.GetEarthBase() && React.createElement(MenuBase, null),
                    React.createElement(MenuBackpack, null),
                    React.createElement(MenuLevel, null),
                    React.createElement(MenuInfo, null)
                )
            )
        );
    }
}

module.exports = function () {
    if (!ThisWidget) {
        ThisWidget = ReactUMG.wrap(React.createElement(MenuView, null));
    }
    return ThisWidget;
};