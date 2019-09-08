#pragma once

#include <cstdlib>
#include <stdlib.h>
#include <new>
#include <initializer_list>

//TODO make growth rate phi
//TODO make use of virtualalloc/make a function to free all the unused data
template<class T> class List
{
public:
	List()
	{
		capacity = 1;
		elements = 0;
		data = (T*)malloc(sizeof(T) * capacity);
	}

	List(u32 elements)
		: capacity(elements), elements(elements)
	{
		data = (T*)malloc(sizeof(T)*elements);
	}

	List(u32 capacity, u32 elements)
		: capacity(capacity), elements(elements)
	{
		data = (T*)malloc(sizeof(T)*capacity);
	}

	List(const List& other)
	{
		capacity = other.capacity;
		elements = other.elements;
		data = (T*)malloc(sizeof(T)*capacity);
		memcpy(data, other.data, sizeof(T)*capacity);
	}

	List(std::initializer_list<T> initList)
	{
		int len = initList.end() - initList.begin();
		capacity = len;
		elements = len;
		data = (T*)malloc(sizeof(T) * capacity);
		T const* begin = initList.begin();
		for (int i = 0; i < len; ++i)
		{
			data[i] = begin[i];
		}
	}

	~List()
	{
		free(data);
	}

	T& add(const T& element)
	{
		if (elements < capacity)
		{
			new(data + elements) T(element);

			elements++;
			return data[elements - 1];
		}
		else
		{
			T* tmp = (T*)malloc(sizeof(T)*capacity * 2);
			memcpy(tmp, data, capacity * sizeof(T));
			capacity *= 2;
			free(data);
			data = tmp;

			new(data + elements) T(element);
			elements++;
			return data[elements - 1];
		}
	}

	void add(List<T>& other)
	{
		if (elements + other.elements > capacity)
		{
			T* tmp = (T*)malloc(sizeof(T)*(elements + other.elements));
			memcpy(tmp, data, capacity * sizeof(T));
			capacity = elements + other.elements;
			free(data);
			data = tmp;

			memcpy(data + elements, other.data, other.elements * sizeof(T));

			elements += other.elements;
		}
		else
		{
			memcpy(data + elements, other.data, other.elements * sizeof(T));
			elements += other.elements;
		}
	}

	T* add()
	{
		if (elements < capacity)
		{
			return &data[elements++];
		}
		else
		{
			T* tmp = (T*)malloc(sizeof(T)*capacity * 2);
			memcpy(tmp, data, capacity * sizeof(T));
			capacity *= 2;
			free(data);
			data = tmp;

			return &data[elements++];
		}
	}

	void insert(i32 index, const T& element)
	{
		if (elements < capacity)
		{
			memmove(data + index + 1, data + (i32)index, (elements - (i32)index) * sizeof(T));
			new(data + index) T(element);
			elements++;
		}
		else
		{
			T* tmp = (T*)malloc(sizeof(T)*capacity * 2);
			memcpy(tmp, data, capacity * sizeof(T));
			capacity *= 2;
			free(data);
			data = tmp;

			memmove(data + index + 1, data + index, sizeof(T) * (elements - (i32)index));
			new(data + index) T(element);
			elements++;
		}
	}

	void set(u32 index, T& element)
	{
		assert(index < elements);
		new(data + index) T(element);
	}

	void remove(unsigned int index)
	{
		assert(index < elements);
		data[index].~T();
		--elements;
		memcpy(&data[index], &data[index + 1], sizeof(T)*(elements - index));

		if (elements + 1 < (capacity + 1) / 2)
		{
			T* tmp = (T*)malloc(sizeof(T)*((capacity + 1) / 2));
			memcpy(tmp, data, sizeof(T)*elements);
			capacity = (capacity + 1) / 2;
			free(data);
			data = tmp;
		}
	}

	void removeNoResize(unsigned int index)
	{
		assert(index < elements);
		data[index].~T();
		--elements;
		memcpy(&data[index], &data[index + 1], sizeof(T)*(elements - index));
	}
	T popBack()
	{
		assert(elements);
		elements--;
		T ret = data[elements];
		data[elements].~T();

		if (elements + 1 < (capacity + 1) / 2)
		{
			T* tmp = (T*)malloc(sizeof(T)*((capacity + 1) / 2));
			memcpy(tmp, data, sizeof(T)*elements);
			capacity = (capacity + 1) / 2;
			free(data);
			data = tmp;
		}
		return ret;
	}
	T popBackNoResize()
	{
		assert(elements);
		elements--;
		T ret = data[elements];
		data[elements].~T();
		return ret;
	}

	void resize(unsigned int newLength)
	{
		if (newLength == elements)
			return;
		else if (newLength > elements)
		{
			if (newLength > capacity)
			{
				T* tmp = (T*)malloc(sizeof(T)*newLength);
				memcpy(tmp, data, sizeof(T)*capacity);
				capacity = newLength;
				free(data);
				data = tmp;
			}
			elements = newLength;
		}
		else
		{
			for (unsigned int i = newLength; i < elements; i++)
				data[i].~T();
			elements = newLength;
			if (elements + 1 < (capacity + 1) / 2)
			{
				T* tmp = (T*)malloc(sizeof(T)*((capacity + 1) / 2));
				memcpy(tmp, data, sizeof(T)*elements);
				capacity = (capacity + 1) / 2;
				free(data);
				data = tmp;
			}
		}
	}
	//if the new size is bigger the new elements will be set to value
	void resize(unsigned int newLength, const T& value)
	{
		if (newLength == elements)
			return;
		else if (newLength > elements)
		{
			if (newLength > capacity)
			{
				T* tmp = (T*)malloc(sizeof(T)*newLength);
				memcpy(tmp, data, sizeof(T)*capacity);
				capacity = newLength;
				free(data);
				data = tmp;
			}
			for (unsigned int i = elements; i < newLength; i++)
				new(data + i) T(value);
			elements = newLength;
		}
		else
		{
			for (unsigned int i = newLength; i < elements; i++)
				data[i].~T();
			elements = newLength;
			if (elements + 1 < (capacity + 1) / 2)
			{
				T* tmp = (T*)malloc(sizeof(T)*((capacity + 1) / 2));
				memcpy(tmp, data, elements);
				capacity = (capacity + 1) / 2;
				free(data);
				data = tmp;
			}
		}
	}

	void reserve(unsigned int newCapacity)
	{
		if (newCapacity == capacity)
			return;
		else if (newCapacity > capacity)
		{
			T* tmp = (T*)malloc(sizeof(T)*newCapacity);
			memcpy(tmp, data, sizeof(T)*capacity);
			capacity = newCapacity;
			free(data);
			data = tmp;
		}
		else
		{
			for (unsigned int i = newCapacity; i < elements; i++)
				data[i].~T();

			T* tmp = (T*)malloc(sizeof(T)*newCapacity);
			memcpy(tmp, data, sizeof(T)*newCapacity);
			free(data);
			data = tmp;

			capacity = newCapacity;
			if (elements > newCapacity)
				elements = newCapacity;
		}
	}

	void clear()
	{
		for (u32 i = 0; i < elements; ++i)
		{
			data[i].~T();
		}
		elements = 0;
	}

	void reset(u32 newSize = 1)
	{
		for (u32 i = 0; i < elements; ++i)
		{
			data[i].~T();
		}
		elements = 0;

		resize(newSize);
	}

	void clear(u32 newReservedSize)
	{
		for (u32 i = 0; i < elements; ++i)
		{
			data[i].~T();
		}
		elements = 0;

		reserve(newReservedSize);
	}

	void shrink()
	{
		reserve(length());
	}

	bool contains(T& other)
	{
		for (u32 i = 0; i < elements; ++i)
			if (data[i] == other)
				return true;
		return false;
	}

	//returns index of first occurrence
	u32 indexOf(T& other)
	{
		for (u32 i = 0; i < elements; ++i)
			if (data[i] == other)
				return i;
		return (u32)-1;
	}

	T* last()
	{
		return &data[elements - 1];
	}

	T* first()
	{
		return data;
	}

	__forceinline T& operator[] (const unsigned int index) const
	{
		assert(index >= 0 && index < elements);
		return data[index];
	}


	void operator=(const List<T>& other)
	{
		this->elements = other.elements;
		this->capacity = other.capacity;

		free(data);
		data = (T*)malloc(sizeof(T)*capacity);
		memcpy(data, other.data, sizeof(T)*capacity);
	}

	__forceinline unsigned int length() const { return elements; }
	__forceinline unsigned int size() const { return elements; }

	T* data;
protected:
	//List(List&) = delete;
	//List operator=(List&) = delete;

	u32 elements;
	u32 capacity;
};