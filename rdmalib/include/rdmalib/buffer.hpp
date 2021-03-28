
#ifndef __RDMALIB_BUFFER_HPP__
#define __RDMALIB_BUFFER_HPP__

#include <cstdint>
#include <utility>

#include <cereal/cereal.hpp>

struct ibv_pd;
struct ibv_mr;
struct ibv_sge;

namespace rdmalib {

  struct ScatterGatherElement;

  namespace impl {

    // move non-template methods from header
    struct Buffer {
    protected:
      uint32_t _size;
      uint32_t _header;
      uint32_t _bytes;
      uint32_t _byte_size;
      void* _ptr;
      ibv_mr* _mr;

      Buffer();
      Buffer(uint32_t size, uint32_t byte_size, uint32_t header);
      Buffer(Buffer &&);
      Buffer & operator=(Buffer && obj);
      ~Buffer();
    public:
      uintptr_t address() const;
      void* ptr() const;
      ibv_mr* mr() const;
      uint32_t data_size() const;
      uint32_t size() const;
      uint32_t bytes() const;
      void register_memory(ibv_pd *pd, int access);
      uint32_t lkey() const;
      uint32_t rkey() const;
      ScatterGatherElement sge(int size, int offset);
    };

  }

  struct RemoteBuffer {
    uintptr_t addr;
    uint32_t rkey;
    uint32_t size;

    RemoteBuffer();
    // When accessing the remote buffer, we might not need to know the size.
    RemoteBuffer(uintptr_t addr, uint32_t rkey, uint32_t size = 0);

    template<class Archive>
    void serialize(Archive & ar)
    {
      ar(CEREAL_NVP(addr), CEREAL_NVP(rkey), CEREAL_NVP(size));
    }
  };

  template<typename T>
  struct Buffer : impl::Buffer{

    Buffer():
      impl::Buffer()
    {}

    Buffer(size_t size, size_t header = 0):
      impl::Buffer(size, sizeof(T), header)
    {}

    Buffer<T> & operator=(Buffer<T> && obj)
    {
      impl::Buffer::operator=(std::move(obj));
      return *this;
    }

    Buffer(const Buffer<T> & obj) = delete;
    Buffer(Buffer<T> && obj) = default;

    T* data() const
    {
      return static_cast<T*>(this->_ptr) + this->_header;
    }
  };

  struct ScatterGatherElement {
    // smallvector in practice
    std::vector<ibv_sge> _sges;

    ScatterGatherElement();

    ScatterGatherElement(uint64_t addr, uint32_t bytes, uint32_t lkey);

    template<typename T>
    ScatterGatherElement(const Buffer<T> & buf)
    {
      add(buf);
    }

    template<typename T>
    void add(const Buffer<T> & buf)
    {
      //emplace_back for structs will be supported in C++20
      _sges.push_back({buf.address(), buf.bytes(), buf.lkey()});
    }

    ibv_sge * array();
    size_t size();
  };
}

#endif

