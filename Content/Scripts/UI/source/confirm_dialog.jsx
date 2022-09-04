const _ = require('lodash')
const React = require('react')
const ReactUMG = require('react-umg')
const Utils = require('../utils')
const EAnchors = require('../anchors')
const {T_Rect, ButtonStyle} = require('../style')


class Dialog extends React.Component {
    constructor(props) {
        super(props)
    }

    render() {
        const {title, content, onAccepted, onRefused, _close} = this.props
        return (
            <uBorder
                Background={{
                    TintColor: { SpecifiedColor: Utils.rgba(0, 0, 0, 0.2) }
                }}
                Padding={Utils.ltrb(0)}
                HorizontalAlignment={EHorizontalAlignment.HAlign_Center}
                VerticalAlignment={EVerticalAlignment.VAlign_Center}
            >
                <uBorder
                    Background={{
                        ResourceObject: T_Rect,
                        DrawAs: ESlateBrushDrawType.Border,
                        TintColor: { SpecifiedColor: Utils.rgba(1, 1, 1, 0.2) },
                        Margin: Utils.ltrb(2/64)
                    }}
                    Padding={Utils.ltrb(2)}
                    OnMouseButtonDownEvent={(MyGeometry, MouseEvent) => EventReply.Handled()}
                >
                    <uSizeBox
                        WidthOverride={520}
                        HeightOverride={300}
                    >
                    <uBorder
                        Background={{
                            TintColor: { SpecifiedColor: Utils.rgba(0, 0, 0, 0.6) }
                        }}
                        Padding={Utils.ltrb(0)}
                    >
                        <div>
                            <uBorder
                                Background={{
                                    TintColor: { SpecifiedColor: Utils.rgba(0, 0, 0, 0.3) }
                                }}
                                HorizontalAlignment={EHorizontalAlignment.HAlign_Center}
                                VerticalAlignment={EVerticalAlignment.VAlign_Center}
                                Padding={Utils.ltrb(10)}
                            >
                                <uTextBlock
                                    Font={{
                                        FontObject: Font.Load('/Game/UI/Font/SourceHanSansSC'),
                                        TypefaceFontName: "Bold",
                                        Size: 16,
                                    }}
                                    ColorAndOpacity={{
                                        SpecifiedColor: Utils.color("#FFF")
                                    }}
                                    Text={title}
                                />
                            </uBorder>

                            <uTextBlock
                                Slot={{
                                    Padding: Utils.ltrb(0, 20),
                                    HorizontalAlignment: EHorizontalAlignment.HAlign_Center
                                }}
                                Font={{
                                    FontObject: Font.Load('/Game/UI/Font/SourceHanSansSC'),
                                    TypefaceFontName: "Bold",
                                    Size: 16,
                                }}
                                WrapTextAt={300}
                                ColorAndOpacity={{
                                    SpecifiedColor: Utils.color("#FFF")
                                }}
                                Text={content}
                            />

                            <uSpacer Slot={{ Size: { SizeRule: ESlateSizeRule.Fill, Value: 1.0 }}} />

                            <span
                                Slot={{
                                    Padding: Utils.ltrb(0, 20),
                                    HorizontalAlignment: EHorizontalAlignment.HAlign_Center
                                }}
                            >
                                {_.map(["取消", "确定"], (val, idx) => (
                                <uSizeBox
                                    Slot={{
                                        Padding: Utils.ltrb(10, 0),
                                    }}
                                    WidthOverride={200}
                                    HeightOverride={50}
                                >
                                    <uButton
                                        WidgetStyle={{...ButtonStyle, 
                                            Normal: {
                                                DrawAs: ESlateBrushDrawType.Border,
                                                TintColor: { SpecifiedColor: Utils.color("#FFF") },
                                                Margin: Utils.ltrb(1)
                                            }
                                        }}
                                        IsFocusable={false}
                                        OnReleased={() => {
                                            [onRefused, onAccepted][idx]()
                                            _close()
                                        }}
                                    >
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
                                                Size: 16,
                                            }}
                                            ColorAndOpacity={{
                                                ColorUseRule: ESlateColorStylingMode.UseColor_Foreground
                                            }}
                                            Text={val}
                                        />
                                    </uButton>
                                </uSizeBox>
                                ))}
                            </span>                            
                        </div>
                    </uBorder>
                    </uSizeBox>
                </uBorder>
            </uBorder>
        )
    }
}


module.exports = {
    open: (title, content, onAccepted, onRefused = ()=>{} ) => {
        let widget = ReactUMG.wrap(
            <Dialog
                title={title}
                content={content}
                onAccepted={onAccepted}
                onRefused={onRefused}
                _close={() => {
                    widget.RemoveFromViewport()
                    widget = null
                }}
            />
        )
        widget.AddToViewport()
    }
}