const React = require('react')
const Utils = require('../utils')
const EAnchors = require('../anchors')

class EngineToward extends React.Component {
    constructor(props) {
        super(props)
    }

    render() {
        let {Max, UpDown, FrontBack} = this.props
        let up_rate = UpDown / Max
        let front_rate = FrontBack / Max
        return (
            <uCanvasPanel {...this.props}>
                <uImage
                    Slot={{ LayoutData: { Anchors: EAnchors.FillAll, Offsets: Utils.ltrb(0) } }}
                    ColorAndOpacity={ UpDown < 0 ? Utils.rgba(1 + up_rate, 1 + up_rate, 1) : Utils.color("#fff") }
                    Brush={{
                        ResourceObject: Utils.icon("T_TowardDown180x180"),
                        ImageSize: {X: 180, Y: 180}
                    }}
                />
                <uImage
                    Slot={{ LayoutData: { Anchors: EAnchors.FillAll, Offsets: Utils.ltrb(0) } }}
                    Brush={{
                        ResourceObject: Utils.icon("T_TowardBG180x180"),
                        ImageSize: {X: 180, Y: 180}
                    }}
                />
                <uImage
                    Slot={{ LayoutData: { Anchors: EAnchors.FillAll, Offsets: Utils.ltrb(0) } }}
                    ColorAndOpacity={ UpDown > 0 ? Utils.rgba(1 - up_rate, 1 - up_rate, 1) : Utils.color("#fff") }
                    Brush={{
                        ResourceObject: Utils.icon("T_TowardUp180x180"),
                        ImageSize: {X: 180, Y: 180}
                    }}
                />
                <uImage
                    Slot={{ LayoutData: { Anchors: EAnchors.FillAll, Offsets: Utils.ltrb(0) } }}
                    ColorAndOpacity={ FrontBack > 0 ? Utils.rgba(1 - front_rate, 1 - front_rate, 1) : Utils.color("#fff") }
                    Brush={{
                        ResourceObject: Utils.icon("T_TowardFront180x180"),
                        ImageSize: {X: 180, Y: 180}
                    }}
                />
                <uImage
                    Slot={{ LayoutData: { Anchors: EAnchors.FillAll, Offsets: Utils.ltrb(0) } }}
                    ColorAndOpacity={ FrontBack < 0 ? Utils.rgba(1 + front_rate, 1 + front_rate, 1) : Utils.color("#fff") }
                    Brush={{
                        ResourceObject: Utils.icon("T_TowardBack180x180"),
                        ImageSize: {X: 180, Y: 180}
                    }}
                />
                <uImage
                    Slot={{ LayoutData: { Anchors: EAnchors.FillAll, Offsets: Utils.ltrb(0) } }}
                    Brush={{
                        ResourceObject: Utils.icon("T_TowardCenter180x180"),
                        ImageSize: {X: 180, Y: 180}
                    }}
                />
            </uCanvasPanel>
        )
    }
}

module.exports = EngineToward