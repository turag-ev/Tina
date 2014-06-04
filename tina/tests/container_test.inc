
BOOST_AUTO_TEST_CASE( TURAG_CONTAINER_TEST(copy) ) {
  {
    CONTAINER test1;
    test1.push_back(1);
    test1.push_back(2);
    test1.push_back(3);
    test1.push_back(4);
    test1.push_back(5);

    CONTAINER test2 = test1;

    BOOST_CHECK_EQUAL(test1.size(), 5);
    BOOST_CHECK(check_container_content(test1, {1, 2, 3, 4, 5}));

    BOOST_CHECK_EQUAL(test2.size(), 5);
    BOOST_CHECK(check_container_content(test2, {1, 2, 3, 4, 5}));

    BOOST_CHECK_EQUAL(Dummy::instances, 10);
    Dummy::instances = 10;
  }
  BOOST_CHECK_EQUAL(Dummy::instances, 0);
  Dummy::instances = 0;
}

BOOST_AUTO_TEST_CASE( TURAG_CONTAINER_TEST(std_find) ) {
  {
    CONTAINER test1;
    test1.push_back(1);
    test1.push_back(2);
    test1.push_back(3);
    test1.push_back(4);
    test1.push_back(5);

    CONTAINER::iterator r1 = std::find(test1.begin(), test1.end(), 3);
    BOOST_CHECK_EQUAL(*r1, 3);
    
    CONTAINER::const_iterator r2 = std::find(test1.cbegin(), test1.cend(), 5);
    BOOST_CHECK_EQUAL(*r2, 5);
    
    CONTAINER::iterator r3 = std::find(test1.begin(), test1.end(), 6);
    BOOST_CHECK_EQUAL(r1, test1.end());
  }
  Dummy::instances = 0;
}

