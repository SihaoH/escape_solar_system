const Utils = require("utils")

const T_Rect = Texture2D.Load("/Game/UI/Icon/T_Rectangle64x64")
const F_Sans = Font.Load('/Game/UI/Font/SourceHanSansSC')

const ButtonStyle = {
    Normal: {
        DrawAs: ESlateBrushDrawType.NoDrawType
    },
    Hovered: {
        DrawAs: ESlateBrushDrawType.Image,
        TintColor: { SpecifiedColor: Utils.color("#FFF") },
    },
    Pressed: {
        DrawAs: ESlateBrushDrawType.Image,
        TintColor: { SpecifiedColor: Utils.color("#DDD") },
    },
    NormalForeground: { SpecifiedColor: Utils.color("#FFF") },
    HoveredForeground: { SpecifiedColor: Utils.color("#333") },
    PressedForeground: { SpecifiedColor: Utils.color("#333") },
    NormalPadding: Utils.ltrb(0),
    PressedPadding: Utils.ltrb(0)
}

const _BackgroundBrush = {
    ResourceObject: T_Rect,
    DrawAs: ESlateBrushDrawType.Border,
    TintColor: { SpecifiedColor: Utils.rgba(1, 1, 1, 0.3) },
    Margin: Utils.ltrb(0, 0, 0, 2/64)
}
const _FillBrush = {
    ResourceObject: T_Rect,
    DrawAs: ESlateBrushDrawType.Border,
    TintColor: { SpecifiedColor: Utils.color("#FFF") },
    Margin: Utils.ltrb(0, 0, 0, 2/64)
}
const SpinBoxProps = {
    WidgetStyle: {
        BackgroundBrush: _BackgroundBrush,
        HoveredBackgroundBrush: _BackgroundBrush,
        ActiveFillBrush: _FillBrush,
        InactiveFillBrush: _FillBrush,
        TextPadding: Utils.ltrb(0)
    },
    ForegroundColor: {
        SpecifiedColor: Utils.color("#FFF")
    },
    Font: {
        FontObject: Font.Load('/Game/UI/Font/SourceHanSansSC'),
        TypefaceFontName: "Bold",
        Size: 20,
    },
    Justification: ETextJustify.Center,
    MinValue: 1,
    MinFractionalDigits: 0,
    MaxFractionalDigits: 0,
    Delta: 1,
    ClearKeyboardFocusOnCommit: true,
    bAlwaysUsesDeltaSnap: true,
}

module.exports = {
    F_Sans: F_Sans,
    T_Rect: T_Rect,
    ButtonStyle: ButtonStyle,
    SpinBoxProps: SpinBoxProps
}