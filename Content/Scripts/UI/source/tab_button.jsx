const React = require('react')
const AD = require('animation-driver')
const Utils = require('../utils')
const EAnchors = require('../anchors')

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
            >
                <uButton
                    WidgetStyle={{
                        Normal: {
                            DrawAs: ESlateBrushDrawType.NoDrawType
                        },
                        Hovered: {
                            ResourceObject: textureRect,
                            DrawAs: ESlateBrushDrawType.Image,
                            TintColor: { SpecifiedColor: Utils.rgba(0, 0, 0, 0.1) },
                        },
                        Pressed: {
                            ResourceObject: textureRect,
                            DrawAs: ESlateBrushDrawType.Image,
                            TintColor: { SpecifiedColor: Utils.rgba(0, 0, 0, 0.3) },
                        },
                        NormalPadding: Utils.ltrb(0),
                        PressedPadding: Utils.ltrb(0)
                    }}
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
                                Size: 22,
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