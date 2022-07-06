const React = require('react')
const Utils = require('../utils')
const {F_Sans} = require('../style')

class ItemToolTip extends React.Component {
    constructor(props) {
        super(props)
    }

    render() {
        const {itemData} = this.props
        return (
            <uBorder
                {...this.props}
                Slot={{ Padding: Utils.ltrb(0) }}
                Padding={Utils.ltrb(20)}
                Background={{
                    TintColor: { SpecifiedColor: Utils.rgba(1, 1, 1, 0.6) }
                }}
            >
                <div>
                    <span>
                        <uImage
                            Slot={{
                                Padding: Utils.ltrb(0)
                            }}
                            Brush={{
                                // Icon变成了string类型
                                ResourceObject: Texture2D.Load(itemData.Icon),
                                ImageSize: {X: 64, Y: 64}
                            }}
                        />
                        <div>
                            <uTextBlock
                                Slot={{
                                    Padding: Utils.ltrb(20, 0),
                                }}
                                Font={{
                                    FontObject: F_Sans,
                                    TypefaceFontName: "Bold",
                                    Size: 16,
                                }}
                                ColorAndOpacity={{
                                    SpecifiedColor: Utils.color("#111")
                                }}
                                Text={itemData.Name} 
                            />
                            <uTextBlock
                                Slot={{
                                    Padding: Utils.ltrb(20, 5),
                                }}
                                Font={{
                                    FontObject: F_Sans,
                                    TypefaceFontName: "Bold",
                                    Size: 12,
                                }}
                                ColorAndOpacity={{
                                    SpecifiedColor: Utils.color("#333")
                                }}
                                Text={`${Math.round(itemData.Mass*100)/100}kg`} 
                            />
                        </div>
                    </span>
                    <uImage
                        Slot={{ Padding: Utils.ltrb(0, 10) }}
                        Brush={{ ImageSize: {X: 32, Y: 2} }}
                        ColorAndOpacity={Utils.rgba(0, 0, 0, 0.1)}
                    />
                    <uTextBlock
                        Font={{
                            FontObject: F_Sans,
                            TypefaceFontName: "Bold",
                            Size: 14,
                        }}
                        WrapTextAt={300}
                        ColorAndOpacity={{
                            SpecifiedColor: Utils.color("#222")
                        }}
                        Text={itemData.Desc} 
                    />
                </div>
            </uBorder>
        )
    }
}

module.exports = ItemToolTip