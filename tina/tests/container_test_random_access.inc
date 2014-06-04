
BOOST_AUTO_TEST_CASE( TURAG_CONTAINER_TEST(rai_copy) ) {
  {
    CONTAINER::iterator test1;
    CONTAINER::iterator test2(test1);
    CONTAINER::iterator test3 = test1;
    test1 = test3;    
  }
  BOOST_CHECK_EQUAL(Dummy::instances, 0);
  Dummy::instances = 0;
}

BOOST_AUTO_TEST_CASE( TURAG_CONTAINER_TEST(rai_comp) ) {
  {
    CONTAINER::iterator test1;
    CONTAINER::iterator test2(test1);
    BOOST_CHECK_EQUAL(test1 == test2, true);
    BOOST_CHECK_EQUAL(test1 != test2, false);
  }
  BOOST_CHECK_EQUAL(Dummy::instances, 0);
  Dummy::instances = 0;
}

BOOST_AUTO_TEST_CASE( TURAG_CONTAINER_TEST(rai_deref) ) {
  {
    CONTAINER test;
    test.push_front(1);
    test.push_front(2);
    test.push_front(3);
    test.push_front(4);
    test.push_front(5);
    test.push_front(6);
    test.push_front(7);
    
    BOOST_CHECK_EQUAL(*test.begin(), 1);
    BOOST_CHECK_EQUAL(*(test.cbegin()+2), 3);
    //TODO BOOST_CHECK_EQUAL(test.begin()->dummy, 1);
  }
  BOOST_CHECK_EQUAL(Dummy::instances, 0);
  Dummy::instances = 0;
}

BOOST_AUTO_TEST_CASE( TURAG_CONTAINER_TEST(rai_inc) ) {
  {
    CONTAINER test;
    test.push_front(1);
    test.push_front(2);
    test.push_front(3);
    test.push_front(4);
    test.push_front(5);
    test.push_front(6);
    test.push_front(7);
    
    CONTAINER::iterator i1 = test.begin();
    BOOST_CHECK_EQUAL(*i1, 1);
    BOOST_CHECK_EQUAL(*(++i1), 2);
    BOOST_CHECK_EQUAL(*i1++, 2);
    BOOST_CHECK_EQUAL(*i1, 3);
    BOOST_CHECK_EQUAL(*(i1++), 3);
    BOOST_CHECK_EQUAL(*i1, 4);
    
    CONTAINER::const_iterator i2 = test.cbegin();
    BOOST_CHECK_EQUAL(*i2, 1);
    BOOST_CHECK_EQUAL(*(++i2), 2);
    BOOST_CHECK_EQUAL(*i2++, 2);
    BOOST_CHECK_EQUAL(*i2, 3);
    BOOST_CHECK_EQUAL(*(i2++), 3);
    BOOST_CHECK_EQUAL(*i2, 4);
  }
  BOOST_CHECK_EQUAL(Dummy::instances, 0);
  Dummy::instances = 0;
}

BOOST_AUTO_TEST_CASE( TURAG_CONTAINER_TEST(rai_dec) ) {
  {
    CONTAINER test;
    test.push_front(1);
    test.push_front(2);
    test.push_front(3);
    test.push_front(4);
    test.push_front(5);
    test.push_front(6);
    test.push_front(7);
    
    CONTAINER::iterator i1 = test.begin()+4;
    BOOST_CHECK_EQUAL(*i1, 5);
    BOOST_CHECK_EQUAL(*(--i1), 4);
    BOOST_CHECK_EQUAL(*i1--, 4);
    BOOST_CHECK_EQUAL(*i1, 3);
    BOOST_CHECK_EQUAL(*(i1--), 3);
    BOOST_CHECK_EQUAL(*i1, 2);
    
    CONTAINER::const_iterator i2 = test.cbegin()+4;
    BOOST_CHECK_EQUAL(*i2, 5);
    BOOST_CHECK_EQUAL(*(--i2), 4);
    BOOST_CHECK_EQUAL(*i2--, 4);
    BOOST_CHECK_EQUAL(*i2, 3);
    BOOST_CHECK_EQUAL(*(i2--), 3);
    BOOST_CHECK_EQUAL(*i2, 2);
  }
  BOOST_CHECK_EQUAL(Dummy::instances, 0);
  Dummy::instances = 0;
}
