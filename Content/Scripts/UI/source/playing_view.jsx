const React = require('react')
const ReactUMG = require('react-umg')

class MyComponent extends React.Component {
    constructor(props, context) {
        super(props, context);
        this.state = {text:"MyComponent"};
    }

    OnTextChanged(value) {
        this.setState({text: value});
    }

    render() {
        return (
            <div>
                <uEditableTextBox Text={this.state.text} OnTextChanged={value=> this.OnTextChanged(value)}/>
                <text Text={this.state.text}/>
            </div>
        )
    }
}

module.exports = ReactUMG.wrap(<MyComponent/>);