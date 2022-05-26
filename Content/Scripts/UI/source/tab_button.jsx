const React = require('react')
const AD = require('animation-driver')
const Utils = require('../utils')
const EAnchors = require('../anchors')
const {ButtonStyle} = require('../style')

const textureRect = Texture2D.Load('/Game/UI/Icon/T_Rectangle64x64')

class TabButton extends React.Component {
    constructor(props) {
        super(props);

        // TODO this.ad = AD()
    }

    componentDidUpdate() {

    }

    render() {
        let {width, height, checked, text, onClicked} = this.props
        width = width || 240
        height = height || 60
        text = text || "Button"
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
                            Margin: Utils.ltrb(0, 0, 0, !!checked ? 0.06 : 0)
                        }}
                        Padding={Utils.ltrb(0)}
                        HorizontalAlignment={EHorizontalAlignment.HAlign_Center}
                        VerticalAlignment={EVerticalAlignment.VAlign_Center}
                    >
                        <uTextBlock
                            Font={{
                                FontObject: Font.Load('/Game/UI/Font/SourceHanSansSC'),
                                TypefaceFontName: "Bold",
                                LetterSpacing: 200,
                                Size: 22,
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