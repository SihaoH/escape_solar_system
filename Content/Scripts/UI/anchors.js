const EAnchors = {
    TopLeft:     { Minimum: {X: 0.0, Y: 0.0}, Maximum: {X: 0.0, Y: 0.0} },
    Top:         { Minimum: {X: 0.5, Y: 0.0}, Maximum: {X: 0.5, Y: 0.0} },
    TopRight:    { Minimum: {X: 1.0, Y: 0.0}, Maximum: {X: 1.0, Y: 0.0} },

    Left:        { Minimum: {X: 0.0, Y: 0.5}, Maximum: {X: 0.0, Y: 0.5} },
    Center:      { Minimum: {X: 0.5, Y: 0.5}, Maximum: {X: 0.5, Y: 0.5} },
    Right:       { Minimum: {X: 1.0, Y: 0.5}, Maximum: {X: 1.0, Y: 0.5} },

    BottomLeft:  { Minimum: {X: 0.0, Y: 1.0}, Maximum: {X: 0.0, Y: 1.0} },
    Bottom:      { Minimum: {X: 0.5, Y: 1.0}, Maximum: {X: 0.5, Y: 1.0} },
    BottomRight: { Minimum: {X: 1.0, Y: 1.0}, Maximum: {X: 1.0, Y: 1.0} },

    FillTop:     { Minimum: {X: 0.0, Y: 0.0}, Maximum: {X: 1.0, Y: 0.0} },
    FillHor:     { Minimum: {X: 0.0, Y: 0.5}, Maximum: {X: 1.0, Y: 0.5} },
    FillBottom:  { Minimum: {X: 0.0, Y: 1.0}, Maximum: {X: 1.0, Y: 1.0} },

    FillLeft:    { Minimum: {X: 0.0, Y: 0.0}, Maximum: {X: 0.0, Y: 1.0} },
    FillVer:     { Minimum: {X: 0.5, Y: 0.0}, Maximum: {X: 0.5, Y: 1.0} },
    FillRight:   { Minimum: {X: 1.0, Y: 0.0}, Maximum: {X: 1.0, Y: 1.0} },

    FillAll:     { Minimum: {X: 0.0, Y: 0.0}, Maximum: {X: 1.0, Y: 1.0} },
}

module.exports = EAnchors