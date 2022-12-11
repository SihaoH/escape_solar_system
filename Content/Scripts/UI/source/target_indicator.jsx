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

let IsVaild = true

class LookedIndicator extends React.Component {
    constructor(props) {
        super(props)
    }
    render() {
        return (
            <span {...this.props} >
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
            <span {...this.props} >
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
        this.helper = new TargetIndicatorHelper()

        this.state = {
            lookedBody: null,
            lockedBody: null,
            spaceship: null,
            lookedLayout: { Position: {X: 0, Y: 0}, Angle: -1 },
            lockedLayout: { Position: {X: 0, Y: 0}, Angle: -1 },
            shipLayout: { Position: {X: 0, Y: 0}, Angle: -1 },
            lockedInfo: { Dist: 0, Speed: 0 },
            shipInfo: { Dist: 0, Speed: 0 }
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
            if (self.state.lookedBody) {
                self.setState({ lookedLayout: self.helper.GetWidgetPosition(self.state.lookedBody) })
            }
            if (self.state.lockedBody) {
                self.setState({
                    lockedLayout: self.helper.GetWidgetPosition(self.state.lockedBody),
                    //lockedInfo: self.helper.GetLockInfo(self.state.lockedBody, self.state.lockedInfo.Dist)
                })
            }

            if (self.state.spaceship !== MainLevelScript.GetSpaceship()) {
                self.setState({ spaceship: MainLevelScript.GetSpaceship() })
            }
            if (self.state.spaceship) {
                self.setState({
                    shipLayout: self.helper.GetWidgetPosition(self.state.spaceship),
                    //shipInfo: self.helper.GetLockInfo(self.state.spaceship, self.state.shipInfo.Dist)
                })
            }

            if (IsVaild) process.nextTick(tick)
        }
        tick()

        setInterval(()=> {
            if (self.state.lockedBody) {
                self.setState({
                    lockedInfo: self.helper.GetLockInfo(self.state.lockedBody, self.state.lockedInfo.Dist)
                })
            }
            if (self.state.spaceship) {
                self.setState({
                    shipInfo: self.helper.GetLockInfo(self.state.spaceship, self.state.shipInfo.Dist)
                })
            }
        }, this.helper.GetLockInfoDeltaSeconds*1000)
    }

    componentWillUnmount() {
        IsVaild = false
        clearInterval(this.updateTimer)
    }

    render() {
        return (
            <uCanvasPanel {...this.props}>
                {this.state.lookedBody && (this.state.lookedBody !== this.state.lockedBody) &&
                <LookedIndicator
                    Slot={{
                        Position: this.state.lookedLayout.Position,
                        LayoutData: {
                            Anchors: EAnchors.TopLeft,
                            Alignment: { X: 0.5, Y: 0.5 },
                        },
                        bAutoSize: true
                    }}
                    owner={this.state.lookedBody}
                    angle={this.state.lookedLayout.Angle}
                />
                }
                {this.state.lockedBody &&
                <LockedIndicator
                    Slot={{
                        Position: this.state.lockedLayout.Position,
                        LayoutData: {
                            Anchors: EAnchors.TopLeft,
                            Alignment: { X: 0.5, Y: 0.5 },
                        },
                        bAutoSize: true
                    }}
                    owner={this.state.lockedBody}
                    angle={this.state.lockedLayout.Angle}
                    info={this.state.lockedInfo}
                />
                }
                {this.state.spaceship && !this.state.spaceship.GetPilot() &&
                <LockedIndicator
                    Slot={{
                        Position: this.state.shipLayout.Position,
                        LayoutData: {
                            Anchors: EAnchors.TopLeft,
                            Alignment: { X: 0.5, Y: 0.5 },
                        },
                        bAutoSize: true
                    }}
                    owner={this.state.spaceship}
                    angle={this.state.shipLayout.Angle}
                    info={this.state.shipInfo}
                />
                }
            </uCanvasPanel>
        )
    }
}

module.exports = View