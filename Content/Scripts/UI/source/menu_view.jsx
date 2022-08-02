const _ = require('lodash')
const UClass = require('UClass')()
const React = require('react')
const ReactUMG = require('react-umg')
const Utils = require('../Utils')
const EAnchors = require('../anchors')
const {F_Sans} = require('../style')

const TabButton = require('./tab_button')
const MenuBase = require('./menu_base')
const MenuBackpack = require('./menu_backpack')
const MenuTech = require('./menu_tech')
const MenuInfo = require('./menu_info')

let ThisWidget = null

class MyOverlayer extends JavascriptWidget {
    ctor() {
        // 这里要延时设置焦点，否则接收不到按键事件
        this.bIsFocusable = true
        this.Visibility = ESlateVisibility.Visible
        setTimeout(() => {
            if (this.GetOwningPlayerPawn()) {
                this.SetFocus()
            }
        }, 50);

        this.close = () => {
            ThisWidget.RemoveFromViewport()
        }
    }
    OnKeyDown(MyGeometry, InKeyEvent) {
        const cur_key = InKeyEvent.GetKey().KeyName
        const menu_key = InputSettings.GetInputSettings().GetActionMappingByName("Menu").OutMappings[0].Key.KeyName
        if (cur_key === menu_key || cur_key === "Escape") {
            this.close()
            return EventReply.Handled()
        }

        if (cur_key === "F10") {
            MainLevelScript.GetMainChar().Body.ChangeHP(-9999)
        }
        return EventReply.Unhandled()
    }
}
let MyOverlayer_C = UClass(global, MyOverlayer)
ReactUMG.Register('uMyOverlayer', MyOverlayer_C)


class MenuView extends React.Component {
    constructor(props) {
        super(props)
        this.state = {
            currentIndex: 0,
            explorePoints: MainPlayerState.Instance().GetExplorePoints()
        }
        let self = this
        this.updatePoints = () => {
            self.setState({explorePoints: MainPlayerState.Instance().GetExplorePoints()})
        }
        MainPlayerState.Instance().ExplorePointsChangedDelegate.Add(this.updatePoints)

        let list = ["存储", "科技", "见闻"]
        this.inBase = !!MainLevelScript.GetEarthBase().FindMainChar()
        this.menuList = this.inBase ? ["基地", ...list] : list
    }

    componentDidMount() {
        Utils.setInputMode(true)
    }

    componentWillUnmount() {
        if (MainPlayerState.Instance()) {
            MainPlayerState.Instance().ExplorePointsChangedDelegate.Remove(this.updatePoints)
        }
        Utils.setInputMode(false)
        ThisWidget = null
    }

    render() {
        return (
            <uMyOverlayer>
            <uCanvasPanel>
                <uBackgroundBlur
                    Slot={{
                        LayoutData: {
                            Anchors: EAnchors.FillAll,
                            Offsets: Utils.ltrb(0)
                        }
                    }}
                    BlurStrength={7}
                >
                    <uImage
                        Slot={{ Padding: Utils.ltrb(0) }}
                        ColorAndOpacity={Utils.rgba(0, 0, 0, 0.2)}
                    />
                </uBackgroundBlur>
                <span
                    Slot={{
                        LayoutData: {
                            Anchors: EAnchors.FillTop,
                            Offsets: Utils.ltrb(50, 50, 50, 80)
                        }
                    }}
                >
                    {_.map(this.menuList, (v, i) => (
                        <TabButton
                            checked={this.state.currentIndex === i}
                            text={v}
                            onClicked={()=>{
                                this.setState({currentIndex: i})
                            }}
                        />)
                    )}
                    <uSpacer Slot={{ Size: { SizeRule: ESlateSizeRule.Fill, Value: 1.0 }}} />
                    <span>
                        <uImage
                            Slot={{
                                VerticalAlignment: EVerticalAlignment.VAlign_Center
                            }}
                            Brush={{
                                ResourceObject: Utils.icon("T_Points64x64"),
                                ImageSize: {X: 48, Y: 48}
                            }}
                        />
                        <uTextBlock
                            Slot={{
                                Padding: Utils.ltrb(10, 0, 0, 0),
                                VerticalAlignment: EVerticalAlignment.VAlign_Center
                            }}
                            Font={{
                                FontObject: F_Sans,
                                TypefaceFontName: "Bold",
                                Size: 20,
                            }}
                            ColorAndOpacity={{
                                SpecifiedColor: Utils.color("#D49D35")
                            }}
                            Text={this.state.explorePoints}
                        />
                    </span>
                </span>
                <uWidgetSwitcher
                    ref={elem => {
                        if (elem && !this.tabView) {
                            this.tabView = elem.ueobj
                        }
                    }}
                    Slot={{
                        LayoutData: {
                            Anchors: EAnchors.FillAll,
                            Offsets: Utils.ltrb(50, 50 + 80, 50, 50)
                        }
                    }}
                    ActiveWidgetIndex={this.state.currentIndex}
                >
                    {this.inBase && <MenuBase />}
                    <MenuBackpack />
                    <MenuTech />
                    <MenuInfo />
                </uWidgetSwitcher>
            </uCanvasPanel>
            </uMyOverlayer>
        )
    }
}


module.exports = function() {
    if (!ThisWidget) {
        ThisWidget = ReactUMG.wrap(<MenuView/>)
    }
    return ThisWidget
}