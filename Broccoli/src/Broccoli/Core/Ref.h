#pragma once

#include <stdint.h>
#include <type_traits>

namespace Broccoli {

	class RefCounted
	{
	public:
		void incrementRefCount() const
		{
			refCount++;
		}
		void decreaseRefCount() const
		{
			refCount--;
		}

		uint32_t getRefCount() const { return refCount; }

	private:
		mutable uint32_t refCount = 0;
	};

	template<typename T>
	class Ref
	{
	public:
		Ref() : instance(nullptr)
		{
		}
		~Ref()
		{
			decreaseRef();
		}

		Ref(std::nullptr_t n) :instance(instance)
		{
		}

		Ref(T* instance) : instance(instance)
		{
			static_assert(std::is_base_of<RefCounted, T>::value, "Class is not RefCounted");
			increaseRef();
		}

		template<typename T2>
		Ref(const Ref<T2>& other)
		{
			instance = (T*)other.instance;
			increaseRef();
		}

		template<typename... Args>
		static Ref<T> create(Args&&... args)
		{
			return Ref<T>(new T(std::forward<Args>(args)...));
		}

		
		template<typename T2>
		Ref(Ref<T2>&& other)
		{
			instance = (T*)other.instance;
			other.instance = nullptr;
		}

		Ref& operator=(std::nullptr_t)
		{
			decreaseRef();
			instance = nullptr;
			return *this;
		}

		Ref& operator=(const Ref<T>& other)
		{
			other.increaseRef();
			decreaseRef();

			instance = other.instance;
			return *this;
		}
		
		template<typename T2>
		Ref& operator=(Ref<T2>&& other)
		{
			decreaseRef();

			instance = other.instance;
			other.instance = nullptr;
			return *this;
		}

		T* operator->() { return instance; }
		const T* operator->() const { return instance; }

		T& operator*() { return *instance; }
		const T& operator*() const { return *instance; }

	private:
		void increaseRef() const
		{
			if (instance)
			{
				instance->incrementRefCount();
			}
		}

		void decreaseRef() const
		{
			if (instance)
			{
				instance->decreaseRefCount();
				if (instance->getRefCount() == 0)
				{
					delete instance;
					instance = nullptr;
				}
			}
		}

	private:
		template<class T2>
		friend class Ref;
		mutable T* instance;
	};
}