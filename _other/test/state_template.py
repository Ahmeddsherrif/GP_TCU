from enum import Enum, auto
from queue import Queue
import threading
import time

class Event(Enum):
    idle = auto()
    start = auto()
    kill = auto()
    terminate = auto()

class State(Enum):
    dead = auto()
    active = auto()

intial_state = State.dead

#ِ All the variables used by the state Machine
class stateMachineContext:
    def __init__(self):
        self.cmd = ""

class EventMessage:
    def __init__(self):
        self.event = Event.idle
        self.data = ""

# Create a mutex lock
mutexEventQueue = threading.Lock()
eventMessageQueue = Queue()

tempEventMessage = EventMessage()
tempEventMessage.event = Event.start

mutexEventQueue.acquire()
eventMessageQueue.put(tempEventMessage)
mutexEventQueue.release()

class StateMachine:
    def __init__(self):
        self.currentState = intial_state
        self.currentEvent = Event.idle
        

        self.stateEntry = True
        self.stateExit = False

        self.context = stateMachineContext()

    def getEvent(self):
        self.currentEvent = Event.idle;

        mutexEventQueue.acquire()
        if eventMessageQueue.empty() == False:
            tempEventMessage = eventMessageQueue.get()
            self.currentEvent = tempEventMessage.event
        mutexEventQueue.release()

    def state_dead_handler_processing(self):
        time.sleep(1)

    def state_active_handler_processing(self):
        time.sleep(1)

    def state_dead_handler(self):
        if self.stateEntry == True:
            self.stateEntry = False
            print("Entering Dead State")

        self.state_dead_handler_processing()

        if self.currentEvent == Event.start:
            self.currentState = State.active
            self.stateExit = True

        if self.stateExit == True:
            self.stateExit = False
            self.stateEntry = True
            print("Exiting Dead State")


    def state_active_handler(self):
        if self.stateEntry == True:
            self.stateEntry = False
            print("Entering active State")

        self.state_active_handler_processing()

        if self.currentEvent == Event.kill:
            self.currentState = State.dead
            self.stateExit = True

        if self.stateExit == True:
            self.stateExit = False
            self.stateEntry = True
            print("Exiting active State")



    def run(self):
        while(True):

            self.getEvent()

            # Break Condition
            if self.currentEvent == Event.terminate:
                break;

            # Process States
            if self.currentState == State.dead:
                self.state_dead_handler()

            elif self.currentState == State.active:
                self.state_active_handler()

            


        print("HASTA LAVISTA")



if __name__ == "__main__":
    sm = StateMachine()
    sm.run()