#ifndef ODFAEG_FAST_DELEGATE_HPP
#define ODFAEG_FAST_DELEGATE_HPP
#include <functional>
#include <iostream>
#include "export.hpp"
#include "erreur.h"
#include <tuple>
#include <utility>
#include <memory>
#include "variant.h"
#define FUN_PARAM void* p0 =nullptr, void* p1 =nullptr, void* p2 =nullptr, void* p3 =nullptr
#define FWD_PARAM p0,p1,p2,p3
/**
  *\namespace odfaeg
  * the namespace of the Opensource Development Framework Adapted for Every Games.
  */
namespace odfaeg {
    namespace core {
        /**
        *  \file  fastDelegate.h
        *  \class IRefVal
        *  \brief Interface for the warppers to references, values and pointers.
        *  This class is used store references, pointers and values to pass to the callack's functions.
        *  \param T : the type of the value to wrap.
        *  \author Duroisin.L
        *  \version 1.0
        *  \date 1/02/2014
        */
        template<class T>
        struct IRefVal {
            /**\fn
            *  \brief default constructor
            */
            IRefVal()=default;
            /**\fn T& get()
            *  \brief get the reference of the wrapped type.
            *  \return the reference of the wrapped type.
            */
            virtual T& bind(FUN_PARAM) = 0;
            virtual T& get(FUN_PARAM) = 0;
            /**\fn std::unique_ptr<IRefVal<T>> clone() const = 0;
            *  \brief copy the wrapper.
            *  \return the copied wrapper.*/
            virtual std::unique_ptr<IRefVal<T>> clone() const = 0;
            /**\fn destructor*/
            virtual ~IRefVal(){}

        protected:
            /**\fn IRefVal(const IRefVal&)
            *  \brief constructor, pass the reference to wrap.
            *  \param const IRefVal& : the reference to wrap.
            */
            IRefVal(const IRefVal&){}
            /** \fn IRefVal& operator=(const IRefVal&)
            *   \brief affector.
            *   \param const IRefVal& : the wrapper to affect.
            *   \return the affected wrapper.*/
            IRefVal& operator=(const IRefVal&)
            { return *this; }
        };
        /**
        *  \file  fastDelegate.h
        *  \class Ref
        *  \brief Warp a reference. (Use std::reference_wrapper so we can pass a reference with std::ref)
        *  \author Duroisin.L
        *  \version 1.0
        *  \date 1/02/2014
        */
        template <typename T>
        class RefVal;
        template<class T>
        struct Ref : IRefVal<T> {
            /**
            *\fn Ref(const std::reference_wrapper<T>& r)
            *\brief Constructor : pass an std::reference_wrapper to the wrapper.
            *\param std::reference_wrapper<T>& r : the reference_wrapper.
            */
            Ref(const std::reference_wrapper<T>& r)
                : ref(r)
            {}
            /**
            * \fn T& get()
            * \brief return a reference to an object.
            * \return T& the reference to the object.
            */
            T& bind(FUN_PARAM) {
              return ref.get();
            }
            T& get(FUN_PARAM)
            { return ref.get(); }
            /**
            * \fn std::unique_ptr<IRefVal<T>> clone() const
            * \brief copy the reference wrapper.
            * \return std::unique_ptr<IRefVal<T>> : the cloned wrapper.
            */
            std::unique_ptr<IRefVal<T>> clone() const
            { return std::make_unique<Ref>(*this); }
        private:
            std::reference_wrapper<T> ref; /**> the std::reference_wrapper which warp the reference.*/
        };
        /**
        *  \file  fastDelegate.h
        *  \class Val
        *  \brief Warp a value.
        *  \author Duroisin.L
        *  \version 1.0
        *  \date 1/02/2014
        */
        template<class T>
        struct Val : IRefVal<T> {
            /**\fn Val(const T& t)
            *  \brief pass a value to the wrapper.
            *  \param const T& t : the value to pass.
            */
            Val(const T& t)
                : val(t)
            {}
            /** \fn
            *   \brief return the value
            *   \return T& : return the value.
            */
            T& bind(FUN_PARAM) {
              return val;
            }
            T& get(FUN_PARAM)
            { return val; }
            /**
            * \fn std::unique_ptr<IRefVal<T>> clone() const
            * \brief copy the value wrapper.
            * \return std::unique_ptr<IRefVal<T>> : the cloned wrapper.
            */
            std::unique_ptr<IRefVal<T>> clone() const
            { return std::make_unique<Val>(*this); }
        private:
            T val; /**> T val : keep the value of the wrapper.*/
        };
        template<size_t I, class T>
        struct ph : IRefVal<T>
        {
            std::unique_ptr<IRefVal<T>> clone() const
            { return std::make_unique<ph>(*this); }
            T& bind(FUN_PARAM) {
              return *static_cast<T*>
                    (std::get<I>(std::make_tuple(FWD_PARAM)));
            }
            T& get(FUN_PARAM)
            {
                return *static_cast<T*>
                    (std::get<I>(std::make_tuple(FWD_PARAM)));
            }
        };
        /**
        *  \file  fastDelegate.h
        *  \class Ref
        *  \brief Warp a pointer.
        *  \author Duroisin.L
        *  \version 1.0
        *  \date 1/02/2014
        */
        /**
        *  \file  fastDelegate.h
        *  \class RefVal
        *  \brief Wrap a pointer, a value or a reference and keep a pointer to the generic wrapper.
        *  Call the right constructor depending on the wrapper's or value's type.
        *  \author Duroisin.L
        *  \version 1.0
        *  \date 1/02/2014
        */
        template<class T>
        struct RefVal {
            /**
            * \fn RefVal (const T& t)
            * \brief constructor : construct a wrapper to a value
            * \param const T& t : the value to
            */
            RefVal(const T& t)
            : rv(std::make_unique<Val<T>>(t))
            {}
            /**
            * \fn RefVal (const std::reference_wrapper<T>& r)
            * \brief constructor : construct a wrapper to an std::reference_wrapper.
            * \param const std::reference_wrapper<T>& : the std::reference_wrapper to pass.
            */
            RefVal(const std::reference_wrapper<T>& r)
            : rv(std::make_unique<Ref<T>>(r))
            {}
            /** \fn RefVal (const RefVal& rhs)
            *   \brief copy constructor, copy the wrapper with the right wrapper type.
            *   \param const RefVal& rhs : the wrapper to copy.
            */
            template<size_t I>
            RefVal(ph<I,T>&&)
                : rv(std::make_unique<ph<I,T>>())
            {}
            RefVal(const RefVal& rhs)
            {
                rv = rhs.rv->clone();
            }
            /** \fn RefVal& operator= (const RefVal& rhs)
            *   \brief affector.
            *   \param const RefVal& rhs : the wrapper to affect.
            */
            RefVal& operator=(const RefVal& rhs)
            { rv=rhs.rv->clone(); return *this; }
            /** \fn T& get() const
            *   \brief get the wrapper.
            *   \return a unique pointer to the generic wrapper interface.
            */
            T& bind(FUN_PARAM) {
                return rv->bind(FWD_PARAM);
            }
            template<class... Arg>
            T& get(Arg&&... arg) const
            { return rv->get(std::forward<Arg>(arg)...); }
        private:
            std::unique_ptr<IRefVal<T>> rv; /**> a pointer to the generic wrapper interface.*/
        };
        //Classe de trait pour déterminer le type à stocker

        //(Interne) Cas général
        /**
        *  \file  fastDelegate.h
        *  \class ToStoreImpl
        *  \param T the type of the wrapper.
        *  \brief Trait class which use an alias on a wrapped type.
        *  \author Duroisin.L
        *  \version 1.0
        *  \date 1/02/2014
        */
        template<class T>
        struct ToStoreImpl
        { using type = T; };
        /**
        *  \file  fastDelegate.h
        *  \class ToStoreImpl
        *  \param T the type warpped in the warpper.
        *  \brief Trait class with keep an alias on the wrapped type.
        *  This class is specialized for std::_reference_wrapper type.
        *  \author Duroisin.L
        *  \version 1.0
        *  \date 1/02/2014
        */
        template<class T>
        struct ToStoreImpl<std::reference_wrapper<T>>
        { using type = T; };
        template<size_t I, class T>
        struct ToStoreImpl<ph<I,T>>
        { using type = T; };
        /**
        *  \file  fastDelegate.h
        *  \class ToStore
        *  \param T the type of the wrapper.
        *  \brief Trait class with keep an alias of the wrapper type. (without the reference)
        *  the inheritance use the right specialized templated class to hold the type of the wrapped object
        *  depending on the wrapper type.
        *  \author Duroisin.L
        *  \version 1.0
        *  \date 1/02/2014
        */
        template<class T>
        struct ToStore
            : ToStoreImpl<std::remove_reference_t<T>>
        {};
        /**
        *  \file  fastDelegate.h
        *  \class ToStore_t
        *  \param T the type of the wrapped object.
        *  \brief Trait class which hold an alias to the real type of the wrapped type.
        *  \author Duroisin.L
        *  \version 1.0
        *  \date 1/02/2014
        */
        template<class T>
        using ToStore_t = typename
            ToStore<T>::type;
        /**
        *  \file  fastDelegate.h
        *  \class DynamicWrapper
        *  \param R the return type of the wrapped functor.
        *  \param C the class type of the wrapped functor.
        *  \param ArgT the arguments types of the wrapped functor.
        *  \brief This class warp a function pointer to a member function.
        *  I don't use an std::function directly here to keep the class type of the member function pointer
        *  because if the passed object is polymorphic, I need to apply a dynamic cast
        *  before calling the member function pointer on the object.
        *  \author Duroisin.L
        *  \version 1.0
        *  \date 1/02/2014
        */
        template<class R, class C, class... ArgT>
        struct DynamicWrapper {
            /**\fn DynamicWrapper(R(C::*pf)(ArgT...)) : pfunc(pf)
            *  \brief warp a pointer to a member function.
            *  \param R(C::*pf)(ArgT...) : the pointer to the member function.
            */
            DynamicWrapper(R(C::*pf)(ArgT...)) : pfunc(pf){}
            /**\fn R operator()(O* o, ArgU&&... arg) const
            *  \brief call the member function's pointer witht he given object and the given argument
            *  apply a dynamic_cast in case of the type of the object and the type of the classe containing the member function
            *  are not the same. (If the object is polymoprhic)
            *  If the cast fails it means that the object is not polymorphic so it throws an error.
            *  \param O* o : the object onwich to call the member function's pointer.
            *  \param ArgU&& arg : the arguments of the member function's pointer to call.
            */
            template<class O, class... ArgU>
            R operator()(O* o, ArgU&&... arg) const
            {
                if(dynamic_cast<C*>(o))
                    return (dynamic_cast<C*>(o)->*pfunc)(std::forward<ArgU>(arg)...);
                throw Erreur(0, "Invalid cast : types are nor polymorphic!", 1);
            }
        private:
            R (C::*pfunc)(ArgT...); /**> a pointer to a member's function.*/
        };
        /**
        *  \file  fastDelegate.h
        *  \class DynamicFunction
        *  \param F the type of the function.
        *  \brief Generic class for every functors type.
        *  \author Duroisin.L
        *  \version 1.0
        *  \date 1/02/2014
        */
        template<class F>
        class DynamicFunction;
        /**
        *  \file  fastDelegate.h
        *  \class DynamicFunction
        *  \param R the return type of the function.
        *  \param ArgT... the type of the arguments of the function.
        *  \brief Specialized template class for functors. (inherit from std::function)
        *  build a functor with the right constructor depending a the pointer's function type.
        *  \author Duroisin.L
        *  \version 1.0
        *  \date 1/02/2014
        */
        template<class R, class... ArgT>
        class DynamicFunction<R(ArgT...)>
            : std::function<R(ArgT...)>
        {
            /**> just an alias to the type of the base class.*/
            using Base = std::function<R(ArgT...)>;

        public:
            /**
            * \fn DynamicFunction(F&& f)
            * \brief pass a functor to the std::function.
            * \param F&& f : the functor to pass to the std::function.
            */
            template<class F>
            DynamicFunction(F&& f) : Base(std::forward<F>(f))
            {}
            /** \fn DynamicFunction (R (C::*pf)(ArgU...))
            *   \brief pass a pointer to a member's function to the DynamicWrapper, and then
            *   pass this wrapper to the std::function, so, the std::function call the operator() of the DynamicWrapper class
            *   and not the operator() of the std::function class so we can perform the dynamic_cast if necessary.
            *   \param R(C::*pf)(ArgU...) : the pointer to the member's function.
            *   \
            */
            template<class C, class... ArgU>
            DynamicFunction(R(C::*pf)(ArgU...))
                : Base(DynamicWrapper<R,C,ArgU...>(pf))
            {}
            /**> just an allias to the operator() of the base class.*/
            using Base::operator();
        };
        /**
        *  \file  fastDelegate.h
        *  \class Delegate
        *  \param R the return type of the function.
        *  \brief Interface with can hold a delegate to every delegates types.
        *  \author Duroisin.L
        *  \version 1.0
        *  \date 1/02/2014
        */
        template<class R>
        struct Delegate {
            /**\fn Delegate()
            *  \brief default constructor.
            */
            Delegate() =default;
            /**\fn virtual std::unique_ptr<Delegate> clone() const = 0;
            *  \brief pure virtual function to redefine in the subclass to copy the delegate.
            *  \return std::unique_ptr<Delegate> a pointer to the copied delegate.
            */
            virtual std::unique_ptr<Delegate> clone() const = 0;
            /**\fn R operator()() = 0;
            *  \brief pure virtual function to redefines to call the std::function of the delegate.
            *  \return R : return the value returned by the std::function.
            */
            virtual void bind(FUN_PARAM) = 0;
            virtual R operator()(FUN_PARAM) = 0;
            /** /fn virtual Delegate()
            * \brief destructor
            */
            virtual ~Delegate(){}

        protected:
            /**
            * \fn Delegate (const Delegate&) {}
            * \brief copy constructor.
            * \param const Delegate& : the delegate to copy.
            */
            Delegate(const Delegate&){}
            /**
            * \fn Delegate& operator= (const Delegate&) {}
            * \brief affector.
            * \return Delegate& : return a reference to the current delegate.
            */
            Delegate& operator=(const Delegate&)
            {
                return *this;
            }
        };
        /**
        *  \file  fastDelegate.h
        *  \class FastDelegateImpl
        *  \brief Implementation of the delegate's interfaces.
        *  \author Duroisin.L
        *  \version 1.0
        *  \date 1/02/2014
        */
        template<class R, class... ArgT>
        struct FastDelegateImpl : Delegate<R> {
            /** \fn FastDelegateImpl(F&& f, ArgU&&... arg)
            *   \brief constructor : create the delegate with the functor and the arguments value passed.
            */
            template<class F, class... ArgU>
            FastDelegateImpl(F&& f, ArgU&&... arg)
                : func(std::forward<F>(f))
                , param(std::forward<ArgU>(arg)...)
            {}
            /** \fn std::unique_ptr<Delegate<R>> clone() const
            *   \brief make a copy of the delegate.
            *   \return the copied delegate.
            */
            std::unique_ptr<Delegate<R>> clone() const
            { return std::make_unique<FastDelegateImpl>(*this); }
            /** \fn R operator()()
            *   \brief call the std::function of the delegate.
            *   \return the value returned by the std::function.
            */
            void bind(FUN_PARAM) {
              bindParams(FWD_PARAM);
            }
            R operator()(FUN_PARAM)
            { return call(std::make_index_sequence<sizeof...(ArgT)>(), FWD_PARAM); }
            /** \fn R operator()()
            *   \brief changed the parameter's values of the std::function.
            *   \param ArgU&&... args : the values for the parameters to the delegate's function.
            */
            template<class... ArgU>
            void setParams(ArgU&&... arg)
            { param=std::make_tuple(std::forward<ArgU>(arg)...); }

        private:
            /** \fn R call(std::index_sequence<I...>)
            *   \brief pass each elements of the tuple to the std::function and unwarp them.
            *   \param std::index_sequence<I...> : a sequence of indexes to get every elements of the tuple.
            *   \return the value returned by the std::function.
            */
            template <std::size_t I = 0, typename... Arg>
            typename std::enable_if<I == sizeof...(ArgT)>::type
            bindParams(Arg&&... arg) {

            }
            template <std::size_t I = 0, typename... Arg>
            typename std::enable_if<(I < sizeof...(ArgT))>::type
            bindParams(Arg&&... arg) {
                std::get<I>(param) = std::get<I>(param).bind(std::forward<Arg>(arg)...);
                bindParams<I+1>(std::forward<Arg>(arg)...);
            }
            template<std::size_t... I, class... ArgU>
            R call(std::index_sequence<I...>, ArgU&&... args) const
            { return func(std::get<I>(param).get(std::forward<ArgU>(args)...)...); }
            DynamicFunction<R(ArgT&...)> func; /**> a functor whith hold the pointer to a callback function.*/
            std::tuple<RefVal<ArgT>...> param; /**> the wrapped values of the parameters to pass to the callback's function.*/
        };
        /**
        *  \file  fastDelegate.h
        *  \class FastDelegate
        *  \brief Class used for the type erasure,
        *  which allow the user be able to store a set of different callback's functions types with the same return type.
        *  \author Duroisin.L
        *  \version 1.0
        *  \date 1/02/2014
        */
        template<class R>
        struct FastDelegate {
            /**\fn default constructor
            */
            FastDelegate() = default;
            /**\fn FastDelegate (F&& f, Arg&& arf)
            *  \param F&& f : the functor to pass.
            *  \param Arg&&... arg : arguments to pass to the functor.
            */
            template<class F, class... Arg>
            FastDelegate(F&& f, Arg&&... arg) :
                delegate(std::make_unique
                    <FastDelegateImpl<R,ToStore_t<Arg>...>>
                    (std::forward<F>(f),std::forward<Arg>(arg)...)
                )
            {}
            /**\fn FastDelegate (FastDelegate& rhs)
            *  \brief copy constructor.
            *  \param FastDelegate& rhs : the delegate to copy.
            */
            FastDelegate(FastDelegate& rhs)
                : delegate(rhs.delegate->clone())
            {}
            /**\fn FastDelegate (const FastDelegate& rhs)
            *  \brief copy constructor.
            *  \param const FastDelegate& rhs : the delegate to copy.
            */
            FastDelegate(const FastDelegate& rhs)
                : delegate(rhs.delegate->clone())
            {}
            /**\fn FastDelegate (FastDelegate& rhs)
            *  \brief move constructor.
            *  \param FastDelegate&& rhs : the delegate to move.
            */
            FastDelegate(FastDelegate&& rhs) =default;
            /**\fn FastDelegate& operator= (FastDelegate& rhs)
            *  \brief affect the content of the delegate to the delegate.
            *  \param const FastDelegate& rhs : the delegate affect.
            *  \return FastDelegate& : the affected delegate.
            */
            FastDelegate& operator=(const FastDelegate& rhs)
            { return operator=(FastDelegate(rhs)); }
            /**\fn FastDelegate& operator= (FastDelegate& rhs)
            *  \brief affect and move the content of the delegate to the delegate.
            *  \param const FastDelegate& rhs : the delegate to move and to affect.
            *  \return FastDelegate& : the moved and affected delegate.
            */
            FastDelegate& operator=(FastDelegate&&) =default;
            /**\fn R operator()() const;
            *  \brief call the std::function of the delegate.
            *  \return the value returned by the std::function.
            */
            template <typename... Arg>
            void bind(Arg&&... arg) const {
                 delegate->bind(&arg...);
            }
            template<class... Arg>
            R operator()(Arg&&... arg) const
            { return (*delegate)(&arg...); }
            /**\fn setParams(Arg&&... arg)
            *  \brief change the parameter's values of the delegate.
            *  \return Arg&&... arg : the values of the parameters of the delegate.
            */
            template<class... Arg>
            void setParams(Arg&&... arg)
            {
                using DynamicType =
                    FastDelegateImpl<R,ToStore_t<Arg>...>*;
                if(dynamic_cast<DynamicType>(delegate.get()))
                    dynamic_cast<DynamicType>(delegate.get())->setParams(std::forward<Arg>(arg)...);
                else
                    throw Erreur(0, "Bad cast!", 5);
            }
        private:
            std::unique_ptr<Delegate<R>> delegate; /**> holds the pointer to the generic delegate.*/
        };
    }
}
#endif
