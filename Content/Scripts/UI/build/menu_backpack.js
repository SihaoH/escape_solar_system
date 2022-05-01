const _ = require('lodash');
const uclass = require('uclass')();
const React = require('react');
const ReactUMG = require('react-umg');
const Utils = require('../utils');
const EAnchors = require('../anchors');

class MenuBase extends React.Component {
    constructor(props) {
        super(props);
    }

    componentDidMount() {}

    render() {
        return React.createElement(
            'uCanvasPanel',
            null,
            React.createElement('uTextBlock', {
                Slot: {
                    LayoutData: {
                        Anchors: EAnchors.Center,
                        Alignment: { X: 0.5, Y: 0.5 },
                        Offsets: Utils.ltrb(0)
                    }
                },
                Text: "Menu Backpack"
            })
        );
    }
}

module.exports = MenuBase;