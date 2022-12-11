const _ = require('lodash')
const React = require('react')
const Utils = require('../utils')
const EAnchors = require('../anchors')
const {F_Sans, ButtonStyle} = require('../style')
const ConfirmDialog = require('confirm_dialog')
const GeneralButton = require('general_button')

class SettingsView extends React.Component {
    constructor(props) {
        super(props)
    }
    componentWillUnmount() {
    }

    render() {
        let window_mode = { Fullscreen: Utils.tr("全屏"), WindowedFullscreen: Utils.tr("无边框全屏"), Windowed: Utils.tr("窗口") }
        return (
            <uCanvasPanel {...this.props}>
                <uTextBlock
                    Font={{
                        FontObject: F_Sans,
                        Size: 16,
                    }}
                    Text={ "TODO" }
                />

                <span
                    Slot={{
                        LayoutData: {
                            Anchors: EAnchors.BottomLeft,
                            Alignment: { X: 0, Y: 1.0 },
                            Offsets: Utils.ltrb(50, -100, 500, 50)
                        }
                    }}
                >
                    <GeneralButton
                        Slot={{ Padding: Utils.ltrb(0, 0, 20, 0) }}
                        borderWidth={2}
                        text={ Utils.tr("返回") }
                        onClicked={() => {
                            if (this.settings.IsDirty()) {
                                ConfirmDialog.open(Utils.tr("提示"), Utils.tr("当前有未保存的修改，是否要放弃并返回"), this.props.onBack)
                            } else {
                                this.props.onBack()
                            }
                        }}
                    />
                    <GeneralButton
                        borderWidth={2}
                        text={ Utils.tr("应用") }
                        onClicked={() => {
                            this.settings.ApplySettings(true)
                        }}
                    />
                </span>
            </uCanvasPanel>
        )
    }
}

module.exports = SettingsView