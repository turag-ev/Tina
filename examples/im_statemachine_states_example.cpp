
//! [Includes]
#include <tina++/im/statemachine.h>


namespace TURAG {
namespace IM {
namespace {
//! [Includes]

	
//! [Basestate]
class HardwareState : public State {
public:
	HardwareState(const char* const name, void * const hardware) :
		State(name), hardware_(hardware) {}

protected:
	void* const hardware_;
};
//! [Basestate]

	
//! [States]
class PrepareHardware: public HardwareState {
private:
	State* myNextState;
public:
	PrepareHardware(void* hardware, State* nextState) : 
		HardwareState("PrepareHardware", hardware), myNextState(nextState) {}
		
	State* transition_function(bool firstRun) override;
};

class WorkStep1: public HardwareState {
private:
	State* myNextState;
public:
	WorkStep1(void* hardware, State* nextState) : 
		HardwareState("WorkStep1", hardware), myNextState(nextState) {}
		
	State* transition_function(bool firstRun) override;
};


class WorkStep2: public HardwareState {
private:
	State* myBeginAgainState;
	State* myTidyUpState;
public:
	WorkStep2(void* hardware, State* beginAgainState, State* tidyUpState) : 
		HardwareState("WorkStep2", hardware), myBeginAgainState(beginAgainState), myTidyUpState(tidyUpState) {}
		
	State* transition_function(bool firstRun) override;
};


class ShutdownHardware: public HardwareState {
public:
	ShutdownHardware(void* hardware) :
		HardwareState("ShutdownHardware", hardware) { }
		
	State* transition_function(bool firstRun) override;
};
//! [States]


	

//! [Functions]
State* PrepareHardware::transition_function(bool firstRun) {
    if (firstRun) {
        // do initializations
    }
    
	if (/* whatever */) {
		return myNextState;
	} else {
		return this;
	}
}


State* WorkStep1::transition_function(bool firstRun) {
    if (firstRun) {
        // do initializations
    }
    
	if (/* whatever */) {
		return myNextState;
	} else {
		return this;
	}
}


State* WorkStep2::transition_function(bool firstRun) {
    if (firstRun) {
        // do initializations
    }
    
	if (/* whatever */) {
		return myBeginAgainState;
	} else if (/* whatever */) {
		return myTidyUpState;
	} else {
		return this;
	}
}


State* ShutdownHardware::transition_function(bool firstRun) {
    if (firstRun) {
        // do initializations
    }
    
	if (/* whatever */) {
		return Statemachine::finished;
	} else {
		return this;
	}
}
//! [Functions]


//! [Instances]
namespace Left {
	
extern PrepareHardware prepareHardware;
extern WorkStep1 workStep1;
extern WorkStep2 workStep2;
extern ShutdownHardware shutdownHardware;

PrepareHardware prepareHardware(&leftDevice, &workStep1);
WorkStep1 workStep1(&leftDevice, &workStep2);
WorkStep2 workStep2(&leftDevice, &workStep1, &shutdownHardware);
ShutdownHardware shutdownHardware(&leftDevice);

}
//! [Instances]

namespace Right {
	
extern PrepareHardware prepareHardware;
extern WorkStep1 workStep1;
extern WorkStep2 workStep2;
extern ShutdownHardware shutdownHardware;

PrepareHardware prepareHardware(&rightDevice, &workStep1);
WorkStep1 workStep1(&rightDevice, &workStep2);
WorkStep2 workStep2(&rightDevice, &workStep1, &shutdownHardware);
ShutdownHardware shutdownHardware(&rightDevice);

}

//! [Statemachine]
} // namespace 

Statemachine hardwareStatemachineLeft(
	"Hardware statemachine",
	&Left::prepareHardware,
	&hardwareStatemachineLeftFinishEvent,
	&hardwareStatemachineLeftCancelOnErrorEvent,
    &Left::shutdownHardware
); 
//! [Statemachine]


//! [Rest]
} // namespace IM
} // namespace TURAG
//! [Rest]

