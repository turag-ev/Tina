#define BOOST_TEST_DYN_LINK
#include "../extentions.h"
#include <boost/test/unit_test.hpp>

#include <tina++/container/variant_class.h>

using namespace TURAG;


BOOST_AUTO_TEST_SUITE(VariantClassTests)

BOOST_AUTO_TEST_CASE(all_true_test) {
	using tt = std::true_type;
	using ft = std::false_type;


	BOOST_CHECK_EQUAL((TURAG::all_true<tt, tt, tt, tt>::value), true);
	BOOST_CHECK_EQUAL((TURAG::all_true<tt, tt, tt, ft>::value), false);
	BOOST_CHECK_EQUAL((TURAG::all_true<ft, tt, tt, tt>::value), false);
	BOOST_CHECK_EQUAL((TURAG::all_true<tt>::value), true);
	BOOST_CHECK_EQUAL((TURAG::all_true<ft>::value), false);
	BOOST_CHECK_EQUAL((TURAG::all_true<ft, ft, ft, ft>::value), false);

	BOOST_CHECK_EQUAL((TURAG::all_false<tt, tt, tt, tt>::value), false);
	BOOST_CHECK_EQUAL((TURAG::all_false<tt, tt, tt, ft>::value), false);
	BOOST_CHECK_EQUAL((TURAG::all_false<ft, tt, tt, tt>::value), false);
	BOOST_CHECK_EQUAL((TURAG::all_false<ft>::value), true);
	BOOST_CHECK_EQUAL((TURAG::all_false<tt>::value), false);
	BOOST_CHECK_EQUAL((TURAG::all_false<ft, ft, ft, ft>::value), true);
}

BOOST_AUTO_TEST_CASE(max_constant_test) {
	using minustwo = std::integral_constant<int, -2>;
	using minusone = std::integral_constant<int, -1>;
	using zero = std::integral_constant<int, 0>;
	using one = std::integral_constant<int, 1>;
	using two = std::integral_constant<int, 2>;

	BOOST_CHECK_EQUAL((max_integral_constant<one, two, zero>::value), 2);
	BOOST_CHECK_EQUAL((max_integral_constant<minusone, minustwo, zero>::value), 0);
	BOOST_CHECK_EQUAL((max_integral_constant<minusone, minustwo>::value), -1);
	BOOST_CHECK_EQUAL((max_integral_constant<minusone>::value), -1);
}

BOOST_AUTO_TEST_CASE(max_value_test) {
	BOOST_CHECK_EQUAL(max_value(0x1111), 0x1111);
	BOOST_CHECK_EQUAL(max_value(1,2,3,4,3,2,1), 4);
	BOOST_CHECK_EQUAL(max_value(-11), -11);
	BOOST_CHECK_EQUAL(max_value(-11, 0), 0);
	BOOST_CHECK_EQUAL(max_value(-11, 0, 11), 11);
	BOOST_CHECK_EQUAL(max_value(11, 0), 11);
}

BOOST_AUTO_TEST_CASE(min_value_test) {
	BOOST_CHECK_EQUAL(min_value(0x1111), 0x1111);
	BOOST_CHECK_EQUAL(min_value(4,2,3,1,2,5), 1);
	BOOST_CHECK_EQUAL(min_value(-11), -11);
	BOOST_CHECK_EQUAL(min_value(-11, 0), -11);
	BOOST_CHECK_EQUAL(min_value(-11, 0, 11), -11);
	BOOST_CHECK_EQUAL(min_value(11, 0), 0);
}

BOOST_AUTO_TEST_CASE(define) {
	class Base {
	public:
		virtual ~Base() { }

		virtual int getValue() const { return 0xBA5E; }
	};

	class A : public Base {
	public:
		int getValue() const override { return 0xAAAA; }
	};

	class B : public Base {
	public:
		int getValue() const override { return 0xBBBB; }
	};

	using Type = VariantClass<Base, A, B>;

	Type variant;
	BOOST_CHECK_EQUAL(variant.empty(), true);
	BOOST_CHECK_EQUAL(variant.get(), (void*)nullptr);

	variant.emplace<A>();
	BOOST_CHECK_EQUAL(variant.empty(), false);
	BOOST_CHECK_NE(variant.get(), (void*)nullptr);
	BOOST_CHECK_EQUAL(variant->getValue(), 0xAAAA);

	variant.emplace<B>();
	BOOST_CHECK_EQUAL(variant.empty(), false);
	BOOST_CHECK_EQUAL(variant->getValue(), 0xBBBB);

	variant.erase();
	BOOST_CHECK_EQUAL(variant.empty(), true);
	BOOST_CHECK_EQUAL(variant.get(), (void*)nullptr);

	variant.emplace<A>();
	BOOST_CHECK_EQUAL(variant.empty(), false);
	BOOST_CHECK_EQUAL(variant->getValue(), 0xAAAA);
}

BOOST_AUTO_TEST_CASE(multi) {
	class Base {
	public:
		virtual ~Base() { }

		virtual int getValue() const { return 0xBA5E; }
	};

	class Base2 {
	public:
		virtual ~Base2() { }

		virtual unsigned get() const { return 0xBA5E2; }
	};

	class A : public Base, public Base2 {
	public:
		int getValue() const override { return 0xAAAA; }
	};

	class B : public Base2, public Base {
	public:
		int getValue() const override { return 0xBBBB; }
	};

	using Type = VariantClass<Base, A, B>;

	Type variant;
	BOOST_CHECK_EQUAL(variant.empty(), true);
	BOOST_CHECK_EQUAL(variant.operator ->(), (void*)nullptr);

	variant.emplace<A>();
	BOOST_CHECK_EQUAL(variant.empty(), false);
	BOOST_CHECK_EQUAL(variant->getValue(), 0xAAAA);

	variant.emplace<B>();
	BOOST_CHECK_EQUAL(variant.empty(), false);

	int value = variant->getValue();
	BOOST_CHECK_EQUAL(value, 0xBBBB);

	variant.erase();
	BOOST_CHECK_EQUAL(variant.empty(), true);
	BOOST_CHECK_EQUAL(variant.operator ->(), (void*)nullptr);
}

BOOST_AUTO_TEST_SUITE_END()

//____________________________________________________________________________//
