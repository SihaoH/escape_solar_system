var _extends = Object.assign || function (target) { for (var i = 1; i < arguments.length; i++) { var source = arguments[i]; for (var key in source) { if (Object.prototype.hasOwnProperty.call(source, key)) { target[key] = source[key]; } } } return target; };

const _ = require('lodash');
const React = require('react');
const ReactUMG = require('react-umg');
const Utils = require('../utils');
const EAnchors = require('../anchors');
const { F_Sans, T_Rect, ButtonStyle } = require('../style');

class Dialog extends React.Component {
    constructor(props) {
        super(props);
    }

    render() {
        const { title, content, onAccepted, onRefused, _close } = this.props;
        return React.createElement(
            'uBorder',
            {
                Background: {
                    TintColor: { SpecifiedColor: Utils.rgba(0, 0, 0, 0.2) }
                },
                Padding: Utils.ltrb(0),
                HorizontalAlignment: EHorizontalAlignment.HAlign_Center,
                VerticalAlignment: EVerticalAlignment.VAlign_Center
            },
            React.createElement(
                'uBorder',
                {
                    Background: {
                        ResourceObject: T_Rect,
                        DrawAs: ESlateBrushDrawType.Border,
                        TintColor: { SpecifiedColor: Utils.rgba(1, 1, 1, 0.2) },
                        Margin: Utils.ltrb(2 / 64)
                    },
                    Padding: Utils.ltrb(2),
                    OnMouseButtonDownEvent: (MyGeometry, MouseEvent) => EventReply.Handled()
                },
                React.createElement(
                    'uSizeBox',
                    {
                        WidthOverride: 520,
                        HeightOverride: 300
                    },
                    React.createElement(
                        'uBorder',
                        {
                            Background: {
                                TintColor: { SpecifiedColor: Utils.rgba(0, 0, 0, 0.6) }
                            },
                            Padding: Utils.ltrb(0)
                        },
                        React.createElement(
                            'div',
                            null,
                            React.createElement(
                                'uBorder',
                                {
                                    Background: {
                                        TintColor: { SpecifiedColor: Utils.rgba(0, 0, 0, 0.3) }
                                    },
                                    HorizontalAlignment: EHorizontalAlignment.HAlign_Center,
                                    VerticalAlignment: EVerticalAlignment.VAlign_Center,
                                    Padding: Utils.ltrb(10)
                                },
                                React.createElement('uTextBlock', {
                                    Font: {
                                        FontObject: F_Sans,
                                        TypefaceFontName: "Bold",
                                        Size: 16
                                    },
                                    ColorAndOpacity: {
                                        SpecifiedColor: Utils.color("#FFF")
                                    },
                                    Text: title
                                })
                            ),
                            React.createElement('uTextBlock', {
                                Slot: {
                                    Padding: Utils.ltrb(0, 20),
                                    HorizontalAlignment: EHorizontalAlignment.HAlign_Center
                                },
                                Font: {
                                    FontObject: F_Sans,
                                    TypefaceFontName: "Bold",
                                    Size: 16
                                },
                                WrapTextAt: 420,
                                ColorAndOpacity: {
                                    SpecifiedColor: Utils.color("#FFF")
                                },
                                Text: content
                            }),
                            React.createElement('uSpacer', { Slot: { Size: { SizeRule: ESlateSizeRule.Fill, Value: 1.0 } } }),
                            React.createElement(
                                'span',
                                {
                                    Slot: {
                                        Padding: Utils.ltrb(0, 20),
                                        HorizontalAlignment: EHorizontalAlignment.HAlign_Center
                                    }
                                },
                                _.map([Utils.tr("取消"), Utils.tr("确定")], (val, idx) => React.createElement(
                                    'uSizeBox',
                                    {
                                        Slot: {
                                            Padding: Utils.ltrb(20, 0)
                                        },
                                        WidthOverride: 180,
                                        HeightOverride: 50
                                    },
                                    React.createElement(
                                        'uButton',
                                        {
                                            WidgetStyle: _extends({}, ButtonStyle, {
                                                Normal: {
                                                    DrawAs: ESlateBrushDrawType.Border,
                                                    TintColor: { SpecifiedColor: Utils.color("#FFF") },
                                                    Margin: Utils.ltrb(1)
                                                }
                                            }),
                                            IsFocusable: false,
                                            OnReleased: () => {
                                                [onRefused, onAccepted][idx]();
                                                _close();
                                            }
                                        },
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
                                                Size: 16
                                            },
                                            ColorAndOpacity: {
                                                ColorUseRule: ESlateColorStylingMode.UseColor_Foreground
                                            },
                                            Text: val
                                        })
                                    )
                                ))
                            )
                        )
                    )
                )
            )
        );
    }
}

module.exports = {
    open: (title, content, onAccepted, onRefused = () => {}) => {
        let widget = ReactUMG.wrap(React.createElement(Dialog, {
            title: title,
            content: content,
            onAccepted: onAccepted,
            onRefused: onRefused,
            _close: () => {
                widget.RemoveFromViewport();
                widget = null;
            }
        }));
        widget.AddToViewport(999);
    }
};