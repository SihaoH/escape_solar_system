const React = require('react')
const AD = require('animation-driver')
const Utils = require('../utils')
const EAnchors = require('../anchors')

const barLayout = { 
    LayoutData: { 
        Anchors: EAnchors.FillAll,
        Offsets: Utils.ltrb(0)
    } 
}

const font = {
    FontObject: Font.Load('/Game/UI/Font/SourceHanSansSC'),
    TypefaceFontName: "Bold",
    Size: 16,
    OutlineSettings: {
        OutlineSize: 2,
        OutlineColor: Utils.rgba(0, 0, 0, 0.6)
    }
}

const textureRect = Texture2D.Load('/Game/UI/Icon/T_Rectangle64x64')

const transStyle = {
    BackgroundImage: {
        ResourceObject: textureRect,
        TintColor: { SpecifiedColor: Utils.color("#80000000") }
    },
    FillImage: {
        ResourceObject: Texture2D.Load('/Game/UI/Icon/T_Rectangle64x64')
    }
}

const fillStyle = {
    BackgroundImage: {
        ResourceObject: textureRect,
        TintColor: { SpecifiedColor: Utils.color("#00ffffff") }
    },
    FillImage: {
        ResourceObject: textureRect
    }
}

class PointBar extends React.Component {
    constructor(props) {
        super(props);

        let {colorWhenAdded, colorWhenReduced, curVal, maxVal} = this.props
        this.colorWhenAdded = colorWhenAdded || Utils.color("#0F0")
        this.colorWhenReduced = colorWhenReduced || Utils.color("#F00")
        this.initVal = curVal / maxVal
        this.ad = AD()
        this.adRunning = false
    }

    componentDidUpdate() {
        let {curVal, maxVal} = this.props
        if (maxVal < 0) { // 无限
            this.uRealBar.SetPercent(1)
            return
        }

        maxVal = maxVal || 1 // 避免除以0的问题
        const EPSILON = 1.0e-8
        let original_val = this.uTransBar.Percent
        let target_val= curVal / maxVal
        let diff_val = original_val - target_val
        if (!this.adRunning && Math.abs(diff_val) > EPSILON) {
            let change_bar = null
            if (diff_val > EPSILON) { // 原来值比当前值多，即量减少了，要变红
                change_bar = this.uTransBar
                this.uRealBar.SetPercent(target_val)
                this.uTransBar.SetFillColorAndOpacity(this.colorWhenReduced)
            } else if (diff_val < -EPSILON) { // 量增大，变绿
                change_bar = this.uRealBar
                this.uTransBar.SetPercent(target_val)
                this.uTransBar.SetFillColorAndOpacity(this.colorWhenAdded)
            }
            
            this.adRunning = true
            this.ad.apply(change_bar, 
                { delay: 0.5, duration: 0.2, completed: () => { this.adRunning = false; this.forceUpdate() } }, 
                { Percent: t => original_val - t * diff_val }
            )
        }
    }

    render() {
        let {curVal, maxVal, fractionDigits, fillColor, fontStyle} = this.props
        curVal = curVal || 0
        maxVal = maxVal || 0
        fractionDigits = fractionDigits || 0
        fillColor = fillColor || Utils.color('#FFF')
        return (
            <uCanvasPanel {...this.props}>
                <uProgressBar
                    ref={elem=>{
                        if (elem && !this.uTransBar) {
                            this.uTransBar = elem.ueobj
                        }
                    }}
                    Slot={barLayout}
                    WidgetStyle={transStyle}
                    Percent={this.initVal}
                />
                <uProgressBar
                    ref={elem=>{
                        if (elem && !this.uRealBar) {
                            this.uRealBar = elem.ueobj
                        }
                    }}
                    Slot={barLayout}
                    WidgetStyle={fillStyle}
                    FillColorAndOpacity={fillColor}
                    Percent={this.initVal}
                />
                <text
                    Slot={{
                        LayoutData: { 
                            Anchors: EAnchors.Center, 
                            Alignment: { X: 0.5, Y: 0.502  } 
                        },
                        bAutoSize: true
                    }}
                    Font={fontStyle || font}
                    Text={`${Utils.num2Txt(curVal, fractionDigits)} / ${maxVal > -1 ? Utils.num2Txt(maxVal, fractionDigits) : "∞"}`}
                />
            </uCanvasPanel>
        )
    }
}

module.exports = PointBar