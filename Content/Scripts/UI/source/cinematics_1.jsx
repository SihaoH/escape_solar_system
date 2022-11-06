const _ = require('lodash')
const React = require('react')
const ReactUMG = require('react-umg')
const AD = require('animation-driver')
const Utils = require('../utils')
const EAnchors = require('../anchors')
const {F_Sans, T_Rect} = require('../style')

const FontSyle = {
    FontObject: F_Sans,
    TypefaceFontName: "Bold",
    Size: 24,
    OutlineSettings: {
        OutlineSize: 2,
        OutlineColor: Utils.rgba(0, 0, 0, 0.6)
    }
}
const FontColor = {
    SpecifiedColor: Utils.rgba(1.0, 1.0, 1.0)
}

const LocationString = "银河系-猎户座旋臂-太阳系"
const DateTimeString = "2122年"

class PromptView extends React.Component {
    constructor(props) {
        super(props)
        this.state = {
            location: "",
            dateTime: ""
        }

        this.play = async function() {
            while (true) {
                if (this.state.location.length < LocationString.length) {
                    const current = this.state.location
                    this.setState({ location: current+ LocationString.charAt(current.length) })
                } else if (this.state.dateTime.length < DateTimeString.length) {
                    const current = this.state.dateTime
                    this.setState({ dateTime: current+ DateTimeString.charAt(current.length) })
                } else {
                    break
                }
                await Utils.delay(150)
            }
        }
        setTimeout(() => { this.play() }, 1000)
    }

    render() {
        return (
            <uCanvasPanel>
                <span
                    Slot={{
                        LayoutData: {
                            Anchors: EAnchors.BottomLeft,
                            Offsets: Utils.ltrb(100, -100, 0, 0),
                            Alignment: { X: 0, Y: 1 },
                        },
                        bAutoSize: true
                    }}
                >
                    <uImage
                        Brush={{
                            ResourceObject: T_Rect,
                            ImageSize: {X: 4, Y: 100}
                        }}
                    />
                    <div
                        Slot={{
                            Padding: Utils.ltrb(20, 0, 0, 0),
                            VerticalAlignment: EVerticalAlignment.VAlign_Center
                        }}
                    >
                        <uTextBlock
                            Font={FontSyle}
                            ColorAndOpacity={FontColor}
                            Text={this.state.location}
                        />
                        <uTextBlock
                            Font={FontSyle}
                            ColorAndOpacity={FontColor}
                            Text={this.state.dateTime}
                        />
                    </div>
                </span>
            </uCanvasPanel>
        )
    }
}

module.exports = ReactUMG.wrap(<PromptView/>);