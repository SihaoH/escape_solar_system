var _extends = Object.assign || function (target) { for (var i = 1; i < arguments.length; i++) { var source = arguments[i]; for (var key in source) { if (Object.prototype.hasOwnProperty.call(source, key)) { target[key] = source[key]; } } } return target; };

const React = require('react');
const Utils = require('../utils');
const { F_Sans, ButtonStyle } = require('../style');

const textureRect = Texture2D.Load('/Game/UI/Icon/T_Rectangle64x64');

class TabButton extends React.Component {
    constructor(props) {
        super(props);
    }

    render() {
        let { width, height, borderWidth, fontSize, text, onClicked } = this.props;
        width = width || 150;
        height = height || 50;
        text = text || "Button";
        fontSize = fontSize || 16;
        borderWidth = borderWidth || 0;
        return React.createElement(
            'uSizeBox',
            _extends({}, this.props, {
                WidthOverride: width,
                HeightOverride: height,
                Cursor: EMouseCursor.Hand
            }),
            React.createElement(
                'uButton',
                {
                    WidgetStyle: ButtonStyle,
                    IsFocusable: false,
                    OnClicked: onClicked
                },
                React.createElement(
                    'uBorder',
                    {
                        Slot: {
                            Padding: Utils.ltrb(0),
                            HorizontalAlignment: EHorizontalAlignment.HAlign_Fill,
                            VerticalAlignment: EVerticalAlignment.VAlign_Fill
                        },
                        Background: {
                            ResourceObject: textureRect,
                            DrawAs: ESlateBrushDrawType.Border,
                            TintColor: { SpecifiedColor: Utils.color("#FFF") },
                            Margin: Utils.ltrb(borderWidth / 64)
                        },
                        Padding: Utils.ltrb(0),
                        HorizontalAlignment: EHorizontalAlignment.HAlign_Center,
                        VerticalAlignment: EVerticalAlignment.VAlign_Center
                    },
                    React.createElement('uTextBlock', {
                        Font: {
                            FontObject: F_Sans,
                            Size: fontSize
                        },
                        ColorAndOpacity: {
                            ColorUseRule: ESlateColorStylingMode.UseColor_Foreground
                        },
                        Text: text
                    })
                )
            )
        );
    }
}

module.exports = TabButton;