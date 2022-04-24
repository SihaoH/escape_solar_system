const _ = require('lodash')
const uclass = require('uclass')()
const React = require('react')
const ReactUMG = require('react-umg')
const Utils = require('../utils')
const EAnchors = require('../anchors')

const Helper = new PlayingHelper()
const PointBar = require('./point_bar')
const MessageListView = require('./message_listview')


class PlayingView extends React.Component {
    constructor(props) {
        super(props)
        this.state = {
            HP: { cur: 0, max: 1},
            MP: { cur: 0, max: 1},
            debugInfo: []
        }

        this.interval = setInterval(() => this.tick(), 200)
        MainLevelScriptActor.GetInstance().MessagedDelegate.Add((msg) => {
            this.msgListView.AppendMsg(msg)
        })
    }

    componentDidMount() {
    }

    tick() {
        let player = MainLevelScriptActor.GetMainChar()
        let ship = MainLevelScriptActor.GetSpaceship()
        if (ship && ship.CurrentPilot) {
            player = ship
        }
        this.setState({
            HP: { cur: player.Body.CurrentHP, max: player.Body.MaximumHP },
            MP: { cur: player.Engine.CurrentEnergy, max: player.Engine.MaximumEnergy},
        })

        if (Utils.isDev()) {
            this.setState({debugInfo: Helper.GetDebugInfo().OutList})
        }
    }

    render() {
        return (
            <uCanvasPanel>
                <div Slot={{LayoutData: { Offsets: Utils.ltrb(50, 50, 400, 50) }}} >
                    <PointBar
                        Slot={{Size: { SizeRule: ESlateSizeRule.Fill }}}
                        curVal={this.state.HP.cur}
                        maxVal={this.state.HP.max}
                    />
                    <uSpacer Size={{ Y: 4 }} />
                    <PointBar
                        Slot={{Size: { SizeRule: ESlateSizeRule.Fill }}}
                        curVal={this.state.MP.cur}
                        maxVal={this.state.MP.max}
                    />
                </div>
                <div Slot={{ Position: {X: 50, Y: 300} }}>
                    {_.map(this.state.debugInfo, str => (
                        <text
                            Font={ {FontObject: Font.Load('/Game/UI/Font/SourceHanSansSC'), Size: 14} }
                            Text={str}
                        />
                    ))}
                </div>
                <MessageListView
                    ref={(elem) => {
                        if (elem) {
                            this.msgListView = elem
                        }
                    }}
                    Slot={{
                        LayoutData: {
                            Anchors: EAnchors.Right,
                            Alignment: { X: 1.0, Y: 0.5 },
                            Offsets: Utils.ltrb(-20, 0, 400, 300)
                        }
                    }}
                />
            </uCanvasPanel>
        )
    }
}

module.exports = ReactUMG.wrap(<PlayingView/>);