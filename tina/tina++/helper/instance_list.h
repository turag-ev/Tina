#ifndef INSTANCE_LIST
#define INSTANCE_LIST
#include <type_traits>

namespace TURAG {

template<class T>
class InstanceList {
public:
    typedef InstanceList<T> own_type;

    struct iterator {
        iterator(own_type* it):
            it_(it)
        { }
        T& operator*() { return *static_cast<T*>(it_); }
        T* operator->() { return static_cast<T*>(it_); }

        bool operator==(const iterator& rhs) const { return it_ == rhs.it_; }
        bool operator!=(const iterator& rhs) const { return it_ != rhs.it_; }

        inline iterator operator++(int) {
            iterator cpy(*this);
            it_ = it_->next_;
            return cpy;
        }
        inline iterator operator--(int) {
            iterator cpy(*this);
            it_ = it_->prev_;
            return cpy;
        }
        inline iterator& operator++() {
            it_ = it_->next_;
            return *this;
        }
        inline iterator& operator--() {
            it_ = it_->prev_;
            return *this;
        }
    private:
        own_type* it_;
    };

    InstanceList():
        next_(nullptr)
    {
        static_assert(std::is_base_of<own_type, T>::value, "");
        if(last_) {
            //list not empty
            last_->next_ = this;
            this->prev_ = last_;
            last_ = this;
        } else {
            first_ = last_ = this;
            prev_ = nullptr;
        }
    }
#if defined(TURAG_USE_LIBSUPCPP_RUNTIME_SUPPORT) && TURAG_USE_LIBSUPCPP_RUNTIME_SUPPORT
    virtual 
#endif
    ~InstanceList() {
        //update endpoints
        if(this == last_)
            last_ = prev_;
        if(this == first_)
            first_ = next_;
        //remove from list
        if(prev_)
            prev_->next_ = next_;
        if(next_)
            next_->prev_ = prev_;
    }

    static iterator begin() {
        return iterator(first_);
    }
    static iterator end() {
        return iterator(nullptr);
    }

private:
    friend class iterator;
    static own_type* first_;
    static own_type* last_;
    own_type* next_;
    own_type* prev_;
};

template<class T>
InstanceList<T>* InstanceList<T>::first_ = nullptr;

template<class T>
InstanceList<T>* InstanceList<T>::last_ = nullptr;

} // namespace TURAG

#endif // INSTANCE_LIST

