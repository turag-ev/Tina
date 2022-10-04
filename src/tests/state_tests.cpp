#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <limits>

#include <tina++/statemachine.h>

using namespace TURAG;

namespace {

enum {
  event_test1 = EventNamespace('#', 'T', 't'),
  event_test2,
  event_test3,
  event_test4,
  event_test5,
  event_test6,
  event_test7,
  event_test8
};

ACTION_CLASS(TestChildAction) { 
  ACTION_STATES(TestChildAction, start_state, end)
  
public:
  bool start_called = false;
};

bool TestChildAction::start_state(EventId id, EventArg data) {
  switch (id) {
  case event_start:
    BOOST_CHECK_EQUAL(data, 12345);
    start_called = true;
    return true;
    
  case event_test4:
    BOOST_CHECK_EQUAL(data, 7980);
    return true;
    
  case event_test5:
    BOOST_CHECK_EQUAL(data, 5);
    return false;
 
  case event_test6:
    BOOST_CHECK_EQUAL(data, 6);
    nextState(&TestChildAction::end);
    return true;
    
  case event_test8:
    exit(event_failure);
    return true;
  }
  
  return false;
}

bool TestChildAction::end(EventId id, EventArg data) {
  switch (id) {
  case event_test7:
    BOOST_CHECK_EQUAL(data, 7);
    start_called = false;
    exit(event_success);
    return true;
  }
  
  return false;
}

ACTION_CLASS(TestMainAction) { 
  ACTION_STATES(TestMainAction, start_state, middle, end)
  
public:
  bool start_called = false;
  bool event1_called = false;
  bool nextState_called = false;
  bool called5 = false;
};

bool TestMainAction::start_state(EventId id, EventArg data) {
  switch (id) {
  case event_start:
    BOOST_CHECK_EQUAL(data, 34569);
    BOOST_CHECK(getChildAction() == nullptr);
    BOOST_CHECK(getParentAction() == nullptr);
    //BOOST_CHECK(getState() == &TestMainAction::start_state);
    //BOOST_CHECK(checkState(&TestMainAction::start_state));
    BOOST_CHECK(start_called == false);
    start_called = true;
    return true;
    
  case event_test1:
    BOOST_CHECK_EQUAL(data, 67890);
    BOOST_CHECK(event1_called == false);
    event1_called = true;
    return true;
    
  case event_test2:
    break;
    
  case event_test3:
    BOOST_CHECK_EQUAL(data, 0);
    nextState(&TestMainAction::middle, 45678);
    return true;
    
  default:
    BOOST_ERROR("sollte hier nicht landen");
  }
  
  return false;
}

bool TestMainAction::middle(EventId id, EventArg data) {
  switch (id) {
  case event_start:
    BOOST_CHECK_EQUAL(data, 45678);
    BOOST_CHECK(getChildAction() == nullptr);
    BOOST_CHECK(getParentAction() == nullptr);
    //BOOST_CHECK(getState() == &TestMainAction::middle);
    //BOOST_CHECK(checkState(&TestMainAction::middle));
    BOOST_CHECK(nextState_called == false);
    nextState_called = true;
    
    startChildAction(TestChildAction::getPointer(), 12345);
    BOOST_CHECK(getChildAction() == TestChildAction::getPointer());
    BOOST_CHECK(start_called == true);
    //BOOST_CHECK(checkState(&TestMainAction::middle));
    return true;
    
  case event_return:
    switch (data) {
    case event_success:
      BOOST_CHECK(getChildAction() == nullptr);
      nextState(&TestMainAction::end, 354657);
      return true;
      
    default:
      BOOST_ERROR("sollte hier nicht landen");
      return true;
    };
    
  case event_test4:
    BOOST_ERROR("sollte hier nicht landen");
    return true;
    
  case event_test5:
    BOOST_CHECK(called5 == false);
    called5 = true;
    return true;
    
  case event_test6:
    BOOST_ERROR("sollte hier nicht landen");
    return true;
    
  case event_test7:
    BOOST_ERROR("sollte hier nicht landen");
    return true;
    
  default:
    BOOST_ERROR("sollte hier nicht landen");
  }
  
  return false;
}

bool TestMainAction::end(EventId id, EventArg data) {
  switch (id) {
  case event_start:
    BOOST_CHECK(getChildAction() == nullptr);
    BOOST_CHECK(getParentAction() == nullptr);
    //BOOST_CHECK(getState() == &TestMainAction::end);
    //BOOST_CHECK(checkState(&TestMainAction::end));
    BOOST_CHECK_EQUAL(data, 354657);
    
    startChildAction(TestChildAction::getPointer(), 12345);
    BOOST_CHECK(getChildAction() == TestChildAction::getPointer());
    return true;
    
  case event_return:
    switch (data) {
    case event_failure:
      BOOST_CHECK(getChildAction() == nullptr);
      exit(event_success);
      return true;
      
    default:
      BOOST_ERROR("sollte hier nicht landen");
    };
  }
  
  return false;
}

}

BOOST_AUTO_TEST_SUITE(StateTests)

BOOST_AUTO_TEST_CASE(ActionTestCase) {
  TestMainAction* action = TestMainAction::getPointer();
  bool retval = false;

  BOOST_CHECK(action->start_called == false);
  action->start(34569);
  BOOST_CHECK(action->start_called == true);
  
  BOOST_CHECK(action->event1_called == false);
  retval = action->func(event_test1, 67890);
  BOOST_CHECK(action->event1_called == true);
  BOOST_CHECK(retval == true);
  
  retval = action->func(event_test2, 0);
  BOOST_CHECK(retval == false);
  
  retval = action->func(event_test3, 0);
  BOOST_CHECK(retval == true);
  
  retval = action->func(event_test4, 7980);
  BOOST_CHECK(retval == true);
  
  retval = action->func(event_test5, 5);
  BOOST_CHECK(retval == true);
  
  retval = action->func(event_test6, 6);
  BOOST_CHECK(retval == true);
  
  retval = action->func(event_test7, 7);
  BOOST_CHECK(retval == true);
  
  retval = action->func(event_test8, 0);
  BOOST_CHECK(retval == true);
}

BOOST_AUTO_TEST_SUITE_END()

//____________________________________________________________________________//
