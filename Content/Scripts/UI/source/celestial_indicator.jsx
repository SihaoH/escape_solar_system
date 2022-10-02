const _ = require('lodash')
const React = require('react')
const {F_Sans} = require('../style')
const Utils = require('../utils')
const EAnchors = require('../anchors')

const FontSyle = {
    FontObject: F_Sans,
    TypefaceFontName: "Bold",
    Size: 14,
    OutlineSettings: {
        OutlineSize: 1,
        OutlineColor: Utils.rgba(0, 0, 0, 0.6)
    }
}
const FontColor = {
    SpecifiedColor: Utils.rgba(0.1, 0.5, 1.0)
}

let LookedWidget = null
let LockedWidget = null
let IsVaild = true

class LookedIndicator extends React.Component {
    constructor(props) {
        super(props)
    }
    render() {
        return (
            <span
                ref={elem => {
                    if (elem && LookedWidget !== elem.ueobj.Slot) {
                        LookedWidget = elem.ueobj.Slot
                    }
                }}
                {...this.props}
            >
                <uSizeBox
                    Slot={{ VerticalAlignment: EVerticalAlignment.VAlign_Center }}
                    WidthOverride={64}
                    HeightOverride={64}
                >
                <uBorder
                    Padding={Utils.ltrb(0)}
                    Background={{
                        ResourceObject: this.props.owner.Icon,
                        ImageSize: {X: 64, Y: 64}
                    }}
                >
                {this.props.angle !== -1 &&
                    <uImage
                        Brush={{
                            ResourceObject: Utils.icon("T_Indicator64x64"),
                            ImageSize: {X: 64, Y: 64}
                        }}
                        RenderTransform={{ Angle: this.props.angle }}
                    />
                }
                </uBorder>
                </uSizeBox>
                <uTextBlock
                    Slot={{ VerticalAlignment: EVerticalAlignment.VAlign_Center }}
                    Font={FontSyle}
                    ColorAndOpacity={FontColor}
                    Text={this.props.owner.Name}
                />
            </span>
        )
    }
}

class LockedIndicator extends React.Component {
    constructor(props) {
        super(props)
    }
    render() {
        return (
            <span
                ref={elem => {
                    if (elem && LockedWidget !== elem.ueobj.Slot) {
                        LockedWidget = elem.ueobj.Slot
                    }
                }}
                {...this.props}
            >
                <uSizeBox
                    Slot={{ VerticalAlignment: EVerticalAlignment.VAlign_Center }}
                    WidthOverride={64}
                    HeightOverride={64}
                >
                <uBorder
                    
                    Padding={Utils.ltrb(0)}
                    Background={{
                        ResourceObject: this.props.owner.Icon,
                        ImageSize: {X: 64, Y: 64}
                    }}
                >
                {this.props.angle !== -1 &&
                    <uImage
                        Brush={{
                            ResourceObject: Utils.icon("T_Indicator64x64"),
                            ImageSize: {X: 64, Y: 64}
                        }}
                        RenderTransform={{ Angle: this.props.angle }}
                    />
                }
                </uBorder>
                </uSizeBox>
                <div
                    Slot={{ VerticalAlignment: EVerticalAlignment.VAlign_Center }}
                >
                    {_.map([this.props.owner.Name, `${Utils.num2Txt(this.props.info.Dist)}m`, `${Utils.num2Txt(this.props.info.Speed)}m/s`], (v) => (
                        <uTextBlock
                            LineHeightPercentage={0.6}
                            Font={FontSyle}
                            ColorAndOpacity={FontColor}
                            Text={v}
                        />)
                    )}
                </div>
            </span>
        )
    }
}

class View extends React.Component {
    constructor(props) {
        super(props)
        this.helper = new CelestialIndicatorHelper()

        this.state = {
            lookedBody: null,
            lockedBody: null,
            lookedAngle: -1,
            lockedAngle: -1,
            lockedInfo: { Dist: 0, Speed: 0 }
        }

        MainLevelScript.Instance().CelestialBodyLookedDelegate.Add((Body) => {
            if (this.state.lookedBody) { // 置一下null，避免图标不更新
                this.setState({ lookedBody: null })
            }
            this.setState({ lookedBody: Body })
        })
        MainLevelScript.Instance().CelestialBodyLockedDelegate.Add((Body) => {
            this.setState({ lockedBody: Body })
        })

        let self = this
        function tick() {
            if (LookedWidget && self.state.lookedBody) {
                let layout = self.helper.GetWidgetPosition(self.state.lookedBody)
                LookedWidget.SetPosition( layout.Position )
                self.setState({ lookedAngle: layout.Angle })
            }
            if (LockedWidget && self.state.lockedBody) {
                let layout = self.helper.GetWidgetPosition(self.state.lockedBody)
                LockedWidget.SetPosition( layout.Position )
                self.setState({ lockedAngle: layout.Angle, lockedInfo: self.helper.GetLockInfo(self.state.lockedBody) })
            }
            if (IsVaild) process.nextTick(tick)
        }
        tick()
    }

    componentWillUnmount() {
        IsVaild = false
        LookedWidget = null
        LockedWidget = null
        clearInterval(this.updateTimer)
    }

    render() {
        return (
            <uCanvasPanel {...this.props}>
                {this.state.lookedBody && (this.state.lookedBody !== this.state.lockedBody) &&
                <LookedIndicator
                    Slot={{
                        LayoutData: {
                            Anchors: EAnchors.TopLeft,
                            Alignment: { X: 0.5, Y: 0.5 },
                        },
                        bAutoSize: true
                    }}
                    angle={this.state.lookedAngle}
                    owner={this.state.lookedBody}
                />
                }
                {this.state.lockedBody &&
                <LockedIndicator
                    Slot={{
                        LayoutData: {
                            Anchors: EAnchors.TopLeft,
                            Alignment: { X: 0.5, Y: 0.5 },
                        },
                        bAutoSize: true
                    }}
                    angle={this.state.lockedAngle}
                    owner={this.state.lockedBody}
                    info={this.state.lockedInfo}
                />
                }
            </uCanvasPanel>
        )
    }
}

module.exports = View