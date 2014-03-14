
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
		
	bool state_function(void);
	State* transition_function(void);
};

class WorkStep1: public HardwareState {
private:
	State* myNextState;
public:
	WorkStep1(void* hardware, State* nextState) : 
		HardwareState("WorkStep1", hardware), myNextState(nextState) {}
		
	bool state_function(void);
	State* transition_function(void);
};


class WorkStep2: public HardwareState {
private:
	State* myBeginAgainState;
	State* myTidyUpState;
public:
	WorkStep2(void* hardware, State* beginAgainState, State* tidyUpState) : 
		HardwareState("WorkStep2", hardware), myBeginAgainState(beginAgainState), myTidyUpState(tidyUpState) {}
		
	bool state_function(void);
	State* transition_function(void);
};


class ShutdownHardware: public HardwareState {
public:
	ShutdownHardware(void* hardware) :
		HardwareState("ShutdownHardware", hardware) { }
		
	bool state_function(void);
	State* transition_function(void);
};
//! [States]


	

//! [Functions]
bool PrepareHardware::state_function(void) {
	// do stuff
	return true;
}
State* PrepareHardware::transition_function(void) {
	if (/* whatever */) {
		return myNextState;
	} else {
		return this;
	}
}


bool WorkStep1::state_function(void) {
	// do stuff	
	return true;
}
State* WorkStep1::transition_function(void) {
	if (/* whatever */) {
		return myNextState;
	} else {
		return this;
	}
}


bool WorkStep2::state_function(void) {
	// do stuff
	return true;
}
State* WorkStep2::transition_function(void) {
	if (/* whatever */) {
		return myBeginAgainState;
	} else if (/* whatever */) {
		return myTidyUpState;
	} else {
		return this;
	}
}


bool ShutdownHardware::state_function(void) {
	// do stuff
	return true;
}
State* ShutdownHardware::transition_function(void) {
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
	&Left::workStep1,
	&hardwareStatemachineLeftInitEvent,
	&Left::shutdownHardware,
	&hardwareStatemachineLeftFinishEvent,
	&hardwareStatemachineLeftCancelOnErrorEvent	
); 
//! [Statemachine]


//! [Rest]
} // namespace IM
} // namespace TURAG
//! [Rest]

