const React = require('react')
const Utils = require('../utils')
const {F_Sans, ButtonStyle} = require('../style')

const textureRect = Texture2D.Load('/Game/UI/Icon/T_Rectangle64x64')

class TabButton extends React.Component {
    constructor(props) {
        super(props)
    }

    render() {
        let {width, height, borderWidth, fontSize, text, onClicked} = this.props
        width = width || 150
        height = height || 50
        text = text || "Button"
        fontSize = fontSize || 16
        borderWidth = borderWidth || 0
        return (
            <uSizeBox 
                {...this.props} 
                WidthOverride={width}
                HeightOverride={height}
                Cursor={EMouseCursor.Hand}
            >
                <uButton
                    WidgetStyle={ButtonStyle}
                    IsFocusable={false}
                    OnClicked={onClicked}
                >
                    <uBorder
                        Slot={{
                            Padding: Utils.ltrb(0),
                            HorizontalAlignment: EHorizontalAlignment.HAlign_Fill,
                            VerticalAlignment: EVerticalAlignment.VAlign_Fill
                        }}
                        Background={{
                            ResourceObject: textureRect,
                            DrawAs: ESlateBrushDrawType.Border,
                            TintColor: { SpecifiedColor: Utils.color("#FFF") },
                            Margin: Utils.ltrb(borderWidth / 64)
                        }}
                        Padding={Utils.ltrb(0)}
                        HorizontalAlignment={EHorizontalAlignment.HAlign_Center}
                        VerticalAlignment={EVerticalAlignment.VAlign_Center}
                    >
                        <uTextBlock
                            Font={{
                                FontObject: F_Sans,
                                Size: fontSize,
                            }}
                            ColorAndOpacity={{
                                ColorUseRule: ESlateColorStylingMode.UseColor_Foreground
                            }}
                            Text={text}
                        />
                    </uBorder>
                </uButton>
            </uSizeBox>
        )
    }
}

module.exports = TabButton