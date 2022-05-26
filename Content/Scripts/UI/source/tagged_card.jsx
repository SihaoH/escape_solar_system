const React = require('react')
const Utils = require('../utils')

class TaggedCard extends React.Component {
    constructor(props) {
        super(props)
    }
    render() {
        let { title } = this.props
        title = title || "Title"
        return (
            <div {...this.props} >
                <uTextBlock
                    Slot={{
                        Padding: Utils.ltrb(0, 10)
                    }}
                    Font={{
                        FontObject: Font.Load('/Game/UI/Font/SourceHanSansSC'),
                        TypefaceFontName: "Bold",
                        Size: 20,
                    }}
                    ColorAndOpacity={{ SpecifiedColor: Utils.color("#CCC") }}
                    ShadowColorAndOpacity={Utils.rgba(0, 0, 0, 0.6)}
                    Text={title}
                />
                {this.props.children}
            </div>
        )
    }
}

module.exports = TaggedCard