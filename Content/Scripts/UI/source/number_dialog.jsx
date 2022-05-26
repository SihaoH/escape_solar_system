const _ = require('lodash')
const uclass = require('uclass')()
const React = require('react')
const ReactUMG = require('react-umg')
const Utils = require('../utils')
const EAnchors = require('../anchors')
const {T_Rect, ButtonStyle, SpinBoxProps} = require('../style')

const T_Close = Texture2D.Load("/Game/UI/Icon/T_Close32x32")

class Dialog extends React.Component {
    constructor(props) {
        super(props)
    }

    render() {
        const {item, opName, onAccepted, maximum, close} = this.props
        const item_data = MainFunctionLibrary.GetItemData(item.RowName)
        return (
            <uBorder
                Background={{
                    TintColor: { SpecifiedColor: Utils.rgba(0, 0, 0, 0.2) }
                }}
                // OnMouseButtonDownEvent={(MyGeometry, MouseEvent) => {
                //     close()
                //     return EventReply.Handled()
                // }}
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
                                Padding={Utils.ltrb(10, 10, 10, 16)}
                            >
                                <uCanvasPanel>
                                    <span
                                        Slot={{
                                            LayoutData: {
                                                Anchors: EAnchors.Center,
                                                Alignment: {X: 0.5, Y: 0.5}
                                            }
                                        }}
                                    >
                                        <uImage
                                            Slot={{
                                                Padding: Utils.ltrb(0)
                                            }}
                                            Brush={{
                                                // Icon变成了string类型
                                                ResourceObject: Texture2D.Load(item_data.Icon),
                                                ImageSize: {X: 32, Y: 32}
                                            }}
                                        />
                                        <text
                                            Slot={{
                                                Padding: Utils.ltrb(10, 0),
                                                HorizontalAlignment: EHorizontalAlignment.HAlign_Center
                                            }}
                                            Font={{
                                                FontObject: Font.Load('/Game/UI/Font/SourceHanSansSC'),
                                                TypefaceFontName: "Bold",
                                                Size: 14,
                                            }}
                                            ColorAndOpacity={{
                                                SpecifiedColor: Utils.color("#FFF")
                                            }}
                                            Text={item_data.Name} 
                                        />
                                    </span>
                                    <uButton
                                        Slot={{
                                            LayoutData: {
                                                Anchors: EAnchors.Right,
                                                Offsets: Utils.ltrb(0, 0, 32, 32),
                                                Alignment: {X: 1.0, Y: 0.5}
                                            }
                                        }}
                                        WidgetStyle={{
                                            Normal: {
                                                ResourceObject: T_Close,
                                                DrawAs: ESlateBrushDrawType.Image,
                                                TintColor: { SpecifiedColor: Utils.color("#EEE") }
                                            },
                                            Hovered: {
                                                ResourceObject: T_Close,
                                                DrawAs: ESlateBrushDrawType.Image,
                                                TintColor: { SpecifiedColor: Utils.color("#FFF") },
                                            },
                                            Pressed: {
                                                ResourceObject: T_Close,
                                                DrawAs: ESlateBrushDrawType.Image,
                                                TintColor: { SpecifiedColor: Utils.color("#DDD") },
                                            },
                                        }}
                                        OnClicked={() => close()}
                                    />
                                </uCanvasPanel>
                            </uBorder>
                            
                            <uSizeBox
                                Slot={{
                                    Padding: Utils.ltrb(0, 20),
                                    HorizontalAlignment: EHorizontalAlignment.HAlign_Center
                                }}
                                WidthOverride={200}
                                HeightOverride={50}
                            >
                            <uSpinBox
                                ref={elem => {
                                    if (elem && !this.uCountBox) {
                                        this.uCountBox = elem.ueobj
                                    }
                                }}
                                {...SpinBoxProps}
                                MaxValue={maximum || item.Count}
                            />
                            </uSizeBox>

                            <uSpacer Slot={{ Size: { SizeRule: ESlateSizeRule.Fill, Value: 1.0 }}} />

                            <uSizeBox
                                Slot={{
                                    Padding: Utils.ltrb(0, 20),
                                    HorizontalAlignment: EHorizontalAlignment.HAlign_Center
                                }}
                                WidthOverride={240}
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
                                        onAccepted(this.uCountBox.GetValue())
                                        close()
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
                                        Text={opName}
                                    />
                                </uButton>
                            </uSizeBox>
                        </div>
                    </uBorder>
                    </uSizeBox>
                </uBorder>
            </uBorder>
        )
    }
}


module.exports = {
    open: (item, op_name, onAccepted, maximum = 0) => {
        let widget = ReactUMG.wrap(
            <Dialog
                item={item}
                opName={op_name}
                onAccepted={onAccepted}
                maximum={maximum}
                close={() => {
                    widget.RemoveFromViewport()
                    widget = null
                }}
            />
        )
        widget.AddToViewport()
    }
}