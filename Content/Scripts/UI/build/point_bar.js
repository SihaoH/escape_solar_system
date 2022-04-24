const React = require('react');
const AD = require('animation-driver');
const Utils = require('../utils');
const EAnchors = require('../anchors');

const barLayout = {
    LayoutData: {
        Anchors: EAnchors.FillAll,
        Offsets: Utils.ltrb(0)
    }
};

const font = {
    FontObject: Font.Load('/Game/UI/Font/SourceHanSansSC'),
    TypefaceFontName: "Bold",
    Size: 16,
    OutlineSettings: {
        OutlineSize: 2,
        OutlineColor: Utils.rgba(0, 0, 0, 0.6)
    }
};

const textureRect = Texture2D.Load('/Game/UI/Icon/T_Rectangle64x64');

const transStyle = {
    BackgroundImage: {
        ResourceObject: textureRect,
        TintColor: { SpecifiedColor: Utils.color("#80000000") }
    },
    FillImage: {
        ResourceObject: Texture2D.Load('/Game/UI/Icon/T_Rectangle64x64')
    }
};

const fillStyle = {
    BackgroundImage: {
        ResourceObject: textureRect,
        TintColor: { SpecifiedColor: Utils.color("#00ffffff") }
    },
    FillImage: {
        ResourceObject: textureRect
    }
};

class PointBar extends React.Component {
    constructor(props) {
        super(props);

        this.ad = AD();
        this.adRunning = false;
    }

    componentDidUpdate() {
        let { curVal, maxVal } = this.props;
        let diff = this.transBar.Percent - curVal / maxVal;
        if (!this.adRunning && Math.abs(diff) > Number.EPSILON) {
            let cur = this.transBar.Percent;
            this.adRunning = true;
            this.ad.apply(this.transBar, { delay: 0.5, duration: 0.2, completed: () => {
                    this.adRunning = false;
                } }, { Percent: t => cur - t * diff });
        }
    }

    render() {
        let { curVal, maxVal, fillColor } = this.props;
        curVal = curVal || 0;
        maxVal = maxVal || 1;
        fillColor = fillColor || Utils.color('#fff');
        return React.createElement(
            'uCanvasPanel',
            this.props,
            React.createElement('uProgressBar', {
                ref: elem => {
                    if (elem && !this.transBar) {
                        this.transBar = elem.ueobj;
                    }
                },
                Slot: barLayout,
                WidgetStyle: transStyle,
                FillColorAndOpacity: Utils.color('#f00'),
                Percent: 1
            }),
            React.createElement('uProgressBar', {
                Slot: barLayout,
                WidgetStyle: fillStyle,
                FillColorAndOpacity: fillColor,
                Percent: curVal / maxVal
            }),
            React.createElement('text', {
                Slot: {
                    LayoutData: {
                        Anchors: EAnchors.Center,
                        Alignment: { X: 0.5, Y: 0.5 }
                    },
                    bAutoSize: true
                },
                Font: font,
                Text: `${Utils.num2Txt(curVal)} / ${Utils.num2Txt(maxVal)}`
            })
        );
    }
}

module.exports = PointBar;