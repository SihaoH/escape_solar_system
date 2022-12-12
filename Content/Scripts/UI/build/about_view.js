const _ = require('lodash');
const React = require('react');
const Utils = require('../utils');
const EAnchors = require('../anchors');
const { F_Sans, ButtonStyle } = require('../style');
const ConfirmDialog = require('confirm_dialog');
const GeneralButton = require('general_button');

class SettingsView extends React.Component {
    constructor(props) {
        super(props);
    }
    componentWillUnmount() {}

    render() {
        let window_mode = { Fullscreen: Utils.tr("全屏"), WindowedFullscreen: Utils.tr("无边框全屏"), Windowed: Utils.tr("窗口") };
        return React.createElement(
            'uCanvasPanel',
            this.props,
            React.createElement('uTextBlock', {
                Font: {
                    FontObject: F_Sans,
                    Size: 16
                },
                Text: "DEMO 0.1.0"
            }),
            React.createElement(
                'span',
                {
                    Slot: {
                        LayoutData: {
                            Anchors: EAnchors.BottomLeft,
                            Alignment: { X: 0, Y: 1.0 },
                            Offsets: Utils.ltrb(50, -100, 500, 50)
                        }
                    }
                },
                React.createElement(GeneralButton, {
                    Slot: { Padding: Utils.ltrb(0, 0, 20, 0) },
                    borderWidth: 2,
                    text: Utils.tr("返回"),
                    onClicked: () => {
                        this.props.onBack();
                    }
                })
            )
        );
    }
}

module.exports = SettingsView;