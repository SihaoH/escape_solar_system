const _ = require('lodash')
const React = require('react')
const ReactUMG = require('react-umg')
const Utils = require('../utils')
const EAnchors = require('../anchors')
const {F_Sans, ButtonStyle} = require('../style')

const SettingsView = require('settings_view')
const AboutView = require('about_view')
const ConfirmDialog = require('confirm_dialog')

let ThisWidget = null

const EPageType = { Home: 0, Settings: 1, About: 2 }

class MainButton extends React.Component {
    constructor(props) {
        super(props)
    }

    render() {
        return (
            <uSizeBox
                Slot={{ Padding: Utils.ltrb(0, 5) }}
                WidthOverride={280}
                HeightOverride={80}
            >
                <uButton
                    WidgetStyle={ButtonStyle}
                    IsFocusable={false}
                    OnReleased={this.props.onClicked}
                >
                    <uTextBlock
                        Slot={{ Padding: Utils.ltrb(0) }}
                        Font={{
                            FontObject: F_Sans,
                            TypefaceFontName: "Bold",
                            Size: 24,
                        }}
                        ColorAndOpacity={{
                            ColorUseRule: ESlateColorStylingMode.UseColor_Foreground
                        }}
                        Text={ this.props.text }
                    />
                </uButton>
            </uSizeBox>
        )
    }
}

class StartView extends React.Component {
    constructor(props) {
        super(props)

        GameUserSettings.GetGameUserSettings().ApplySettings(true)

        this.state = {
            pageType: EPageType.Home
        }

        this.openMainLv = (need_load) => {
            MainSaveGame.SetNeedLoad(need_load)
            ThisWidget.RemoveFromViewport()
            // 第二参数直接使用string不行，要这样先加载关卡对象
            setTimeout(() => {
                GameplayStatics.OpenLevel(GWorld, World.Load("/Game/MainBP/Maps/SolarSystemMap"))
            }, 2000)
        }
    }

    componentDidMount() {
        process.nextTick(() => {
            // 画面淡入效果
            GWorld.GetPlayerCameraManager(0).StartCameraFade(1.0, 0.0, 3.0)
            Utils.setInputMode(true)
        })
    }

    componentWillUnmount() {
        ThisWidget = null
        Utils.setInputMode(false)
        let CM = GWorld ? GWorld.GetPlayerCameraManager(0) : null
        if (CM) {
            CM.StartCameraFade(0.0, 1.0, 2.0, Utils.rgba(0, 0, 0), false, true)
        }
    }

    render() {
        return (
            <uCanvasPanel>
                {this.state.pageType === EPageType.Home &&
                <uCanvasPanel
                    Slot={{
                        LayoutData: {
                            Anchors: EAnchors.Left,
                            Alignment: { X: 0, Y: 0.5 },
                            Offsets: Utils.ltrb(100, 0, 0, 0),
                        },
                        bAutoSize: true
                    }}
                >
                    <uTextBlock
                        Slot={{
                            LayoutData: {
                                Anchors: EAnchors.Left,
                                Alignment: { X: 0, Y: 1.0 },
                                Offsets: Utils.ltrb(100, -50, 0, 0)
                            },
                            bAutoSize: true
                        }}
                        Font={{
                            FontObject: F_Sans,
                            TypefaceFontName: "Bold",
                            Size: 96,
                            LetterSpacing: 100,
                            OutlineSettings: {
                                OutlineSize: 4,
                                OutlineColor: Utils.rgba(0, 0, 0, 0.6)
                            }
                        }}
                        ColorAndOpacity={{ SpecifiedColor: Utils.color("#CCF") }}
                        Text={ Utils.tr("逃离：太阳系") }
                    />
                    <div
                        Slot={{
                            LayoutData: {
                                Anchors: EAnchors.Left,
                                Alignment: { X: 0, Y: 0 },
                                Offsets: Utils.ltrb(100, 50, 0, 0)
                            },
                            bAutoSize: true
                        }}
                    >
                        {MainSaveGame.HasAr() &&
                        <MainButton
                            text={Utils.tr("继续")}
                            onClicked={() => {
                                this.openMainLv(true)
                            }}
                        />}
                        <MainButton
                            text={Utils.tr("新游戏")}
                            onClicked={() => {
                                if (MainSaveGame.HasAr()) {
                                    ConfirmDialog.open(Utils.tr("提示"), Utils.tr("开始新游戏会覆盖已有存档"), () => {
                                        this.openMainLv(false)
                                    })
                                } else {
                                    this.openMainLv(false)
                                }
                            }}
                        />
                        <MainButton
                            text={Utils.tr("设置")}
                            onClicked={() => {
                                this.setState( {pageType: EPageType.Settings} )
                            }}
                        />
                        <MainButton
                            text={Utils.tr("关于")}
                            onClicked={() => {
                                this.setState( {pageType: EPageType.About} )
                            }}
                        />
                        <MainButton
                            text={Utils.tr("退出游戏")}
                            onClicked={() => {
                                ThisWidget.RemoveFromViewport()
                                process.nextTick(() => GWorld.QuitGame())
                            }}
                        />
                    </div>
                </uCanvasPanel>}

                {this.state.pageType === EPageType.Settings &&
                <SettingsView
                    Slot={{
                        LayoutData: {
                            Anchors: EAnchors.Left,
                            Alignment: { X: 0, Y: 0.5 },
                            Offsets: Utils.ltrb(100, 0, 1000, 800)
                        }
                    }}
                    onBack={() => {
                        this.setState( {pageType: EPageType.Home} )
                    }}
                />}
                {this.state.pageType === EPageType.Settings &&
                <AboutView
                    Slot={{
                        LayoutData: {
                            Anchors: EAnchors.Left,
                            Alignment: { X: 0, Y: 0.5 },
                            Offsets: Utils.ltrb(100, 0, 1000, 800)
                        }
                    }}
                    onBack={() => {
                        this.setState( {pageType: EPageType.Home} )
                    }}
                />}
            </uCanvasPanel>
        )
    }
}

ThisWidget = ReactUMG.wrap(<StartView/>)
module.exports = ThisWidget