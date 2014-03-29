
BOOST_AUTO_TEST_CASE( TURAG_CONTAINER_TEST(insert_own_value) ) {
  {
    CONTAINER test;
    test.push_back(1);
    test.push_back(2);
    test.push_back(3);

    test.push_back(test[0]);
    BOOST_CHECK(check_container_content(test, {1, 2, 3, 1}));

    test.push_back(test[1]);
    BOOST_CHECK(check_container_content(test, {1, 2, 3, 1, 2}));

    test.push_front(test[2]);
    BOOST_CHECK(check_container_content(test, {3, 1, 2, 3, 1, 2}));

    test.push_front(test[0]);
    BOOST_CHECK(check_container_content(test, {3, 3, 1, 2, 3, 1, 2}));

    test.insert(test.begin() + 2, test[2]);
    BOOST_CHECK(check_container_content(test, {3, 3, 1, 1, 2, 3, 1, 2}));

    BOOST_CHECK_EQUAL(Dummy::instances, 8);
    Dummy::instances = 8;
  }
  BOOST_CHECK_EQUAL(Dummy::instances, 0);
  Dummy::instances = 0;
}

BOOST_AUTO_TEST_CASE( TURAG_CONTAINER_TEST(erase) ) {
  CONTAINER test;
  test.push_back(1);
  test.push_back(2);
  test.push_back(3);
  test.push_back(4);
  test.push_back(5);

  test.erase(test.begin() + 3);
  BOOST_CHECK_EQUAL(test.size(), 4);
  BOOST_CHECK(check_container_content(test, {1, 2, 3, 5}));

  test.erase(test.begin() + 1, test.begin() + 3);
  BOOST_CHECK_EQUAL(test.size(), 2);
  BOOST_CHECK(check_container_content(test, {1, 5}));

  test.erase(test.begin(), test.end());
  BOOST_CHECK_EQUAL(test.size(), 0);

  BOOST_CHECK_EQUAL(Dummy::instances, 0);
  Dummy::instances = 0;
}

BOOST_AUTO_TEST_CASE( TURAG_CONTAINER_TEST(insert) ) {
  CONTAINER test;
  test.push_back(1);
  test.push_back(2);
  test.push_back(3);
  test.push_back(4);
  test.push_back(5);

  test.insert(test.begin() + 3, 6);
  BOOST_CHECK_EQUAL(test.size(), 6);
  BOOST_CHECK(check_container_content(test, {1, 2, 3, 6, 4, 5}));

  test.insert(test.begin() + 6, 7);
  BOOST_CHECK_EQUAL(test.size(), 7);
  BOOST_CHECK(check_container_content(test, {1, 2, 3, 6, 4, 5, 7}));

  test.insert(test.begin(), 0);
  BOOST_CHECK_EQUAL(test.size(), 8);
  BOOST_CHECK(check_container_content(test, {0, 1, 2, 3, 6, 4, 5, 7}));
}

BOOST_AUTO_TEST_CASE( TURAG_CONTAINER_TEST(emplace) ) {
  CONTAINER test;
  test.push_back(1);
  test.push_back(2);
  test.push_back(3);
  test.push_back(4);
  test.push_back(5);

  test.emplace(test.begin() + 3, 6);
  BOOST_CHECK_EQUAL(test.size(), 6);
  BOOST_CHECK(check_container_content(test, {1, 2, 3, 6, 4, 5}));

  test.emplace(test.begin() + 6, 7);
  BOOST_CHECK_EQUAL(test.size(), 7);
  BOOST_CHECK(check_container_content(test, {1, 2, 3, 6, 4, 5, 7}));

  test.emplace(test.begin(), 0);
  BOOST_CHECK_EQUAL(test.size(), 8);
  BOOST_CHECK(check_container_content(test, {0, 1, 2, 3, 6, 4, 5, 7}));
}

