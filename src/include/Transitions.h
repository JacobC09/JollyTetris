#pragma once
#include "pch.h"

enum TransitionTypes {
    Normal,
    Arrow,
    ArrowReversed
};

class Transition {
public:
    int timer = 0;
    int duration;
    const char *name;

    Transition() = default;
    Transition(int _duration, const char* _name="");

    virtual void Draw();
    virtual TransitionTypes GetType() {return TransitionTypes::Normal;};
    float GetValue();
    bool isFinished();
};

class ArrowTransition : public Transition {
public:
    ArrowTransition(int _duration, int _arrowOffset, Color _color=BLACK, const char* _name="");
    void Draw();
    TransitionTypes GetType() {return TransitionTypes::Arrow;};

protected:
    int arrowOffset;
    Color color;
};

class ArrowTransitionReversed : public ArrowTransition {
public:
    using ArrowTransition::ArrowTransition;

    void Draw();
    TransitionTypes GetType() {return TransitionTypes::ArrowReversed;};

};

void DrawTransitions();
void AddTransition(Transition* transition);
Transition* GetTransition(TransitionTypes type);
Transition* GetTransition(const char* name);
Transition* GetTransition(TransitionTypes type, const char* name);
bool IsFinished(Transition* transition);
