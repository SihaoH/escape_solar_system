const _ = require('lodash');
const React = require('react');
const ReactUMG = require('react-umg');
const Utils = require('../utils');
const EAnchors = require('../anchors');
const { F_Sans, ButtonStyle } = require('../style');
const TabButton = require('./tab_button');
const ConfirmDialog = require('confirm_dialog');
const GeneralButton = require('general_button');

class ComboBoxBar extends React.Component {
    constructor(props) {
        super(props);
        this.uComboBox = null;
    }

    render() {
        return React.createElement(
            'span',
            this.props,
            React.createElement('uTextBlock', {
                Slot: { Padding: Utils.ltrb(20, 0) },
                Font: {
                    FontObject: F_Sans,
                    Size: 16
                },
                Text: `${this.props.tag}: `
            }),
            React.createElement('uComboBoxString', {
                ref: elem => {
                    if (elem && !this.uComboBox) {
                        this.uComboBox = elem.ueobj;
                        for (let i in this.props.options) {
                            this.uComboBox.AddOption(this.props.options[i]);
                        }
                        this.uComboBox.SetSelectedOption(this.props.selection);
                    }
                },
                ItemStyle: { TextColor: { ColorUseRule: ESlateColorStylingMode.UseColor_Foreground_Subdued } },
                OnSelectionChanged: (SelectedItem, SelectionType) => {
                    this.props.onSelectionChanged(this.uComboBox.GetSelectedIndex(), SelectedItem, SelectionType);
                }
            })
        );
    }
}

class SettingsView extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            currentIndex: 0
        };

        this.settings = GameUserSettings.GetGameUserSettings();
    }
    componentWillUnmount() {}

    render() {
        let window_mode = { Fullscreen: Utils.tr("全屏"), WindowedFullscreen: Utils.tr("无边框全屏"), Windowed: Utils.tr("窗口") };
        return React.createElement(
            'uCanvasPanel',
            this.props,
            React.createElement(
                'span',
                {
                    Slot: {
                        LayoutData: {
                            Anchors: EAnchors.FillTop,
                            Offsets: Utils.ltrb(50, 50, 50, 80)
                        }
                    }
                },
                _.map([Utils.tr("显示"), Utils.tr("声音"), Utils.tr("控制")], (v, i) => React.createElement(TabButton, {
                    checked: this.state.currentIndex === i,
                    text: v,
                    onClicked: () => {
                        this.setState({ currentIndex: i });
                    }
                }))
            ),
            React.createElement(
                'uWidgetSwitcher',
                {
                    Slot: {
                        LayoutData: {
                            Anchors: EAnchors.FillAll,
                            Offsets: Utils.ltrb(50, 50 + 80, 50, 50)
                        }
                    },
                    ActiveWidgetIndex: this.state.currentIndex
                },
                React.createElement(
                    'div',
                    null,
                    React.createElement(ComboBoxBar, {
                        Slot: { Padding: Utils.ltrb(0, 50, 0, 10) },
                        tag: Utils.tr("分辨率"),
                        options: ["1280×720", "1920×1080", "2560×1440"],
                        selection: `${this.settings.GetScreenResolution().X}×${this.settings.GetScreenResolution().Y}`,
                        onSelectionChanged: (index, item, type) => {
                            let point = item.split("×");
                            this.settings.SetScreenResolution({ X: parseInt(point[0]), Y: parseInt(point[1]) });
                        }
                    }),
                    React.createElement(ComboBoxBar, {
                        Slot: { Padding: Utils.ltrb(0, 10) },
                        tag: Utils.tr("窗口模式"),
                        options: window_mode,
                        selection: window_mode[this.settings.GetFullscreenMode()],
                        onSelectionChanged: (index, item, type) => {
                            this.settings.SetFullscreenMode(Object.keys(window_mode)[index]);
                        }
                    })
                ),
                React.createElement(
                    'div',
                    null,
                    React.createElement(ComboBoxBar, {
                        Slot: { Padding: Utils.ltrb(0, 50, 0, 10) },
                        tag: Utils.tr("xxxx"),
                        options: ["1280×720", "1920×1080", "2560×1440"],
                        selection: `${this.settings.GetScreenResolution().X}×${this.settings.GetScreenResolution().Y}`,
                        onSelectionChanged: (index, item, type) => {
                            let point = item.split("×");
                            this.settings.SetScreenResolution({ X: parseInt(point[0]), Y: parseInt(point[1]) });
                        }
                    })
                ),
                React.createElement(
                    'div',
                    null,
                    React.createElement('uTextBlock', {
                        Slot: { Padding: Utils.ltrb(20, 0) },
                        Font: {
                            FontObject: F_Sans,
                            Size: 16
                        },
                        Text: "TODO"
                    })
                )
            ),
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
                        if (this.settings.IsDirty()) {
                            ConfirmDialog.open(Utils.tr("提示"), Utils.tr("当前有未保存的修改，是否要放弃并返回"), this.props.onBack);
                        } else {
                            this.props.onBack();
                        }
                    }
                }),
                React.createElement(GeneralButton, {
                    borderWidth: 2,
                    text: Utils.tr("应用"),
                    onClicked: () => {
                        this.settings.ApplySettings(true);
                    }
                })
            )
        );
    }
}

module.exports = SettingsView;