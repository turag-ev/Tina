
BOOST_AUTO_TEST_CASE( TURAG_CONTAINER_TEST(push_front) ) {
  {
    CONTAINER test;

    test.push_front(1);
    test.push_front(2);
    test.push_front(3);
    test.push_front(4);
    test.push_front(5);
    test.push_front(6);
    test.push_front(7);

    BOOST_CHECK(check_container_content(test, {7, 6, 5, 4, 3, 2, 1}));

    test.push_back(0);
    test.push_front(8);
    test.push_front(9);

    BOOST_CHECK(check_container_content(test, {9, 8, 7, 6, 5, 4, 3, 2, 1, 0}));

    BOOST_CHECK_EQUAL(Dummy::instances, 10);
    Dummy::instances = 10;
  }
  BOOST_CHECK_EQUAL(Dummy::instances, 0);
  Dummy::instances = 0;
}

BOOST_AUTO_TEST_CASE( TURAG_CONTAINER_TEST(emplace_front) ) {
  {
    CONTAINER test;

    test.emplace_front(1);
    test.emplace_front(2);
    test.emplace_front(3);
    test.emplace_front(4);
    test.emplace_front(5);
    test.emplace_front(6);
    test.emplace_front(7);

    BOOST_CHECK(check_container_content(test, {7, 6, 5, 4, 3, 2, 1}));

    test.emplace_back(0);
    test.emplace_front(8);
    test.emplace_front(9);

    BOOST_CHECK(check_container_content(test, {9, 8, 7, 6, 5, 4, 3, 2, 1, 0}));

    BOOST_CHECK_EQUAL(Dummy::instances, 10);
    Dummy::instances = 10;
  }
  BOOST_CHECK_EQUAL(Dummy::instances, 0);
  Dummy::instances = 0;
}

BOOST_AUTO_TEST_CASE( TURAG_CONTAINER_TEST(push_back) ) {
  {
    CONTAINER test;

    test.push_back(1);
    test.push_back(2);
    test.push_back(3);
    test.push_back(4);
    test.push_back(5);
    test.push_back(6);
    test.push_back(7);

    BOOST_CHECK(check_container_content(test, {1, 2, 3, 4, 5, 6, 7}));

    BOOST_CHECK_EQUAL(test.front(), 1);
    BOOST_CHECK_EQUAL(test[0], 1);
    BOOST_CHECK_EQUAL(*test.begin(), 1);
    BOOST_CHECK_EQUAL(*test.cbegin(), 1);

    BOOST_CHECK_EQUAL(test.back(), 7);
    BOOST_CHECK_EQUAL(test[6], 7);

    BOOST_CHECK_EQUAL(test.size(), 7);
//    BOOST_CHECK_EQUAL(test.capacity(), 20);

    BOOST_CHECK_EQUAL(Dummy::instances, 7);
    Dummy::instances = 7;
  }
  BOOST_CHECK_EQUAL(Dummy::instances, 0);
  Dummy::instances = 0;
}

BOOST_AUTO_TEST_CASE( TURAG_CONTAINER_TEST(emplace_back) ) {
  {
    CONTAINER test;

    test.emplace_back(1);
    test.emplace_back(2);
    test.emplace_back(3);
    test.emplace_back(4);
    test.emplace_back(5);
    test.emplace_back(6);
    test.emplace_back(7);

    BOOST_CHECK(check_container_content(test, {1, 2, 3, 4, 5, 6, 7}));

    BOOST_CHECK_EQUAL(test.front(), 1);
    BOOST_CHECK_EQUAL(test[0], 1);
    BOOST_CHECK_EQUAL(*test.begin(), 1);
    BOOST_CHECK_EQUAL(*test.cbegin(), 1);

    BOOST_CHECK_EQUAL(test.back(), 7);
    BOOST_CHECK_EQUAL(test[6], 7);

    BOOST_CHECK_EQUAL(test.size(), 7);
//    BOOST_CHECK_EQUAL(test.capacity(), 20);

    BOOST_CHECK_EQUAL(Dummy::instances, 7);
    Dummy::instances = 7;
  }
  BOOST_CHECK_EQUAL(Dummy::instances, 0);
  Dummy::instances = 0;
}

BOOST_AUTO_TEST_CASE( TURAG_CONTAINER_TEST(add_own_value) ) {
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

    BOOST_CHECK_EQUAL(Dummy::instances, 8);
    Dummy::instances = 8;
  }
  BOOST_CHECK_EQUAL(Dummy::instances, 0);
  Dummy::instances = 0;
}

BOOST_AUTO_TEST_CASE( TURAG_CONTAINER_TEST(pop_front) ) {
  CONTAINER test;
  test.push_back(1);
  test.push_back(2);
  test.push_back(3);
  test.push_back(4);
  test.push_back(5);

  test.pop_front();
  BOOST_CHECK_EQUAL(test.size(), 4);
  BOOST_CHECK(check_container_content(test, {2, 3, 4, 5}));

  BOOST_CHECK_EQUAL(Dummy::instances, 4);
  Dummy::instances = 4;
}

BOOST_AUTO_TEST_CASE( TURAG_CONTAINER_TEST(pop_back) ) {
  CONTAINER test;
  test.push_back(1);
  test.push_back(2);
  test.push_back(3);
  test.push_back(4);
  test.push_back(5);

  test.pop_back();
  BOOST_CHECK_EQUAL(test.size(), 4);
  BOOST_CHECK(check_container_content(test, {1, 2, 3, 4}));

  BOOST_CHECK_EQUAL(Dummy::instances, 4);
  Dummy::instances = 4;
}

