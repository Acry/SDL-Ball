# InputManager

so, jetzt entscheide dich mal. entweder hauen wir von außen einen keyboard boost rein, mit float moveStep = 8.0f * deltaTime;
oder wir machen das von innen:

const bool isMouseInput = std::abs(targetX - pos_x) > 0.1f;

später kommen die modifier in die controller settings, aber jetzt nicht

Paddle:

```c++
enum class InputType {
MOUSE,
KEYBOARD,
CONTROLLER
};

void Paddle::moveTo(const float targetX, const float deltaTime, InputType inputType = InputType::KEYBOARD) {
switch (inputType) {
case InputType::MOUSE:
// Mauseingabe: Direkte Positionierung mit Dämpfung
pos_x += (targetX - pos_x) * 8.0f * deltaTime;
break;

        case InputType::KEYBOARD:
        case InputType::CONTROLLER:
            // Tastatur/Controller: Konstante Geschwindigkeit
            if (targetX != pos_x) {
                const float direction = (targetX > pos_x) ? 1.0f : -1.0f;
                const float speed = (inputType == InputType::KEYBOARD) ? 1.2f : 1.0f;
                pos_x += direction * speed * deltaTime;
            }
            break;
    }

    // Begrenzung auf den Bildschirmrand
    if (pos_x < -1.0f + width) pos_x = -1.0f + width;
    if (pos_x > 1.0f - width) pos_x = 1.0f - width;
}
```
