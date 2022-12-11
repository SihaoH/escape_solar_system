const _ = require('lodash');
const React = require('react');
const ReactUMG = require('react-umg');
const AD = require('animation-driver');
const Utils = require('../utils');
const EAnchors = require('../anchors');
const { F_Sans } = require('../style');

const MessageListView = require('./message_listview');

const PromptColor = [Utils.color("#EEE"), Utils.color("#EE1"), Utils.color("#E11")];

class PromptView extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            promptText: "",
            promptType: EPromptType.Normal,
            bannerText: ""
        };

        MainLevelScript.Instance().MessagedDelegate.Add(Msg => {
            this.msgListView.AppendMsg(Msg);
        });
        MainLevelScript.Instance().PromptDelegate.Add((Msg, Type) => {
            this.setState({ promptText: Msg, promptType: Type });
            setTimeout(() => {
                this.setState({ promptText: "" });
            }, 3000);
        });
        MainLevelScript.Instance().EnteredDelegate.Add(() => {
            this.msgListView.toggleReview();
        });

        MainLevelScript.Instance().ExplorePointsDelegate.Add(Msg => {
            this.setState({ bannerText: Msg });

            const banner_width = this.uBanner.GetCachedGeometry().GetLocalSize().X;
            this.fadeAnime = AD();
            this.fadeAnime.apply(this.uBanner, { duration: 0.3 }, { RenderTranslation: t => {
                    return { X: (1 - t) * banner_width, Y: 0 };
                } }).then(_ => {
                this.fadeAnime = AD();
                this.fadeAnime.apply(this.uBanner, { delay: 3, duration: 0.3 }, { RenderTranslation: t => {
                        return { X: -t * banner_width, Y: 0 };
                    } }).then(_ => {
                    this.uBanner.SetRenderTranslation({ X: 9999, Y: 0 });
                });
            });
        });
    }

    componentDidMount() {}

    render() {
        return React.createElement(
            'uCanvasPanel',
            null,
            '/* \u6A2A\u5E45\u63D0\u793A */',
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
            ),
            React.createElement(
                'uBorder',
                {
                    ref: elem => {
                        if (elem && !this.uPromptBox) {
                            this.uPromptBox = elem.ueobj;
                        }
                    },
                    Visibility: this.state.promptText ? ESlateVisibility.Visible : ESlateVisibility.Collapsed,
                    Slot: {
                        LayoutData: {
                            Anchors: EAnchors.Top,
                            Offsets: Utils.ltrb(0, 150, 0, 0)
                        },
                        bAutoSize: true
                    },
                    Background: {
                        TintColor: { SpecifiedColor: Utils.rgba(0, 0, 0, 0.5) }
                    },
                    Padding: Utils.ltrb(16, 5)
                },
                React.createElement('uTextBlock', {
                    Font: {
                        FontObject: F_Sans,
                        Size: 14
                    },
                    ColorAndOpacity: {
                        SpecifiedColor: PromptColor[this.state.promptType]
                    },
                    Text: this.state.promptText
                })
            ),
            '/* \u4FE1\u606F\u6846 */',
            React.createElement(MessageListView, {
                ref: elem => {
                    if (elem) {
                        this.msgListView = elem;
                    }
                },
                Slot: {
                    LayoutData: {
                        Anchors: EAnchors.Right,
                        Alignment: { X: 1.0, Y: 0.5 },
                        Offsets: Utils.ltrb(-20, 0, 400, 300)
                    }
                }
            })
        );
    }
}

module.exports = ReactUMG.wrap(React.createElement(PromptView, null));