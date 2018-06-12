#ifndef _P2CL_CIRCULARINDEX_HPP_
#define _P2CL_CIRCULARINDEX_HPP_
#include <algorithm>
#include <functional>
#include <iostream>

namespace p2cl{
	class CircularIndex{
		private:
			bool inVirtualRegion;
			int index;
			int length;
			int bufferSize;
			int sizeCurrentFrame;

			std::function<void(int)> callback;

			static void voidfunction(int){};
		public:

			operator int() const
			{
				return index;
			}
	
			CircularIndex()
			{
				length = 0;
				bufferSize = 0;
				sizeCurrentFrame = 0;
				index = 0;
				inVirtualRegion = false;
			}

			CircularIndex(int length, int bufferSize, std::function<void(int)> callback = voidfunction)
			{
				this->length = length;
				this->bufferSize = bufferSize;
				this->callback = callback;
				sizeCurrentFrame = 0;
				index = 0;
				inVirtualRegion = false;
			}

			void testCallback()
			{
				std::cout<<"test callback"<<std::endl;
				callback(0);
			}

			void increment()
			{
				int startTmp = index + 1;
				callback(index);
				if(startTmp >= length)
				{
					index = startTmp - length;
					
					inVirtualRegion = !inVirtualRegion;
				}else{
					index = startTmp;
				}
			}

			int write(void * buffer, void * src, int size, void ** nextSrc)
			{
				int sizeWritten;
				unsigned char * pDes = (unsigned char *)buffer;
				unsigned char * pSrc = (unsigned char *)src;
				int sizeTmp = sizeCurrentFrame + size;
				if(sizeTmp >= bufferSize)
				{
					sizeWritten = bufferSize - sizeCurrentFrame ;
					std::copy(pSrc, pSrc + sizeWritten, pDes + sizeCurrentFrame);
					sizeCurrentFrame = 0;
					increment();
				}else{
					sizeWritten = size;
					std::copy(pSrc, pSrc + sizeWritten, pDes + sizeCurrentFrame);
					sizeCurrentFrame += sizeWritten;
				}
				*nextSrc = pSrc + sizeWritten;
				return sizeWritten;
			}

			static bool isFull(const CircularIndex & writeIndex, const CircularIndex & readIndex)
			{
				return (writeIndex.index == readIndex.index) &&
				(writeIndex.inVirtualRegion != readIndex.inVirtualRegion);
			}

			static bool isEmpty(const CircularIndex & writeIndex, const CircularIndex & readIndex)
			{
				return (writeIndex.index == readIndex.index) &&
				(writeIndex.inVirtualRegion == readIndex.inVirtualRegion);
			}
	};
}

#endif
