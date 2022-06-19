const _ = require('lodash');
const React = require('react');
const ReactUMG = require('react-umg');
const AD = require('animation-driver');
const Utils = require('../utils');
const EAnchors = require('../anchors');
const { F_Sans } = require('../style');

const banner_width = 2030;

class PromptView extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            bannerText: ""
        };

        MainLevelScriptActor.Instance().ExplorePointsDelegate.Add(Msg => {
            this.setState({ bannerText: Msg });
            this.fadeAnime = AD();
            this.fadeAnime.apply(this.uBanner, { duration: 0.3 }, { RenderTranslation: t => {
                    return { X: (1 - t) * banner_width, Y: 0 };
                } }).then(_ => {
                this.fadeAnime = AD();
                this.fadeAnime.apply(this.uBanner, { delay: 3, duration: 0.3 }, { RenderTranslation: t => {
                        return { X: -t * banner_width, Y: 0 };
                    } });
            });
        });
    }

    componentDidMount() {}

    render() {
        return React.createElement(
            'uCanvasPanel',
            null,
            React.createElement(
                'uBorder',
                {
                    ref: elem => {
                        if (elem && !this.uBanner) {
                            this.uBanner = elem.ueobj;
                        }
                    },
                    Slot: {
                        LayoutData: {
                            Anchors: EAnchors.FillTop,
                            Offsets: Utils.ltrb(0, 200, 0, 180)
                        }
                    },
                    Padding: Utils.ltrb(0, 5),
                    Background: {
                        TintColor: { SpecifiedColor: Utils.rgba(0, 0, 0, 0.1) }
                    },
                    RenderTransform: {
                        Translation: { X: 9999, Y: 0 }
                    }
                },
                React.createElement(
                    'uBorder',
                    {
                        Background: {
                            TintColor: { SpecifiedColor: Utils.rgba(1, 1, 1, 0.9) }
                        },
                        HorizontalAlignment: EHorizontalAlignment.HAlign_Center,
                        VerticalAlignment: EVerticalAlignment.VAlign_Center
                    },
                    React.createElement('uTextBlock', {
                        Font: {
                            FontObject: F_Sans,
                            TypefaceFontName: "Bold",
                            Size: 32
                        },
                        ColorAndOpacity: {
                            SpecifiedColor: Utils.color("#0F1020")
                        },
                        Text: this.state.bannerText
                    })
                )
            )
        );
    }
}

module.exports = ReactUMG.wrap(React.createElement(PromptView, null));