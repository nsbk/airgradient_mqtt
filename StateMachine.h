/*
 * Copyright 2017 Antonio Coratelli - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the BSD 3-Clause license.
 */
#pragma once
#include <iostream.h>
#include <stdexcept.h>
#include <string.h>
#include "Event.h"
#include "State.h"
#include "Transition.h"

// namespace antoniocoratelli {
// namespace statemachine {

/**
 * StateMachine implements the logic to check the type of the current state,
 * handles state changes for ControllableEvents, and delegates the handling of
 * NonControllableEvents to the current State object.
 * There is no need in theory to subclass StateMachine, but in practice it's
 * nice to override the name() and info() methods to display more
 * meaningful and descriptive information.
 */
class StateMachine {
 public:
    /**
     * Constructs the state with the desired initial state.
     */
    explicit StateMachine(State::uptr_t initial_state);

    /**
     * Returns true iff the current state of the object is `state_t`.
     */
    template<class state_t> bool in() const;

    /**
     * Short string representation of the state machine.
     */
    virtual std::string name() const;

    /**
     * Extended string representation of the state machine.
     */
    virtual std::string info() const;

    /**
     * Trigger a ControllableEvent and moves the state of the machine to the
     * destination state specified by the Event.
     * @throws std::logic_error If the event is not attached to the current state.
     */
    template<class controllable_event_t> void trigger();

    /**
     * Calls the update method of the current state, to handle the possible
     * NonControllableEvents defined by the state itself.
     * @returns true Iff the state of the machine changes as effect of the call.
     */
    virtual bool update() final;  // NOLINT(readability/inheritance)

    virtual ~StateMachine() {}

 protected:
    State::uptr_t _current_state;

 private:
    void apply(Transition t);
};

inline StateMachine::StateMachine(State::uptr_t initial_state):
    _current_state(initial_state.release()) {
}

inline std::string StateMachine::name() const {
    return _current_state->name();
}

inline std::string StateMachine::info() const {
    return _current_state->info();
}

inline void StateMachine::apply(Transition transition) {
    if (transition) {
        auto next_state = transition.value();
        std::cout << "{" << name() << "} Transitioning to: " << next_state->name() << std::endl;
        _current_state.reset(next_state);
    }
}

inline bool StateMachine::update() {
    Transition transition = _current_state->update();
    this->apply(transition);
    return transition.operator bool();
}

template<class state_t> inline bool StateMachine::in() const {
    static_assert(std::is_base_of<State, state_t>::value);
    State* current = _current_state.get();
    state_t* current_casted = dynamic_cast<state_t*>(current);
    return current_casted != NULL;
}

template<class controllable_event_t> inline void StateMachine::trigger() {
    static_assert(std::is_base_of<ControllableEvent, controllable_event_t>::value);
    controllable_event_t event;
    if (_current_state->triggers<controllable_event_t>()) {
        this->apply(event.execute());
    } else {
        std::string e = event.name();
        std::string s = _current_state->name();
        throw std::logic_error("Event<" + e + "> can't be triggered in State<" + s + ">");
    }
}

// }  // namespace statemachine
// }  // namespace antoniocoratelli