const _ = require('lodash');
const uclass = require('uclass')();
const React = require('react');
const ReactUMG = require('react-umg');
const Utils = require('../utils');
const EAnchors = require('../anchors');
const { ButtonStyle } = require('../style');

class ContextMenu extends React.Component {
    constructor(props) {
        super(props);
        this.state = { position: Vector2D.Vector2D_Zero() };
    }

    componentDidMount() {
        // 从屏幕（绝对）坐标转换为UI的本地坐标
        // 由于Geometry需要在UI被渲染出来后才能获取（并且process.nextTick还不行）
        // 所以延迟50ms后进行
        setTimeout(() => {
            this.setState({ position: this.uPanel.GetUIGeometry().AbsoluteToLocal(this.props.position) });
        }, 50);
    }

    render() {
        const { options, onSelected, close } = this.props;
        return React.createElement(
            'uBorder',
            {
                ref: elem => {
                    if (elem && !this.uPanel) {
                        this.uPanel = elem.ueobj;
                    }
                },
                Background: {
                    DrawAs: ESlateBrushDrawType.NoDrawType
                },
                OnMouseButtonDownEvent: (MyGeometry, MouseEvent) => {
                    close();
                    return EventReply.Handled();
                }
            },
            React.createElement(
                'uCanvasPanel',
                {
                    Slot: {
                        Padding: Utils.ltrb(0),
                        HorizontalAlignment: EHorizontalAlignment.HAlign_Fill,
                        VerticalAlignment: EVerticalAlignment.VAlign_Fill
                    }
                },
                React.createElement(
                    'uBorder',
                    {
                        ref: elem => {
                            if (elem && !this.state.position.IsZero2D()) {
                                elem.ueobj.Slot.SetPosition(this.state.position.Add_Vector2DFloat(1));
                            }
                        },
                        Slot: {
                            LayoutData: {
                                Offsets: Utils.ltrb(9999, 9999, 0, 0)
                            },
                            bAutoSize: true
                        },
                        Background: {
                            TintColor: { SpecifiedColor: Utils.rgba(0.6, 0.6, 0.6, 0.8) }
                        }
                    },
                    React.createElement(
                        'div',
                        { Slot: { Padding: Utils.ltrb(1) } },
                        _.map(options, (val, i) => [React.createElement(
                            'uSizeBox',
                            {
                                WidthOverride: 240,
                                HeightOverride: 40
                            },
                            React.createElement(
                                'uButton',
                                {
                                    WidgetStyle: ButtonStyle,
                                    OnPressed: () => {
                                        onSelected(i);
                                        close();
                                    }
                                },
                                React.createElement('uTextBlock', {
                                    Font: {
                                        FontObject: Font.Load('/Game/UI/Font/SourceHanSansSC'),
                                        Size: 14
                                    },
                                    ColorAndOpacity: {
                                        ColorUseRule: ESlateColorStylingMode.UseColor_Foreground
                                    },
                                    Text: val
                                })
                            )
                        ), React.createElement('uImage', {
                            Slot: { Padding: Utils.ltrb(10, -1) },
                            Brush: { ImageSize: { X: 32, Y: i < options.length - 1 ? 1 : 0 } },
                            ColorAndOpacity: Utils.color("#FFF")
                        })])
                    )
                )
            )
        );
    }
}

module.exports = {
    open: (position, options, onSelected) => {
        let widget = ReactUMG.wrap(React.createElement(ContextMenu, {
            position: position,
            options: options,
            onSelected: onSelected,
            close: () => {
                widget.RemoveFromViewport();
                widget = null;
            }
        }));
        widget.AddToViewport();
    }
};