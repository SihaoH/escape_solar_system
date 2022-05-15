var _extends = Object.assign || function (target) { for (var i = 1; i < arguments.length; i++) { var source = arguments[i]; for (var key in source) { if (Object.prototype.hasOwnProperty.call(source, key)) { target[key] = source[key]; } } } return target; };

const _ = require('lodash');
const UClass = require('uclass')();
const AD = require('animation-driver');
const React = require('react');
const ReactUMG = require('react-umg');
const Utils = require('../utils');
const EAnchors = require('../anchors');

let lastEnter = false;

class MyObject extends UObject {
    ctor() {
        this.msg = "";
    }
}
let MyObject_C = UClass(global, MyObject);

class MyEntry extends React.Component {
    constructor(props) {
        super(props);

        this.ad = AD();
        this.precious = [];
        this.completed = elem => {
            if (!elem) return;

            this.widget = elem.ueobj;
            let { review } = this.props;
            if (!review) {
                this.ad.apply(this.widget, { delay: 3, duration: 0.8 }, { RenderOpacity: t => 1 - t });
            }
        };
    }

    render() {
        return (
            // ListView的Entry的parent还是一个SizeBox
            React.createElement(
                'uSizeBox',
                _extends({}, this.props, {
                    ref: this.completed,
                    Slot: {
                        Padding: Utils.ltrb(10, 0)
                    },
                    HeightOverride: 60
                }),
                React.createElement('text', {
                    Slot: {
                        VerticalAlignment: EVerticalAlignment.VAlign_Center
                    },
                    ColorAndOpacity: {
                        SpecifiedColor: Utils.color("#00BFFF")
                    },
                    Font: {
                        FontObject: Font.Load('/Game/UI/Font/SourceHanSansSC'),
                        TypefaceFontName: "Bold",
                        Size: 14,
                        OutlineSettings: {
                            OutlineSize: 1,
                            OutlineColor: Utils.rgba(0, 0, 0, 0.6)
                        }
                    },
                    Clipping: EWidgetClipping.ClipToBounds,
                    TextOverflowPolicy: ETextOverflowPolicy.Ellipsis,
                    AutoWrapText: true,
                    Text: this.props.item.msg
                })
            )
        );
    }
}

class MessageListView extends React.Component {
    constructor(props) {
        super(props);
        this.precious = [];

        this.state = { review: false };

        this.toggleReview = () => {
            let current = this.state.review;
            Utils.setInputMode(!current);

            this.setState({ review: !current });
            this.ListView.RegenerateAllEntries();
            this.ListView.SetScrollbarVisibility(current ? ESlateVisibility.Collapsed : ESlateVisibility.Visible);
            this.ListView.ScrollToBottom();
            this.InputBox.SetKeyboardFocus();
        };
    }

    AppendMsg(msg) {
        let obj = new MyObject_C();
        obj.msg = msg;
        this.precious.push(obj);

        this.ListView.SetItems(this.precious);
        if (!this.state.review) {
            this.ListView.SetScrollbarVisibility(ESlateVisibility.Collapsed);
            this.ListView.ScrollToBottom();
        }
    }

    render() {
        return React.createElement(
            'div',
            this.props,
            React.createElement('uJavascriptListView', {
                ref: elem => {
                    if (elem && !this.ListView) {
                        this.ListView = elem.ueobj;
                    }
                },
                Slot: {
                    Size: { SizeRule: ESlateSizeRule.Fill }
                }
                //SelectionMode={ESelectionMode.None}
                , WidgetStyle: {
                    BackgroundBrush: {
                        DrawAs: ESlateBrushDrawType.Box,
                        TintColor: {
                            SpecifiedColor: this.state.review ? Utils.rgba(0, 0, 0, 0.3) : Utils.rgba(0, 0, 0, 0)
                        }
                    }
                },
                OnGenerateRow: (item, widget) => {
                    return ReactUMG.wrap(React.createElement(MyEntry, { item: item, review: this.state.review }));
                }
            }),
            React.createElement(
                'uSizeBox',
                {
                    HeightOverride: 40,
                    Visibility: this.state.review ? ESlateVisibility.Visible : ESlateVisibility.Collapsed },
                React.createElement('uEditableTextBox', {
                    ref: elem => {
                        if (elem) {
                            this.InputBox = elem.ueobj;
                        }
                    },
                    WidgetStyle: {
                        Font: {
                            FontObject: Font.Load('/Game/UI/Font/SourceHanSansSC'),
                            Size: 12
                        },
                        FocusedForegroundColor: {
                            SpecifiedColor: Utils.color("#000"),
                            ColorUseRule: ESlateColorStylingMode.UseColor_Specified
                        }
                    },
                    OnTextCommitted: (text, method) => {
                        this.InputBox.SetText("");
                        if (this.state.review && method !== ETextCommit.OnEnter) {
                            this.toggleReview();
                        } else {
                            lastEnter = true;
                            this.InputBox.SetKeyboardFocus();
                            if (text) {
                                MainFunctionLibrary.SendMessage(text);
                            }
                        }
                    }
                })
            )
        );
    }
}

module.exports = MessageListView;