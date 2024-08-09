#pragma once



#include <array>
#include <cmath>
#include <cstddef>
#include <functional>



namespace fb
{
	namespace util
	{
		template<class T, int N>
		class CycleBuffer
		{
			public:
				class CycleBufferIterator
				{
					public:
						using iterator_category = std::forward_iterator_tag;
						using value_type = T;
						using element_type = T;
						using pointer = value_type*;
						using reference = value_type&;
						using const_reference = const value_type&;
						using difference_type = std::ptrdiff_t;
						


						static constexpr int InvalidIndex = -1;



						CycleBufferIterator(const std::array<T, N>& data, int index)
							: _data(data), _index(index)
						{

						}

						const_reference operator*() const
						{
							return _data[_index];
						}

						bool operator==(const CycleBufferIterator& r) const
						{
							return _index == r._index;
						}

						CycleBufferIterator& operator++()
						{
							//TODO: make it loop over
							_index++;
							if(_index >= _data.size()){
								_index = 0;
							}

							return *this;
						}

						operator bool() const
						{
							return _index != InvalidIndex;
						}

					private:
						const std::array<T, N>& _data;
						int _index;
				};



				using Iterator = CycleBufferIterator;



				//TODO: put, get, iterator?, current position

				//запихивает значение в буффер, если места нет то затирает наиболее старый элемент
				void pushValue(const T& val)
				{
					_data[_writeIndex] = val;
					_writeIndex = (_writeIndex + 1) % _data.size();

					_length = std::min(_length + 1, static_cast<int>(_data.size()));
					if(_length == _data.size()){
						_readIndex = (_readIndex + 1) % _data.size();
					}
				}

				//находит индекс желаемого предиката
				Iterator findValueIndex(const std::function<bool(const T&)>& predicate) const
				{
					int index = _readIndex;
					while(index != _writeIndex){
						if(std::invoke(predicate, _data[index])){
							return Iterator(_data, index);
						}

						index = (index + 1) % _data.size();
					}

					return Iterator(_data, Iterator::InvalidIndex);
				}

				Iterator end() const
				{
					return Iterator(_data, _writeIndex);
				}

				// const T& getValue(int index);

				//количество элементов которое можно считать
				// int readLength() const;

			private:
				std::array<T, N> _data;

				int _writeIndex = 0;
				int _readIndex = 0;
				int _length = 0;
		};
	}
}