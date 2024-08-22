#ifndef VOXEL_DYNAMICS_ALLOCATOR
#define VOXEL_DYNAMICS_ALLOCATOR

#include "VoxelDynamicsMath.h"
#include <vector>

template <typename T>
struct IVDSortable {
    virtual ~IVDSortable() = default;
    virtual bool operator<(const T& other) const = 0;
    virtual bool operator==(const T& other) const = 0;
    virtual bool operator>(const T & other) const = 0;
};

struct IVDHashable {
    virtual ~IVDHashable() = default;
    virtual VDuint hashValue() const = 0;
};

struct VDPointer : IVDSortable<VDPointer>, IVDHashable
{
    uintptr_t value;

    VDPointer()
    {
        value = (uintptr_t)nullptr;
    }

    VDPointer(uintptr_t _value)
    {
        value = _value;
    }

    VDPointer(const VDPointer& other) : 
        value(other.value) {}

    template<typename T>
    VDPointer(T* ptr)
        : value(reinterpret_cast<uintptr_t>(ptr)) {}

    VDPointer& operator=(const VDPointer& other)
    {
        if (this != &other)
        {
            value = other.value;
        }
        return *this;
    }

    VDPointer& operator=(const uintptr_t& _value)
    {
        value = _value;
        return *this;
    }

    template<typename T>
    VDPointer& operator=(T* ptr)
    {
        value = reinterpret_cast<uintptr_t>(ptr);
        return *this;
    }

    template<typename T>
    operator T* () const
    {
        return reinterpret_cast<T*>(value);
    }


    bool operator<(const VDPointer& other) const override
    {
        return value < other.value;
    }

    bool operator>(const VDPointer& other) const override
    {
        return value > other.value;
    }

    bool operator==(const VDPointer& other) const override
    {
        return value == other.value;
    }

    VDuint hashValue() const override
    {
        return static_cast<VDuint>(value);
    }
};

struct VDBaseObjectPool 
{
    virtual ~VDBaseObjectPool() = default;
    virtual void* allocate() = 0;
    virtual void free(void* obj) = 0;
};

template <typename T>
struct VDObjectPool : public VDBaseObjectPool
{
    std::vector<std::vector<std::vector<T>>> pools;
    std::vector<T*> available;
    VDuint capacity;
    VDuint arraySize;

    void addPool()
    {
        std::vector<std::vector<T>> pool;
        pool.reserve(capacity);
        for (VDuint i = 0; i < capacity; ++i)
        {
            std::vector<T> arr;
            arr.resize(arraySize);
            pool.push_back(arr);
        }
        pools.push_back(std::move(pool));

        size_t lastInd = pools.size() - 1;
        for (VDuint i = 0; i < capacity; ++i)
        {
            available.push_back(&pools[lastInd][i][0]);
        }
    }

    VDObjectPool(VDuint _capacity, VDuint _arraySize) : 
        capacity(_capacity), arraySize(_arraySize)
    {
        addPool();
        available.reserve(_capacity);
    }

    ~VDObjectPool()
    {

    }

    void* allocate() override
    {
        if (available.empty()) 
        {
            addPool();
        }
        T* obj = available.back();
        available.pop_back();
        return obj;
    }


    void free(void* obj) override
    {
        available.push_back(static_cast<T*>(obj));
    }
};

#include <typeindex>
#include <typeinfo>
#include <unordered_map>

#define ALLOCATOR_INITIAL_CAPACITY 100
#define ALLOCATOR_HASH_DEFUALT_CAPACITY 10

struct VDAllocator 
{
    std::unordered_map<std::type_index, std::unordered_map<VDuint, VDBaseObjectPool*>> pools;

    template <typename T>
    void createPool(unsigned int capacity, VDuint arraySize) 
    {
        pools[typeid(T)][arraySize] = new VDObjectPool<T>(capacity, arraySize);
    }

    template <typename T>
    T* allocate(VDuint arraySize) 
    {
        return allocate<T>(arraySize, ALLOCATOR_INITIAL_CAPACITY);
    }
    
    template <typename T>
    T* allocate(VDuint arraySize, VDuint allocatorCapacity)
    {
        auto typeIt = pools.find(typeid(T));
        if (typeIt == pools.end())
        {
            createPool<T>(allocatorCapacity, arraySize);
            typeIt = pools.find(typeid(T));
        }

        auto sizeIt = typeIt->second.find(arraySize);
        if (sizeIt == typeIt->second.end())
        {
            createPool<T>(allocatorCapacity, arraySize);
            sizeIt = typeIt->second.find(arraySize);
        }

        return static_cast<T*>(sizeIt->second->allocate());
    }

    template <typename T>
    void free(T* obj, VDuint size) 
    {
        auto poolIt = pools.find(typeid(T));
        if (poolIt != pools.end())
        {
            auto sizeIt = poolIt->second.find(size);
            if (sizeIt != poolIt->second.end())
                sizeIt->second->free(obj);
        }
    }
};

VDAllocator gAllocator;

template <typename T>
struct VDList : IVDHashable, IVDSortable<VDList<T>>
{

    struct VDListData
    {
        T item;
        VDListData* pNext;

        VDListData()
        {
            pNext = nullptr;
        }

        VDListData(T _item)
        {
            item = _item;
            pNext = nullptr;
        }

        VDListData(T _item, VDListData* _pNext)
        {
            item = _item;
            pNext = _pNext;
        }

        void setData(T _item, VDListData* _pNext)
        {
            item = _item;
            pNext = _pNext;
        }
    };

    VDuint count;
    VDListData* pFirst;
    VDuint id;
    bool autoFree;

    VDList(bool _autoFree = false)
    {
        count = 0;
        pFirst = nullptr;
        autoFree = _autoFree;
    }

    T* insert(T item)
    {
        if (pFirst == nullptr)
        {
            pFirst = gAllocator.allocate<VDListData>(1);
            pFirst->setData(item, nullptr);
        }
        else
        {
            VDListData* pNext = pFirst;
            pFirst = gAllocator.allocate<VDListData>(1);
            pFirst->setData(item, pNext);
        }
        count++;
        return &pFirst->item;
    }

    VDList& operator=(const VDList& other)
    {
        if (&other != this)
        {
            pFirst = other.pFirst;
            count = other.count;
            id = other.id;
            autoFree = other.autoFree;
        }
        return *this;
    }

    bool contains(T item)
    {
        for (VDListData* pData = pFirst; pData != nullptr; pData = pData->pNext)
        {
            if (item == pData->item)
                return true;
            return false;
        }
    }

    bool insertUnique(T item)
    {
        if (!contains(item))
        {
            VDListData* newNode = gAllocator.allocate<VDListData>();
            newNode->setData(item, pFirst);
            pFirst = newNode;
            count++;
            return true;
        }
    }

    T* insertSorted(T item)
    {
        T* itemLoc;
        if (pFirst == nullptr || item < pFirst->item)
        {
            // Allocate a new node and point it to the current first node
            VDListData* newNode = gAllocator.allocate<VDListData>(1);
            newNode->setData(item, pFirst);
            // Update pFirst to point to the new node
            pFirst = newNode;
            itemLoc = &newNode->item;
        }
        else
        {
            VDListData* current = pFirst;
            while (current->pNext != nullptr && current->pNext->item < item)
            {
                current = current->pNext;
            }
            // Insert the new node in the correct position
            VDListData* newNode = gAllocator.allocate<VDListData>(1);
            newNode->setData(item, current->pNext);
            current->pNext = newNode;
            itemLoc = &newNode->item;
        }
        count++;
        return itemLoc;
    }

    T* insertSortedUnique(T item)
    {
        T* itemLoc = nullptr;
        // Check if the list is empty or if the new item should be the first element
        if (pFirst == nullptr || item < pFirst->item)
        {
            // Allocate and insert the new node at the beginning
            VDListData* newNode = gAllocator.allocate<VDListData>(1);
            newNode->setData(item, pFirst);
            pFirst = newNode;
            count++;
            itemLoc = &newNode->item;
        }
        else if (item == pFirst->item)
        {
            // Do nothing if the sort value is not unique (i.e., equal to the first node's sort value)
            return nullptr;
        }
        else
        {
            VDListData* current = pFirst;

            // Traverse the list to find the correct position to insert
            while (current->pNext != nullptr && current->pNext->item < item)
            {
                current = current->pNext;
            }

            // Check if the sort value is unique before insertion
            if (current->pNext == nullptr || current->pNext->item > item)
            {
                // Allocate and insert the new node at the correct position
                VDListData* newNode = gAllocator.allocate<VDListData>(1);
                newNode->setData(item, current->pNext);
                current->pNext = newNode;
                itemLoc = &newNode->item;
                count++;
            }
            // If sort values match, do not insert
        }
        return itemLoc;
    }

    void remove(T item) 
    {
        if (pFirst == nullptr) return;

        if (pFirst->item == item) 
        {
            VDListData* temp = pFirst;
            pFirst = pFirst->pNext;
            gAllocator.free<VDListData>(temp);
            count--;
            return;
        }

        VDListData* current = pFirst;
        while (current->pNext != nullptr) 
        {
            if (current->pNext->item == item) 
            {
                VDListData* temp = current->pNext;
                current->pNext = current->pNext->pNext;
                gAllocator.free<VDListData>(temp);
                count--;
                return;
            }
            current = current->pNext;
        }
    }

    void removeSorted(T item) {
        if (pFirst == nullptr) return;

        if (pFirst->item == item) 
        {
            VDListData* temp = pFirst;
            pFirst = pFirst->pNext;
            gAllocator.free<VDListData>(temp, 1);
            count--;
            return;
        }

        VDListData* current = pFirst;
        while (current->pNext != nullptr) 
        {
            if (current->pNext->item == item) 
            {
                VDListData* temp = current->pNext;
                current->pNext = current->pNext->pNext;
                gAllocator.free<VDListData>(temp, 1);
                count--;
                return;
            }
            if (current->pNext->item > item) 
            {
                // Since the list is sorted, we can break early
                break;
            }
            current = current->pNext;
        }
    }

    void free()
    {
        for (VDListData* pData = pFirst; pData != nullptr; pData = pData->pNext)
        {
            gAllocator.free<VDListData>(pData, 1);
        }
        pFirst = nullptr;
        count = 0;
    }

    std::vector<T> toVector()
    {
        std::vector<T> vec;
        vec.reserve(count);
        for (VDListData* pData = pFirst; pData != nullptr; pData = pData->pNext)
        {
            vec.push_back(pData->item);
        }
        return vec;
    }

    ~VDList()
    {
       if(autoFree)
            free();
    }

    VDuint hashValue() const override
    {
        return id;
    }

    bool operator<(const VDList<T>& other) const override
    {
        return id < other.id;
    }

    bool operator==(const VDList<T>& other) const override
    {
        return id == other.id;
    }

    bool operator>(const VDList<T>& other) const override
    {
        return id > other.id;
    }
};


template <typename T>
struct VDHashList
{
    VDuint arraySize;
    VDList<T>* listArray;

    VDHashList()
    {
        arraySize = 0;
        listArray = nullptr;
    }

    VDHashList(VDuint _arraySize, VDuint allocatorCapacity) : arraySize(_arraySize)
    {
        listArray = gAllocator.allocate<VDList<T>>(arraySize, allocatorCapacity);
    }

    VDHashList(VDuint _arraySize) : VDHashList(_arraySize, ALLOCATOR_HASH_DEFUALT_CAPACITY)
    {
    }

    VDHashList& operator=(const VDHashList& other)
    {
        if (this != &other)
        {
            arraySize = other.arraySize;
            listArray = other.listArray;
        }
        return *this;
    }

    void insert(T item)
    {
        VDuint index = static_cast<IVDHashable*>(&item)->hashValue()%arraySize;
        listArray[index].insert(item);
    }

    void insertSorted(T item)
    {
        VDuint index = static_cast<IVDHashable*>(&item)->hashValue() % arraySize;
        listArray[index].insertSorted(item);
    }

    void insertSortedUnique(T item)
    {
        VDuint index = static_cast<IVDHashable*>(&item)->hashValue() % arraySize;
        listArray[index].insertSortedUnique(item);
    }

    void remove(T item)
    {
        VDuint index = static_cast<IVDHashable*>(&item)->hashValue() % arraySize;
        listArray[index].remove(item);
    }

    void removeSorted(T item)
    {
        VDuint index = static_cast<IVDHashable*>(&item)->hashValue() % arraySize;
        listArray[index].removeSorted(item);
    }

    T* getItemByHashValue(VDuint hashValue)
    {
        hashValue = hashValue % arraySize;
        for (auto it = listArray[hashValue].pFirst; it != nullptr; it = it->pNext)
        {
            if (static_cast<IVDHashable*>(&it->item)->hashValue() == hashValue)
                return &it->item;
        }
        return nullptr;
    }

    ~VDHashList()
    {
        for (VDuint i = 0; i < arraySize; i++)
        {
            listArray[i].free();
        }
        gAllocator.free<VDList<T>>(listArray, arraySize);
    }

    VDList<T> toList()
    {
        VDList<T> list;
        for (VDuint i = 0; i < arraySize; i++)
        {
            for (auto it = listArray[i].pFirst; it != nullptr; it = it->pNext)
            {
                list.insert(it->item);
            }
        }
        return list;
    }

};  

#endif