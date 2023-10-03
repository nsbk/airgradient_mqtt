/*
 * Copyright 2017 Antonio Coratelli - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the BSD 3-Clause license.
 */
#pragma once
#include <memory.h>
#include <set.h>
#include <string.h>
#include <typeindex.h>
#include "Event.h"
#include "Transition.h"

//namespace antoniocoratelli {
//namespace statemachine {

class ControllableEvent;
class NonControllableEvent;

/**
 * State defines the interface for implementing all the states of a given StateMachine.
 * Each concrete state must register the ControllableEvents that can be triggered when the
 * state is active, calling `State::attach<ControllableEvent>()` in the constructor.
 * For example:
 * @code
 *   class StateIncomingCall: public State {
 *    public:
 *     StateIncomingCall() {
 *       this->attach<EventAnswer>();
 *       this->attach<EventIgnore>();
 *     }
 *   }
 * @endcode
 * will enable the ControllableEvents "Answer" and "Ignore" when the system is in state
 * "IncomingCall".
 */
class State {
 public:
    typedef std::unique_ptr<State> uptr_t;

    /**
     * Must be implemented by each concrete state to define the logic for handling
     * NonControllable Events.
     * If the state is designed not to have any NonControllable Events, the function
     * can be simply defined as: `return {}`.
     */
    virtual Transition update() = 0;

    /**
     * Short string representation of the state.
     */
    virtual std::string name() = 0;

    /**
     * Extended string representation of the state.
     */
    virtual std::string info() = 0;

    /**
     * Returns true iff the state can trigger an event of type `controllable_event_t`,
     * that is, iff the event `controllable_event_t` was attached to the state.
     */
    template<class controllable_event_t> bool triggers() const;

    virtual ~State() {}

 protected:
    /**
     * Registers events of type `controllable_event_t` as a ControllableEvent
     * that can be triggered when the StateMachine is in the state defined
     * by this object.
     */
    template<class controllable_event_t> void attach();

 private:
    /**
     * Set of the ControllableEvents that were attached to this state.
     */
    std::set<std::type_index> _allowed_controllable_events;
};

template<class controllable_event_t> inline void State::attach() {
    static_assert(std::is_base_of<ControllableEvent, controllable_event_t>::value);
    std::type_index event_type(typeid(controllable_event_t));
    _allowed_controllable_events.insert(event_type);
}

template<class controllable_event_t> inline bool State::triggers() const {
    static_assert(std::is_base_of<ControllableEvent, controllable_event_t>::value);
    std::type_index event_type(typeid(controllable_event_t));
    return _allowed_controllable_events.count(event_type) > 0;
}

//}  // namespace statemachine
//}  // namespace antoniocoratelli