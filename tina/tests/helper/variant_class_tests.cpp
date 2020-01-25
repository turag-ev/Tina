#define BOOST_TEST_DYN_LINK
#include "../extentions.h"
#include <boost/test/unit_test.hpp>

#include <tina++/container/variant_class.h>

using namespace TURAG;

constexpr void* null = nullptr;


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
	BOOST_CHECK_GE(sizeof(Type), sizeof(A));
	BOOST_CHECK_GE(sizeof(Type), sizeof(B));
	BOOST_CHECK_EQUAL(variant.empty(), true);
	BOOST_CHECK_EQUAL(variant.get(), null);

	variant.emplace<A>();
	BOOST_CHECK_EQUAL(variant.empty(), false);
	BOOST_CHECK_NE(variant.get(), null);
	BOOST_CHECK_EQUAL(variant->getValue(), 0xAAAA);

	variant.emplace<B>();
	BOOST_CHECK_EQUAL(variant.empty(), false);
	BOOST_CHECK_EQUAL(variant->getValue(), 0xBBBB);

	variant.erase();
	BOOST_CHECK_EQUAL(variant.empty(), true);
	BOOST_CHECK_EQUAL(variant.get(), null);

	variant.emplace<A>();
	BOOST_CHECK_EQUAL(variant.empty(), false);
	BOOST_CHECK_EQUAL(variant->getValue(), 0xAAAA);

	Type variant2(variant);
	BOOST_CHECK_NE(variant.get(), variant2.get());
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
	BOOST_CHECK_EQUAL(variant.operator ->(), null);
	BOOST_CHECK_GE(sizeof(Type), sizeof(A));
	BOOST_CHECK_GE(sizeof(Type), sizeof(B));
	BOOST_CHECK_EQUAL(variant.get(), null);

	variant.emplace<A>();
	BOOST_CHECK_EQUAL(variant.empty(), false);
	BOOST_CHECK_EQUAL(variant->getValue(), 0xAAAA);
	BOOST_CHECK_NE(variant.get(), null);

	variant.emplace<B>();
	BOOST_CHECK_EQUAL(variant.empty(), false);
	BOOST_CHECK_EQUAL(variant->getValue(), 0xBBBB);

	int value = variant->getValue();
	BOOST_CHECK_EQUAL(value, 0xBBBB);

	variant.erase();
	BOOST_CHECK_EQUAL(variant.empty(), true);
	BOOST_CHECK_EQUAL(variant.operator ->(), null);

	variant.emplace<A>();
	BOOST_CHECK_EQUAL(variant.empty(), false);
	BOOST_CHECK_EQUAL(variant->getValue(), 0xAAAA);

	Type variant2(variant);
	BOOST_CHECK_NE(variant.get(), variant2.get());
}

BOOST_AUTO_TEST_CASE(CopyMoveCtorSingleInherit) {
	class Base {
	public:
		virtual ~Base() { }

		virtual int getValue() const { return 0xBA5E; }
	};

	class A : public Base {
	public:
        A() {
            defaults++;
        }
        A(const A& a) : defaults(a.defaults), copies(a.copies), moves(a.moves) {
            copies++;
        }
        A(A&& a) : defaults(a.defaults), copies(a.copies), moves(a.moves) {
            moves++;
        }
		int getValue() const override { return 0xAAAA; }
        unsigned defaults = 0;
        unsigned copies = 0;
        unsigned moves = 0;
	};

	class B : public Base {
	public:
		int getValue() const override { return 0xBBBB; }
	};

	using Type = VariantClass<Base, A, B>;
    
    Type variant1 = B(); // construct by passing derived class (note: this is default construct and copy construct!)
	BOOST_CHECK_EQUAL(variant1->getValue(), 0xBBBB);
    variant1 = A(); // assign by passing derived class
	BOOST_CHECK_EQUAL(variant1->getValue(), 0xAAAA);
	BOOST_CHECK_EQUAL(static_cast<A&>(*variant1).defaults, 1);
	BOOST_CHECK_EQUAL(static_cast<A&>(*variant1).copies, 1);
	BOOST_CHECK_EQUAL(static_cast<A&>(*variant1).moves, 0);
    
    Type variant2 = variant1; // copy construct
	BOOST_CHECK_EQUAL(variant2->getValue(), 0xAAAA);
	BOOST_CHECK_EQUAL(static_cast<A&>(*variant2).defaults, 1);
	BOOST_CHECK_EQUAL(static_cast<A&>(*variant2).copies, 2);
	BOOST_CHECK_EQUAL(static_cast<A&>(*variant2).moves, 0);
    
    Type variant3; // create empty
    BOOST_CHECK_EQUAL(variant3.get(), null);
    variant3 = variant1; // copy assign
	BOOST_CHECK_EQUAL(variant3->getValue(), 0xAAAA);
	BOOST_CHECK_EQUAL(static_cast<A&>(*variant3).defaults, 1);
	BOOST_CHECK_EQUAL(static_cast<A&>(*variant3).copies, 2);
	BOOST_CHECK_EQUAL(static_cast<A&>(*variant3).moves, 0);
    
    Type variant4 = std::move(variant1); // move construct
	BOOST_CHECK_EQUAL(variant4->getValue(), 0xAAAA);
	BOOST_CHECK_EQUAL(static_cast<A&>(*variant4).defaults, 1);
	BOOST_CHECK_EQUAL(static_cast<A&>(*variant4).copies, 1);
	BOOST_CHECK_EQUAL(static_cast<A&>(*variant4).moves, 1);
    
    Type variant5; // create empty
    BOOST_CHECK_EQUAL(variant5.get(), null);
    variant5 = std::move(variant1); // move assign
	BOOST_CHECK_EQUAL(variant5->getValue(), 0xAAAA);
	BOOST_CHECK_EQUAL(static_cast<A&>(*variant5).defaults, 1);
	BOOST_CHECK_EQUAL(static_cast<A&>(*variant5).copies, 1);
	BOOST_CHECK_EQUAL(static_cast<A&>(*variant5).moves, 1);
}

BOOST_AUTO_TEST_CASE(CopyMoveCtorMultiInherit) {
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
        A() {
            defaults++;
        }
        A(const A& a) : defaults(a.defaults), copies(a.copies), moves(a.moves) {
            copies++;
        }
        A(A&& a) : defaults(a.defaults), copies(a.copies), moves(a.moves) {
            moves++;
        }
		int getValue() const override { return 0xAAAA; }
        unsigned defaults = 0;
        unsigned copies = 0;
        unsigned moves = 0;
	};

	class B : public Base2, public Base {
	public:
		int getValue() const override { return 0xBBBB; }
	};

	using Type = VariantClass<Base, A, B>;
    
    Type variant_a = A();
    Type variant_b = B();
	BOOST_CHECK_EQUAL(variant_a->getValue(), 0xAAAA);
    BOOST_CHECK_EQUAL(static_cast<A&>(*variant_a).defaults, 1);
	BOOST_CHECK_EQUAL(static_cast<A&>(*variant_a).copies, 1);
	BOOST_CHECK_EQUAL(static_cast<A&>(*variant_a).moves, 0);
	BOOST_CHECK_EQUAL(variant_b->getValue(), 0xBBBB);
    
    Type variant1 = variant_a; // copy construct a
	BOOST_CHECK_EQUAL(variant1->getValue(), 0xAAAA);
    BOOST_CHECK_EQUAL(static_cast<A&>(*variant1).defaults, 1);
	BOOST_CHECK_EQUAL(static_cast<A&>(*variant1).copies, 2);
	BOOST_CHECK_EQUAL(static_cast<A&>(*variant1).moves, 0);
    variant1 = variant_b; // copy assign b
	BOOST_CHECK_EQUAL(variant1->getValue(), 0xBBBB);
    
    Type variant2 = variant_b; // copy construct b
	BOOST_CHECK_EQUAL(variant2->getValue(), 0xBBBB);
    variant2 = variant_a; // copy assign a
	BOOST_CHECK_EQUAL(variant2->getValue(), 0xAAAA);
    BOOST_CHECK_EQUAL(static_cast<A&>(*variant2).defaults, 1);
	BOOST_CHECK_EQUAL(static_cast<A&>(*variant2).copies, 2);
	BOOST_CHECK_EQUAL(static_cast<A&>(*variant2).moves, 0);
    
    Type variant3 = std::move(variant_a); // move construct a
	BOOST_CHECK_EQUAL(variant3->getValue(), 0xAAAA);
    BOOST_CHECK_EQUAL(static_cast<A&>(*variant3).defaults, 1);
	BOOST_CHECK_EQUAL(static_cast<A&>(*variant3).copies, 1);
	BOOST_CHECK_EQUAL(static_cast<A&>(*variant3).moves, 1);
    variant3 = std::move(variant_b); // move assign b
	BOOST_CHECK_EQUAL(variant3->getValue(), 0xBBBB);
    
    Type variant4 = std::move(variant1); // move construct b
	BOOST_CHECK_EQUAL(variant4->getValue(), 0xBBBB);
    variant4 = std::move(variant2); // move assign a
	BOOST_CHECK_EQUAL(variant4->getValue(), 0xAAAA);
    BOOST_CHECK_EQUAL(static_cast<A&>(*variant4).defaults, 1);
	BOOST_CHECK_EQUAL(static_cast<A&>(*variant4).copies, 2);
	BOOST_CHECK_EQUAL(static_cast<A&>(*variant4).moves, 1);
}

BOOST_AUTO_TEST_SUITE_END()

//____________________________________________________________________________//
