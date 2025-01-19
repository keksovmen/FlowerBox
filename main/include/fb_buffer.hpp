#pragma once



#include <array>
#include <string_view>
#include <cstring>



namespace fb
{
	namespace util
	{
		template<int N>
		class Buffer
		{
			public:

				/**
				 * @brief writes data in to buffer
				 * moves write index to new position
				 * 
				 * @param data 
				 */

				void putData(std::string_view data)
				{
					const int remainSpace = _array.size() - _writeIndex;

					if(remainSpace >= data.length()){
						//will fit
						strncpy(_array.data() + _writeIndex, data.data(), data.length());
						_writeIndex += data.length();
						//for case when _writeIndex == _array.size() need to make it 0
						_writeIndex = _writeIndex % _array.size();
					}else{
						//will overwrite the begining
						//put in to the tail
						strncpy(_array.data() + _writeIndex, data.data(), remainSpace);

						//put in to the head
						const int remains = data.length() - remainSpace;
						strncpy(_array.data(), data.data() + remainSpace, remains);
						_writeIndex = remains;
					}

					_writeCount += data.length();
					if(_writeCount > _array.size()){
						_writeCount = _array.size();
						_readIndex = _writeIndex;
					}
				}

				/**
				 * @brief 
				 * 
				 * @return true data must be read in 2 calls 
				 * @return false data contains only in first call
				 */

				bool isPartitioned() const
				{
					return _readIndex + _writeCount > _array.size();
				}

				/**
				 * @brief moves read index to write index in case of one partition
				 * or moves it to 0 in case of two partitions
				 * 
				 * @return std::string_view if length 0 -> no data
				 */

				std::string_view readPartition()
				{
					std::string_view result;
					if(isPartitioned()){
						result = std::string_view(_array.data() + _readIndex, _array.size() - _readIndex);
						assert((int) result.length() >= 0);

						_readIndex = 0;
					}else{
						result = std::string_view(_array.data() + _readIndex, _writeCount);
						assert((int) result.length() >= 0);

						_readIndex = _writeIndex;
					}

					_writeCount -= result.length();

					return result;
				}

			private:
				std::array<char, N> _array;
				
				int _writeIndex = 0;
				int _readIndex = 0;

				int _writeCount = 0;
		};
	}
}