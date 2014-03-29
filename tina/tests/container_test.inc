
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

