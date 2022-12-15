#include <string.h>
#include "Transitions.h"
#include "debug.h"
#include "Easing.h"

std::vector<Transition*> transitions;

/* ============= Base Class ============= */
Transition::Transition(int _duration, const char* _name) {
    duration = _duration;
    name = _name;
}

void Transition::Draw() {
    if (timer < duration)
        timer++;
}

float Transition::GetValue() {
    return (float) timer / duration;
}

bool Transition::isFinished() {
    return timer >= duration;
}

/* ============= Arrow Transition ============= */
ArrowTransition::ArrowTransition(int _duration, int _arrowOffset, Color _color, const char* _name) : Transition(_duration, _name) {
    arrowOffset = _arrowOffset;
    color = _color;
}

void ArrowTransition::Draw() {
    if (timer < duration)
        timer++;

    float x = EaseCubicInOut(timer, duration, -arrowOffset, GetScreenWidth() + arrowOffset);
    DrawRectangleRec({0, 0, x < 0 ? 0 : x, (float) GetScreenHeight()}, color);
    DrawTriangle({x, (float) GetScreenHeight()}, {x + arrowOffset, GetScreenHeight() * 0.5f}, {x, 0}, color);

}

/* ============= Arrow Transition Reversed ============= */

void ArrowTransitionReversed::Draw() {
    if (timer < duration)
        timer++;

    float x = EaseCubicInOut(timer, duration, -arrowOffset, GetScreenWidth() + arrowOffset);
    
    DrawRectangleRec({x + arrowOffset, 0, GetScreenWidth() - x - arrowOffset, (float) GetScreenHeight()}, color);
    DrawTriangle({x + arrowOffset, GetScreenHeight() * 0.5f}, {x + arrowOffset, 0}, {x, 0}, color);
    DrawTriangle({x + arrowOffset, (float) GetScreenHeight()}, {x + arrowOffset, GetScreenHeight() * 0.5f}, {x, (float) GetScreenHeight()}, color);

}

/* ============= Functions ============= */

void DrawTransitions() {
    for (int index = (signed) transitions.size() - 1; index > -1; index--) {
        Transition* transition = transitions[index];

        if (transition->isFinished()) {
            free(transition);
            transitions.erase(transitions.begin() + index);
            continue;
        }
        
        transition->Draw();
    }
}

void AddTransition(Transition* transition) {
    transitions.push_back(transition);
}

Transition* GetTransition(TransitionTypes type) {
    for (int index = 0; index < (signed) transitions.size(); index++) {
        if (transitions[index]->GetType() == type)
            return transitions[index];
    }
    return nullptr;
}

Transition* GetTransition(const char* name) {
    for (int index = 0; index < (signed) transitions.size(); index++) {
        if (strcmp(transitions[index]->name, name) == 0)
            return transitions[index];
    }
    return nullptr;
}

Transition* GetTransition(TransitionTypes type, const char* name) {
    for (int index = 0; index < (signed) transitions.size(); index++) {
        if (transitions[index]->GetType() == type && strcmp(transitions[index]->name, name) == 0)
            return transitions[index];
    }
    return nullptr;
}

bool IsFinished(Transition* transition) {
    if (transition == nullptr) return false;
    
    return transition->isFinished();
}
