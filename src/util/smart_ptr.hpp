//+----------------------------------------------------------------------------+
//| Description:  Magic Set Editor - Program to make Magic (tm) cards          |
//| Copyright:    (C) 2001 - 2010 Twan van Laarhoven and Sean Hunt             |
//| License:      GNU General Public License 2 or later (see file COPYING)     |
//+----------------------------------------------------------------------------+

#ifndef HEADER_UTIL_SMART_PTR
#define HEADER_UTIL_SMART_PTR

/** @file util/smart_ptr.hpp
 *
 *  @brief Utilities related to boost smart pointers
 */

// ----------------------------------------------------------------------------- : Includes

#include <util/atomic.hpp>
#ifdef HAVE_FAST_ATOMIC
	/// Using intrusive_ptr where possible? (as opposed to smart_ptr)
	#define USE_INTRUSIVE_PTR
#else
	#error "need a fast atomic type, because we need intrusive_ptr"
#endif

#if defined(_MSC_VER) && _MSC_VER <= 1300
	// Use slightly less fancy template stuff, so msvc7.1 doesn't crash with an internal compiler error
	#define BOOST_SP_NO_SP_CONVERTIBLE
#endif

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#ifdef USE_INTRUSIVE_PTR
	#include <boost/intrusive_ptr.hpp>
#endif

// Can't do  using namespace boost;
// because boost::shared_ptr conflicts with std::tr1::shared_ptr
// and some boost headers do include boost/shared_ptr themselves
#if _HAS_TR1
	using std::tr1::shared_ptr;
#else
	using boost::shared_ptr;
#endif
using boost::intrusive_ptr;
using boost::scoped_ptr;

// ----------------------------------------------------------------------------- : Declaring

/// Declares the type TypeP as a shared_ptr<Type>
#define DECLARE_SHARED_POINTER_TYPE(Type)	\
	class Type;								\
	typedef shared_ptr<Type> Type##P;

// ----------------------------------------------------------------------------- : Non-null pointers

#ifdef _DEBUG
	#define USE_NON_NULL_TYPE 1
#else
	// non_null types are only useful for assertions and stronger type checks,
	// they might make work harder for the compiler, so disable them in release builds
	#define USE_NON_NULL_TYPE 0
#endif

#define WARN_IMPLICIT_NULLABLE_CONVERSION 1
#define WARN_IMPLICIT_NULLABLE_ALLOC      1
#define WARN_IMPLICIT_NULLABLE_NULL       1

#if USE_NON_NULL_TYPE

	template <typename T>
	struct intrusive_ptr_non_null;
	namespace boost{
		template <typename T, typename U>
		inline intrusive_ptr_non_null<T> static_pointer_cast(intrusive_ptr_non_null<U> const& ptr);
	}

	/// An intrusive_ptr that can't be null
	template <typename T>
	struct intrusive_ptr_non_null : public intrusive_ptr<T> {
		typedef T element_type;
		
		template <typename V>
		inline intrusive_ptr_non_null(intrusive_ptr_non_null<V> const& x) : intrusive_ptr<T>(x) {}
		
	  private:
		// some STL classes need default constructors
		friend class std::vector<intrusive_ptr_non_null>;
		friend class std::map<String,intrusive_ptr_non_null>;
		friend class std::map<size_t,intrusive_ptr_non_null>;
		template <typename T> friend intrusive_ptr_non_null<T> from_non_null(intrusive_ptr<T> const& ptr);
		template <typename T> friend intrusive_ptr_non_null<T> intrusive(T* ptr);
		template <typename T> friend intrusive_ptr_non_null<T> intrusive_from_this(T* ptr);
		template <typename T> friend intrusive_ptr_non_null<T> null_for_guaranteed_assignment();
		template <typename T> friend intrusive_ptr_non_null<T> null_for_stl_ctor();
		template <typename T, typename U> friend intrusive_ptr_non_null<T> boost::static_pointer_cast(intrusive_ptr_non_null<U> const& ptr);
		
		#if WARN_IMPLICIT_NULLABLE_CONVERSION
			private:
		#else
			public:
		#endif
		template <typename V>
		inline intrusive_ptr_non_null(intrusive_ptr<V> const& x) : intrusive_ptr<T>(x) {}
		
		#if WARN_IMPLICIT_NULLABLE_ALLOC
			private:
		#else
			public:
		#endif
		inline intrusive_ptr_non_null(T* x) : intrusive_ptr<T>(x) {}
		
		#if WARN_IMPLICIT_NULLABLE_NULL
			private:
		#else
			public:
		#endif
		inline intrusive_ptr_non_null() {}
		
		#if WARN_UNNECESSARY_NULL_CHECKS
		  private:
			// don't need this, because the value can't be null
			inline operator bool() { return true; }
		#endif
	};

	namespace boost{
		template <typename T, typename U>
		inline intrusive_ptr_non_null<T> static_pointer_cast(intrusive_ptr_non_null<U> const& ptr) {
			return intrusive_ptr_non_null<T>(static_cast<T*>(ptr.get()));
		}
		// note: put in namespace boost to prevent name conflicts with static_pointer_cast(boost::intrusive_ptr)
	}

#else

	#define intrusive_ptr_non_null intrusive_ptr

#endif

// ----------------------------------------------------------------------------- : Intrusive pointers

/// Declares the types:
///  * TypeP_nullable as a nullable intrusive_ptr<Type>
///  * TypeP as a non-nullable intrusive_ptr<Type>
///  * TypeR_nullable as a nullable Type*
///  * TypeR as a non-nullable Type*
#define DECLARE_POINTER_TYPE(Type) \
	class Type; \
	typedef intrusive_ptr<Type> Type##P_nullable; \
	typedef intrusive_ptr_non_null<Type> Type##P;

// ----------------------------------------------------------------------------- : Creating

/// Wrap a newly allocated pointer in an shared_ptr
/** Usage:
	*    return shared(new T(stuff)));
	*/
template <typename T>
inline shared_ptr<T> shared(T* ptr) {
	assert(ptr);
	return shared_ptr<T>(ptr);
}

/// Wrap a newly allocated pointer in an intrusive_ptr
/** Usage:
 *    return intrusive(new T(stuff)));
 */
template <typename T>
inline intrusive_ptr_non_null<T> intrusive(T* ptr) {
	assert(ptr);
	assert(ptr->ref_count == 0);
	return intrusive_ptr_non_null<T>(ptr);
}

/// Wrap this in an intrusive_ptr
/** Usage:
 *    return intrusive(this);
 *  Note: this is only safe to do for things that are always handled via intrusive_ptrs
 */
template <typename T>
inline intrusive_ptr_non_null<T> intrusive_from_this(T* ptr) {
	assert(ptr);
	return intrusive_ptr_non_null<T>(ptr);
}

/// Declare that something is non_null (this is not checked statically)
template <typename T>
inline intrusive_ptr_non_null<T> from_non_null(intrusive_ptr<T> const& ptr) {
	assert(ptr);
	return intrusive_ptr_non_null<T>(ptr);
}

// STL classes need objects with constructors, so for them we allow intrusive_ptr_non_null values that are null
// Requirements for usage
//  * only use in a default constructor
//  * that constructor MUST be private
template <typename T>
inline intrusive_ptr_non_null<T> null_for_stl_ctor() {
	return intrusive_ptr_non_null<T>(nullptr);
}

// Create a null value, which will later be assigned some value before being read
// this condition is not checked
template <typename T>
inline intrusive_ptr_non_null<T> null_for_guaranteed_assignment() {
	return intrusive_ptr_non_null<T>(nullptr);
}

using boost::static_pointer_cast;
using boost::dynamic_pointer_cast;

// ----------------------------------------------------------------------------- : Intrusive pointer base

template <typename T> class IntrusivePtrBase;
template <typename T> void intrusive_ptr_add_ref(IntrusivePtrBase<T>*);
template <typename T> void intrusive_ptr_release(IntrusivePtrBase<T>*);
/// Base class for objects wishing to use intrusive_ptrs.
/** There is no implicit virtual destructor, objects are destructed as type T
*   Usage:
 *  @code
 *    DECLARE_POINTER_TYPE(MyClass);
 *    class MyClass : public IntrusivePtrBase<MyClass> { ... }
 *  @endcode
 */
template <typename T> class IntrusivePtrBase {
  public:
	inline IntrusivePtrBase()                        : ref_count(0) {}
	// don't copy construct the reference count!
	inline IntrusivePtrBase(const IntrusivePtrBase&) : ref_count(0) {}
	// don't assign the reference count!
	inline void operator = (const IntrusivePtrBase&) { }
  protected:
	/// Delete this object, can be overloaded
	inline void destroy() {
		delete static_cast<T*>(this);
	}
  private:
	AtomicInt ref_count;
	friend void intrusive_ptr_add_ref <> (IntrusivePtrBase*);
	friend void intrusive_ptr_release <> (IntrusivePtrBase*);
	template <typename T> friend intrusive_ptr_non_null<T> intrusive(T*);
};

template <typename T> void intrusive_ptr_add_ref(IntrusivePtrBase<T>* p) {
	++(p->ref_count);
}

template <typename T> void intrusive_ptr_release(IntrusivePtrBase<T>* p) {
	if (--p->ref_count == 0) {
		static_cast<T*>(p)->destroy();
	}
}
// ----------------------------------------------------------------------------- : Intrusive pointer base : virtual

/// IntrusivePtrBase with a virtual destructor
class IntrusivePtrVirtualBase : public IntrusivePtrBase<IntrusivePtrVirtualBase> {
  public:
	virtual ~IntrusivePtrVirtualBase() {}
};

// ----------------------------------------------------------------------------- : Intrusive pointer base : with delete

/// Base class for objects wishing to use intrusive_ptrs, using a manual delete function
class IntrusivePtrBaseWithDelete : public IntrusivePtrBase<IntrusivePtrBaseWithDelete> {
  public:
	virtual ~IntrusivePtrBaseWithDelete() {}
  protected:
	/// Delete this object
	virtual void destroy() {
		delete this;
	}
	template <typename T> friend void intrusive_ptr_release(IntrusivePtrBase<T>*);
};

/// Pointer to 'anything'
typedef intrusive_ptr<IntrusivePtrVirtualBase> VoidP_nullable;
typedef intrusive_ptr_non_null<IntrusivePtrVirtualBase> VoidP;


// ----------------------------------------------------------------------------- : EOF
#endif
