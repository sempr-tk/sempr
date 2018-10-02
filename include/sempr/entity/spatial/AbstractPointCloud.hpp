#ifndef SEMPR_ENTITY_SPATIAL_ABSTRACTPOINTCLOUD_HPP_
#define SEMPR_ENTITY_SPATIAL_ABSTRACTPOINTCLOUD_HPP_

#include <vector>
#include <memory>
#include <functional>
#include <cstdint>


namespace sempr { namespace entity {

// A list of channel type recommendations. Feel free to use own or additional type identifications in your use-case.
enum ChannelType  // Note: Strong typed enum classes do not allow an implicit downcast to int.
{
    N_X = 0,    // Normale in X                     (double)
    N_Y = 1,    // Normale in Y                     (double)
    N_Z = 2,    // Normale in Z                     (double)
    V = 4,      // Velocity in normal direction     (float)
    I = 5,      // Intensity, normalized            (float)
    R = 10,     // Red                              (uint8_t)
    G = 11,     // Green                            (uint8_t)
    B = 12,     // Blue                             (uint8_t)
    
    H_0 = 100   // Hyperspectral Channel 0
    // e.g. H_0 + 10 = Hyperspectral Channel 10
};


/**
 * The AbstractPoint interface defines how the access the components (X, Y, Z) of a point.
 * The realization of this interface shall map there datastructure with speciftic types to this methodes. 
 */
template<typename T>
class AbstractPoint
{
public:
    using Ptr = std::shared_ptr< AbstractPoint<T> >;

    virtual T getX() = 0;
    virtual T getY() = 0;
    virtual T getZ() = 0;

    virtual const T& operator[](std::size_t idx) const = 0;
};


/**
 * The AbstractChannel interface defines how to access the additional information for every point.
 * It allows an read and write access to all elements and shall the same size than the pointcloud has.
 */
template<typename T>
class AbstractChannel
{
public:
    virtual std::size_t size() const = 0;
    virtual T& operator[](std::size_t idx) = 0;
    virtual const T& operator[](std::size_t idx) const = 0;

    class iterator
    {
    public:
        iterator(AbstractChannel* ptr, std::size_t index = 0) : ptr_(ptr), index_(index) { }
        iterator operator++() { iterator i = *this; index_++; return i; }
        iterator operator++(int junk) { index_++; return *this; }
        T& operator*() { return (*ptr_)[index_]; }
        T* operator->() { return &operator*(); }
        bool operator==(const iterator& rhs) { return ptr_ == rhs.ptr_ && index_ == rhs.index_; }
        bool operator!=(const iterator& rhs) { return ptr_ != rhs.ptr_ || index_ != rhs.index_;; }
    private:
        AbstractChannel* const ptr_;
        std::size_t index_;
    };

    class const_iterator
    {
    public:
        const_iterator(const AbstractChannel* ptr, std::size_t index = 0) : ptr_(ptr), index_(index) { }
        const_iterator operator++() { const_iterator i = *this; index_++; return i; }
        const_iterator operator++(int junk) { index_++; return *this; }
        const T& operator*() { return (*ptr_)[index_]; }
        const T* operator->() { return &operator*(); }
        bool operator==(const const_iterator& rhs) { return ptr_ == rhs.ptr_ && index_ == rhs.index_; }
        bool operator!=(const const_iterator& rhs) { return ptr_ != rhs.ptr_ || index_ != rhs.index_;; }
    private:
        const AbstractChannel* ptr_;
        std::size_t index_;
    };

    const_iterator cbegin() { return const_iterator(this, 0); };
    const_iterator begin() const { return const_iterator(this, 0); };
    iterator begin() { return iterator(this, 0); };

    const_iterator cend() { return const_iterator(this, size()); }; 
    const_iterator end() const { return const_iterator(this, size()); };
    iterator end() { return iterator(this, size()); };
};


/**
 * The AbstractPointCloud is an interface that defines how to access the elements of a point cloud (channels and points)
 * In future a PointCloud implementation have to fullfill this interface.
 * 
 * Note: For now there is only readable access to the points of the PointCloud. If you like to modify the point you have to grab them and put they into a new PointCloud object.
 */
template<typename T>
class AbstractPointCloud
{
public:
    using Ptr = std::shared_ptr<AbstractPointCloud>;

    virtual bool hasChannel(int type) const = 0;

    /**
     * It isnt possible to declare a template to a virtual method.
     * And C++ have no return value overloading so the the only this manual way to do.
     * It shall look like:
     * 
     * template<typename D>
     * typename std::enable_if<std::is_fundamental<D>::value>
     * virtual AbstractChannel<D>& getChannel(int type) = 0;
     */
    virtual void getChannel(int type, AbstractChannel<int8_t>& channel) const = 0;
    virtual void getChannel(int type, AbstractChannel<int16_t>& channel) const = 0;
    virtual void getChannel(int type, AbstractChannel<int32_t>& channel) const = 0;
    virtual void getChannel(int type, AbstractChannel<int64_t>& channel) const = 0;

    virtual void getChannel(int type, AbstractChannel<uint8_t>& channel) const = 0;
    virtual void getChannel(int type, AbstractChannel<uint16_t>& channel) const = 0;
    virtual void getChannel(int type, AbstractChannel<uint32_t>& channel) const = 0;
    virtual void getChannel(int type, AbstractChannel<uint64_t>& channel) const = 0;

    virtual void getChannel(int type, AbstractChannel<float>& channel) const = 0;
    virtual void getChannel(int type, AbstractChannel<double>& channel) const = 0;

    virtual std::size_t size() const = 0;

    virtual const std::shared_ptr< AbstractPoint<T> > operator[](std::size_t idx) const = 0;

    class const_iterator
    {
    public:
        const_iterator(const AbstractPointCloud* ptr, std::size_t index = 0) : ptr_(ptr), index_(index) { }
        const_iterator operator++() { const_iterator i = *this; index_++; return i; }
        const_iterator operator++(int junk) { index_++; return *this; }
        const AbstractPoint<T>& operator*() { return *(*ptr_)[index_]; }
        const AbstractPoint<T>* operator->() { return &operator*(); }
        bool operator==(const const_iterator& rhs) { return ptr_ == rhs.ptr_ && index_ == rhs.index_; }
        bool operator!=(const const_iterator& rhs) { return ptr_ != rhs.ptr_ || index_ != rhs.index_;; }
    private:
        const AbstractPointCloud* ptr_;
        std::size_t index_;
    };

    const_iterator cbegin() { return const_iterator(this, 0); };
    const_iterator begin() const { return const_iterator(this, 0); };

    const_iterator cend() { return const_iterator(this, size()); }; 
    const_iterator end() const { return const_iterator(this, size()); };

};


    
}}

#endif /* end of include guard: SEMPR_ENTITY_SPATIAL_ABSTRACTPOINTCLOUD_HPP_ */
