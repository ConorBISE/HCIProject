#ifndef SCREEN_EVENT_H
#define SCREEN_EVENT_H

enum class ButtonPosition {
    Top,
    Left,
    Right
};

enum class ButtonState {
    Pressed,
    Released
};

struct ButtonEvent {
    ButtonPosition position;
    ButtonState state;
};

#endif // SCREEN_EVENT_H