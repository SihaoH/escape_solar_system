const _ = require('lodash')
const uclass = require('uclass')()
const React = require('react')
const ReactUMG = require('react-umg')
const Utils = require('../utils')
const EAnchors = require('../anchors')

const TabButton = require('./tab_button')
const MenuBase = require('./menu_base')
const MenuBackpack = require('./menu_backpack')
const MenuLevel = require('./menu_level')
const MenuInfo = require('./menu_info')

let ThisWidget = null

class MyOverlayer extends JavascriptWidget {
    ctor() {
        this.bIsFocusable = true
        this.Visibility = ESlateVisibility.Visible
        setTimeout(() => {
            if (this.GetOwningPlayerPawn()) {
                this.SetFocus()
            }
        }, 50);

        this.close = () => {
            ThisWidget.RemoveFromViewport()
            Utils.setInputMode(false)
            ThisWidget = null
        }
    }
    OnKeyDown(MyGeometry, InKeyEvent) {
        if (InKeyEvent.GetKey().KeyName === "Escape") {
            this.close()
            return EventReply.Handled()
        }
        return EventReply.Unhandled()
    }
}
let MyOverlayer_C = uclass(global, MyOverlayer)
ReactUMG.Register('uMyOverlayer', MyOverlayer_C)


class MenuView extends React.Component {
    constructor(props) {
        super(props)
        this.state = {currentIndex: 0}

        Utils.setInputMode(true)
    }

    componentDidMount() {
    }

    componentWillUnmount() {
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
                            Offsets: Utils.ltrb(50, 50, 0, 80)
                        }
                    }}
                >
                    {_.map(["基地", "存储", "科技", "情报"], (v, i) => (
                        <TabButton
                            checked={this.state.currentIndex === i}
                            text={v}
                            onClicked={()=>{
                                this.setState({currentIndex: i})
                                //this.tabView.SetActiveWidgetIndex(i)
                            }}
                        />)
                    )}
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
                    <MenuBase />
                    <MenuBackpack />
                    <MenuLevel />
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